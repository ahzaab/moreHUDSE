#include "PCH.h"

#include "SpellmakingMenu.h"

#include <map>

#include "CLIK/Array.h"
#include "Scaleform.h"

namespace Scaleform
{
	namespace
	{
		Slider::Slider() :
			header(),
			slider(),
			text(),
			_isDragging(false)
		{}


		void Slider::UpdateText()
		{
			auto num = static_cast<std::ptrdiff_t>(slider.Value());
			auto val = std::to_string(num);
			text.HTMLText(val);
		}


		void Slider::SetDragging(bool a_isDragging)
		{
			_isDragging = a_isDragging;
		}


		bool Slider::IsDragging() const
		{
			return _isDragging;
		}
	}


	SpellmakingMenu::SpellmakingMenu() :
		_available(),
		_selected(),
		_range(),
		_type(),
		_magnitude(),
		_duration(),
		_area(),
		_availableMappings(),
		_selectedMappings(),
		_rangeMappings(),
		_name(),
		_selectedIdx(kInvalid)
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

			auto logger = new Logger<SpellmakingMenu>();
			a_def->SetState(StateType::kLog, logger);
			logger->Release();
		});

		if (!success) {
			assert(false);
			logger::critical("SpellmakingMenu did not have a view due to missing dependencies! Aborting process!\n");
			std::abort();
		}

		depthPriority = 5;	// JournalMenu == 5
		menuFlags.set(Flag::kDisablePauseMenu, Flag::kTopmostRenderedMenu, Flag::kPausesGame);
		inputContext = Context::kFavor;

		InitExtensions();
		uiMovie->SetVisible(false);
	}


	void SpellmakingMenu::Accept(RE::FxDelegateHandler::CallbackProcessor* a_processor)
	{
		a_processor->Process("Log", Log);
		a_processor->Process("OnAvailablePress", OnAvailablePress);
		a_processor->Process("OnSelectedPress", OnSelectedPress);
		a_processor->Process("OnMagnitudeDragBegin", OnMagnitudeDragBegin);
		a_processor->Process("OnMagnitudeDragEnd", OnMagnitudeDragEnd);
		a_processor->Process("OnMagnitudeChange", OnMagnitudeChange);
		a_processor->Process("OnDurationDragBegin", OnDurationDragBegin);
		a_processor->Process("OnDurationDragEnd", OnDurationDragEnd);
		a_processor->Process("OnDurationChange", OnDurationChange);
		a_processor->Process("OnAreaDragBegin", OnAreaDragBegin);
		a_processor->Process("OnAreaDragEnd", OnAreaDragEnd);
		a_processor->Process("OnAreaChange", OnAreaChange);
		a_processor->Process("CraftSpell", CraftSpell);
	}


	auto SpellmakingMenu::ProcessMessage(RE::UIMessage& a_message) 
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


	void SpellmakingMenu::AdvanceMovie(float a_interval, uint32_t a_currentTime)
	{
		if (_magnitude.IsDragging()) {
			_magnitude.UpdateText();
		}

		if (_duration.IsDragging()) {
			_duration.UpdateText();
		}

		if (_area.IsDragging()) {
			_area.UpdateText();
		}

		Base::AdvanceMovie(a_interval, a_currentTime);
	}


	void SpellmakingMenu::Open()
	{
		auto uiQueue = RE::UIMessageQueue::GetSingleton();
		uiQueue->AddMessage(Name(), RE::UI_MESSAGE_TYPE::kShow, 0);
	}


	void SpellmakingMenu::Close()
	{
		auto uiQueue = RE::UIMessageQueue::GetSingleton();
		uiQueue->AddMessage(Name(), RE::UI_MESSAGE_TYPE::kHide, 0);
	}


	void SpellmakingMenu::Register()
	{
		auto ui = RE::UI::GetSingleton();
		ui->Register(Name(), Create);

		logger::info("Registered {} (pls work)", Name().data());
	}


	RE::IMenu* SpellmakingMenu::Create()
	{
		return new SpellmakingMenu();
	}


	void SpellmakingMenu::Log(const RE::FxDelegateArgs& a_params)
	{
		assert(a_params.GetArgCount() == 1);
		assert(a_params[0].IsString());

		logger::info("{}: {}", Name().data(), a_params[0].GetString());
	}

	void SpellmakingMenu::OnTextChange(const RE::FxDelegateArgs& a_params)
	{
		assert(a_params.GetArgCount() == 1);
		logger::info("{} args", a_params.GetArgCount());
		//logger::info("current text: %s", _name.Text());
		auto menu = static_cast<SpellmakingMenu*>(a_params.GetHandler());
		auto text = a_params[0];
		if (a_params[0].GetType() == RE::GFxValue::ValueType::kString) {
			logger::info("returned text: {}", text.GetString());
		}
		menu->OnTextChange();
	}


	void SpellmakingMenu::OnAvailablePress(const RE::FxDelegateArgs& a_params)
	{
		assert(a_params.GetArgCount() == 1);
		assert(a_params[0].IsNumber());

		auto menu = static_cast<SpellmakingMenu*>(a_params.GetHandler());
		auto num = a_params[0].GetUInt();
		menu->OnAvailablePress(num);
	}


	void SpellmakingMenu::OnSelectedPress(const RE::FxDelegateArgs& a_params)
	{
		assert(a_params.GetArgCount() == 2);
		assert(a_params[0].IsNumber());
		assert(a_params[1].IsBool());

		auto menu = static_cast<SpellmakingMenu*>(a_params.GetHandler());
		auto index = a_params[0].GetUInt();
		auto remove = a_params[1].GetBool();
		menu->OnSelectedPress(index, remove);
	}


	void SpellmakingMenu::OnMagnitudeDragBegin(const RE::FxDelegateArgs& a_params)
	{
		assert(a_params.GetArgCount() == 0);

		auto menu = static_cast<SpellmakingMenu*>(a_params.GetHandler());
		menu->OnMagnitudeDragChange(true);
	}


	void SpellmakingMenu::OnMagnitudeDragEnd(const RE::FxDelegateArgs& a_params)
	{
		assert(a_params.GetArgCount() == 0);

		auto menu = static_cast<SpellmakingMenu*>(a_params.GetHandler());
		menu->OnMagnitudeDragChange(false);
	}


	void SpellmakingMenu::OnMagnitudeChange(const RE::FxDelegateArgs& a_params)
	{
		assert(a_params.GetArgCount() == 0);

		auto menu = static_cast<SpellmakingMenu*>(a_params.GetHandler());
		menu->OnMagnitudeChange();
	}


	void SpellmakingMenu::OnDurationDragBegin(const RE::FxDelegateArgs& a_params)
	{
		assert(a_params.GetArgCount() == 0);

		auto menu = static_cast<SpellmakingMenu*>(a_params.GetHandler());
		menu->OnDurationDragChange(true);
	}


	void SpellmakingMenu::OnDurationDragEnd(const RE::FxDelegateArgs& a_params)
	{
		assert(a_params.GetArgCount() == 0);

		auto menu = static_cast<SpellmakingMenu*>(a_params.GetHandler());
		menu->OnDurationDragChange(false);
	}


	void SpellmakingMenu::OnDurationChange(const RE::FxDelegateArgs& a_params)
	{
		assert(a_params.GetArgCount() == 0);

		auto menu = static_cast<SpellmakingMenu*>(a_params.GetHandler());
		menu->OnDurationChange();
	}


	void SpellmakingMenu::OnAreaDragBegin(const RE::FxDelegateArgs& a_params)
	{
		assert(a_params.GetArgCount() == 0);

		auto menu = static_cast<SpellmakingMenu*>(a_params.GetHandler());
		menu->OnAreaDragChange(true);
	}


	void SpellmakingMenu::OnAreaDragEnd(const RE::FxDelegateArgs& a_params)
	{
		assert(a_params.GetArgCount() == 0);

		auto menu = static_cast<SpellmakingMenu*>(a_params.GetHandler());
		menu->OnAreaDragChange(false);
	}


	void SpellmakingMenu::OnAreaChange(const RE::FxDelegateArgs& a_params)
	{
		assert(a_params.GetArgCount() == 0);

		auto menu = static_cast<SpellmakingMenu*>(a_params.GetHandler());
		menu->OnAreaChange();
	}


	void SpellmakingMenu::CraftSpell(const RE::FxDelegateArgs& a_params)
	{
		assert(a_params.GetArgCount() == 0);

		auto menu = static_cast<SpellmakingMenu*>(a_params.GetHandler());
		menu->CraftSpell();
	}


	void SpellmakingMenu::OnMenuOpen()
	{
		bool success;
		uiMovie->SetVisible(true);
		std::vector<std::pair<CLIK::Object*, std::string>> toGet;
		toGet.push_back(std::make_pair(&_available, "available"));
		toGet.push_back(std::make_pair(&_selected, "selected"));
		toGet.push_back(std::make_pair(&_range.header, "rangeHeader"));
		toGet.push_back(std::make_pair(&_range.dropdown, "range"));
		toGet.push_back(std::make_pair(&_type.header, "typeHeader"));
		toGet.push_back(std::make_pair(&_type.dropdown, "type"));
		toGet.push_back(std::make_pair(&_magnitude.header, "magnitudeHeader"));
		toGet.push_back(std::make_pair(&_magnitude.slider, "magnitude"));
		toGet.push_back(std::make_pair(&_magnitude.text, "magnitudeText"));
		toGet.push_back(std::make_pair(&_duration.header, "durationHeader"));
		toGet.push_back(std::make_pair(&_duration.slider, "duration"));
		toGet.push_back(std::make_pair(&_duration.text, "durationText"));
		toGet.push_back(std::make_pair(&_area.header, "areaHeader"));
		toGet.push_back(std::make_pair(&_area.slider, "area"));
		toGet.push_back(std::make_pair(&_area.text, "areaText"));
		toGet.push_back(std::make_pair(&_name, "name"));
		toGet.push_back(std::make_pair(&_craft, "craft"));
		RE::GFxValue var;
		for (auto& elem : toGet) {
			success = uiMovie->GetVariable(&var, elem.second.c_str());
			assert(success);
			*elem.first = var;
		}

		CLIK::Object obj("ScrollBar");
		_available.ScrollBar(obj);
		_range.dropdown.ScrollBar(obj);
		_type.dropdown.ScrollBar(obj);

		assert(_name.Editable());
		assert(!_name.Disabled());

		logger::info("current text: {}", _name.Text());


		_craft.Label("Craft Spell");
		_craft.Disabled(true);

		InitEffectInfo();
		SetAvailable();
		SetEffectInfo();
	}


	void SpellmakingMenu::OnMenuClose()
	{
		return;
	}


	void SpellmakingMenu::InitExtensions()
	{
		RE::GFxValue boolean(true);
		bool		 success;

		success = uiMovie->SetVariable("_global.gfxExtensions", boolean);
		assert(success);
		success = uiMovie->SetVariable("_global.noInvisibleAdvance", boolean);
		assert(success);
	}


	void SpellmakingMenu::InitEffectInfo()
	{
		using Type = decltype(_rangeMappings)::value_type::Type;

		_range.header.HTMLText("Range");

		_rangeMappings.clear();
		_rangeMappings.push_back({ "Self", Type::kSelf });
		_rangeMappings.push_back({ "Touch", Type::kTargetActor });	// touch doesnt work so i replaced it
		_rangeMappings.push_back({ "Aimed", Type::kAimed });

		CLIK::Array	 arr(uiMovie);
		CLIK::Object elem;
		for (auto& range : _rangeMappings) {
			elem = range.text;
			arr.Push(elem);
		}

		_range.dropdown.Disabled(false);
		_range.dropdown.DataProvider(arr);


		_type.header.HTMLText("Cast Type");
		std::vector<std::string> types;
		types.push_back("Fire and Forget");
		types.push_back("Concentration");

		CLIK::Array arr2(uiMovie);
		for (auto& range : types) {
			elem = range;
			arr2.Push(elem);
		}

		_type.dropdown.Disabled(false);
		_type.dropdown.DataProvider(arr2);

		_magnitude.header.HTMLText("Magnitude");
		_duration.header.HTMLText("Duration");
		_area.header.HTMLText("Area");
	}


	void SpellmakingMenu::SetAvailable()
	{
		using value_type = decltype(_availableMappings)::value_type;

		struct _effectData
		{
			RE::FormID		   effectID;
			RE::EffectSetting* effect;
			RE::SpellItem*	   spell;
			bool			   operator<(const _effectData& t) const
			{
				return (this->effectID < t.effectID);
			}
		};
		//std::map<RE::FormID, RE::EffectSetting*> knownEffects;
		std::set<_effectData> knownEffects;
#if 1
		auto player = RE::PlayerCharacter::GetSingleton();
		for (auto& spell : player->addedSpells) {
			for (auto& effect : spell->effects) {
				auto mgef = effect->baseEffect;
				//knownEffects.insert(std::make_pair(mgef->formID, mgef));
				knownEffects.insert({ mgef->formID, mgef, spell });
				//if (!mgef->fullName.empty()) {
				//	std::string name(mgef->fullName);
				//	SanitizeString(name);
				//	_availableMappings.push_back({ std::move(name), mgef->formID });
				//}
			}
		}

		auto base = player->GetActorBase();
		if (base->actorEffects) {
			auto effects = base->actorEffects;
			for (uint32_t i = 0; i < effects->numSpells; ++i) {
				auto spell = effects->spells[i];
				for (auto& effect : spell->effects) {
					auto mgef = effect->baseEffect;
					knownEffects.insert({ mgef->formID, mgef, spell });
					//knownEffects.insert(std::make_pair(mgef->formID, mgef));
				}
			}
		}
#else
		auto dataHandler = RE::TESDataHandler::GetSingleton();
		for (auto& mgef : dataHandler->GetFormArray<RE::EffectSetting>()) {
			knownEffects.insert(std::make_pair(mgef->formID, mgef));
		}
#endif

		for (auto& effect : knownEffects) {
			auto mgef = effect.effect;
			if (!mgef->fullName.empty()) {
				std::string name(mgef->fullName);
				SanitizeString(name);
				_availableMappings.push_back({ std::move(name), mgef->formID, effect.spell });
			}
		}

		std::sort(_availableMappings.begin(), _availableMappings.end(), [](const value_type& a_lhs, const value_type& a_rhs) {
			return a_lhs.text < a_rhs.text;
		});

		CLIK::Array	 arr(uiMovie);
		CLIK::Object elem;
		for (auto& avail : _availableMappings) {
			elem = avail.text;
			arr.Push(elem);
		}

		_available.DataProvider(arr);
	}


	void SpellmakingMenu::SetEffectInfo()
	{
		if (_selectedIdx < _selectedMappings.size()) {
			auto& selected = _selectedMappings[_selectedIdx];

			_magnitude.slider.Disabled(false);
			_magnitude.slider.Value(selected.magnitude);
			_magnitude.text.HTMLText(std::to_string(selected.magnitude));

			_duration.slider.Disabled(false);
			_duration.slider.Value(selected.duration);
			_duration.text.HTMLText(std::to_string(selected.duration));

			_area.slider.Disabled(false);
			_area.slider.Value(selected.area);
			_area.text.HTMLText(std::to_string(selected.area));
		} else {
			_magnitude.slider.Disabled(true);
			_magnitude.text.HTMLText("-");

			_duration.slider.Disabled(true);
			_duration.text.HTMLText("-");

			_area.slider.Disabled(true);
			_area.text.HTMLText("-");
		}
	}


	void SpellmakingMenu::CommitSelection()
	{
		if (_selectedIdx < _selectedMappings.size()) {
			auto& selected = _selectedMappings[_selectedIdx];
			selected.magnitude = static_cast<uint32_t>(_magnitude.slider.Value());
			selected.duration = static_cast<uint32_t>(_duration.slider.Value());
			selected.area = static_cast<uint32_t>(_area.slider.Value());
		}
	}

	// straight from open morrowind kinda
	float SpellmakingMenu::calculateEffectCost(const Selected& effect, RE::EffectSetting* effectSetting)
	{
		float x = 0.5f * effect.magnitude;
		x *= 0.1f * effectSetting->data.baseCost;
		x *= 1 + effect.duration;
		x += 0.05f * effect.area * effectSetting->data.baseCost;
		if (_range.dropdown.SelectedIndex() == 2.0) {
			x *= 1.5;
		}
		return x;
	}


	void SpellmakingMenu::CraftSpell()
	{
		CommitSelection();

		auto factory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::SpellItem>();
		if (!factory) {
			return;
		}

		auto spell = factory->Create();
		if (!spell) {
			logger::info("couldnt create spell...");
			return;
		}

		spell->fullName = _name.Text();
		//spell->fullName = "Spell of Awesomeness";
		spell->data.spellType = RE::MagicSystem::SpellType::kSpell;

		switch (static_cast<int>(_type.dropdown.SelectedIndex())) {
		case 0:
			spell->data.castingType = RE::MagicSystem::CastingType::kConstantEffect;  // the enum for castingtype is off by 1
			break;
		case 1:
			spell->data.castingType = RE::MagicSystem::CastingType::kFireAndForget;
		}

		switch (static_cast<int>(_range.dropdown.SelectedIndex())) {
		case 0:
			spell->data.delivery = RE::MagicSystem::Delivery::kSelf;
			break;
		case 1:
			spell->data.delivery = RE::MagicSystem::Delivery::kTargetActor;
			spell->data.range = 0.1f;  // simulate touch since the touch delivery doesnt seem to work (though neither does the range var but tagetActor gets us most of the way there)
			break;
		case 2:
			spell->data.delivery = RE::MagicSystem::Delivery::kAimed;
			break;
		default:
			logger::info("howd you manage to break this? impressive...");
			break;
		}

		//spell->data.delivery = static_cast<RE::MagicSystem::Delivery>(static_cast<int>(_range.dropdown.SelectedIndex())); //static_cast<RE::MagicSystem::Delivery>(static_cast<std::ptrdiff_t>(_area.slider.Value()));
		logger::info("menu display from {}", _selectedMappings[0].spell->fullName.c_str());
		// tests
		spell->data.chargeTime = 1.0;
		// probably need some function to determine charge time...
		//

		//spell->menuDispObject = RE::TESForm::LookupByID<RE::TESObjectSTAT>(0x000A0E7D);	// MAGINVHealSpellArt
		spell->menuDispObject = _selectedMappings[0].spell->GetMenuDisplayObject();

		spell->boundData.boundMin.x = 0;
		spell->boundData.boundMin.y = 0;
		spell->boundData.boundMin.z = 0;
		spell->boundData.boundMax.x = 0;
		spell->boundData.boundMax.y = 0;
		spell->boundData.boundMax.z = 0;

		//spell->avEffectSetting = RE::TESForm::LookupByID<RE::EffectSetting>(_selectedMappings[0].effectID);

		for (auto& selected : _selectedMappings) {
			auto effect = new RE::Effect();
			effect->effectItem.magnitude = static_cast<float>(selected.magnitude);
			effect->effectItem.area = selected.area;
			effect->effectItem.duration = selected.duration;
			effect->baseEffect = RE::TESForm::LookupByID<RE::EffectSetting>(selected.effectID);

			effect->cost = calculateEffectCost(selected, effect->baseEffect);  // only doing this until I can figure out why the costOverride doesn't work

			spell->effects.push_back(effect);
		}

		auto player = RE::PlayerCharacter::GetSingleton();
		auto cost = spell->CalculateMagickaCost(player);

		logger::info("calculated cost: %f", cost);

		spell->data.costOverride = 1000;  // doesnt work for some reason...

		player->AddSpell(spell);
		logger::info("added {}", spell->fullName.c_str());

		Close();
	}
	bool SpellmakingMenu::OnTextChange()
	{
		return true;
	}

	bool SpellmakingMenu::OnAvailablePress(std::size_t a_availIdx)
	{
		using value_type = decltype(_selectedMappings)::value_type;

		if (a_availIdx >= _availableMappings.size() || _selectedMappings.size() >= MAX_EFFECTS) {
			return false;
		}

		auto& avail = _availableMappings[a_availIdx];
		auto  it = std::find_if(_selectedMappings.begin(), _selectedMappings.end(), [&](const value_type& a_val) -> bool {
			 return a_val.effectID == avail.effectID;
		 });
		if (it != _selectedMappings.end()) {
			return false;
		}

		_selectedMappings.push_back({ avail.text, avail.effectID, avail.spell, 1, 0, 1 });
		//std::stable_sort(_selectedMappings.begin(), _selectedMappings.end(), [](const value_type& a_lhs, const value_type& a_rhs) -> bool
		//{
		//	return a_lhs.text < a_rhs.text;
		//});
		// i think id rather have the effects chronologically than alphabetically

		CLIK::Array	 arr(uiMovie);
		CLIK::Object elem;
		for (auto& selected : _selectedMappings) {
			elem = selected.text;
			arr.Push(elem);
		}

		_selected.DataProvider(arr);
		//_craft.Disabled(_selectedMappings.empty());
		//_selectedIdx = _selectedMappings.size() - 1;
		//logger::info("%s", _selectedMappings[0].text);
		OnSelectedPress(_selectedMappings.size() - 1, false);

		return true;
	}


	bool SpellmakingMenu::OnSelectedPress(std::size_t a_selectedIdx, bool a_remove)
	{
		if (a_selectedIdx < _selectedMappings.size() && a_remove) {
			_selectedMappings.erase(_selectedMappings.begin() + a_selectedIdx);

			CLIK::Array	 arr(uiMovie);
			CLIK::Object elem;
			for (auto& selected : _selectedMappings) {
				elem = selected.text;
				arr.Push(elem);
			}

			_selected.DataProvider(arr);
			_selected.SelectedIndex(-1);
			_selectedIdx = kInvalid;
		} else {
			CommitSelection();
			_selectedIdx = a_selectedIdx;
		}

		SetEffectInfo();
		_craft.Disabled(_selectedMappings.empty());

		return true;
	}


	bool SpellmakingMenu::OnMagnitudeDragChange(bool a_isDragging)
	{
		_magnitude.SetDragging(a_isDragging);
		return true;
	}


	bool SpellmakingMenu::OnMagnitudeChange()
	{
		_magnitude.UpdateText();
		return true;
	}


	bool SpellmakingMenu::OnDurationDragChange(bool a_isDragging)
	{
		_duration.SetDragging(a_isDragging);
		return true;
	}


	bool SpellmakingMenu::OnDurationChange()
	{
		_duration.UpdateText();
		return true;
	}


	bool SpellmakingMenu::OnAreaDragChange(bool a_isDragging)
	{
		_area.SetDragging(a_isDragging);
		return true;
	}


	bool SpellmakingMenu::OnAreaChange()
	{
		_area.UpdateText();
		return true;
	}


	void SpellmakingMenu::SanitizeString(std::string& a_str)
	{
		while (!a_str.empty() && std::isspace(a_str.back())) {
			a_str.pop_back();
		}
		while (!a_str.empty() && std::isspace(a_str.front())) {
			a_str.assign(a_str, 1);
		}
	}

	void SpellmakingMenu::_openMenu(RE::StaticFunctionTag*)
	{
		Open();
	}
	void SpellmakingMenu::_closeMenu(RE::StaticFunctionTag*)
	{
		Close();
	}

	bool SpellmakingMenu::RegisterFuncs(RE::BSScript::IVirtualMachine* a_vm)
	{
		a_vm->RegisterFunction("OpenSpellmaking", "SpellMaking", Scaleform::SpellmakingMenu::_openMenu);
		a_vm->RegisterFunction("CloseSpellmaking", "SpellMaking", Scaleform::SpellmakingMenu::_closeMenu);
		return true;
	}
}
