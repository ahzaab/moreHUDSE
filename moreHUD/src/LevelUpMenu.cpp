#include "PCH.h"
#include "LevelUpMenu.h"

#include <cassert>

#include "Scaleform.h"
#include "SKSE/Logger.h"

namespace Scaleform
{
	LevelUpMenu::LevelUpMenu()
	{
		using Context = RE::UserEvents::INPUT_CONTEXT_ID;
		using Flag = RE::IMenu::Flag;

		menuFlags.set(Flag::kUpdateUsesCursor);
		auto loader = RE::BSScaleformManager::GetSingleton();
		auto success = loader->LoadMovieEx(this, SWF_NAME, [this](RE::GFxMovieDef* a_def) {
			using StateType = RE::GFxState::StateType;

			fxDelegate.reset(new RE::FxDelegate());
			fxDelegate->RegisterHandler(this);
			a_def->SetState(StateType::kExternalInterface, fxDelegate.get());
			fxDelegate->Release();

			auto logger = new Logger<LevelUpMenu>();
			a_def->SetState(StateType::kLog, logger);
			logger->Release();
		});

		if (!success) {
			assert(false);
			logger::critical("LevelUpMenu did not have a view due to missing dependencies! Aborting process!");
			std::abort();
		}

		menuFlags.set(Flag::kPausesGame, Flag::kModal, Flag::kDisablePauseMenu, Flag::kDisablePauseMenu);
		inputContext = Context::kMenuMode;

		InitExtensions();
		uiMovie->SetVisible(false);
	}


	void LevelUpMenu::Accept(RE::FxDelegateHandler::CallbackProcessor* a_cbReg)
	{
		a_cbReg->Process("AdvanceLevel", AdvanceLevel);
		a_cbReg->Process("CloseMenu", CloseMenu);
		a_cbReg->Process("GetGlobal", GetGlobal);
		a_cbReg->Process("GetPlayerAV", GetPlayerAV);
		a_cbReg->Process("GetPlayerLevel", GetPlayerLevel);
		a_cbReg->Process("Log", Log);
		a_cbReg->Process("ModGlobal", ModGlobal);
		a_cbReg->Process("ModPlayerAV", ModPlayerAV);
		a_cbReg->Process("PlaySound", PlaySound);
	}


	auto LevelUpMenu::ProcessMessage(RE::UIMessage& a_message)
		-> Result
	{
		using Message = RE::UI_MESSAGE_TYPE;

		switch (*a_message.type) {
		case Message::kShow:
			OnMenuOpen();
			return Result::kHandled;
		case Message::kHide:
			OnMenuClose();
			return Result::kHandled;
		default:
			return Base::ProcessMessage(a_message);
		}
	}


	void LevelUpMenu::Open()
	{
		auto uiQueue = RE::UIMessageQueue::GetSingleton();
		uiQueue->AddMessage(Name(), RE::UI_MESSAGE_TYPE::kShow, 0);
	}


	void LevelUpMenu::Close()
	{
		auto uiQueue = RE::UIMessageQueue::GetSingleton();
		uiQueue->AddMessage(Name(), RE::UI_MESSAGE_TYPE::kHide, 0);
	}


	void LevelUpMenu::Register()
	{
		auto ui = RE::UI::GetSingleton();
		auto it = ui->menuMap.find(Name());
		if (it != ui->menuMap.end()) {
			it->second.create = Create;
		} else {
			ui->Register(Name(), Create);
		}

		logger::info("Registered {}"sv, Name().data());
	}


	RE::IMenu* LevelUpMenu::Create()
	{
		return new LevelUpMenu();
	}


	void LevelUpMenu::AdvanceLevel([[maybe_unused]] const RE::FxDelegateArgs& a_params)
	{
		assert(a_params.GetArgCount() == 0);

		auto player = RE::PlayerCharacter::GetSingleton();
		player->skills->AdvanceLevel(false);
	}


	void LevelUpMenu::CloseMenu([[maybe_unused]] const RE::FxDelegateArgs& a_params)
	{
		assert(a_params.GetArgCount() == 0);

		Close();
	}


	void LevelUpMenu::GetGlobal(const RE::FxDelegateArgs& a_params)
	{
		assert(a_params.GetArgCount() == 2);
		assert(a_params[0].IsNumber());
		assert(a_params[1].IsString());

		RE::FxResponseArgs<1> response;
		RE::GFxValue		  retVal;
		retVal.SetUndefined();

		auto baseID = static_cast<RE::FormID>(a_params[0].GetUInt());
		auto pluginName = a_params[1].GetString();

		auto dataHandler = RE::TESDataHandler::GetSingleton();
		auto global = dataHandler->LookupForm<RE::TESGlobal>(baseID, pluginName);
		if (global) {
			retVal.SetNumber(global->value);
		}

		response.Add(retVal);
		a_params.Respond(response);
	}


