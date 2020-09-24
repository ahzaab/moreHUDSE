#include "PCH.h"
#include "Patches.h"
#include "AHZScaleformHook.h"

#include "REL/Relocation.h"
#include "SKSE/API.h"
#include "SKSE/Trampoline.h"


namespace Patches
{
	void Install()
	{
        AHZEnemyHealthUpdateHook::Install();
		logger::info("Installed all patches");
	}
}
