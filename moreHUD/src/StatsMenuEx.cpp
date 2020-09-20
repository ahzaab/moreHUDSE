#include "PCH.h"


#include "StatsMenuEx.h"
#include "Offsets.h"

#include <queue>
#include <unordered_set>


#include "CLIK/Array.h"
#include "Scaleform.h"
#include "SKSE/API.h"

namespace Scaleform
{
    namespace
    {
        AttributeMap::AttributeMap() :
            Base()
        {
            insert({ STRENGTH, RE::ActorValue::kFavorActive });
            insert({ INTELLIGENCE, RE::ActorValue::kFavorsPerDayTimer });
            insert({ WILLPOWER, RE::ActorValue::kWaitingForPlayer });
            insert({ ENDURANCE, RE::ActorValue::kFavorsPerDay });
            insert({ AGILITY, RE::ActorValue::kLastBribedIntimidated });
            insert({ SPEED, RE::ActorValue::kLastFlattered });
            insert({ PERSONALITY, RE::ActorValue::kFame });
            insert({ LUCK, RE::ActorValue::kInfamy });
        }

        FakeSkillMap::FakeSkillMap() :
            Base()
        {
            insert({ SkywindSkills::kEnchanting, { 0x001068CD, 0x0010F6F2 } });
            insert({ SkywindSkills::kMediumArmor, { 0x001068CF, 0x0010F6F1 } });
            insert({ SkywindSkills::kSmithing, { 0x001068CE, 0x0010F6EE } });
            insert({ SkywindSkills::kAlchemy, { 0x001068D7, 0x0010F6F3 } });
            insert({ SkywindSkills::kHandToHand, { 0x001068BE, 0x0010F6EF } });
            insert({ SkywindSkills::kUnarmorerd, { 0x001068D0, 0x0010F6F0 } });
        }


        AVToFormMap::AVToFormMap() :
            Base()
        {
            using AV = RE::ActorValue;

            insert({ AV::kOneHanded, kAVOneHanded });
            insert({ AV::kTwoHanded, kAVTwoHanded });
            insert({ AV::kArchery, kAVMarksman });
            insert({ AV::kBlock, kAVBlock });
            insert({ AV::kSmithing, kAVSmithing });
            insert({ AV::kHeavyArmor, kAVHeavyArmor });
            insert({ AV::kLightArmor, kAVLightArmor });
            insert({ AV::kPickpocket, kAVPickpocket });
            insert({ AV::kLockpicking, kAVLockpicking });
            insert({ AV::kSneak, kAVSneak });
            insert({ AV::kAlchemy, kAVAlchemy });
            insert({ AV::kSpeech, kAVSpeechCraft });
            insert({ AV::kAlteration, kAVAlteration });
            insert({ AV::kConjuration, kAVConjuration });
            insert({ AV::kDestruction, kAVDestruction });
            insert({ AV::kIllusion, kAVMysticism });
            insert({ AV::kRestoration, kAVRestoration });
            insert({ AV::kEnchanting, kAVEnchanting });
            insert({ AV::kMagickaRateMult, kAVMagickaRateMod });
            insert({ AV::kHealRateMult, kAVHealRatePowerMod });
            insert({ AV::kFavorActive, kAVFavorActive });
            insert({ AV::kFavorsPerDay, kAVFavorsPerDay });
            insert({ AV::kFavorsPerDayTimer, kAVFavorsPerDayTimer });
            insert({ AV::kWaitingForPlayer, kAVWaitingForPlayer });
            insert({ AV::kLastBribedIntimidated, kAVLastBribedIntimidated });
            insert({ AV::kLastFlattered, kAVLastFlattered });
            insert({ AV::kFame, kAVFame });
            insert({ AV::kInfamy, kAVInfamy });
            insert({ AV::kAlchemyModifier, kAVAlchemyMod });
            insert({ AV::kEnchantingModifier, kAVEnchantingMod });
            insert({ AV::kLightArmorModifier, kAVLightArmorMod });
            insert({ AV::kSneakingModifier, kAVSneakMod });
            insert({ AV::kHeavyArmorModifier, kAVHeavyArmorMod });
            insert({ AV::kSmithingModifier, kAVSmithingMod });
        }

        RootMap::RootMap() :
            Base()
        {
            using AV = RE::ActorValue;
            auto player = RE::PlayerCharacter::GetSingleton();

            std::decay_t<decltype(Root::text)> text;
            std::decay_t<decltype(Root::info)> info;

            text = CLASS;
            info.clear();
            push_back({ text, info, true });

            text = DIVIDER;
            info.clear();
            push_back({ text, info, true });

            text = COMBAT;
            info.clear();
            info.push_back({ kAVAxe, AV::kEnchanting, SkywindSkills::kAxe });
            info.push_back({ kAVBlock, AV::kBlock, SkywindSkills::kBlock });
            info.push_back({ kAVBlunt, AV::kTwoHanded, SkywindSkills::kBlunt });
            info.push_back({ kAVHeavyArmor, AV::kHeavyArmor, SkywindSkills::kHeavyArmor });
            info.push_back({ kAVLongBlade, AV::kOneHanded, SkywindSkills::kLongBlade });
            info.push_back({ kAVMediumArmor, AV::kHeavyArmorModifier, SkywindSkills::kMediumArmor });
            info.push_back({ kAVPolearm, AV::kSmithing, SkywindSkills::kPolearm });
            info.push_back({ kAVSmithingFake, AV::kSmithingModifier, SkywindSkills::kSmithing });
            push_back({ text, info, true });

            text = MAGIC;
            info.clear();
            info.push_back({ kAVAlchemyFake, AV::kAlchemyModifier, SkywindSkills::kAlchemy });
            info.push_back({ kAVAlteration, AV::kAlteration, SkywindSkills::kAlteration });
            info.push_back({ kAVConjuration, AV::kConjuration, SkywindSkills::kConjuration });
            info.push_back({ kAVDestruction, AV::kDestruction, SkywindSkills::kDestruction });
            info.push_back({ kAVEnchantingFake, AV::kEnchantingModifier, SkywindSkills::kEnchanting });
            info.push_back({ kAVIllusion, AV::kIllusion, SkywindSkills::kIllusion });
            info.push_back({ kAVMysticismFake, AV::kAlchemy, SkywindSkills::kMysticism });
            info.push_back({ kAVRestoration, AV::kRestoration, SkywindSkills::kRestoration });
            push_back({ text, info, true });

            text = STEALTH;
            info.clear();
            info.push_back({ kAVHandToHand, AV::kLightArmorModifier, SkywindSkills::kHandToHand });
            info.push_back({ kAVLightArmor, AV::kLightArmor, SkywindSkills::kLightArmor });
            info.push_back({ kAVMarksman, AV::kArchery, SkywindSkills::KMarksman });
            info.push_back({ kAVSecurity, AV::kLockpicking, SkywindSkills::kSecurity });
            info.push_back({ kAVShortBlade, AV::kPickpocket, SkywindSkills::kShortBlade });
            info.push_back({ kAVSneak, AV::kSneak, SkywindSkills::kSneak });
            info.push_back({ kAVSpeechCraft, AV::kSpeech, SkywindSkills::kSpeech });
            info.push_back({ kAVUnarmorerd, AV::kSneakingModifier, SkywindSkills::kUnarmorerd });
            push_back({ text, info, true });

            // TODO: Still have a long way to go for werewolfs
            text = WEREWOLF;
            info.clear();
            info.push_back({ kAVHealRatePowerMod, AV::kNone });
            push_back({ text, info, true });

            // TODO: Still have a long way to go for vampires
            text = VAMPIRE;
            info.clear();
            info.push_back({ kAVHealRatePowerMod, AV::kNone });
            push_back({ text, info, true });
        }


        void HeaderList::Visible(bool a_visible)
        {
            list.Visible(a_visible);
            header.Visible(a_visible);
        }


        void Description::Init()
        {
            text.WordWrap(true);
        }


        void Description::Visible(bool a_visible)
        {
            text.Visible(a_visible);
            requisites.Visible(a_visible);
            unlocks.Visible(a_visible);
            unlock.Visible(a_visible);
        }


        Stats::Stats() :
            _fakeSkillMap(),
            _state(State::kDefault),
            _factionIds{ 0x00761A, 0x00761B, 0x220001, 0x22000F, 0x220003, 0x220005, 0x220000, 0x220006, 0x220008, 0x220002, 0x22000A, 0x220009, 0x220019, 0x22001A }
        {}

        void Stats::Update()
        {
            auto player = RE::PlayerCharacter::GetSingleton();
            _levelMeterPercent = GetPercentToNextLevel();
            _healthMeter = GetPlayerMeterData(RE::ActorValue::kHealth);
            _magickaMeter = GetPlayerMeterData(RE::ActorValue::kMagicka);
            _staminaMeter = GetPlayerMeterData(RE::ActorValue::kStamina);
            _playerName = player->GetDisplayFullName();
            _playerLevel = player->GetLevel();
            _playerRace = player->GetRace()->GetFullName();
            UpdatePlayerAttributes();


            auto dataHandler = RE::TESDataHandler::GetSingleton();
            auto faction = dataHandler->LookupForm<RE::TESFaction>(k0CrimeFactionMorrowindFormId, ESM_PLUGIN_NAME);
            _bounty = faction ? player->GetCrimeGoldValue(faction) : 0;

            auto reputationGlob = dataHandler->LookupForm<RE::TESGlobal>(kfbmwReputationFormId, ESM_PLUGIN_NAME);
            _reputation = reputationGlob ? static_cast<int32_t>(round(reputationGlob->value)) : 0;
        };


