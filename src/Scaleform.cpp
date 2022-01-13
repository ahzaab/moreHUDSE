#include "PCH.h"
#include "Scaleform.h"
#include "AHZScaleform.h"
#include "SKSE/API.h"
#include "AHZPapyrusMoreHud.h"
#include "HashUtil.h"

namespace Scaleform
{
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
            CAHZScaleform::ProcessTargetObject(CAHZPlayerInfo::GetTargetRef(), a_params);
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
            CAHZScaleform::ProcessTargetEffects(CAHZPlayerInfo::GetTargetRef(), a_params);
        }
    };

    class SKSEScaleform_GetIsBookAndWasRead : public RE::GFxFunctionHandler
    {
    public:
        void Call(Params& a_params) override
        {
            CAHZScaleform::ProcessIsBookAndWasRead(CAHZPlayerInfo::GetTargetRef(), a_params);
        }
    };

    class SKSEScaleform_GetArmorWeightClassString : public RE::GFxFunctionHandler
    {
    public:
        void Call(Params& a_params) override
        {
            CAHZScaleform::ProcessArmorClass(CAHZPlayerInfo::GetTargetRef(), a_params);
        }
    };

    class SKSEScaleform_GetValueToWeightString : public RE::GFxFunctionHandler
    {
    public:
        void Call(Params& a_params) override
        {
            CAHZScaleform::ProcessValueToWeight(CAHZPlayerInfo::GetTargetRef(), a_params);
        }
    };

    class SKSEScaleform_GetTargetWarmthRating : public RE::GFxFunctionHandler
    {
    public:
        void Call(Params& a_params) override
        {
            CAHZScaleform::ProcessArmorWarmthRating(CAHZPlayerInfo::GetTargetRef(), a_params);
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
            CAHZScaleform::ProcessBookSkill(CAHZPlayerInfo::GetTargetRef(), a_params);
        }
    };

    class SKSEScaleform_GetIsValidTarget : public RE::GFxFunctionHandler
    {
    public:
        void Call(Params& a_params) override
        {
            CAHZScaleform::ProcessValidTarget(CAHZPlayerInfo::GetTargetRef(), a_params);
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
            CAHZScaleform::ProcessIsKnownEnchantment(CAHZPlayerInfo::GetTargetRef(), a_params);
        }
    };

    class SKSEScaleform_IsTargetInFormList : public RE::GFxFunctionHandler
    {
    public:
        void Call(Params& a_params) override
        {
            CAHZScaleform::ProcessIsTargetInFormList(CAHZPlayerInfo::GetTargetRef(), a_params);
        }
    };

    class SKSEScaleform_IsTargetInIconList : public RE::GFxFunctionHandler
    {
    public:
        void Call(Params& a_params) override
        {
            CAHZScaleform::ProcessIsTargetInIconList(CAHZPlayerInfo::GetTargetRef(), a_params);
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
