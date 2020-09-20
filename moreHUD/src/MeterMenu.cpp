#include "PCH.h"
#include "MeterMenu.h"

#include <cassert>

#include "Scaleform.h"

#include "SKSE/API.h"

namespace Scaleform
{
	namespace
	{
		Meter::Meter() :
			super()
		{}


		Meter::Meter(const Meter& a_rhs) :
			super(a_rhs)
		{}


		Meter::Meter(Meter&& a_rhs) :
			super(std::move(a_rhs))
		{}


		Meter::Meter(const CLIK::GFx::Core::UIComponent& a_rhs) :
			super(a_rhs)
		{}


		Meter::Meter(CLIK::GFx::Core::UIComponent&& a_rhs) :
			super(std::move(a_rhs))
		{}


		Meter::Meter(const RE::GFxValue& a_val) :
			super(a_val)
		{
			assert(_instance.IsObject());
		}


		Meter::Meter(RE::GFxValue&& a_val) :
			super(std::move(a_val))
		{
			assert(_instance.IsObject());
		}


		Meter::~Meter()
		{}


		Meter& Meter::operator=(const Meter& a_rhs)
		{
			super::operator=(a_rhs);
			return *this;
		}


		Meter& Meter::operator=(Meter&& a_rhs)
		{
			super::operator=(std::move(a_rhs));
			return *this;
		}


		Meter& Meter::operator=(const CLIK::GFx::Core::UIComponent& a_rhs)
		{
			super::operator=(a_rhs);
			return *this;
		}


		Meter& Meter::operator=(CLIK::GFx::Core::UIComponent&& a_rhs)
		{
			super::operator=(std::move(a_rhs));
			return *this;
		}


		Meter& Meter::operator=(const RE::GFxValue& a_rhs)
		{
			_instance = a_rhs;
			assert(_instance.IsObject());
			return *this;
		}


		Meter& Meter::operator=(RE::GFxValue&& a_rhs)
		{
			_instance = std::move(a_rhs);
			assert(_instance.IsObject());
			return *this;
		}


		void Meter::SetPercent(double a_percent, bool a_force)
		{
			enum
			{
				kPercent,
				kForce,
				kNumArgs
			};

			RE::GFxValue args[kNumArgs];

			args[kPercent] = a_percent;
			assert(args[kPercent].IsNumber());

			args[kForce] = a_force;
			assert(args[kForce].IsBool());

			[[maybe_unused]] auto success = _instance.Invoke("setPercent", 0, args, kNumArgs);
			assert(success);
		}
	}


	MeterMenu::MeterMenu() :
		Base(),
		_meter()
	{
		using Context = RE::UserEvents::INPUT_CONTEXT_ID;
		using Flag = RE::IMenu::Flag;

		auto loader = RE::BSScaleformManager::GetSingleton();
		auto success = loader->LoadMovieEx(this, SWF_NAME, [this](RE::GFxMovieDef* a_def) {
			using StateType = RE::GFxState::StateType;

			fxDelegate.reset(new RE::FxDelegate());
			fxDelegate->RegisterHandler(this);
			a_def->SetState(StateType::kExternalInterface, fxDelegate.get());
			fxDelegate->Release();

			auto logger = new Logger<MeterMenu>();
			a_def->SetState(StateType::kLog, logger);
			logger->Release();
		});

		if (!success) {
			assert(false);
			logger::critical("MeterMenu did not have a view due to missing dependencies! Aborting process!");
			std::abort();
		}

		inputContext = Context::kGameplay;

		InitExtensions();
		uiMovie->SetVisible(false);
	}


	void MeterMenu::Accept(RE::FxDelegateHandler::CallbackProcessor* a_cbReg)
	{
		a_cbReg->Process("Log", Log);
		a_cbReg->Process("GetFramerate", GetFramerate);
	}


	auto MeterMenu::ProcessMessage(RE::UIMessage& a_message)
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


	void MeterMenu::AdvanceMovie(float a_interval, uint32_t a_currentTime)
	{
		_meter.Invalidate();
		Base::AdvanceMovie(a_interval, a_currentTime);
	}


	void MeterMenu::Open()
	{
		auto uiQueue = RE::UIMessageQueue::GetSingleton();
		uiQueue->AddMessage(Name(), RE::UI_MESSAGE_TYPE::kShow, 0);
	}


	void MeterMenu::Close()
	{
		auto uiQueue = RE::UIMessageQueue::GetSingleton();
		uiQueue->AddMessage(Name(), RE::UI_MESSAGE_TYPE::kHide, 0);
	}


	void MeterMenu::Register()
	{
		auto ui = RE::UI::GetSingleton();
		ui->Register(Name(), Create);

		logger::info("Registered {}", Name().data());
	}


	RE::IMenu* MeterMenu::Create()
	{
		return new MeterMenu();
	}


	void MeterMenu::TweenTo(double a_percent)
	{
		auto task = SKSE::GetTaskInterface();
		task->AddUITask([a_percent]() {
			auto ui = RE::UI::GetSingleton();
			auto menu = ui->GetMenu<MeterMenu>(Name());
			if (menu) {
				menu->SetPercent(a_percent);
			}
		});
	}


	void MeterMenu::SetPercent(double a_percent)
	{
		_meter.SetPercent(a_percent);
	}


	void MeterMenu::Log(const RE::FxDelegateArgs& a_params)
	{
		assert(a_params.GetArgCount() == 1);
		assert(a_params[0].IsString());

		logger::info("{}: {}", Name().data(), a_params[0].GetString());
	}


	void MeterMenu::GetFramerate(const RE::FxDelegateArgs& a_params)
	{
		assert(a_params.GetArgCount() == 0);

		RE::FxResponseArgs<1> response;
		RE::GFxValue		  retVal;

		retVal.SetNumber(a_params.GetMovie()->GetFrameRate());

		response.Add(retVal);
		a_params.Respond(response);
	}


	void MeterMenu::OnMenuOpen()
	{
		bool success;
		uiMovie->SetVisible(true);

		std::vector<std::pair<CLIK::Object*, std::string>> toGet;
		toGet.push_back(std::make_pair(&_meter, "meter"));
		RE::GFxValue var;
		for (auto& elem : toGet) {
			success = uiMovie->GetVariable(&var, elem.second.c_str());
			assert(success);
			*elem.first = var;
		}
	}


	void MeterMenu::OnMenuClose()
	{
		RE::GFxValue boolean(false);
		uiMovie->Invoke("Selection.captureFocus", 0, &boolean, 1);
	}


	void MeterMenu::InitExtensions()
	{
		RE::GFxValue boolean(true);
		bool		 success;

		success = uiMovie->SetVariable("_global.gfxExtensions", boolean);
		assert(success);
		success = uiMovie->SetVariable("_global.noInvisibleAdvance", boolean);
		assert(success);
	}
}
