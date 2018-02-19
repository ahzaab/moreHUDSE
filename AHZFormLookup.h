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
#include <iostream>
#include <fstream>
#include <regex>
#include "AHZConsole.h"
#include "string.h"
using namespace std;

class CAHZACTIHelper
{
public:
	static CAHZACTIHelper& Instance();
	TESForm * GetTESForm(TESObjectREFR * targetReference);

	inline static TESObjectREFR * GetReference(TESForm * theForm)
	{
      if (theForm->GetFormType() == kFormType_Reference)
      {
         TESObjectREFR *reference = DYNAMIC_CAST(theForm, TESForm, TESObjectREFR);
         return reference;
      }
      else
      {
         return NULL;
      }
	}

private:
	CAHZACTIHelper();
	~CAHZACTIHelper();
	TESForm * GetAttachedForm(TESObjectREFR *form);
	static TESForm * GetAttachedForm(TESObjectREFR *form, string variableName);
	CAHZACTIHelper(CAHZACTIHelper const&);      // copy ctor is hidden
	CAHZACTIHelper& operator=(CAHZACTIHelper const&) {}; // assign op is hidden
	std::vector<string> m_vanillaItems;
};

#define AHZGetForm(x) (CAHZACTIHelper::Instance().GetTESForm((x)))
#define AHZGetReference(x) (CAHZACTIHelper::Instance().GetReference((x)))