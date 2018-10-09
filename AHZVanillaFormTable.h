#pragma once
#include "AHZFormLookup.h"
#include <vector>
#include <map>
#include <tuple>

using namespace std;

class CAHZVanillaFormTable
{
public:

   typedef std::tuple<string, UInt32> AHZFormEntry;
	typedef std::vector<string> VMScriptEntries;
	typedef std::vector<AHZFormEntry> AHZLookupMap;

   static AHZLookupMap CreateACTIBaseMap()
   {
      AHZLookupMap m;
      m.push_back(AHZFormEntry("HearthFires.esm", 0x020110C3));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x020110C7));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x020110BF));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x0201045E));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x02010461));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x020110B9));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x020110BC));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x02010462));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x02010463));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x020110C2));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x02010464));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x02010465));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x0201045F));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x020110C5));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x02010466));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x02010467));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x02010468));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x02010460));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x020110BE));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x020110C6));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x02010469));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x020110C8));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x020110BA));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x020110C0));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x020110C1));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x020110C9));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x020110CA));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x020110CB));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x020110BD));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x020110CC));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x020110BB));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x020110CD));
      m.push_back(AHZFormEntry("HearthFires.esm", 0x020110C4));
      return m;
   }
  
   static AHZLookupMap CreateACTITargetMap()
   {
      AHZLookupMap m;
      m.push_back(AHZFormEntry("Skyrim.esm", 0x0004DA20));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x0004DA25));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x00077E1C));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x00064B3F));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x0006ABCB));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x00064B40));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x000B2183));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x000516C8));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x000889A2));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x0004DA00));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x0007E8C1));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x0007EE01));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x0010D666));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x0004DA23));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x0006AC4A));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x0005076E));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x00045C28));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x000669A5));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x000EC870));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x0004DA24));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x0002F44C));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x00059B86));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x00064B41));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x00077E1E));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x00077E1D));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x0006F950));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x0001B3BD));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x00083E64));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x0007E8B7));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x000134AA));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x0003F7F8));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x0004B0BA));
      m.push_back(AHZFormEntry("Skyrim.esm", 0x0004DA22));
      return m;
   }

   static VMScriptEntries CreateVMVariables()
   {
      VMScriptEntries v;
      v.push_back("lootable");
      v.push_back("Nirnroot");
      v.push_back("TempleBlessing");
      v.push_back("nonIngredientLootable");
      v.push_back("myIngredient");
      v.push_back("myFood");
      return v;
   }

   static void LoadACTIForms(CAHZFormLookup& loader)
   {
      _MESSAGE("Loading Vanilla ACTI Forms...");
      const AHZLookupMap AHZACTIForms = CreateACTIBaseMap();
      const AHZLookupMap AHZACTITargetForms = CreateACTITargetMap();

		// They must be of equal size
      ASSERT((AHZACTIForms.size() == AHZACTITargetForms.size()));

		AHZLookupMap::const_iterator itEnd = AHZACTIForms.cend();
		AHZLookupMap::const_iterator itSource = AHZACTIForms.cbegin();
		AHZLookupMap::const_iterator itTarget = AHZACTITargetForms.cbegin();
		while (itSource != itEnd) {
			loader.AddFormID(get<0>(*itSource), get<1>(*itSource), get<0>(*itTarget), get<1>(*itTarget));
			itSource++;
			itTarget++;
		}
	};

	static void LoadVMVariables(CAHZFormLookup& loader)
	{
		_MESSAGE("Loading Vanilla VM Script Variables...");
      const VMScriptEntries AHZVMVariables = CreateVMVariables();

		VMScriptEntries::const_iterator itEnd = AHZVMVariables.cend();
		VMScriptEntries::const_iterator it = AHZVMVariables.cbegin();
		while (it != itEnd) {
			loader.AddScriptVarable(*it);
			it++;
		}
	};

   private:
      CAHZVanillaFormTable() {};
      ~CAHZVanillaFormTable() {};
};