        auto Stats::UpdatePlayerAttributes() -> void
        {
            _attributes.clear();

            for (auto const& [key, val] : _attributeMap) {
                _attributes.push_back(GetAttribute(key));
            }
        }

        auto Stats::GetPlayerClass() -> std::string const
        {
            auto        player = RE::PlayerCharacter::GetSingleton();
            std::string playerClass;

            auto var = GetScriptVariable(player, "fbmw_ClassMenuInformation", "PlayerClass");

            if (var.IsString()) {
                playerClass.assign(var.GetString());
            }

            return playerClass;
        }

        auto Stats::GetPlayerBirthSign() -> std::string const
        {
            auto        player = RE::PlayerCharacter::GetSingleton();
            std::string birthSign;
            auto        var = GetScriptVariable(player, "fbmw_ClassMenuInformation", "BirthSign");

            if (var.IsString()) {
                birthSign.append(var.GetString());
            }

            return birthSign;
        }

        auto Stats::GetPlayerSpecialization() -> std::string const
        {
            auto        player = RE::PlayerCharacter::GetSingleton();
            std::string specialization;
            auto        var = GetScriptVariable(player, "fbmw_ClassMenuInformation", "Specialization");

            if (var.IsString()) {
                specialization.append(var.GetString());
            }

            return specialization;
        }

        auto Stats::GetPlayerMajorSkills() -> std::vector<std::string_view> const
        {
            auto skills = std::vector<std::string_view>();

            auto player = RE::PlayerCharacter::GetSingleton();

            auto skillValue = 1;
            bool skillFound = true;
            do {
                std::string skillName("Skill");
                skillName += std::to_string(skillValue++);
                auto var = GetScriptVariable(player, "fbmw_ClassMenuInformation", skillName.c_str());

                if (var.IsString()) {
                    skills.push_back(var.GetString());
                } else {
                    skillFound = false;
                }

            } while (skillFound);
            return skills;
        }

        auto Stats::GetFactionRank(RE::Actor* a_actor, RE::TESFaction* a_faction, bool a_isPlayer) -> int32_t
        {
            using func_t = decltype(&Stats::GetFactionRank);
            REL::Relocation<func_t> func{ Offsets::StatsMenuEx::GetFactionRank };
            return func(a_actor, a_faction, a_isPlayer);
        }

        auto Stats::GetPlayerFactionsAndRank() -> std::vector<FactionInfo> const
        {
            std::vector<FactionInfo> factionInfo;

            auto player = RE::PlayerCharacter::GetSingleton();

            auto dataHandler = RE::TESDataHandler::GetSingleton();
            for (auto& factionId : _factionIds) {
                auto faction = dataHandler->LookupForm<RE::TESFaction>(factionId, ESM_PLUGIN_NAME);

                if (faction) {
                    auto factionName = faction->GetFullName();

                    if (faction->IsPlayerExpelled()) {
                        factionInfo.push_back({ factionName, "$Expelled" });
                    } else if (player->IsInFaction(faction)) {
                        auto        factionRank = GetFactionRank(player, faction, true);
                        std::string factionRankName("");
                        auto        rankIdx = 0;
                        if (factionRank > -1) {
                            for (auto& entry : faction->rankData) {
                                if (rankIdx == factionRank) {
                                    factionRankName = player->GetActorBase()->IsFemale() ? entry->femaleRankTitle.data() : entry->maleRankTitle.data();
                                    factionInfo.push_back({ factionName, factionRankName });
                                    break;
                                }
                                rankIdx++;
                            }
                        }
                    }
                }
            }

            return factionInfo;
        }


        auto Stats::GetPlayerMajorAttributes() -> std::vector<std::string_view> const
        {
            auto majorAttributes = std::vector<std::string_view>();

            auto player = RE::PlayerCharacter::GetSingleton();

            auto attributeValue = 1;
            bool attributeFound = true;
            do {
                std::string skillName("Attribute");
                skillName += std::to_string(attributeValue++);
                auto var = GetScriptVariable(player, "fbmw_ClassMenuInformation", skillName.c_str());

                if (var.IsString()) {
                    majorAttributes.push_back(var.GetString());
                } else {
                    attributeFound = false;
                }

            } while (attributeFound);
            return majorAttributes;
        }

        auto Stats::GetScriptVariable(RE::TESForm* a_form, const char* a_scriptName, const char* a_scriptVariable) -> RE::BSScript::Variable const
        {
            RE::BSScript::Variable var;
            if (a_form == nullptr) {
                return var;
            }

            auto variableName = std::string(a_scriptVariable);
            variableName.insert(0, "::");
            variableName.append("_var");
            auto                                      vm = RE::SkyrimVM::GetSingleton();
            auto                                      vmImpl = vm->impl;
            auto                                      handlePolicy = vmImpl.get()->GetObjectHandlePolicy();
            auto                                      handle = handlePolicy->GetHandleForObject(a_form->GetFormType(), a_form);
            RE::BSTSmartPointer<RE::BSScript::Object> result;
            vmImpl->FindBoundObject(handle, a_scriptName, result);

            if (!result.get()) {
                return var;
            }

            RE::BSTSmartPointer<RE::BSScript::ObjectTypeInfo> info;
            vmImpl->GetScriptObjectType(a_scriptName, info);
            auto iter = info->GetVariableIter();
            if (iter) {
                for (std::uint32_t i = 0; i < info->GetTotalNumVariables(); ++i) {
                    auto& prop = iter[i];
                    if (std::string(prop.name.c_str()) == variableName) {
                        vmImpl->GetVariableValue(result, i, var);
                        return var;
                    }
                }
            }

            return var;
        }


        auto Stats::GetPlayerMeterData(RE::ActorValue a_av) -> PlayerMeterData const
        {
            PlayerMeterData playerMeterData;

            auto player = RE::PlayerCharacter::GetSingleton();
            auto fCur = player->GetActorValue(a_av);
            auto fCurMax = player->GetPermanentActorValue(a_av);
            auto fBaseMax = player->GetBaseActorValue(a_av);


            auto iCur = static_cast<int32_t>(round(fCur));
            auto iCurMax = static_cast<int32_t>(round(fCurMax));
            auto iBaseMax = static_cast<int32_t>(round(fBaseMax));

            playerMeterData.currentValue = iCur;
            playerMeterData.maxValue = iCurMax;

            if (iCurMax > iBaseMax)
                playerMeterData.color = POS_COLOR;
            else if (iCurMax < iBaseMax)
                playerMeterData.color = NEG_COLOR;
            else
                playerMeterData.color = DEFAULT_COLOR;


            return playerMeterData;
        }

        auto Stats::GetSkillMeterData(RE::ActorValue a_av, SkywindSkills a_skill) -> SkillMeterData const
        {
            SkillMeterData skillMeterData;
            auto           player = RE::PlayerCharacter::GetSingleton();

            auto fCur = player->GetActorValue(a_av);
            auto fCurMax = player->GetPermanentActorValue(a_av);
            auto fBaseMax = player->GetBaseActorValue(a_av);


            auto iCur = static_cast<int32_t>(round(fCur));
            auto iCurMax = static_cast<int32_t>(round(fCurMax));
            auto iBaseMax = static_cast<int32_t>(round(fBaseMax));


            skillMeterData.currentValue = iCur;

            if (!FakeSkillMap::IsFake(a_skill)) {
                auto skill = player->skills->data->skills[static_cast<uint32_t>(a_skill)];
                skillMeterData.maxValue = static_cast<uint32_t>(round(skill.levelThreshold));
                skillMeterData.minValue = static_cast<uint32_t>(round(skill.xp));
                skillMeterData.meterPercent = static_cast<uint32_t>((skill.xp / skill.levelThreshold) * 100.0);
            } else {
                assert(_fakeSkillMap.find(a_skill) != _fakeSkillMap.end());

                auto dataHandler = RE::TESDataHandler::GetSingleton();
                auto xp = dataHandler->LookupForm<RE::TESGlobal>(_fakeSkillMap[a_skill].xp, ESM_PLUGIN_NAME);
                auto levelThreshold = dataHandler->LookupForm<RE::TESGlobal>(_fakeSkillMap[a_skill].levelThreshold, ESM_PLUGIN_NAME);

                if (xp && levelThreshold) {
                    skillMeterData.maxValue = static_cast<uint32_t>(levelThreshold->value);
                    skillMeterData.minValue = static_cast<uint32_t>(round(xp->value));
                    skillMeterData.meterPercent = std::fpclassify(levelThreshold->value) == FP_ZERO ? 0 : static_cast<uint32_t>((xp->value / levelThreshold->value) * 100.0);
                }
            }


            if (fCur > fCurMax)
                skillMeterData.color = POS_COLOR;
            else if (fCur < fCurMax)
                skillMeterData.color = NEG_COLOR;
            else
                skillMeterData.color = DEFAULT_COLOR;


            return skillMeterData;
        }

        void Stats::SetDefault()
        {
            _state = State::kDefault;
        }


        void Stats::SetVampire()
        {
            _state = State::kVampire;
        }


        void Stats::SetWerewolf()
        {
            _state = State::kWerewolf;
        }


        auto Stats::IsBeastMode() const -> bool
        {
            switch (_state) {
            case State::kVampire:
            case State::kWerewolf:
                return true;
            case State::kDefault:
            default:
                return false;
            }
        }

        auto Stats::IsVampire() const -> bool
        {
            switch (_state) {
            case State::kVampire:
                return true;
            case State::kWerewolf:
            case State::kDefault:
            default:
                return false;
            }
        }

