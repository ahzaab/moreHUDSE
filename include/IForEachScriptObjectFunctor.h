#pragma once

namespace RE
{
    namespace BSScript
    {
        class IForEachScriptObjectFunctor
        {
        public:
            // This type is not fully decoded or correctly sized, just enough to use the functor
            struct SCRIPT_OBJECT_MESSSAGE
            {
                uint32_t        unk00;
                ObjectTypeInfo* typeInfo;
                void*           unk08;
                uint32_t        unk0C;
                uint32_t        unk10;
                RE::VMHandle    handle;
            };

            IForEachScriptObjectFunctor() = default;
            virtual ~IForEachScriptObjectFunctor() = default;

            // return true to continue
            virtual bool Visit(SCRIPT_OBJECT_MESSSAGE* a_message, [[maybe_unused]] void* arg2) = 0;
        };
    }
}
