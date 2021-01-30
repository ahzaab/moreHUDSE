#pragma once

namespace RE
{
    namespace BSScript
    {
        class IForEachScriptObjectFunctor
        {
        public:
            struct SCRIPT_OBJECT_MESSAGE
            {
                uint32_t        unk00;  // 00
                uint32_t        unk04;  // 04
                ObjectTypeInfo* typeInfo;   // 08
                uint64_t           unk0C;  // 0C
                uint32_t        unk14;  // 14  
                uint32_t        unk18;  // 18
                RE::VMHandle    handle; //20
            };
            static_assert(sizeof(SCRIPT_OBJECT_MESSAGE) == 0x28);
            
            IForEachScriptObjectFunctor() = default;
            virtual ~IForEachScriptObjectFunctor() = default;

            // return true to continue
            virtual bool Visit(SCRIPT_OBJECT_MESSAGE* a_message, [[maybe_unused]] void* a_unk1) = 0;
        };
    }
}