        auto Stats::IsWereWolf() const -> bool
        {
            switch (_state) {
            case State::kWerewolf:
                return true;
            case State::kVampire:
            case State::kDefault:
            default:
                return false;
            }
        }


        auto Stats::GetPerkPoints() const -> uint32_t
        {
            using Object = RE::DEFAULT_OBJECT;

            std::size_t obj;
            switch (_state) {
            case State::kVampire:
                obj = Object::kVampireAvailablePerks;
                break;
            case State::kWerewolf:
                obj = Object::kWerewolfAvailablePerks;
                break;
            case State::kDefault:
            default:
            {
                auto player = RE::PlayerCharacter::GetSingleton();
                return player->perkCount;
            } break;
            }

            auto dobj = RE::BGSDefaultObjectManager::GetSingleton();
            auto global = dobj->GetObject<RE::TESGlobal>(obj);
            return global ? static_cast<uint32_t>(global->value) : 0;
        }


        void Stats::DecPerkPoints() const
        {
            using Object = RE::DEFAULT_OBJECT;

            std::size_t obj;
            switch (_state) {
            case State::kVampire:
                obj = Object::kVampireAvailablePerks;
                break;
            case State::kWerewolf:
                obj = Object::kWerewolfAvailablePerks;
                break;
            case State::kDefault:
            default:
            {
                auto player = RE::PlayerCharacter::GetSingleton();
                player->perkCount -= 1;
                return;
            } break;
            }

            auto dobj = RE::BGSDefaultObjectManager::GetSingleton();
            auto global = dobj->GetObject<RE::TESGlobal>(obj);
            if (global) {
                global->value -= 1;
            }
        }


        auto Stats::GetExperienceForLevel(uint16_t level) -> float_t const
        {
            // fXPLevelUpMult * level + fXPLevelUpBase

            float_t fXPLevelUpBase = 0.0f;
            float_t fXPLevelUpMult = 0.0f;

            auto settings = RE::GameSettingCollection::GetSingleton();
            assert(settings);
            auto levelUpBase = settings->GetSetting("fXPLevelUpBase");
            if (levelUpBase) {
                fXPLevelUpBase = static_cast<float_t>(levelUpBase->GetFloat());
            }
            auto levelUpMult = settings->GetSetting("fXPLevelUpMult");
            if (levelUpMult) {
                fXPLevelUpMult = static_cast<float_t>(levelUpMult->GetFloat());
            }
            auto player = RE::PlayerCharacter::GetSingleton();

            return fXPLevelUpBase + fXPLevelUpMult * static_cast<float_t>(level);
        }

        auto Stats::GetPlayerExperience() -> float_t
        {
            float_t playerXP = 0.0f;
            auto    player = RE::PlayerCharacter::GetSingleton();
            if (player->skills) {
                playerXP = static_cast<float_t>(player->skills->data->xp);
            }

            return playerXP;
        }

        auto Stats::GetPercentToNextLevel() -> float_t
        {
            auto player = RE::PlayerCharacter::GetSingleton();
            auto currentLevel = player->GetLevel();
            auto currentXp = GetPlayerExperience();
            auto currentLevelXp = GetExperienceForLevel(player->GetLevel());

            auto percentToNext = (currentXp / currentLevelXp) * 100.0f;

            return percentToNext;
        }


        auto Stats::GetAttribute(const char* name) -> Attribute const
        {
            Attribute _result;
            auto      player = RE::PlayerCharacter::GetSingleton();
            auto      fBase = player->GetActorValue(_attributeMap[name]);
            auto      fCur = player->GetPermanentActorValue(_attributeMap[name]);
            auto      base2 = player->GetBaseActorValue(_attributeMap[name]);

            auto base = static_cast<int32_t>(round(fBase));
            auto mod = static_cast<int32_t>(fBase - round(fCur));

            _result.name = std::string(name);
            _result.currentValue = base;
            _result.modValue = mod;

            _result.modColor = mod > 0 ? POS_COLOR : mod < 0 ? NEG_COLOR :
                                                               DEFAULT_COLOR;

            return _result;
        }


        auto Stats::BuildStatString(RE::ActorValue a_av) const -> std::string
        {
            auto player = RE::PlayerCharacter::GetSingleton();
            auto fBase = player->GetActorValue(a_av);
            auto fCur = player->GetPermanentActorValue(a_av);

            auto base = static_cast<int32_t>(fBase);
            auto mod = static_cast<int32_t>(fCur - fBase);

            std::string text;
            text = std::to_string(base);
            if (mod != 0) {
                text += " (";
                if (mod > 0) {
                    text += '+';
                } else {
                    text += '-';
                }
                text += std::to_string(std::abs(mod));
                text += ')';
            }

            return text;
        }
    }

    StatsMenuEx::StatsMenuEx() :

        _roots(),
        _trees(),
        _perks(),
        _ranks(),
        _desc(),
        _stats(),
        _rootMappings(),
        _treeMappings(),
        _perkMappings(),
        _rankMappings(),
        _requisiteMappings(),
        _unlockMappings(),
        _avMap()
    {
        using Context = RE::UserEvents::INPUT_CONTEXT_ID;
        using Flag = RE::IMenu::Flag;
        _thumbStickState = ThumbStickState::kNone;
        auto ini = RE::INISettingCollection::GetSingleton();
        auto fMenuKeyRepeatShort = ini->GetSetting("fMenuKeyRepeatShort:Interface");
        auto fMenuKeyRepeatLong = ini->GetSetting("fMenuKeyRepeatLong:Interface");
        auto fLThumbDeadzone = ini->GetSetting("fLThumbDeadzone:Controls");
        assert(fMenuKeyRepeatShort);
        assert(fMenuKeyRepeatLong);
        _fMenuKeyRepeatShort = fMenuKeyRepeatShort ? fMenuKeyRepeatShort->GetFloat() * 1000.0f : 100.0f;
        _fMenuKeyRepeatLong = fMenuKeyRepeatLong ? fMenuKeyRepeatLong->GetFloat() * 1000.0f : 500.0f;
        _fLThumbDeadzone = fLThumbDeadzone ? fLThumbDeadzone->GetFloat() : 0.1f;
        _thumbstickRepeat = std::chrono::steady_clock::now();


        menuFlags.set(Flag::kUpdateUsesCursor);
        auto loader = RE::BSScaleformManager::GetSingleton();
        auto success = loader->LoadMovieEx(this, SWF_NAME, [this](RE::GFxMovieDef* a_def) {
            using StateType = RE::GFxState::StateType;

            fxDelegate.reset(new RE::FxDelegate());
            fxDelegate->RegisterHandler(this);
            a_def->SetState(StateType::kExternalInterface, fxDelegate.get());
            fxDelegate->Release();

            a_def->SetState(
                RE::GFxState::StateType::kLog,
                RE::make_gptr<Logger<StatsMenuEx>>().get());
        });

        if (!success) {
            assert(false);
            logger::critical("StatsMenuEx did not have a view due to missing dependencies! Aborting process!\n");
            std::abort();
        }


        auto input = RE::BSInputDeviceManager::GetSingleton();
        if (input->IsGamepadEnabled()) {
            //Don't use the cursor if using the gamepad
            menuFlags.set(Flag::kPausesGame, Flag::kUsesMenuContext, Flag::kDisablePauseMenu, Flag::kCustomRendering);
            ;
        } else {
            //Matched the flags used by the Native StatsMenu. kCustomRendering is needed to allow the Tween Menu animation
            menuFlags.set(Flag::kPausesGame, Flag::kUsesCursor, Flag::kUsesMenuContext, Flag::kDisablePauseMenu, Flag::kCustomRendering);
        }

        inputContext = Context::kStats;
        depthPriority = 5;  // JournalMenu == 5

        auto mc = RE::MenuControls::GetSingleton();
        mc->RegisterHandler(this);
        input->AddEventSink(this);

        InitExtensions();
        uiMovie->SetVisible(false);
    }

    StatsMenuEx::~StatsMenuEx()
    {
        auto mc = RE::MenuControls::GetSingleton();
        mc->RemoveHandler(this);
        auto input = RE::BSInputDeviceManager::GetSingleton();
        input->RemoveEventSink(this);
    }

#pragma region Development Only

    // Default the Ext menu as active by default (Development only)
    bool StatsMenuEx::s_active = true;

    auto StatsMenuEx::SetActive(bool active) -> bool
    {
        auto ui = RE::UI::GetSingleton();
        if (ui->IsMenuOpen(Scaleform::StatsMenuEx::Name()) || ui->IsMenuOpen(RE::StatsMenu::MENU_NAME)) {
            return false;
        }

        StatsMenuEx::s_active = active;

        return true;
    }

    auto StatsMenuEx::GetActive() -> bool
    {
        return StatsMenuEx::s_active;
    }

#pragma endregion

    void StatsMenuEx::Accept(RE::FxDelegateHandler::CallbackProcessor* a_processor)
    {
        a_processor->Process("Log", Log);
        a_processor->Process("OnRootPress", OnRootPress);
        a_processor->Process("OnTreePress", OnTreePress);
        a_processor->Process("OnPerkPress", OnPerkPress);
        a_processor->Process("OnRankPress", OnRankPress);
        a_processor->Process("OnRequisitePress", OnRequisitePress);
        a_processor->Process("OnUnlockPress", OnUnlockPress);
        a_processor->Process("UnlockPerk", UnlockPerk);
        a_processor->Process("PlaySound", PlaySound);
        a_processor->Process("CloseMenu", CloseMenu);
    }

