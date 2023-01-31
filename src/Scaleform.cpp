#include "pch.h"
#include "Scaleform.h"
#include "AHZScaleform.h"
#include "SKSE/API.h"
#include "AHZPapyrusMoreHud.h"
#include "HashUtil.h"

namespace Scaleform
{
	struct moreHUDmessage {

		RE::FormID m_formID;
		bool m_icontype; // false = New, true = Found
		bool m_display;
	};

    moreHUDmessage s_messageFromCompletionist{};
    

    void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
    {
        if (a_msg->type != 1)
        {
            return;
        }

        if (!a_msg->data)
        {
            return;
        }

        s_messageFromCompletionist = *static_cast<moreHUDmessage*>(a_msg->data);
    }

    void RegisterMessageListener()
    {
        if (WinAPI::GetModuleHandle(L"Completionist"))
        {
            logger::info("Completionist is installed, registering listener"sv);        
            auto messageInterface = SKSE::GetMessagingInterface();
            messageInterface->RegisterListener("Completionist", MessageHandler); 
        }
    }

    
    class SKSEScaleform_InstallHooks : public RE::GFxFunctionHandler
    {
    public:
        void Call([[maybe_unused]] Params& a_params) override
        {
        }
    };

    class SKSEScaleform_GetTargetObjectData : public RE::GFxFunctionHandler
    {
    public:
        void Call(Params& a_params) override
        {
            const auto ref = CAHZTarget::Singleton().GetTarget();
            CAHZScaleform::ProcessTargetObject(ref, a_params);
        }
    };

    class SKSEScaleform_GetPlayerData : public RE::GFxFunctionHandler
    {
    public:
        void Call(Params& a_params) override
        {
            CAHZScaleform::ProcessPlayerData(a_params);
        }
    };

    class SKSEScaleform_GetIsPlayerInCombat : public RE::GFxFunctionHandler
    {
    public:
        void Call(Params& a_params) override
        {
            a_params.retVal->SetBoolean(CAHZPlayerInfo::GetIsInCombat());
        }
    };

    class SKSEScaleform_GetTargetEffects : public RE::GFxFunctionHandler
    {
    public:
        void Call(Params& a_params) override
        {
            const auto ref = CAHZTarget::Singleton().GetTarget();
            CAHZScaleform::ProcessTargetEffects(ref, a_params);
        }
    };

    class SKSEScaleform_GetIsBookAndWasRead : public RE::GFxFunctionHandler
    {
    public:
        void Call(Params& a_params) override
        {
            const auto ref = CAHZTarget::Singleton().GetTarget();

            // If the target is not valid or it can't be picked up by the player
            if (!ref.isValid) {
                a_params.retVal->SetBoolean(false);
                return;
            }
            a_params.retVal->SetBoolean(ref.bookRead);
        }
    };

    class SKSEScaleform_GetArmorWeightClassString : public RE::GFxFunctionHandler
    {
    public:
        void Call(Params& a_params) override
        {
            const auto ref = CAHZTarget::Singleton().GetTarget();
            CAHZScaleform::ProcessArmorClass(ref, a_params);
        }
    };

    class SKSEScaleform_GetValueToWeightString : public RE::GFxFunctionHandler
    {
    public:
        void Call(Params& a_params) override
        {
            const auto ref = CAHZTarget::Singleton().GetTarget();
            CAHZScaleform::ProcessValueToWeight(ref, a_params);
        }
    };

    class SKSEScaleform_GetTargetWarmthRating : public RE::GFxFunctionHandler
    {
    public:
        void Call(Params& a_params) override
        {
            const auto ref = CAHZTarget::Singleton().GetTarget();
            a_params.retVal->SetNumber(ref.armorWarmthRating);
        }
    };

    class SKSEScaleform_AHZLog : public RE::GFxFunctionHandler
    {
    public:
        void Call(Params& a_params) override
        {
            assert(a_params.argCount);
            logger::trace("{}", a_params.args[0].GetString());
        }
    };

