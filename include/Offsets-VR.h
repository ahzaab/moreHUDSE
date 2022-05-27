#pragma once

#include "REL\Relocation.h"

namespace moreHUDSE
{
    namespace Offsets
    {
        constexpr REL::Offset IsSentient{0x0060F1D0};
        constexpr REL::Offset GetSoulType{0x003D0FC0};
        constexpr REL::Offset GetMagicItemDescription{0x008C0940};
        constexpr REL::Offset EnemyUpdateHookBase{0x008AFE70};
        constexpr REL::Offset WandUpdateHookBase{0x0053EC60};
    }
}
