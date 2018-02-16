#pragma once

#include <wchar.h> 
#include <stdlib.h>
#include <list>
#include <algorithm>
#include "skse64/GameReferences.h"
#include "skse64/GameObjects.h"
#include "skse64/GameData.h"
#include "skse64/GameRTTI.h"
#include "skse64/GameSettings.h"
#include "skse64/GameExtraData.h"
#include "skse64/ScaleformCallbacks.h"
#include "skse64/ScaleformMovie.h"
#include "skse64/PapyrusIngredient.h"
#include "skse64/PapyrusSpell.h"
#include "skse64/PapyrusArgs.h"
#include "skse64/PapyrusVM.h"
#include "AHZArmorInfo.h"
#include "AHZPlayerInfo.h"
#include "AHZWeaponInfo.h"
#include "IngredientLUT.h"
#include <iostream>
#include <fstream>
#include <regex>
#include "AHZConsole.h"
#include "string.h"
using namespace std;

class CAHZTargetFinder
{
public:
   static CAHZTargetFinder& Instance();
   TESForm * GetAttachedForm(TESObjectREFR *form);
private:
   CAHZTargetFinder();
   ~CAHZTargetFinder();
   static TESForm * GetAttachedForm(TESObjectREFR *form, string variableName);
   CAHZTargetFinder(CAHZTargetFinder const&);      // copy ctor is hidden
   CAHZTargetFinder& operator=(CAHZTargetFinder const&) {}; // assign op is hidden
   std::vector<string> m_vanillaItems;
};