    class SKSEScaleform_GetBookSkillString : public RE::GFxFunctionHandler
    {
    public:
        void Call(Params& a_params) override
        {
            const auto ref = CAHZTarget::Singleton().GetTarget();
            CAHZScaleform::ProcessBookSkill(ref, a_params);
        }
    };

    class SKSEScaleform_GetIsValidTarget : public RE::GFxFunctionHandler
    {
    public:
        void Call(Params& a_params) override
        {
            const auto ref = CAHZTarget::Singleton().GetTarget();
            CAHZScaleform::ProcessValidTarget(ref, a_params);
        }
    };

    class SKSEScaleform_GetEnemyInformation : public RE::GFxFunctionHandler
    {
    public:
        void Call(Params& a_params) override
        {
            CAHZScaleform::ProcessEnemyInformation(a_params);
        }
    };

    class SKSEScaleform_IsAKnownEnchantedItem : public RE::GFxFunctionHandler
    {
    public:
        void Call(Params& a_params) override
        {
            const auto ref = CAHZTarget::Singleton().GetTarget();

            // If the target is not valid or it can't be picked up by the player
            if (!ref.isValid) {
                a_params.retVal->SetNumber(0);
                return;
            }
            a_params.retVal->SetNumber(static_cast<uint32_t>(ref.enchantmentType));
        }
    };

    class SKSEScaleform_IsTargetInFormList : public RE::GFxFunctionHandler
    {
    public:
        void Call(Params& a_params) override
        {
            assert(a_params.args);
            assert(a_params.argCount);
            if (a_params.args[0].GetType() == RE::GFxValue::ValueType::kString) {
                const auto ref = CAHZTarget::Singleton().GetTarget();
                // If the target is not valid then say false
                if (!ref.isValid) {
                    a_params.retVal->SetBoolean(false);
                    return;
                }

                auto keyName = string(a_params.args[0].GetString());

                a_params.retVal->SetBoolean(PapyrusMoreHud::HasForm(keyName, ref.formId));
                return;
            }

            a_params.retVal->SetBoolean(false);
            return;
        }
    };

    class SKSEScaleform_GetFormIcons : public RE::GFxFunctionHandler
    {
    public:
        void Call(Params& a_params) override
        {
            a_params.movie->CreateArray(a_params.retVal);
            const auto ref = CAHZTarget::Singleton().GetTarget();
            // If the target is not valid then return an empty array
            if (!ref.isValid) {
                a_params.retVal->SetArraySize(0);
                return;
            }

            auto formId = ref.formId;
            auto customIcons = PapyrusMoreHud::GetFormIcons(formId);

            if (s_messageFromCompletionist.m_display && s_messageFromCompletionist.m_formID == formId)
            {
                customIcons.emplace_back(s_messageFromCompletionist.m_icontype ? "cmpFound"sv : "cmpNew"sv);
            }

            if (!customIcons.empty()) {
                RE::GFxValue entry;
                a_params.retVal->SetArraySize(static_cast<uint32_t>(customIcons.size()));
                auto idx = 0;
                for (auto& ci : customIcons) {
                    entry.SetString(ci);
                    a_params.retVal->SetElement(idx++, entry);
                }
            } else {
                a_params.retVal->SetArraySize(0);
            }
        }
    };

    class SKSEScaleform_IsTargetInIconList : public RE::GFxFunctionHandler
    {
    public:
        void Call(Params& a_params) override
        {
            assert(a_params.args);
            assert(a_params.argCount);
            if (a_params.args[0].GetType() == RE::GFxValue::ValueType::kString) {
                auto iconName = string(a_params.args[0].GetString());

                const auto ref = CAHZTarget::Singleton().GetTarget();
                // If the target is not valid then say false
                if (!ref.isValid) {
                    a_params.retVal->SetBoolean(false);
                    return;
                }

                auto name = ref.name;

                // Can't get the name for the crc
                if (name.empty()) {
                    a_params.retVal->SetBoolean(false);
                    return;
                }

                auto hash = static_cast<int32_t>(SKSE::HashUtil::CRC32(name.c_str(), ref.formId & 0x00FFFFFF));

                auto resultIconName = string(PapyrusMoreHud::GetIconName(hash));

                if (!resultIconName.length()) {
                    a_params.retVal->SetBoolean(false);
                    return;
                }

                a_params.retVal->SetBoolean(resultIconName == iconName);
                return;
            }
            a_params.retVal->SetBoolean(false);
        }
    };

