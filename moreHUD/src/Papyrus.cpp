#include "PCH.h"
#include "Papyrus.h"
#include "AHZPapyrusMoreHud.h"

namespace moreHUD
{
    bool Papyrus::Register()
    {
        auto papyrus = SKSE::GetPapyrusInterface();
        if (!papyrus->Register(PapyrusMoreHud::RegisterFunctions)) {
            return false;
        } else {
            logger::info("registered papyrus PapyrusMoreHud funcs");
        }

        return true;
    }
}
