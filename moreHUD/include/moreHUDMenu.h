#include "RE\BSScript\ErrorLogger.h"
#include "RE\BSScript\IObjectProcessor\LinkerProcessor.h"
#include "RE\BSCore\BSCoreTypes.h"

namespace Scaleform
{
	class HudMenu
	{
		using VM = RE::BSScript::IVirtualMachine;

	public:
		void static SendNotification(RE::StaticFunctionTag*, RE::BSFixedString a_message, int32_t a_Level, float a_startPercent, float a_endPercent);
		bool static RegisterFuncs(VM* a_vm);
	};
}