    auto StatsMenuEx::ProcessMessage(RE::UIMessage& a_message)
        -> Result
    {
        using Message = RE::UI_MESSAGE_TYPE;

        // Not used right now.  It will be for developerment
        using ProcessMessage_t = decltype(&RE::StatsMenu::ProcessMessage);
        REL::Relocation<ProcessMessage_t> _ProcessMessage(REL::ID(51638));


        switch (*a_message.type) {
        case Message::kShow:
        {
            OnMenuOpen();
            return Result::kHandled;
        }
        case Message::kHide:
            OnMenuClose();
            return Result::kHandled;
        default:
            return Base::ProcessMessage(a_message);
        }
    }

    void StatsMenuEx::SetPlayerInfo()
    {
        using func_t = decltype(&StatsMenuEx::SetPlayerInfo);
        REL::Relocation<func_t> func{ Offsets::StatsMenuEx::SetPlayerInfo };
        return func(this);
    }

    void StatsMenuEx::SetPerkCount(uint32_t a_perkCount)
    {
        RE::FxResponseArgs<1> response;
        RE::GFxValue          perkCount;
        perkCount.SetNumber(a_perkCount);
        response.Add(perkCount);
        RE::FxDelegate::Invoke(uiMovie.get(), "SetPerkCount", response);
    }

    void StatsMenuEx::SetPlayerInfoEx()
    {
        RE::FxResponseArgs<4> response;
        RE::GFxValue          playerClass;
        RE::GFxValue          playerBounty;
        RE::GFxValue          playerReputation;
        RE::GFxValue          attribute;
        RE::GFxValue          label;
        RE::GFxValue          base;
        RE::GFxValue          mod;
        RE::GFxValue          modColor;
        RE::GFxValue          attributes;
        uiMovie.get()->CreateArray(std::addressof(attributes));
        auto playerAttributes = _stats.GetPlayerAttributes();
        attributes.SetArraySize(static_cast<uint32_t>(playerAttributes.size()));
        uint32_t idx = 0;
        for (auto const& e : std::as_const(playerAttributes)) {
            uiMovie.get()->CreateObject(std::addressof(attribute));

            label.SetString(e.name);
            base.SetNumber(e.currentValue < 0 ? 0 : e.currentValue);
            mod.SetNumber(e.modValue);
            modColor.SetString(e.modColor);


            attribute.SetMember("label", label);
            attribute.SetMember("base", base);
            attribute.SetMember("mod", mod);
            attribute.SetMember("modColor", modColor);

            attributes.SetElement(idx++, attribute);
        }

        auto classValue = _stats.GetPlayerClass();
        playerClass.SetString(classValue);
        response.Add(playerClass);
        response.Add(attributes);

        playerBounty.SetNumber(_stats.GetPlayerBounty());
        response.Add(playerBounty);
        playerReputation.SetNumber(_stats.GetPlayerReputation());
        response.Add(playerReputation);


        RE::FxDelegate::Invoke(uiMovie.get(), "SetPlayerInfoEx", response);
    }

    void StatsMenuEx::SetSkillInfo(std::int64_t a_treeIdx)  //(std::string description, RE::ActorValue a_av)
    {
        RE::FxResponseArgs<6> response;
        RE::GFxValue          description;
        RE::GFxValue          color;
        RE::GFxValue          skillValue;
        RE::GFxValue          maxValue;
        RE::GFxValue          minValue;
        RE::GFxValue          meterPercent;

        if (a_treeIdx < 0 || _treeMappings.empty()) {
            return;
        }

        auto& treeElem = _treeMappings[a_treeIdx];

        description.SetString(treeElem.text);
        response.Add(description);

        auto meterData = _stats.GetSkillMeterData(treeElem.av, treeElem.skill);

        color.SetString(meterData.color.c_str());
        response.Add(color);

        skillValue.SetNumber(meterData.currentValue);
        response.Add(skillValue);

        maxValue.SetNumber(meterData.maxValue);
        response.Add(maxValue);

        minValue.SetNumber(meterData.minValue);
        response.Add(minValue);

        meterPercent.SetNumber(meterData.meterPercent);
        response.Add(meterPercent);

        RE::FxDelegate::Invoke(uiMovie.get(), "SetSkillInfo", response);
    }

    StatsMenuEx::PerkConfirmCallback::PerkConfirmCallback(
        StatsMenuEx* menu,
        size_t       rankIndex,
        size_t       perkIndex,
        size_t       treeIndex) :
        _menu(menu),
        _rankIndex(rankIndex),
        _perkIndex(perkIndex),
        _treeIndex(treeIndex)
    {
    }


    auto StatsMenuEx::ProcessEvent(RE::InputEvent* const* a_event, [[maybe_unused]] RE::BSTEventSource<RE::InputEvent*>* a_eventSource) -> RE::BSEventNotifyControl
    {
        if (!a_event) {
            return RE::BSEventNotifyControl::kContinue;
        }

        for (auto event = *a_event; event; event = event->next) {
            auto button = static_cast<RE::ButtonEvent*>(event);
            if (event->eventType != RE::INPUT_EVENT_TYPE::kDeviceConnect) {
                continue;
            }

            auto input = RE::BSInputDeviceManager::GetSingleton();
            auto ui = RE::UI::GetSingleton();
            auto infr = RE::InterfaceStrings::GetSingleton();
            if (input->IsGamepadEnabled()) {
                if (ui->IsMenuOpen(infr->cursorMenu)) {
                    auto uiQueue = RE::UIMessageQueue::GetSingleton();
                    uiQueue->AddMessage(infr->cursorMenu, RE::UI_MESSAGE_TYPE::kHide, 0);
                }
            } else {
                if (!ui->IsMenuOpen(infr->cursorMenu)) {
                    auto uiQueue = RE::UIMessageQueue::GetSingleton();
                    uiQueue->AddMessage(infr->cursorMenu, RE::UI_MESSAGE_TYPE::kShow, 0);
                }
            }
        }


        return RE::BSEventNotifyControl::kContinue;
    }

    auto StatsMenuEx::CanProcess(RE::InputEvent* a_event) -> bool
    {
        return (a_event && a_event->eventType == RE::INPUT_EVENT_TYPE::kThumbstick);
    }


    auto StatsMenuEx::ProcessThumbstick(RE::ThumbstickEvent* a_event) -> bool
    {
        if (a_event->IsLeft()) {
            // TODO: I feel like I am reinventing the wheel here :sigh:  I havent had a chance to see you the inventory menu does it.
            auto end = std::chrono::steady_clock::now();
            auto pressTrigger = false;
            switch (_thumbStickState) {
            default:
            case ThumbStickState::kNone:
            {
                if (abs(a_event->xValue) > _fLThumbDeadzone || abs(a_event->yValue) > _fLThumbDeadzone) {
                    _thumbstickRepeat = std::chrono::steady_clock::now();
                    _thumbStickState = ThumbStickState::kLongPress;
                    pressTrigger = true;
                }
            } break;
            case ThumbStickState::kLongPress:
            {
                if (std::chrono::duration_cast<std::chrono::milliseconds>(end - _thumbstickRepeat).count() > _fMenuKeyRepeatLong) {
                    _thumbstickRepeat = std::chrono::steady_clock::now();
                    _thumbStickState = ThumbStickState::kShortPress;
                    pressTrigger = true;
                }

            } break;
            case ThumbStickState::kShortPress:
            {
                if (std::chrono::duration_cast<std::chrono::milliseconds>(end - _thumbstickRepeat).count() > _fMenuKeyRepeatShort) {
                    _thumbstickRepeat = std::chrono::steady_clock::now();
                    pressTrigger = true;
                }

            } break;
            }


            if (pressTrigger) {
                RE::FxResponseArgs<2> response;
                RE::GFxValue          args;
                RE::GFxValue          afX;
                RE::GFxValue          afy;

                auto updown = (abs(a_event->xValue) < abs(a_event->yValue));

                //logger::trace("Press");
                if (updown && a_event->yValue > 0 && abs(a_event->yValue) > 0.0f) {
                    // up
                    afX.SetNumber(0);
                    afy.SetNumber(1);
                } else if (updown && a_event->yValue < 0 && abs(a_event->yValue) > 0.0f) {
                    // down
                    afX.SetNumber(0);
                    afy.SetNumber(-1);
                } else if (!updown && a_event->xValue > 0 && abs(a_event->xValue) > 0.0f) {
                    // Right
                    afX.SetNumber(1);
                    afy.SetNumber(0);
                } else if (!updown && a_event->xValue < 0 && abs(a_event->xValue) > 0.0f) {
                    // Left
                    afX.SetNumber(-1);
                    afy.SetNumber(0);
                } else {
                    afX.SetNumber(0);
                    afy.SetNumber(0);
                }

                response.Add(afX);
                response.Add(afy);

                RE::FxDelegate::Invoke(uiMovie.get(), "onLeftStickInput", response);
            }

            if (Equals(a_event->xValue, 0.0f) && Equals(a_event->yValue, 0.0f)) {
                _thumbStickState = ThumbStickState::kNone;
                _thumbstickRepeat = std::chrono::steady_clock::now();
            }
            return true;
        }

        return false;
    }

    auto StatsMenuEx::Equals(float_t A, float_t B) -> bool
    {
        auto diff = A - B;
        return (abs(diff) < std::numeric_limits<float_t>::epsilon());
    }