    typedef std::map<const std::type_info*, RE::GFxFunctionHandler*> FunctionHandlerCache;
    static FunctionHandlerCache                                      g_functionHandlerCache;

    template <typename T>
    void RegisterFunction(RE::GFxValue* dst, RE::GFxMovieView* movie, const char* name)
    {
        // either allocate the object or retrieve an existing instance from the cache
        RE::GFxFunctionHandler* fn = nullptr;

        // check the cache
        FunctionHandlerCache::iterator iter = g_functionHandlerCache.find(&typeid(T));
        if (iter != g_functionHandlerCache.end())
            fn = iter->second;

        if (!fn) {
            // not found, allocate a new one
            fn = new T;

            // add it to the cache
            // cache now owns the object as far as refcounting goes
            g_functionHandlerCache[&typeid(T)] = fn;
        }

        // create the function object
        RE::GFxValue fnValue;
        movie->CreateFunction(&fnValue, fn);

        // register it
        dst->SetMember(name, fnValue);
    }

    auto RegisterScaleformFunctions(RE::GFxMovieView* a_view, RE::GFxValue* a_root) -> bool
    {
        RegisterFunction<SKSEScaleform_InstallHooks>(a_root, a_view, "InstallHooks");
        RegisterFunction<SKSEScaleform_GetTargetObjectData>(a_root, a_view, "GetTargetObjectData");
        RegisterFunction<SKSEScaleform_GetPlayerData>(a_root, a_view, "GetPlayerData");
        RegisterFunction<SKSEScaleform_GetIsValidTarget>(a_root, a_view, "GetIsValidTarget");
        RegisterFunction<SKSEScaleform_GetIsPlayerInCombat>(a_root, a_view, "GetIsPlayerInCombat");
        RegisterFunction<SKSEScaleform_GetTargetEffects>(a_root, a_view, "GetTargetEffects");
        RegisterFunction<SKSEScaleform_GetIsBookAndWasRead>(a_root, a_view, "GetIsBookAndWasRead");
        RegisterFunction<SKSEScaleform_GetArmorWeightClassString>(a_root, a_view, "GetArmorWeightClassString");
        RegisterFunction<SKSEScaleform_GetBookSkillString>(a_root, a_view, "GetBookSkillString");
        RegisterFunction<SKSEScaleform_GetValueToWeightString>(a_root, a_view, "GetValueToWeightString");
        RegisterFunction<SKSEScaleform_GetTargetWarmthRating>(a_root, a_view, "GetTargetWarmthRating");
        RegisterFunction<SKSEScaleform_GetEnemyInformation>(a_root, a_view, "GetEnemyInformation");
        RegisterFunction<SKSEScaleform_IsAKnownEnchantedItem>(a_root, a_view, "IsAKnownEnchantedItem");
        RegisterFunction<SKSEScaleform_IsTargetInFormList>(a_root, a_view, "IsTargetInFormList");
        RegisterFunction<SKSEScaleform_IsTargetInIconList>(a_root, a_view, "IsTargetInIconList");
        RegisterFunction<SKSEScaleform_GetFormIcons>(a_root, a_view, "GetFormIcons");
        RegisterFunction<SKSEScaleform_AHZLog>(a_root, a_view, "AHZLog");

        return true;
    }

    void RegisterCallbacks()
    {
        auto scaleform = SKSE::GetScaleformInterface();
        scaleform->Register(RegisterScaleformFunctions, "AHZmoreHUDPlugin");
        logger::info("Registered all scaleform callbacks");
    }

}
