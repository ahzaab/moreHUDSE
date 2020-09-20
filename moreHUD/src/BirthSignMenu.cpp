#include "PCH.h"

#include "BirthSignMenu.h"

#include "Papyrus.h"
#include "Scaleform.h"


namespace Scaleform
{
	BirthSignMenu::BirthSignMenu() :
		MenuBase(),
		HandlerBase(),
		_root()
	{
		using Context = RE::UserEvents::INPUT_CONTEXT_ID;
		using Flag = RE::IMenu::Flag;

		this->menuFlags.set(Flag::kUpdateUsesCursor);
		auto loader = RE::BSScaleformManager::GetSingleton();
		auto success = loader->LoadMovieEx(this, SWF_NAME, [this](RE::GFxMovieDef* a_def) {
			using StateType = RE::GFxState::StateType;

			fxDelegate.reset(new RE::FxDelegate());
			fxDelegate->RegisterHandler(this);
			a_def->SetState(StateType::kExternalInterface, fxDelegate.get());
			fxDelegate->Release();

			auto logger = new Logger<BirthSignMenu>();
			a_def->SetState(StateType::kLog, logger);
			logger->Release();
		});

		if (!success) {
			assert(false);
			logger::critical("BirthSignMenu did not have a view due to missing dependencies! Aborting process!"sv);
			std::abort();
		}

		this->depthPriority = 5;  // JournalMenu == 5
		this->menuFlags.set(Flag::kDisablePauseMenu, Flag::kTopmostRenderedMenu, Flag::kPausesGame);
		this->inputContext = Context::kFavor;

		auto mc = RE::MenuControls::GetSingleton();
		mc->RegisterHandler(this);
		uiMovie->SetVisible(false);
	}


	BirthSignMenu::~BirthSignMenu()
	{
		auto mc = RE::MenuControls::GetSingleton();
		mc->RemoveHandler(this);
	}


	BirthSignMenu::Result BirthSignMenu::ProcessMessage(RE::UIMessage& m_message)
	{
		using Message = RE::UI_MESSAGE_TYPE;

		switch (*m_message.type) {
		case Message::kShow:
			OnMenuOpen();
			return Result::kHandled;
		case Message::kHide:
			OnMenuClose();
			return Result::kHandled;
		default:
			return MenuBase::ProcessMessage(m_message);
		}
	}


	bool BirthSignMenu::CanProcess(RE::InputEvent* a_event)
	{
		return a_event && a_event->eventType == RE::INPUT_EVENT_TYPE::kButton;
	}


	bool BirthSignMenu::ProcessButton(RE::ButtonEvent* a_event)
	{
		using Device = RE::INPUT_DEVICE;

		for (auto button = a_event; button; button = static_cast<RE::ButtonEvent*>(button->next)) {
			switch (*button->device) {
			case Device::kKeyboard:
			{
				using Key = RE::BSWin32KeyboardDevice::Key;
				switch (button->idCode) {
				case Key::kEnter:
					SendAcceptEvent();
					break;
				case Key::kEscape:
					Close();
					break;
				default:
					break;
				}
			} break;
			case Device::kGamepad:
			{
				using Key = RE::BSWin32GamepadDevice::Key;
				switch (button->idCode) {
				case Key::kA:
					SendAcceptEvent();
					break;
				case Key::kBack:
					Close();
					break;
				default:
					break;
				}
			} break;
			default:
				break;
			}
		}

		return true;
	}


	void BirthSignMenu::Open()
	{
		auto uiQueue = RE::UIMessageQueue::GetSingleton();
		uiQueue->AddMessage(Name(), RE::UI_MESSAGE_TYPE::kShow, 0);
	}


	void BirthSignMenu::Close()
	{
		auto uiQueue = RE::UIMessageQueue::GetSingleton();
		uiQueue->AddMessage(Name(), RE::UI_MESSAGE_TYPE::kHide, 0);
	}


	void BirthSignMenu::Register()
	{
		auto ui = RE::UI::GetSingleton();
		ui->Register(Name(), Create);

		logger::info("Registered {}", Name().data());
	}


	RE::IMenu* BirthSignMenu::Create()
	{
		return new BirthSignMenu();
	}