    void StatsMenuEx::PerkConfirmCallback::Run(Message message)
    {
        // 4 = OK, however commonlibSSE does not have this message type defined, so just casting it as an int for now
        if (static_cast<uint32_t>(message) != 4) {
            return;
        }

        RE::PlaySound("UISkillsPerkSelect2D");
        _menu->UnlockPerk(_rankIndex, _perkIndex, _treeIndex);
    }

    void StatsMenuEx::Open()
    {
        if (!StatsMenuEx::s_active) {
            return;
        }

        auto uiQueue = RE::UIMessageQueue::GetSingleton();
        uiQueue->AddMessage(Name(), RE::UI_MESSAGE_TYPE::kShow, 0);
        uiQueue->AddMessage(RE::FaderMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, 0);
        uiQueue->AddMessage(RE::StatsMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, 0);
    }


    void StatsMenuEx::Close()
    {
        if (!StatsMenuEx::s_active) {
            return;
        }

        auto uiQueue = RE::UIMessageQueue::GetSingleton();
        uiQueue->AddMessage(Name(), RE::UI_MESSAGE_TYPE::kHide, 0);
    }


    void StatsMenuEx::Register()
    {
        auto ui = RE::UI::GetSingleton();
        ui->Register(Name(), Create);

        logger::info("Registered {}", Name().data());
    }

    auto StatsMenuEx::Create() -> RE::IMenu*
    {
        return new StatsMenuEx();
    }

    void StatsMenuEx::Log(const RE::FxDelegateArgs& a_params)
    {
        assert(a_params.GetArgCount() == 1);
        assert(a_params[0].IsString());

        logger::trace("{}: {}", Name().data(), a_params[0].GetString());
    }

    void StatsMenuEx::PlaySound(const RE::FxDelegateArgs& a_params)
    {
        assert(a_params.GetArgCount() == 1);
        assert(a_params[0].IsString());
        RE::PlaySound(a_params[0].GetString());
    }

    void StatsMenuEx::CloseMenu([[maybe_unused]] const RE::FxDelegateArgs& a_params)
    {
        Scaleform::StatsMenuEx::Close();
    }

    void StatsMenuEx::OnRootPress(const RE::FxDelegateArgs& a_params)
    {
        //TestFunc(a_params);

        assert(a_params.GetArgCount() == 1);
        assert(a_params[0].IsNumber());

        auto menu = static_cast<StatsMenuEx*>(a_params.GetHandler());
        auto num = a_params[0].GetSInt();
        menu->OnRootPress(num);
    }

    void StatsMenuEx::DisplayConfirmation(std::int64_t a_rankIdx, std::int64_t a_perkIdx, std::int64_t a_treeIdx)
    {
        const auto factoryManager = RE::MessageDataFactoryManager::GetSingleton();
        const auto uiStrHolder = RE::InterfaceStrings::GetSingleton();
        const auto factory = factoryManager->GetCreator<RE::MessageBoxData>(uiStrHolder->messageBoxData);
        const auto messageBox = factory->Create();
        messageBox->unk4C = 4;
        messageBox->unk38 = 25;
        const auto gameSettings = RE::GameSettingCollection::GetSingleton();
        const auto sStatsPerKConfirm = gameSettings->GetSetting("sStatsPerkConfirm");
        const auto sOk = gameSettings->GetSetting("sOk");
        const auto sCancel = gameSettings->GetSetting("sCancel");
        messageBox->bodyText = sStatsPerKConfirm->GetString();
        messageBox->buttonText.push_back(sOk->GetString());
        messageBox->buttonText.push_back(sCancel->GetString());
        messageBox->callback = RE::BSTSmartPointer<PerkConfirmCallback>(new StatsMenuEx::PerkConfirmCallback(this, a_rankIdx, a_perkIdx, a_treeIdx));

        REL::Relocation<void(RE::MessageBoxData*)> QueueMessage{ REL::ID(51422) };
        QueueMessage(messageBox);
    }

    void StatsMenuEx::OnTreePress(const RE::FxDelegateArgs& a_params)
    {
        auto uiQueue = RE::UIMessageQueue::GetSingleton();
        auto menu = static_cast<StatsMenuEx*>(a_params.GetHandler());

        assert(a_params.GetArgCount() == 1);
        assert(a_params[0].IsNumber());

        //auto menu = static_cast<StatsMenuEx*>(a_params.GetHandler());
        auto num = a_params[0].GetSInt();
        menu->OnTreePress(num);
    }


    void StatsMenuEx::OnPerkPress(const RE::FxDelegateArgs& a_params)
    {
        assert(a_params.GetArgCount() == 2);
        assert(a_params[0].IsNumber());
        assert(a_params[1].IsNumber());

        auto menu = static_cast<StatsMenuEx*>(a_params.GetHandler());
        auto perkIndex = a_params[0].GetSInt();
        auto treeIndex = a_params[1].GetSInt();
        menu->OnPerkPress(perkIndex, treeIndex);
    }


    void StatsMenuEx::OnRankPress(const RE::FxDelegateArgs& a_params)
    {
        assert(a_params.GetArgCount() == 2);
        assert(a_params[0].IsNumber());
        assert(a_params[1].IsNumber());

        auto menu = static_cast<StatsMenuEx*>(a_params.GetHandler());
        auto rankIndex = a_params[0].GetSInt();
        auto treeIndex = a_params[1].GetSInt();
        menu->OnRankPress(rankIndex, treeIndex);
    }


    void StatsMenuEx::OnRequisitePress(const RE::FxDelegateArgs& a_params)
    {
        assert(a_params.GetArgCount() == 2);
        assert(a_params[0].IsNumber());
        assert(a_params[1].IsNumber());

        auto menu = static_cast<StatsMenuEx*>(a_params.GetHandler());
        auto requisiteIndex = a_params[0].GetSInt();
        auto treeIndex = a_params[1].GetSInt();
        menu->OnRequisitePress(requisiteIndex, treeIndex);
    }


    void StatsMenuEx::OnUnlockPress(const RE::FxDelegateArgs& a_params)
    {
        assert(a_params.GetArgCount() == 2);
        assert(a_params[0].IsNumber());
        assert(a_params[1].IsNumber());

        auto menu = static_cast<StatsMenuEx*>(a_params.GetHandler());
        auto unlockIndex = a_params[0].GetSInt();
        auto treeIndex = a_params[1].GetSInt();
        menu->OnUnlockPress(unlockIndex, treeIndex);
    }


    void StatsMenuEx::UnlockPerk(const RE::FxDelegateArgs& a_params)
    {
        assert(a_params.GetArgCount() == 3);
        assert(a_params[0].IsNumber());
        assert(a_params[1].IsNumber());
        assert(a_params[2].IsNumber());

        auto menu = static_cast<StatsMenuEx*>(a_params.GetHandler());
        auto rankIndex = a_params[0].GetSInt();
        auto perkIndex = a_params[1].GetSInt();
        auto treeIndex = a_params[2].GetSInt();

        menu->DisplayConfirmation(rankIndex, perkIndex, treeIndex);
    }

    void StatsMenuEx::OnMenuOpen()
    {
        auto bm = RE::UIBlurManager::GetSingleton();

        // set blur
        bm->IncrementBlurCount();

        auto uiQueue = RE::UIMessageQueue::GetSingleton();
        uiQueue->AddMessage(RE::FaderMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, 0);

        bool success;
        uiMovie->SetVisible(true);
        std::vector<std::pair<CLIK::Object*, std::string>> toGet;
        toGet.push_back(std::make_pair(&_roots, "Root_mc.List_mc"));
        toGet.push_back(std::make_pair(&_trees, "Trees_mc.List_mc"));
        toGet.push_back(std::make_pair(&_perks, "Perks_mc.List_mc"));
        toGet.push_back(std::make_pair(&_ranks, "Ranks_mc.List_mc"));
        toGet.push_back(std::make_pair(&_desc.text, "PerkInfo_mc.descText_mc"));
        toGet.push_back(std::make_pair(&_desc.requisites, "PerkInfo_mc.requisites"));
        toGet.push_back(std::make_pair(&_desc.unlocks, "PerkInfo_mc.unlocks"));
        toGet.push_back(std::make_pair(&_desc.unlock, "PerkInfo_mc.UnlockButton_mc.button"));
        toGet.push_back(std::make_pair(&_skillsInfo, "SkillInfo_mc"));
        toGet.push_back(std::make_pair(&_perkInfo, "PerkInfo_mc"));
        toGet.push_back(std::make_pair(&_perkText, "PerkInfo_mc.perkLabel"));


        RE::GFxValue var;
        for (auto& elem : toGet) {
            std::string root("StatsMenuEx_mc.");

            auto element = root + elem.second;
            success = uiMovie->GetVariable(&var, element.c_str());
            assert(success);
            *elem.first = var;
        }

        _desc.Visible(false);
        _skillsInfo.Visible(false);
        _perkInfo.Visible(false);

        _desc.Init();
        _stats.Update();
        SetPerkCount(_stats.GetPerkPoints());

        SetRoots();
        SetPlayerInfo();
        SetPlayerInfoEx();

        RE::PlaySound("UISkillsTransitionUp");
    }

    void StatsMenuEx::OnMenuClose()
    {
        auto bm = RE::UIBlurManager::GetSingleton();
        // Remove blur
        bm->DecrementBlurCount();

        auto ui = RE::UI::GetSingleton();
        if (ui->IsMenuOpen(RE::TweenMenu::MENU_NAME)) {
            // Start to close the animation
            TweenMenu::StartCloseMenu();

            // Close the Tween Menu, This function needs to be called to allow the animation to finish;
            TweenMenu::CloseMenu();
        }

        RE::PlaySound("UISkillsStopSD");

        // Make sure we are back on the Hud
        auto uiQueue = RE::UIMessageQueue::GetSingleton();
        uiQueue->AddMessage(RE::FaderMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, 0);
        uiQueue->AddMessage(RE::CursorMenu::MENU_NAME, RE::UI_MESSAGE_TYPE::kHide, 0);
    }


