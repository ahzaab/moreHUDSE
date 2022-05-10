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
                uint32_t        unk00;
                ObjectTypeInfo* typeInfo;
                void*           unk08;
                uint32_t        unk0C;
                uint32_t        unk10;
                RE::VMHandle    handle;
            };
            static_assert(sizeof(SCRIPT_OBJECT_MESSAGE) == 0x28);

            IForEachScriptObjectFunctor() = default;
            virtual ~IForEachScriptObjectFunctor() = default;

            // return true to continue
            virtual bool Visit(SCRIPT_OBJECT_MESSAGE* a_message, [[maybe_unused]] void* a_unk1) = 0;
        };
    }
}