	void BirthSignMenu::OnMenuOpen()
	{
		AdjustScale();
		SetInputs();
		uiMovie->SetVisible(true);

		bool											   success;
		std::vector<std::pair<RE::GFxValue*, std::string>> toGet;
		toGet.push_back(std::make_pair(&_root, "birthSignList"));
		RE::GFxValue var;
		for (auto& elem : toGet) {
			success = uiMovie->GetVariable(&var, elem.second.c_str());
			assert(success);
			*elem.first = var;
		}
	}


	void BirthSignMenu::OnMenuClose()
	{
		return;
	}


	void BirthSignMenu::AdjustScale(double a_scale)
	{
		auto def = uiMovie->GetMovieDef();
		auto width = def->GetWidth();
		auto height = def->GetHeight();

		RE::GFxValue root;
		RE::GFxValue num;
		bool		 success = false;

		success = uiMovie->GetVariable(&root, "_root");
		assert(success);

		RE::GFxValue::DisplayInfo displayInfo;
		success = root.GetDisplayInfo(&displayInfo);
		assert(success);

		displayInfo.SetX(width * (1.0 - a_scale) / 2);
		displayInfo.SetY(height * (1.0 - a_scale) / 2);

		success = root.SetDisplayInfo(displayInfo);
		assert(success);

		num.SetNumber(width * a_scale);
		success = root.SetMember("_width", num);
		assert(success);

		num.SetNumber(height * a_scale);
		success = root.SetMember("_height", num);
		assert(success);
	}


	void BirthSignMenu::SendAcceptEvent()
	{
		RE::GFxValue		  result;
		[[maybe_unused]] auto success = _root.Invoke("getCurrentSign", &result, 0, 0);
		assert(success);
		auto id = static_cast<std::uint32_t>(result.GetUInt());
		if (id != 0) {
			auto handler = OnBirthSignAcceptHandler::GetSingleton();
			handler->QueueEvent(id);
			BirthSignMenu::Close();
		}
	}


	void BirthSignMenu::SetInputs()
	{
		enum
		{
			kAcceptText,
			kAcceptIcon,
			kCancelText,
			kCancelIcon,
			kNumArgs
		};

		uint32_t acceptKey;
		uint32_t cancelKey;

		auto input = RE::BSInputDeviceManager::GetSingleton();
		if (input->IsGamepadEnabled()) {
			using Key = RE::BSWin32GamepadDevice::Key;
			acceptKey = GetGamepadIndex(Key::kA);
			cancelKey = GetGamepadIndex(Key::kBack);
		} else {
			acceptKey = kEnter;
			cancelKey = kESC;
		}

		auto		gmst = RE::GameSettingCollection::GetSingleton();
		auto		sAccept = gmst->GetSetting("sAccept");
		std::string acceptText = sAccept->GetString();
		auto		sCancel = gmst->GetSetting("sCancel");
		std::string cancelText = sCancel->GetString();

		RE::GFxValue args[kNumArgs];
		args[kAcceptText].SetString(acceptText.c_str());
		args[kAcceptIcon].SetNumber(acceptKey);
		args[kCancelText].SetString(cancelText.c_str());
		args[kCancelIcon].SetNumber(cancelKey);
		[[maybe_unused]] auto success = _root.Invoke("setInputs", 0, args, kNumArgs);
		assert(success);
	}


	uint32_t BirthSignMenu::GetGamepadIndex(RE::BSWin32GamepadDevice::Key a_key)
	{
		using Key = RE::BSWin32GamepadDevice::Key;

		uint32_t index;
		switch (a_key) {
		case Key::kUp:
			index = 0;
			break;
		case Key::kDown:
			index = 1;
			break;
		case Key::kLeft:
			index = 2;
			break;
		case Key::kRight:
			index = 3;
			break;
		case Key::kStart:
			index = 4;
			break;
		case Key::kBack:
			index = 5;
			break;
		case Key::kLeftThumb:
			index = 6;
			break;
		case Key::kRightThumb:
			index = 7;
			break;
		case Key::kLeftShoulder:
			index = 8;
			break;
		case Key::kRightShoulder:
			index = 9;
			break;
		case Key::kA:
			index = 10;
			break;
		case Key::kB:
			index = 11;
			break;
		case Key::kX:
			index = 12;
			break;
		case Key::kY:
			index = 13;
			break;
		case Key::kLeftTrigger:
			index = 14;
			break;
		case Key::kRightTrigger:
			index = 15;
			break;
		default:
			index = kInvalidButton;
			break;
		}

		return index != kInvalidButton ? index + kGamepadOffset : kInvalidButton;
	}
}