    void StatsMenuEx::InitExtensions()
    {
        RE::GFxValue boolean(true);
        bool         success;

        success = uiMovie->SetVariable("_global.gfxExtensions", boolean);
        assert(success);
        success = uiMovie->SetVariable("_global.noInvisibleAdvance", boolean);
        assert(success);
    }


    void StatsMenuEx::SetRoots()
    {
        CLIK::Array  arr(uiMovie);
        CLIK::Object elem;
        for (auto& root : _rootMappings) {
            if (!_stats.IsWereWolf() && root.text == WEREWOLF) {
                continue;
            }
            if (!_stats.IsVampire() && root.text == VAMPIRE) {
                continue;
            }
            elem = root.text;
            arr.Push(elem);
        }

        _roots.DataProvider(arr);
    }


    void StatsMenuEx::UnlockPerk(std::int64_t a_rankIdx, std::int64_t a_perkIdx, std::int64_t a_treeIdx)
    {
        assert(a_rankIdx < static_cast<int64_t>(_rankMappings.size()));
        assert(a_perkIdx < static_cast<int64_t>(_perkMappings.size()));
        assert(a_treeIdx < static_cast<int64_t>(_treeMappings.size()));

        _desc.unlock.Disabled(true);

        auto& rank = _rankMappings[a_rankIdx];
        auto  perk = RE::TESForm::LookupByID<RE::BGSPerk>(rank.perkID);
        if (!perk) {
            return;
        }

        auto player = RE::PlayerCharacter::GetSingleton();
        player->AddPerk(perk);
        _stats.DecPerkPoints();

        auto idToFind = _perkMappings[a_perkIdx].perkID;
        UpdatePerks(a_treeIdx);

        CLIK::Array arr(uiMovie);
        for (auto& mappedPerk : _perkMappings) {
            RE::GFxValue text;
            RE::GFxValue state;
            RE::GFxValue entry;
            this->uiMovie.get()->CreateObject(std::addressof(entry));
            text.SetString(mappedPerk.text);
            state.SetNumber(static_cast<uint32_t>(mappedPerk.availability));
            entry.SetMember("text", text);
            entry.SetMember("state", state);
            CLIK::Object str(entry);
            arr.Push(str);
        }

        _perks.DataProvider(arr);

        for (std::size_t i = 0; i < _perkMappings.size(); ++i) {
            if (_perkMappings[i].perkID == idToFind) {
                _perks.SelectedIndex(static_cast<double>(i));
                break;
            }
        }

        _stats.Update();
        SetPerkCount(_stats.GetPerkPoints());
    }

    void StatsMenuEx::SetClassAndFactions()
    {
        RE::FxResponseArgs<6> response;
        RE::GFxValue          playerClass;
        RE::GFxValue          playerBirthsign;
        RE::GFxValue          playerSpecialization;
        RE::GFxValue          playerMajorSkills;
        RE::GFxValue          playerFactions;
        RE::GFxValue          playerMajorAttributes;

        uiMovie.get()->CreateArray(std::addressof(playerMajorSkills));
        auto skills = _stats.GetPlayerMajorSkills();
        playerMajorSkills.SetArraySize(static_cast<uint32_t>(skills.size()));
        auto skillIdx = 0;
        for (auto const& e : std::as_const(skills)) {
            if (e.empty()) {
                continue;
            }
            RE::GFxValue skill;
            skill.SetString(e.data());
            playerMajorSkills.SetElement(skillIdx++, skill);
        }

        uiMovie.get()->CreateArray(std::addressof(playerMajorAttributes));
        auto majorAttributes = _stats.GetPlayerMajorAttributes();
        playerMajorAttributes.SetArraySize(static_cast<uint32_t>(majorAttributes.size()));
        auto majorAttributeIdx = 0;
        for (auto const& e : std::as_const(majorAttributes)) {
            if (e.empty()) {
                continue;
            }
            RE::GFxValue majorAttribute;
            majorAttribute.SetString(e.data());
            playerMajorAttributes.SetElement(majorAttributeIdx++, majorAttribute);
        }

        uiMovie.get()->CreateArray(std::addressof(playerFactions));
        auto factionAndRanks = _stats.GetPlayerFactionsAndRank();
        playerFactions.SetArraySize(static_cast<uint32_t>(factionAndRanks.size()));
        auto factionIdx = 0;
        for (auto const& e : std::as_const(factionAndRanks)) {
            RE::GFxValue factionAndRank;
            uiMovie.get()->CreateObject(std::addressof(factionAndRank));
            RE::GFxValue factionName;
            RE::GFxValue rankName;
            factionName.SetString(e.faction.c_str());
            rankName.SetString(e.rank.c_str());
            factionAndRank.SetMember("factionName", factionName);
            factionAndRank.SetMember("rankName", rankName);
            playerFactions.SetElement(factionIdx++, factionAndRank);
        }

        auto classValue = _stats.GetPlayerClass();
        playerClass.SetString(classValue);
        response.Add(playerClass);
        auto birthsignValue = _stats.GetPlayerBirthSign();
        playerBirthsign.SetString(birthsignValue);
        response.Add(playerBirthsign);
        response.Add(playerMajorSkills);
        response.Add(playerFactions);
        auto specializationValue = _stats.GetPlayerSpecialization();
        playerSpecialization.SetString(specializationValue);
        response.Add(playerSpecialization);
        response.Add(playerMajorAttributes);

        RE::FxDelegate::Invoke(uiMovie.get(), "SetClassAndFactions", response);
    }

    auto StatsMenuEx::OnRootPress(std::int64_t a_classIdx) -> bool
    {
        // Do nothing then the "class" page is selected
        if (a_classIdx == 0) {
            SetClassAndFactions();
            return false;
        }

        CLIK::Array arr(uiMovie);
        UpdateTrees(a_classIdx);

        if (!_treeMappings.empty()) {
            CLIK::Object str;
            for (auto& tree : _treeMappings) {
                str = tree.text;
                arr.Push(str);
            }
            _trees.Visible(true);
            InvalidatePerks();
            _trees.DataProvider(arr);
            _trees.SelectedIndex(-1);
            return true;
        }
        InvalidateTrees();
        return false;
    }


    auto StatsMenuEx::OnTreePress(std::int64_t a_treeIdx) -> bool
    {
        CLIK::Array arr(uiMovie);
        UpdatePerks(a_treeIdx);

        if (a_treeIdx < 0) {
            return false;
        }

        _skillsInfo.Visible(true);
        SetSkillInfo(a_treeIdx);

        if (!_perkMappings.empty()) {
            for (auto& perk : _perkMappings) {
                RE::GFxValue text;
                RE::GFxValue state;
                RE::GFxValue entry;
                this->uiMovie.get()->CreateObject(std::addressof(entry));
                text.SetString(perk.text);
                state.SetNumber(static_cast<uint32_t>(perk.availability));
                entry.SetMember("text", text);
                entry.SetMember("state", state);
                CLIK::Object str(entry);
                arr.Push(str);
            }
            _perks.Visible(true);
            InvalidateRanks();
            _perks.DataProvider(arr);
            _perks.SelectedIndex(-1);
            return true;
        }
        InvalidatePerks();
        return false;
    }


    auto StatsMenuEx::OnPerkPress(std::int64_t a_perkIdx, std::int64_t a_treeIdx) -> bool
    {
        CLIK::Array arr(uiMovie);
        UpdateRanks(a_perkIdx);

        if (a_treeIdx < 0) {
            return false;
        }

        if (a_perkIdx > -1 && a_perkIdx < static_cast<int64_t>(_perkMappings.size())) {
            _perkInfo.Visible(true);
            auto&       elem = _perkMappings[a_perkIdx];
            std::string perkText(elem.text);
            _perkText.Text(perkText);
            _perkText.Visible(true);
        } else {
            _perkText.Visible(false);
            _perkInfo.Visible(false);
        }

        if (!_rankMappings.empty()) {
            CLIK::Object str;
            for (auto& rank : _rankMappings) {
                str = rank.text;
                arr.Push(str);
            }
            InvalidateDesc();
            _ranks.DataProvider(arr);
            if (OnRankPress(0, a_treeIdx)) {
                _ranks.SelectedIndex(0);
            }
            return true;
        }
        InvalidateRanks();
        return false;
    }


    auto StatsMenuEx::OnRankPress(std::int64_t a_rankIdx, std::int64_t a_treeIdx) -> bool
    {
        if (a_rankIdx >= static_cast<int64_t>(_rankMappings.size()) || a_rankIdx < 0) {
            InvalidateDesc();
            return false;
        }

        auto&        elem = _rankMappings[a_rankIdx];
        auto         perk = RE::TESForm::LookupByID<RE::BGSPerk>(elem.perkID);
        RE::BSString desc;
        perk->GetDescription(desc, perk);

        _perkInfo.Visible(true);
        _desc.Visible(true);
        _desc.text.Text(desc);

        auto player = RE::PlayerCharacter::GetSingleton();
        bool disabled = _stats.GetPerkPoints() == 0 || player->HasPerk(perk) || !perk->perkConditions.IsTrue(player, player);


        _desc.unlock.Disabled(disabled);
        UpdateLeads(a_rankIdx, a_treeIdx);
        return true;
    }