	void LevelUpMenu::GetPlayerAV(const RE::FxDelegateArgs& a_params)
	{
		assert(a_params.GetArgCount() == 1);
		assert(a_params[0].IsNumber());

		RE::FxResponseArgs<1> response;
		RE::GFxValue		  retVal;

		auto av = static_cast<RE::ActorValue>(a_params[0].GetUInt());
		auto player = RE::PlayerCharacter::GetSingleton();
		auto currentAV = player->GetActorValue(av);
		retVal.SetNumber(currentAV);

		response.Add(retVal);
		a_params.Respond(response);
	}


	void LevelUpMenu::GetPlayerLevel(const RE::FxDelegateArgs& a_params)
	{
		assert(a_params.GetArgCount() == 0);

		RE::FxResponseArgs<1> response;
		RE::GFxValue		  retVal;

		auto player = RE::PlayerCharacter::GetSingleton();
		auto level = static_cast<double>(player->GetLevel());
		retVal.SetNumber(level);

		response.Add(retVal);
		a_params.Respond(response);
	}


	void LevelUpMenu::Log(const RE::FxDelegateArgs& a_params)
	{
		assert(a_params.GetArgCount() == 1);
		assert(a_params[0].IsString());

		logger::info("{}: {}"sv, Name().data(), a_params[0].GetString());
	}


	void LevelUpMenu::ModGlobal(const RE::FxDelegateArgs& a_params)
	{
		assert(a_params.GetArgCount() == 3);
		assert(a_params[0].IsNumber());
		assert(a_params[1].IsNumber());
		assert(a_params[2].IsString());

		auto baseID = static_cast<RE::FormID>(a_params[0].GetUInt());
		auto pluginName = a_params[2].GetString();

		auto dataHandler = RE::TESDataHandler::GetSingleton();
		auto global = dataHandler->LookupForm<RE::TESGlobal>(baseID, pluginName);
		if (global) {
			auto mod = a_params[1].GetNumber();
			global->value += static_cast<float>(mod);
		}
	}


	void LevelUpMenu::ModPlayerAV(const RE::FxDelegateArgs& a_params)
	{
		assert(a_params.GetArgCount() == 2);
		assert(a_params[0].IsNumber());
		assert(a_params[1].IsNumber());

		auto av = static_cast<RE::ActorValue>(a_params[0].GetUInt());
		auto mod = static_cast<float>(a_params[1].GetNumber());
		auto player = RE::PlayerCharacter::GetSingleton();
		player->ModActorValue(av, mod);
	}


	void LevelUpMenu::PlaySound(const RE::FxDelegateArgs& a_params)
	{
		assert(a_params.GetArgCount() == 1);
		assert(a_params[0].IsString());

		RE::PlaySound(a_params[0].GetString());
	}


	void LevelUpMenu::OnMenuOpen()
	{
		bool success;
		uiMovie->SetVisible(true);

		std::vector<std::pair<RE::GFxValue*, std::string>> toGet;
		toGet.push_back(std::make_pair(&_root, "LevelUpMenu_mc"));
		RE::GFxValue var;
		for (auto& elem : toGet) {
			success = uiMovie->GetVariable(&var, elem.second.c_str());
			assert(success);
			*elem.first = var;
		}
	}


	void LevelUpMenu::OnMenuClose()
	{
		RE::GFxValue boolean(false);
		uiMovie->Invoke("Selection.captureFocus", 0, &boolean, 1);
	}


	void LevelUpMenu::InitExtensions()
	{
		RE::GFxValue boolean(true);
		bool		 success;

		auto input = RE::BSInputDeviceManager::GetSingleton();
		if (input->IsGamepadEnabled()) {
			uiMovie->Invoke("Selection.captureFocus", 0, &boolean, 1);
		}

		success = uiMovie->SetVariable("_global.gfxExtensions", boolean);
		assert(success);
		success = uiMovie->SetVariable("_global.noInvisibleAdvance", boolean);
		assert(success);
		success = uiMovie->SetVariable("Selection.alwaysEnableArrowKeys", boolean);
		assert(success);
		success = uiMovie->SetVariable("Selection.alwaysEnableKeyboardPress", boolean);
		assert(success);
		success = uiMovie->SetVariable("Selection.disableFocusAutoRelease", boolean);
		assert(success);
	}
}
