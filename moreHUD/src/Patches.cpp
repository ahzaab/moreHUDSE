#include "PCH.h"
#include "Patches.h"

#include "REL/Relocation.h"
#include "SKSE/API.h"
#include "SKSE/Trampoline.h"


namespace Patches
{
	namespace
	{
		//void DisableLegendaryPrompt()
		//{
		//	{
		//		constexpr std::size_t RUN_START = 0x163;
		//		constexpr std::size_t RUN_END = 0x1C9;
		//		constexpr uint8_t		  NOP = 0x90;

		//		REL::Relocation<std::uintptr_t> funcBase = REL::ID(51654);
		//		for (auto i = RUN_START; i < RUN_END; ++i) {
		//			REL::safe_write(funcBase.address() + i, NOP);
		//		}
		//	}

		//	{
		//		constexpr std::size_t RUN_START = 0x115;
		//		constexpr std::size_t RUN_END = 0x35A;
		//		constexpr uint8_t	  NOP = 0x90;

		//		REL::Relocation<std::uintptr_t> funcBase = REL::ID(51647);
		//		for (auto i = RUN_START; i < RUN_END; ++i) {
		//			REL::safe_write(funcBase.address() + i, NOP);
		//		}
		//	}

		//	logger::info("Disabled legendary prompt");
		//}


		//void PatchEra()
		//{
		//	// Morrowind lives in the Third era 
		//	REL::Relocation<std::uintptr_t> target = REL::ID(259069);
		//	constexpr uint8_t			NEW_STR[] = ", 3E %d";
		//	for (std::size_t i = 0; i < std::extent<decltype(NEW_STR)>::value; ++i) {
		//		REL::safe_write(target.address() + i, NEW_STR[i]);
		//	}

		//	logger::info("Patched era");
		//}
	}


	void Install()
	{
		//DisableLegendaryPrompt();
		//PatchEra();
		logger::info("Installed all patches");
	}
}