    auto StatsMenuEx::OnRequisitePress(std::int64_t a_requisiteIdx, std::int64_t a_treeIdx) -> bool
    {
        return OnLeadPress(_requisiteMappings, a_requisiteIdx, a_treeIdx);
    }


    auto StatsMenuEx::OnUnlockPress(std::int64_t a_unlockIdx, std::int64_t a_treeIdx) -> bool
    {
        return OnLeadPress(_unlockMappings, a_unlockIdx, a_treeIdx);
    }


    auto StatsMenuEx::OnLeadPress(std::vector<TextPerkLevel>& a_lead, std::int64_t a_leadIdx, std::int64_t a_treeIdx) -> bool
    {
        if (a_leadIdx >= static_cast<int64_t>(a_lead.size()) || a_leadIdx < 0) {
            return false;
        }

        if (a_treeIdx < 0) {
            return false;
        }

        auto& lead = a_lead[a_leadIdx];
        for (std::size_t i = 0; i < _perkMappings.size(); ++i) {
            if (_perkMappings[i].perkID == lead.perkID) {
                _perks.SelectedIndex(static_cast<double>(i));
                UpdateRanks(i);
                CLIK::Array  arr(uiMovie);
                CLIK::Object str;
                for (auto& rank : _rankMappings) {
                    str = rank.text;
                    arr.Push(str);
                }
                _ranks.DataProvider(arr);
                _ranks.SelectedIndex(0);
                OnRankPress(0, a_treeIdx);
                break;
            }
        }

        return true;
    }


    void StatsMenuEx::UpdateTrees(std::int64_t a_classIdx)
    {
        // "Class" page selected
        if (a_classIdx == 0) {
            return;
        }

        using value_type = decltype(_treeMappings)::value_type;

        _treeMappings.clear();
        _skillsInfo.Visible(false);
        _stats.SetDefault();

        // nothing selected
        if (a_classIdx == -1) {
            return;
        }

        if (a_classIdx < static_cast<int64_t>(_rootMappings.size())) {
            auto& elem = _rootMappings[a_classIdx];
            auto  player = RE::PlayerCharacter::GetSingleton();
            for (auto& info : elem.info) {
                switch (info.id) {
                case kAVVampire:
                    _stats.SetVampire();
                    break;
                case kAVWerewolf:
                    _stats.SetWerewolf();
                    break;
                }

                auto avInfo = RE::TESForm::LookupByID<RE::ActorValueInfo>(info.id);
                if (avInfo && !avInfo->fullName.empty()) {
                    std::string name(avInfo->fullName);
                    SanitizeString(name);
                    _treeMappings.push_back({ std::move(name), info.id, info.skill, info.av });
                }
            }

            if (elem.sort) {
                std::sort(_treeMappings.begin(), _treeMappings.end(), [](const value_type& a_lhs, const value_type& a_rhs) -> bool {
                    return a_lhs.text < a_rhs.text;
                });
            }
        }

        _stats.Update();
        SetPerkCount(_stats.GetPerkPoints());
    }


    auto StatsMenuEx::GetAdditionalPerkRequirements(RE::BGSPerk* perk) -> std::vector<std::string>
    {
        std::vector<std::string> additionalRequirements;
        auto                     player = RE::PlayerCharacter::GetSingleton();
        auto                     current = perk->perkConditions.head;
        while (current) {
            auto                     compareValue = current->data.comparisonValue;
            auto                     flags = current->data.flags;
            RE::ConditionCheckParams condition(player, player);


            // Only deal with AV conditions
            if ((current->data.functionData.function == RE::FUNCTION_DATA::FunctionID::kGetActorValue ||
                    current->data.functionData.function == RE::FUNCTION_DATA::FunctionID::kGetBaseActorValue) &&
                (flags.opCode == OpCode::kGreaterThan || flags.opCode == OpCode::kGreaterThanOrEqualTo)) {
                // Get the Actor Value Info Form from the AV Value
                auto skillOrAttribute = std::find_if(_avMap.begin(), _avMap.end(), [current](const std::map<RE::ActorValue, ActorValueInfoID>::value_type& element) -> bool {
                    // When the Condition is checking against the AV's, this parameter is the AV value
                    auto param = reinterpret_cast<uint64_t>(current->data.functionData.params[0]);
                    return (static_cast<uint32_t>(element.first) == param);
                });


                // If the attribute is found
                if (skillOrAttribute != _avMap.end()) {
                    auto mgr = RE::BGSDefaultObjectManager::GetSingleton();
                    auto avInfo = mgr->LookupByID<RE::ActorValueInfo>(skillOrAttribute->second);
                    if (avInfo) {
                        auto    name = avInfo->GetFullName();
                        float_t compValue = 0.0f;
                        if (flags.global) {
                            compValue = compareValue.g->value;
                        } else {
                            compValue = compareValue.f;
                        }

                        auto compAsInt = static_cast<int32_t>(round(compValue));

                        std::string text = name;
                        if (compAsInt > 0) {
                            text += " (";
                            text += std::to_string(compAsInt);
                            text += ")";
                        }

                        additionalRequirements.push_back(text);
                    }
                }
            }
            current = current->next;
        }

        return additionalRequirements;
    }

    auto StatsMenuEx::GetAvailability(RE::BGSPerk* perk) -> PerkAvailability
    {
        auto player = RE::PlayerCharacter::GetSingleton();
        if (player->HasPerk(perk)) {
            return PerkAvailability::kKnown;
        }

        if (perk->perkConditions.IsTrue(player, player)) {
            return PerkAvailability::kAvailable;
        }

        return PerkAvailability::kUnAvailable;
    }

    void StatsMenuEx::UpdatePerks(std::int64_t a_treeIdx)
    {
        using value_type = decltype(_perkMappings)::value_type;

        _perkMappings.clear();
        CLIK::Array arr(uiMovie);

        //Clear lists when switching perk trees
        _desc.requisites.DataProvider(arr);
        _desc.unlocks.DataProvider(arr);
        _perkInfo.Visible(false);

        if (a_treeIdx < 0) {
            return;
        }

        if (a_treeIdx < static_cast<int64_t>(_treeMappings.size())) {
            auto& elem = _treeMappings[a_treeIdx];
            auto  av = RE::TESForm::LookupByID<RE::ActorValueInfo>(elem.avInfoID);
            auto  player = RE::PlayerCharacter::GetSingleton();
            BFSOnPerkTree(av, [&](RE::BGSSkillPerkTreeNode* a_node) -> bool {
                auto perk = a_node->perk;
                if (perk && !perk->fullName.empty()) {
                    std::string name(perk->fullName);
                    SanitizeString(name);
                    int32_t level = 0;
                    bool    incomplete = false;
                    if (!_stats.IsBeastMode()) {
                        for (auto rank = perk; rank; rank = rank->nextPerk) {
                            level = GetPerkLvlReq(rank).value_or(level);
                            if (!player->HasPerk(rank)) {
                                incomplete = true;
                                break;
                            }
                        }
                    }

                    if (!incomplete) {
                        level = value_type::kInvalid;
                    }

                    _perkMappings.push_back({ std::move(name), perk->formID, level, GetAvailability(perk) });
                }
                return true;
            });
        }


        std::stable_sort(_perkMappings.begin(), _perkMappings.end(), [](const value_type& a_lhs, const value_type& a_rhs) -> bool {
            return a_lhs.level < a_rhs.level;
        });

        std::stable_sort(_perkMappings.begin(), _perkMappings.end(), [](const value_type& a_lhs, const value_type& a_rhs) -> bool {
            return a_lhs.availability < a_rhs.availability;
        });
    }


    void StatsMenuEx::UpdateRanks(std::int64_t a_perkIdx)
    {
        _rankMappings.clear();

        if (a_perkIdx < 0) {
            return;
        }

        if (a_perkIdx < static_cast<int64_t>(_perkMappings.size())) {
            auto& elem = _perkMappings[a_perkIdx];
            auto  perk = RE::TESForm::LookupByID<RE::BGSPerk>(elem.perkID);

            std::size_t idx = 1;
            std::string name;
            uint32_t    reqRank = 0;
            while (perk) {
                name = std::to_string(idx++);

                if (!_stats.IsBeastMode()) {
                    reqRank = GetPerkLvlReq(perk).value_or(reqRank);
                    if (reqRank > 0) {
                        name += " (";
                        name += std::to_string(reqRank);
                        name += ')';
                    }
                }

                //logger::trace("{} Perk: {:#010x}", name, perk->formID);
                _rankMappings.push_back({ std::move(name), perk->formID });
                perk = perk->nextPerk;
            }
        }
    }


    void StatsMenuEx::UpdateLeads(std::int64_t a_rankIdx, std::int64_t a_treeIdx)
    {
        _requisiteMappings.clear();
        _unlockMappings.clear();

        if (a_treeIdx < 0) {
            return;
        }

        if (a_rankIdx < 0) {
            return;
        }

        // On The first rank, show all the reuired perks, skills, and attributes
        if (a_rankIdx == 0) {
            _desc.requisites.SelectedIndex(-1);
            _desc.unlocks.SelectedIndex(-1);
            auto perkIDToFind = _rankMappings[0].perkID;
            //logger::trace("{} PerkToFind :{:#010x}", _rankMappings[a_rankIdx].text, perkIDToFind);
            auto av = RE::TESForm::LookupByID<RE::ActorValueInfo>(_treeMappings[a_treeIdx].avInfoID);
            BFSOnPerkTree(av, [&](RE::BGSSkillPerkTreeNode* a_node) -> bool {
                if (a_node->perk && a_node->perk->formID == perkIDToFind) {
                    UpdateLead(_requisiteMappings, a_node->parents, _desc.requisites, a_node->perk);
                    UpdateLead(_unlockMappings, a_node->children, _desc.unlocks, nullptr);
                    return false;
                }
                return true;
            });
        } else {
            // For all ofther ranks, only show the required skills and attributes, since "Ranks" are not in the tree and do not unlock anything.
            // TODO: May have to check HasPerk condtion if certain perks need a rank

            _desc.requisites.SelectedIndex(-1);
            _desc.unlocks.SelectedIndex(-1);
            auto                                    parentPerkIDToFind = _rankMappings[0].perkID;
            RE::BGSPerk*                            rankPerk = nullptr;
            std::vector<RE::BGSPerk*>               rankRequiredPerks;
            auto                                    rankPerkIDToFind = _rankMappings[a_rankIdx].perkID;
            RE::BSTArray<RE::BGSSkillPerkTreeNode*> parentNodes;
            RE::BSTArray<RE::BGSSkillPerkTreeNode*> childNodes;

            auto av = RE::TESForm::LookupByID<RE::ActorValueInfo>(_treeMappings[a_treeIdx].avInfoID);
            BFSOnPerkTree(av, [&](RE::BGSSkillPerkTreeNode* a_node) -> bool {
                if (a_node->perk && a_node->perk->formID == parentPerkIDToFind) {
                    parentNodes = a_node->parents;
                    childNodes = a_node->children;
                    rankPerk = a_node->perk;
                    return false;
                }
                return true;
            });

            while (rankPerk) {
                if (rankPerk->GetFormID() == rankPerkIDToFind) {
                    break;
                } else {
                    rankRequiredPerks.push_back(rankPerk);
                }
                rankPerk = rankPerk->nextPerk;
            }

            CLIK::Array arr(uiMovie);
            _desc.requisites.DataProvider(arr);
            _desc.unlocks.DataProvider(arr);
            // I don't believe ranks can unlock anything because they do not have a perk tree, so only show requirements
            UpdateLead(_requisiteMappings, parentNodes, _desc.requisites, rankRequiredPerks, rankPerk);
        }
    }

    void StatsMenuEx::UpdateLead(std::vector<TextPerkLevel>& a_lead, RE::BSTArray<RE::BGSSkillPerkTreeNode*>& a_srcArr, CLIK::GFx::Controls::ScrollingList& a_headerList, std::vector<RE::BGSPerk*>& rankRequiredPerks, RE::BGSPerk* rankPerk)
    {
        using value_type = std::decay_t<decltype(a_lead)>::value_type;

        bool disabled = true;
        auto player = RE::PlayerCharacter::GetSingleton();

        for (auto& node : a_srcArr) {
            if (node->perk && !node->perk->fullName.empty()) {
                disabled = false;
                std::string name(node->perk->fullName);
                SanitizeString(name);

                int32_t level = value_type::kInvalid;
                if (!_stats.IsBeastMode() && !player->HasPerk(node->perk)) {
                    level = GetPerkLvlReq(node->perk).value_or(0);
                    if (level > 0) {
                        name += " (";
                        name += std::to_string(level);
                        name += ')';
                    }
                }

                a_lead.push_back({ std::move(name), node->perk->formID, level });
            }
        }

        for (auto& perk : rankRequiredPerks) {
            if (!perk->fullName.empty()) {
                disabled = false;
                std::string name(perk->fullName);
                SanitizeString(name);

                int32_t level = value_type::kInvalid;
                if (!_stats.IsBeastMode() && !player->HasPerk(perk)) {
                    level = GetPerkLvlReq(perk).value_or(0);
                    if (level > 0) {
                        name += " (";
                        name += std::to_string(level);
                        name += ')';
                    }
                }

                a_lead.push_back({ std::move(name), 0 /* Dont want to select prev rank perks this has little value */, level });
            }
        }


        if (rankPerk) {
            auto additional = GetAdditionalPerkRequirements(rankPerk);

            for (auto& element : additional) {
                a_lead.push_back({ std::move(element), static_cast<RE::FormID>(0), -1 });
            }
        }

        std::stable_sort(a_lead.begin(), a_lead.end(), [](const value_type& a_lhs, const value_type& a_rhs) -> bool {
            return a_lhs.text < a_rhs.text;
        });


        // Keep the parent nodes at the top
        std::stable_sort(a_lead.begin(), a_lead.end(), [](const value_type& a_lhs, [[maybe_unused]] const value_type& a_rhs) -> bool {
            return static_cast<uint32_t>(a_lhs.perkID) > static_cast<uint32_t>(a_rhs.perkID);
        });

        std::stable_sort(a_lead.begin(), a_lead.end(), [](const value_type& a_lhs, const value_type& a_rhs) -> bool {
            return a_lhs.level < a_rhs.level;
        });

        CLIK::Array  arr(uiMovie);
        CLIK::Object str;
        for (auto& elem : a_lead) {
            str = elem.text;
            arr.Push(str);
        }

        a_headerList.DataProvider(arr);
    }

    void StatsMenuEx::UpdateLead(std::vector<TextPerkLevel>& a_lead, RE::BSTArray<RE::BGSSkillPerkTreeNode*>& a_srcArr, CLIK::GFx::Controls::ScrollingList& a_headerList, RE::BGSPerk* parentPerk)
    {
        using value_type = std::decay_t<decltype(a_lead)>::value_type;

        bool disabled = true;
        auto player = RE::PlayerCharacter::GetSingleton();

        for (auto& node : a_srcArr) {
            if (node->perk && !node->perk->fullName.empty()) {
                disabled = false;
                std::string name(node->perk->fullName);
                SanitizeString(name);

                int32_t level = value_type::kInvalid;
                if (!_stats.IsBeastMode() && !player->HasPerk(node->perk)) {
                    level = GetPerkLvlReq(node->perk).value_or(0);
                    if (level > 0) {
                        name += " (";
                        name += std::to_string(level);
                        name += ')';
                    }
                }

                a_lead.push_back({ std::move(name), node->perk->formID, level });
            }
        }

        if (parentPerk) {
            //logger::trace("Additional for {:#010x}", parentPerk->GetFormID());
            auto additional = GetAdditionalPerkRequirements(parentPerk);

            for (auto& element : additional) {
                a_lead.push_back({ std::move(element), static_cast<RE::FormID>(0), -1 });
            }
        }

        std::stable_sort(a_lead.begin(), a_lead.end(), [](const value_type& a_lhs, const value_type& a_rhs) -> bool {
            return a_lhs.text < a_rhs.text;
        });

        std::stable_sort(a_lead.begin(), a_lead.end(), [](const value_type& a_lhs, const value_type& a_rhs) -> bool {
            return a_lhs.level < a_rhs.level;
        });


        CLIK::Array  arr(uiMovie);
        CLIK::Object str;
        for (auto& elem : a_lead) {
            str = elem.text;
            arr.Push(str);
        }

        a_headerList.DataProvider(arr);
    }


    void StatsMenuEx::InvalidateTrees()
    {
        CLIK::Array arr(uiMovie);
        _trees.DataProvider(arr);
        _skillsInfo.Visible(false);
        InvalidatePerks();
    }


    void StatsMenuEx::InvalidatePerks()
    {
        CLIK::Array arr(uiMovie);
        _desc.requisites.DataProvider(arr);
        _desc.unlocks.DataProvider(arr);
        _perks.DataProvider(arr);
        _perkInfo.Visible(false);
        InvalidateRanks();
    }


    void StatsMenuEx::InvalidateRanks()
    {
        CLIK::Array arr(uiMovie);
        _ranks.DataProvider(arr);
        InvalidateDesc();
    }


    void StatsMenuEx::InvalidateDesc()
    {
        _desc.Visible(false);
    }

    void StatsMenuEx::BFSOnPerkTree(RE::ActorValueInfo* a_av, std::function<bool(RE::BGSSkillPerkTreeNode*)> a_predicate)
    {
        if ((a_av == nullptr) || (a_av->perkTree == nullptr)) {
            return;
        }

        std::unordered_set<uint32_t>          visited;
        std::queue<RE::BGSSkillPerkTreeNode*> q;
        auto                                  node = a_av->perkTree;
        q.push(node);
        visited.insert(node->index);
        do {
            node = q.front();
            q.pop();
            if (!a_predicate(node)) {
                break;
            }
            for (auto& child : node->children) {
                auto result = visited.insert(child->index);
                if (result.second) {
                    q.push(child);
                }
            }
        } while (!q.empty());
    }


    auto StatsMenuEx::GetPerkLvlReq(RE::BGSPerk* a_perk) -> std::optional<uint32_t>
    {
        using FunctionID = RE::FUNCTION_DATA::FunctionID;

        for (auto cond = a_perk->perkConditions.head; cond; cond = cond->next) {
            if (cond->data.functionData.function == FunctionID::kGetBaseActorValue) {
                return std::make_optional(static_cast<uint32_t>(cond->data.comparisonValue.f));
            }
        }
        return std::nullopt;
    }


    void StatsMenuEx::SanitizeString(std::string& a_str)
    {
        while (!a_str.empty() && (std::isspace(a_str.back()) != 0)) {
            a_str.pop_back();
        }
        while (!a_str.empty() && (std::isspace(a_str.front()) != 0)) {
            a_str.assign(a_str, 1);
        }
    }

}
