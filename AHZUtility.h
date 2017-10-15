#pragma once
#include <wchar.h> 
#include <stdlib.h>
#include <list>
#include <algorithm>
#include "skse64/GameReferences.h"
#include "skse64/GameObjects.h"
#include "skse64/GameRTTI.h"
#include "skse64/GameSettings.h"
#include "skse64/GameExtraData.h"
#include "skse64/ScaleformCallbacks.h"
#include "skse64/ScaleformMovie.h"
#include "skse64/PapyrusIngredient.h"
#include "skse64/PapyrusSpell.h"
#include "AHZArmorInfo.h"
#include "AHZPlayerInfo.h"
#include "AHZWeaponInfo.h"
#include "IngredientLUT.h"
#include <iostream>
#include <fstream>
#include <regex>

using namespace std;
using namespace papyrusIngredient;

template<typename T> struct TypeHolder { };

#define AHZT(x) this->StringToWString(TypeHolder<T>(), (x))

float GetBaseDamage(TESAmmo* pthisAmmo);
double GetActualDamage(AHZWeaponData *weaponData);
double GetActualArmorRating(AHZArmorData* armorData);
double GetTotalActualArmorRating(void);
double GetArmorRatingDiff(TESObjectREFR *targetArmor);
double GetWeaponDamageDiff(TESObjectREFR *targetWeaponOrAmmo);
double GetTotalActualWeaponDamage(void);

bool IsTwoHanded(TESObjectWEAP * thisWeapon);
bool IsOneHanded(TESObjectWEAP * thisWeapon);
bool IsBow(TESObjectWEAP * thisWeapon);
bool IsCrossBow(TESObjectWEAP * thisWeapon);
bool isBolt(TESAmmo *thisAmmo);
double mRound(double d);
IngredientItem* GetIngredient(TESObjectREFR *thisObject);
AlchemyItem* GetFood(TESObjectREFR *thisObject);
//bool GetIsNthEffectKnown(IngredientItem* thisMagic, UInt32 index);
const char * GetTargetName(TESObjectREFR *thisObject);
bool CanPickUp(UInt32 formType);
bool IsEnglish();

//#define AHZT(x) this->StringToWString(TypeHolder<T>(), (x))
//typedef  int(__stdcall *GET_MAGIC_ITEM_DESCRIPTION)(TESForm *magicItem, BSString * temp);
//static GET_MAGIC_ITEM_DESCRIPTION GetMagicItemDescription = (GET_MAGIC_ITEM_DESCRIPTION)0x0086C3D0;



//0x00891320

typedef bool(*_IsSurvivalMode)();
extern RelocAddr<_IsSurvivalMode> IsSurvivalMode;

template <class T>
class CAHZUtility
{
public:
	
	 T GetTargetName(TESObjectREFR *thisObject);
	 bool GetIsBookAndWasRead(TESObjectREFR *theObject);
	 T GetArmorWeightClass(TESObjectREFR *theObject);
	 T GetEffectsDescription(TESObjectREFR *theObject);
	 T GetBookSkill(TESObjectREFR *theObject);
	 std::wstring StringToWString(TypeHolder<std::wstring>, const char* cstr);
	 std::string StringToWString(TypeHolder<std::string>, const char* cstr);
	
	 void ProcessTargetObject(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args);
	 void ProcessInventoryCount(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args);
	 void ProcessTargetEffects(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args);
	 void ProcessArmorClass(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args);
	 void ProcessBookSkill(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args);
	 bool ProcessValidTarget(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args);
	 void ProcessPlayerData(GFxFunctionHandler::Args *args);
	 void ProcessIngredientData(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args);

private:
	 void ReplaceStringInPlace(std::wstring& subject, const std::wstring& search,
                          const std::wstring& replace); 
	 void ReplaceStringInPlace(std::string& subject, const std::string& search,
						  const std::string& replace);

	 void RegisterTString(GFxValue * dst,  GFxMovieView * view, const char * name, const char * str);
	 void RegisterTString(GFxValue * dst,  GFxMovieView * view, const char * name, const wchar_t * str);
	 void RegisterNumber(GFxValue * dst, const char * name, double value);

	 void SetResultTString(GFxFunctionHandler::Args *args, const wchar_t * str);
	 void SetResultTString(GFxFunctionHandler::Args *args, const char * str);

	 void GetMagicItemDescription(MagicItem * item, std::string& description);
	 void GetMagicItemDescription(MagicItem * item, std::wstring& description);

	 void AppendDescription(TESDescription *desObj, TESForm *parent, std::wstring& description);
	 void AppendDescription(TESDescription *desObj, TESForm *parent, std::string& description);
};

template <class T>
T CAHZUtility<T>::GetTargetName(TESObjectREFR *thisObject)
{
	T name;
	TESFullName* pFullName = DYNAMIC_CAST(thisObject->baseForm, TESForm, TESFullName);
	const char* displayName = thisObject->extraData.GetDisplayName(thisObject->baseForm);
	
	// If the name can be created
	if (displayName)
	{
		name.append(AHZT(displayName));
	}
	// Use the base name
	else if (pFullName)
	{
		name.append(AHZT(pFullName->name.data));			
	}
	
	// If this is a soul gem, also get the gem size name
	if (thisObject->baseForm->formType == kFormType_SoulGem)
	{
		TESSoulGem *gem = DYNAMIC_CAST(thisObject->baseForm, TESForm, TESSoulGem);
		if (gem)
		{
			char * soulName = NULL;
			SettingCollectionMap	* settings = *g_gameSettingCollection;
			switch(gem->soulSize)
			{
				case 1: soulName = settings->Get("sSoulLevelNamePetty")->data.s; break;
				case 2: soulName = settings->Get("sSoulLevelNameLesser")->data.s; break;
				case 3: soulName = settings->Get("sSoulLevelNameCommon")->data.s; break;
				case 4: soulName = settings->Get("sSoulLevelNameGreater")->data.s; break;
				case 5: soulName = settings->Get("sSoulLevelNameGrand")->data.s; break;
				default: break;
			}
			
			if (soulName)
			{
				name.append(AHZT(" ("));
				name.append(AHZT(soulName));
				name.append(AHZT(")"));
			}
		}
	}

	return name;
};

template <class T>
std::wstring CAHZUtility<T>::StringToWString(TypeHolder<std::wstring>, const char* cstr)
{
	wchar_t tempStrBuffer[512] = L"\0";
	std::wstring tempStr;
	size_t sizeConverted;
	mbstowcs_s(
		&sizeConverted, 
		tempStrBuffer, 
		512, 
		cstr, 
		512-1);

	tempStr.append(tempStrBuffer);
	return tempStr;
};

template <class T>
std::string CAHZUtility<T>::StringToWString(TypeHolder<std::string>, const char* cstr)
{
	std::string tempStr;
	tempStr.append(cstr);
	return tempStr;
};

template <class T>
bool CAHZUtility<T>::GetIsBookAndWasRead(TESObjectREFR *theObject)
{
	if (!theObject)
		return false;
	
	if (theObject->baseForm->GetFormType() != kFormType_Book)
		return false;

	TESObjectBOOK *item = DYNAMIC_CAST(theObject->baseForm, TESForm, TESObjectBOOK);	
	if (item && ((item->data.flags & TESObjectBOOK::Data::kType_Read) == TESObjectBOOK::Data::kType_Read))
	{
		return true;	
	}
	else
	{
		return false;
	}
};

template <class T>
T CAHZUtility<T>::GetArmorWeightClass(TESObjectREFR *theObject)
{
	T desc;

	if (!theObject)
		return desc;
	
	if (theObject->baseForm->GetFormType() != kFormType_Armor)
		return desc;

	TESObjectARMO *item = DYNAMIC_CAST(theObject->baseForm, TESForm, TESObjectARMO);
	if (!item)
		return desc;

	ActorValueList * avList = ActorValueList::GetSingleton();	
	if (avList && item->bipedObject.data.weightClass <= 1)
	{	
		// Utilize the AV value to get the localized name for "Light Armor"
		if (item->bipedObject.data.weightClass == 0)
		{
			ActorValueInfo * info = avList->GetActorValue(12);
			if(info)
			{
				TESFullName *fname = DYNAMIC_CAST(info, ActorValueInfo, TESFullName);
				if (fname && fname->name.data)
				{
					desc.append(AHZT("<FONT FACE=\"$EverywhereMediumFont\"SIZE=\"15\"COLOR=\"#999999\"KERNING=\"0\">     "));
					desc.append(AHZT(fname->name.data));
					desc.append(AHZT("<\\FONT>"));
				}
			}
		}
		// Utilize the AV value to get the localized name for "Heavy Armor"
		else if (item->bipedObject.data.weightClass == 1)
		{
			ActorValueInfo * info = avList->GetActorValue(11);
			if(info)
			{
				TESFullName *fname = DYNAMIC_CAST(info, ActorValueInfo, TESFullName);
				if (fname && fname->name.data)
				{
					desc.append(AHZT("<FONT FACE=\"$EverywhereMediumFont\"SIZE=\"15\"COLOR=\"#999999\"KERNING=\"0\">     "));
					desc.append(AHZT(fname->name.data));
					desc.append(AHZT("<\\FONT>"));
				}
			}
		}
	}
	return desc;
};


template <class T>
T CAHZUtility<T>::GetBookSkill(TESObjectREFR *theObject)
{
	T desc;
	if (theObject->baseForm->GetFormType() == kFormType_Book)
	{
		TESObjectBOOK *item = DYNAMIC_CAST(theObject->baseForm, TESForm, TESObjectBOOK);
		
		if (!item)
			return desc;
		
		// If this is a spell book, then it is not a skill book
		if ((item->data.flags & TESObjectBOOK::Data::kType_Spell) == TESObjectBOOK::Data::kType_Spell)
			return desc;

		if (((item->data.flags & TESObjectBOOK::Data::kType_Skill) == TESObjectBOOK::Data::kType_Skill) && 
			item->data.teaches.skill)
		{
			ActorValueList * avList = ActorValueList::GetSingleton();
			if(avList)
			{
				ActorValueInfo * info = avList->GetActorValue(item->data.teaches.skill);
				if(info)
				{
					TESFullName *fname = DYNAMIC_CAST(info, ActorValueInfo, TESFullName);				
					if (fname && fname->name.data)
					{	
						desc.append(AHZT("<FONT FACE=\"$EverywhereMediumFont\"SIZE=\"15\"COLOR=\"#999999\"KERNING=\"0\">       "));
						desc.append(AHZT(fname->name.data));
						desc.append(AHZT("<\\FONT>"));	
					}
				}
			}
		}
	}
	return desc;
}

template <class T>
void CAHZUtility<T>::AppendDescription(TESDescription *desObj, TESForm *parent, std::wstring& description)
{
	BSString bsDescription;
	wstring tempString = L"";
	CALL_MEMBER_FN(desObj, Get)(&bsDescription, parent, 'DESC');
	
	if (&bsDescription)
	{
		tempString.append(AHZT(bsDescription.Get()));

		if (tempString != L"LOOKUP FAILED!")
		{
			description.append(tempString);
		}
	}
}

template <class T>
void CAHZUtility<T>::AppendDescription(TESDescription *desObj, TESForm *parent, std::string& description)
{
	BSString bsDescription;
	string tempString = "";
	CALL_MEMBER_FN(desObj, Get)(&bsDescription, parent, 'DESC');

	if (&bsDescription)
	{
		tempString.append(AHZT(bsDescription.Get()));

		if (tempString != "LOOKUP FAILED!")
		{
			description.append(tempString);
		}
	}
}

template <class T>
T CAHZUtility<T>::GetEffectsDescription(TESObjectREFR *theObject)
{
	BSString description;
	T effectDescription;
	T desc;
	T effectsString;
	MagicItem * magicItem = NULL;
	if (!theObject)
		return desc;
	
	tArray<MagicItem::EffectItem*> *effectList = NULL;
	SettingCollectionMap *settings = *g_gameSettingCollection;

	if (theObject->baseForm->GetFormType() == kFormType_Potion)
	{
		AlchemyItem *item = DYNAMIC_CAST(theObject->baseForm, TESForm, AlchemyItem);

		if (ExtraEnchantment* extraEnchant = static_cast<ExtraEnchantment*>(theObject->extraData.GetByType(kExtraData_Enchantment))) // Enchanted
		{
			if (extraEnchant->enchant)
			{
				GetMagicItemDescription(extraEnchant->enchant, effectDescription);
				desc.append(effectDescription);
			}
		}


		if (item)
		{
			GetMagicItemDescription(item,effectDescription);
			desc.append(effectDescription);
		}
	}
	else if (theObject->baseForm->GetFormType() == kFormType_Weapon)
	{
		TESObjectWEAP *item = DYNAMIC_CAST(theObject->baseForm, TESForm, TESObjectWEAP);

		//Get enchantment description
		if (item && item->enchantable.enchantment)
		{
			GetMagicItemDescription(item->enchantable.enchantment,effectDescription);
			desc.append(effectDescription);
		}
		// Items modified by the player
		else if(ExtraEnchantment* extraEnchant = static_cast<ExtraEnchantment*>(theObject->extraData.GetByType(kExtraData_Enchantment))) // Enchanted
		{
			if (extraEnchant->enchant)
			{
				GetMagicItemDescription(extraEnchant->enchant,effectDescription);
				desc.append(effectDescription);
			}
		}
		
		// If there was no effects, then display athe description if available
		if (item && !desc.length())
		{
			// Get the description if any (Mostly Dawnguard and Dragonborn stuff uses the descriptions)
			//CALL_MEMBER_FN(&item->description, Get)(&description, NULL, 1129530692);
			//CALL_MEMBER_FN(&item->description, Get)(&description, item, 'DESC');
			//desc.append(AHZT(description.Get()));
			AppendDescription(&item->description, item, desc);
		}
	}
	else if (theObject->baseForm->GetFormType() == kFormType_Armor)
	{
		TESObjectARMO *item = DYNAMIC_CAST(theObject->baseForm, TESForm, TESObjectARMO);
		
		//Get enchantment description
		if (item && item->enchantable.enchantment)
		{
			GetMagicItemDescription(item->enchantable.enchantment,effectDescription);
			desc.append(effectDescription);
		}
		// Items modified by the player
		else if(ExtraEnchantment* extraEnchant = static_cast<ExtraEnchantment*>(theObject->extraData.GetByType(kExtraData_Enchantment))) // Enchanted
		{
			if (extraEnchant->enchant)
			{
				GetMagicItemDescription(extraEnchant->enchant,effectDescription);
				desc.append(effectDescription);
			}
		}

		// If there was no effects, then display athe description if available
		if (item && !desc.length())
		{
			// Get the description if any (Mostly Dawnguard and Dragonborn stuff uses the descriptions)
			//CALL_MEMBER_FN(&item->description, Get)(&description, NULL, 1129530692);
			//CALL_MEMBER_FN(&item->description, Get)(&description, item, 'DESC');
			//desc.append(AHZT(description.Get()));
			AppendDescription(&item->description, item, desc);
		}
	}
	else if (theObject->baseForm->GetFormType() == kFormType_Ammo)
	{
		TESAmmo *item = DYNAMIC_CAST(theObject->baseForm, TESForm, TESAmmo);
	
		if (item)
		{
			// Get the description if any (Mostly Dawnguard and Dragonborn stuff uses the descriptions)
			//CALL_MEMBER_FN(&item->description, Get)(&description, item, 'DESC');//)1129530692);
			//desc.append(AHZT(description.Get()));
			AppendDescription(&item->description, item, desc);
		}
		
		if(ExtraEnchantment* extraEnchant = static_cast<ExtraEnchantment*>(theObject->extraData.GetByType(kExtraData_Enchantment))) // Enchanted
		{
			if (extraEnchant->enchant)
			{
				GetMagicItemDescription(extraEnchant->enchant,effectDescription);
				desc.append(effectDescription);
			}
		}
	}
	else if (theObject->baseForm->GetFormType() == kFormType_Book)
	{
		TESObjectBOOK *item = DYNAMIC_CAST(theObject->baseForm, TESForm, TESObjectBOOK);
		
		if (item)
		{
			// Get the description if any (Mostly Dawnguard and Dragonborn stuff uses the descriptions)
			//CALL_MEMBER_FN(&item->description2, Get)(&description, NULL, 1296125507);
			//CALL_MEMBER_FN(&item->description2, Get)(&description, item, 'DESC');
			//desc.append(AHZT(description.Get()));
			AppendDescription(&item->description2, item, desc);
		}

		if (item && 
			((item->data.flags & TESObjectBOOK::Data::kType_Spell) == TESObjectBOOK::Data::kType_Spell))
		{
			if (item->data.teaches.spell)
			{
				GetMagicItemDescription(item->data.teaches.spell,effectDescription);
				desc.append(effectDescription);
			}
		}
	}
	else if (theObject->baseForm->GetFormType() == kFormType_ScrollItem)
	{
		ScrollItem *item = DYNAMIC_CAST(theObject->baseForm, TESForm, ScrollItem);				
		if (item)
		{
			// Get the description if any (Mostly Dawnguard and Dragonborn stuff uses the descriptions)
			//CALL_MEMBER_FN(&item->description, Get)(&description, NULL, 1129530692);
			//CALL_MEMBER_FN(&item->description, Get)(&description, item, 'DESC');
			//desc.append(AHZT(description.Get()));
			AppendDescription(&item->description, item, desc);

			GetMagicItemDescription(item,effectDescription);
			desc.append(effectDescription);



		}
	}
	return desc;
};

template <class T>
void CAHZUtility<T>::ProcessTargetEffects(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args)
{
	TESObjectREFR * pTargetReference = targetObject;
	
	// See if its harvestable food
	AlchemyItem *food = GetFood(pTargetReference);

	// If the target is not valid or it can't be picked up by the player
	if ((!pTargetReference) || 
		((!CanPickUp(pTargetReference->baseForm->GetFormType())) &&
		(!food)))
	{
		args->args[0].DeleteMember("effectsObj");
		return;
	}
	
	// Used to store the name
	T name;


	PlayerCharacter* pPC = (*g_thePlayer);

	if (pPC)
	{
		InventoryEntryData objDesc(targetObject->baseForm, 0);

		// Allocate a list to send
		objDesc.extendDataList = new tList<BaseExtraList>();

		objDesc.extendDataList->Insert(&targetObject->extraData);


		const char *s;
		s = CALL_MEMBER_FN(&objDesc, GenerateName)();

		float fDamage = CALL_MEMBER_FN(pPC, GetDamage)(&objDesc);

		// Delete the allocated dummy list
		delete objDesc.extendDataList;

		// This could be rounded, but the the script decide
		float ff = mRound(fDamage);
	}


	// If this is harvestable food or normal food get the magic item description
	if (food)
	{
		T effectDescription;
		GetMagicItemDescription(food,effectDescription);
		name.append(effectDescription);
	}
	else
	{
		// Get the effects description if it exists for this object
		name = this->GetEffectsDescription(pTargetReference);
	}

	// If the name contains a string
	if (name.length())
	{
		GFxValue obj;
		args->movie->CreateObject(&obj);			
		
		RegisterTString(&obj, args->movie, "effectsDescription", name.c_str());

		// Add the object to the scaleform function
		args->args[0].SetMember("effectsObj",&obj);
	}
	else
	{
		args->args[0].DeleteMember("effectsObj");
	}
};

template <class T>
void CAHZUtility<T>::ProcessArmorClass(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args)
{
	TESObjectREFR * pTargetReference = targetObject;
	static T weightClass;

	// If the target is not valid or it can't be picked up by the player
	if (!pTargetReference)
	{
		SetResultTString(args, AHZT("").c_str());
		return;
	}

	weightClass.clear();
	weightClass.append(this->GetArmorWeightClass(pTargetReference).c_str());
	
	SetResultTString(args, weightClass.c_str());
};

template <class T>
void CAHZUtility<T>::ProcessBookSkill(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args)
{
	TESObjectREFR * pTargetReference = targetObject;
	static T bookSkill;

	// If the target is not valid or it can't be picked up by the player
	if (!pTargetReference)
	{
		SetResultTString(args, AHZT("").c_str());
		return;
	}

	bookSkill.clear();
	bookSkill.append(this->GetBookSkill(pTargetReference).c_str());
	
	SetResultTString(args,
		bookSkill.c_str());
};

template <class T>
void CAHZUtility<T>::SetResultTString(GFxFunctionHandler::Args *args, const wchar_t * str)
{
	args->result->SetWideString(str);
};

template <class T>
void CAHZUtility<T>::SetResultTString(GFxFunctionHandler::Args *args, const char * str)
{
	args->result->SetString(str);
};


template <class T>
void CAHZUtility<T>::ReplaceStringInPlace(std::wstring& subject, const std::wstring& search,
                      const std::wstring& replace) 
{
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) 
	{
		 subject.replace(pos, search.length(), replace);
		 pos += replace.length();
	}
};

template <class T>
void CAHZUtility<T>::ReplaceStringInPlace(std::string& subject, const std::string& search,
                      const std::string& replace) 
{
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) 
	{
		 subject.replace(pos, search.length(), replace);
		 pos += replace.length();
	}
};

template <class T>
void CAHZUtility<T>::ProcessTargetObject(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args)
{
	TESObjectREFR * pTargetReference = targetObject;
	float totalArmorOrWeapon = 0.0; 
	float difference = 0.0; 

	// If the target is not valid or it can't be picked up by the player
	if (!CAHZUtility<T>::ProcessValidTarget(targetObject, NULL))
	{
		args->args[0].DeleteMember("targetObj");
		return;
	}

	GFxValue obj;
	args->movie->CreateObject(&obj);
	
	if (pTargetReference->baseForm->GetFormType() == kFormType_Weapon || 
		pTargetReference->baseForm->GetFormType() == kFormType_Ammo)
	{
		TESForm *form = NULL;
		TESAmmo *ammo = NULL;
		
		// If ammo is NULL, it is OK
		totalArmorOrWeapon = GetTotalActualWeaponDamage();
		difference = GetWeaponDamageDiff(pTargetReference);
	}
	else if (pTargetReference->baseForm->GetFormType() == kFormType_Armor)
	{
		totalArmorOrWeapon = GetTotalActualArmorRating();
		difference = GetArmorRatingDiff(pTargetReference);
	}
	
	// Enter the data into the Scaleform function
	this->RegisterNumber(&obj, "ratingOrDamage", totalArmorOrWeapon);
	this->RegisterNumber(&obj, "difference", difference);

	float weight = CALL_MEMBER_FN(pTargetReference,GetWeight)();	
	if (pTargetReference->extraData.HasType(kExtraData_Count))
	{
		ExtraCount* xCount = static_cast<ExtraCount*>(pTargetReference->extraData.GetByType(kExtraData_Count));
		if (xCount)
		{
			weight = weight * (float)(SInt16)(xCount->count & 0x7FFF);
		}
	}

	this->RegisterNumber(&obj, "objWeight", weight);

	// Used by the scaleform script to know if this is a weapon, armor, or something else
	this->RegisterNumber(&obj, "formType", pTargetReference->baseForm->GetFormType());
	args->args[0].SetMember("targetObj",&obj);
};

template <class T>
void CAHZUtility<T>::ProcessIngredientData(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args)
{
	TESObjectREFR * pTargetReference = targetObject;
	IngredientItem * ingredient = GetIngredient(pTargetReference);
	ofstream myfile;

	// If no ingredient, then we are done here
	if (!ingredient)
	{
		args->args[0].DeleteMember("ingredientObj");
		return;
	}
	
	T strings[4];
	for (int i = 0; i < 4; i++)
	{
		strings[i].clear();
		if (GetIsNthEffectKnown(ingredient, i))
		{
			MagicItem::EffectItem* pEI = NULL;
			ingredient->effectItemList.GetNthItem(i, pEI);
			if (pEI)
			{
				TESFullName* pFullName = DYNAMIC_CAST(pEI->mgef, TESForm, TESFullName);
				if (pFullName)
				{
					myfile << pFullName->name.data << endl;
					strings[i].append(AHZT(pFullName->name.data));
					myfile << strings[i].c_str() << endl;
				}
			}
		}
	}
	GFxValue obj2;
	args->movie->CreateObject(&obj2);
	this->RegisterTString(&obj2, args->movie, "effect1", strings[0].c_str());
	this->RegisterTString(&obj2, args->movie, "effect2", strings[1].c_str());
	this->RegisterTString(&obj2, args->movie, "effect3", strings[2].c_str());
	this->RegisterTString(&obj2, args->movie, "effect4", strings[3].c_str());
	args->args[0].SetMember("ingredientObj", &obj2);
};

template <class T>
void CAHZUtility<T>::ProcessInventoryCount(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args)
{
	TESObjectREFR * pTargetReference = targetObject;

	IngredientItem * ingredient = GetIngredient(pTargetReference);
	AlchemyItem *food = NULL;
	// If not an ingredient, then see if its food
	if (!ingredient)
		food = GetFood(pTargetReference);

	// If the target is not valid or it can't be picked up by the player
	if ((!pTargetReference) || 
		((!CanPickUp(pTargetReference->baseForm->GetFormType())) &&
		(!ingredient) &&
		(!food)))
	{
		args->args[0].DeleteMember("dataObj");
		return;
	}
	
	// Used to store the name
	T name;
	// Used to store the count of the item
	UInt32 itemCount;

	if (ingredient)
	{
		// Get the number of this in the inventory
		itemCount = CAHZPlayerInfo::GetItemAmount(ingredient->formID);
		TESFullName* pFullName = DYNAMIC_CAST(ingredient, TESForm, TESFullName);
		if (pFullName)
		{
			name.append(AHZT(pFullName->name.data));
		}
	}
	else if (food)
	{
		// Get the number of this in the inventory
		itemCount = CAHZPlayerInfo::GetItemAmount(food->formID);
		TESFullName* pFullName = DYNAMIC_CAST(food, TESForm, TESFullName);
		if (pFullName)
		{
			name.append(AHZT(pFullName->name.data));
		}
	}
	else
	{
		// Get the number of this in the inventory
		itemCount = CAHZPlayerInfo::GetItemAmount(pTargetReference->baseForm->formID);
		name = CAHZUtility::GetTargetName(pTargetReference);
	}

	// If the name contains a string
	if (name.length())
	{
		GFxValue obj;
		args->movie->CreateObject(&obj);			
		
		this->RegisterTString(&obj, args->movie, "inventoryName", name.c_str());
		this->RegisterNumber(&obj, "inventoryCount", itemCount);

		// Add the object to the scaleform function
		args->args[0].SetMember("dataObj",&obj);
	}
	else
	{
		args->args[0].DeleteMember("dataObj");
	}
};

template <class T>
void CAHZUtility<T>::RegisterTString(GFxValue * dst,  GFxMovieView * view, const char * name, const char * str)
{
	GFxValue	fxValue;
	view->CreateString(&fxValue, str);
	dst->SetMember(name, &fxValue);
};

template <class T>
void CAHZUtility<T>::RegisterTString(GFxValue * dst,  GFxMovieView * view, const char * name, const wchar_t * str)
{
	GFxValue	fxValue;
	view->CreateWideString(&fxValue, str);
	dst->SetMember(name, &fxValue);
};

template <class T>
void CAHZUtility<T>::RegisterNumber(GFxValue * dst, const char * name, double value)
{
	GFxValue	fxValue;
	fxValue.SetNumber(value);
	dst->SetMember(name, &fxValue);
};

template <class T>
bool CAHZUtility<T>::ProcessValidTarget(TESObjectREFR* targetObject, GFxFunctionHandler::Args *args)
{
	TESObjectREFR * pTargetReference = targetObject;
	IngredientItem * ingredient = GetIngredient(pTargetReference);
	AlchemyItem * food = NULL;
	
	// If not an ingredient, then see if its food
	if (!ingredient)
	{
		food = GetFood(pTargetReference);
	}

	// If the target is not valid or it can't be picked up by the player
	if ((!pTargetReference) || 
		((!CanPickUp(pTargetReference->baseForm->GetFormType())) &&
		(!ingredient) &&
		(!food)))
	{
		if (args)
		{
			// return false, indicating that the target object is not valid for acquiring data
			args->result->SetBool(false);
		}
		return false;
	}
	else
	{
		if (args)
		{
			// The object is valid
			args->result->SetBool(true);
		}
		return true;
	}
}

template <class T>
void CAHZUtility<T>::ProcessPlayerData(GFxFunctionHandler::Args *args)
{
	GFxValue obj;
	args->movie->CreateObject(&obj);
		
	UInt32 actorValue = LookupActorValueByName("InventoryWeight");
	float encumbranceNumber = ((*g_thePlayer)->actorValueOwner.GetCurrent(actorValue));
	actorValue = LookupActorValueByName("CarryWeight");
	float maxEncumbranceNumber = ((*g_thePlayer)->actorValueOwner.GetCurrent(actorValue));


	// Enter the data into the Scaleform function
	this->RegisterNumber(&obj, "encumbranceNumber", encumbranceNumber);
	this->RegisterNumber(&obj, "maxEncumbranceNumber", maxEncumbranceNumber);
	this->RegisterNumber(&obj, "goldNumber", CAHZPlayerInfo::GetGoldAmount());	
	args->args[0].SetMember("playerObj",&obj);
}

template <class T>
void CAHZUtility<T>::GetMagicItemDescription(MagicItem * item, std::string& description)
{
	string outerString = AHZT("");
	description.clear();

	const char numberFormatter[] = "<font face = '$EverywhereMediumFont' size = '20' color = '#FFFFFF'>%.0f</font>";
	const char stringFormatter[] = "<font face = '$EverywhereMediumFont' size = '20' color = '#FFFFFF'>%s</font>";
	char tempformatter[1000];
	bool canBeAdded = true;



	for (int i = 0; i < item->effectItemList.count; i++)
	{
		float magnitude = item->effectItemList[i]->magnitude;
		float duration = item->effectItemList[i]->duration;
		string s = item->effectItemList[i]->mgef->description;

		std::regex survivalRegex("\\[SURV=.+\\]");
		std::smatch survivalMatch;
		const string survivalConst = const_cast<string &>(s);
		if ((regex_search(survivalConst.begin(), survivalConst.end(), survivalMatch, survivalRegex)))
		{
			ReplaceStringInPlace(s, "[SURV=", "");
			size_t offset = (size_t)(s.length() - 1);
			size_t count = 1;
			s.erase(offset, count);
			canBeAdded = IsSurvivalMode();
		}
		else
		{
			canBeAdded = true;
		}

		if (canBeAdded)
		{
			sprintf_s(tempformatter, 1000, numberFormatter, magnitude);
			ReplaceStringInPlace(s, "<mag>", tempformatter);
			sprintf_s(tempformatter, 1000, numberFormatter, duration);
			ReplaceStringInPlace(s, "<dur>", tempformatter);


			std::regex rgx("\\<\\d+?\\.?\\d*\\>|\\<\\w*\\>");
			std::smatch match;

			const string cs = const_cast<string &>(s);

			if (regex_search(cs.begin(), cs.end(), match, rgx))
			{
				for (int i = 0; i < match.size(); i++)
				{
					string temps = match[i];
					ReplaceStringInPlace(temps, "<", "");
					ReplaceStringInPlace(temps, ">", "");
					string origMatch = match[i];

					sprintf_s(tempformatter, 1000, stringFormatter, temps.c_str());
					ReplaceStringInPlace(s, origMatch, tempformatter);
				}
			}
			outerString.append(s);
			outerString.append(" ");
		}
	}

	description.append(outerString);
}

typedef int*(*_GetEFEscrp)(PlayerCharacter* item, InventoryEntryData *data);
extern RelocAddr<_GetEFEscrp> GetEFEscrp;

//int __fastcall GetDescription(TESDescription *a1, BSString *a2, SInt64 a3, const char *a4, const char *a5, unsigned int a6)


typedef  void (*GET_MAGIC_ITEM_DESCRIPTION)(void *, TESForm* a1, BSString *a2);// , SInt64 a3, const char *a4, const char *a5, unsigned int a6);
extern RelocAddr<GET_MAGIC_ITEM_DESCRIPTION> GetMagicItemDescription2;

template <class T>
void CAHZUtility<T>::GetMagicItemDescription(MagicItem * item, std::wstring& description)
{
	wstring outerString = L"";
	description.clear();
	const wchar_t numberFormatter[] = L"<font face = '$EverywhereMediumFont' size = '20' color = '#FFFFFF'>%.0f</font>";
	const wchar_t stringFormatter[] = L"<font face = '$EverywhereMediumFont' size = '20' color = '#FFFFFF'>%s</font>";
	wchar_t tempformatter[1000];
	bool canBeAdded = true;

	const char a4Val[] = "<font face = '$EverywhereMediumFont' size = '20' color = '#FFFFFF'>";
	const char a5Val[] = "</font>";


	//BSString temp;
	BSString temp;
	BSString temp2;
	//temp = GetMagicItemDescription2(item);
	//wstring tempppp;
	//AppendDescription(&pFullName->description, item, tempppp);
	//wstring sss;
	//sss.append(AHZT(temp.Get()));
	//PlayerCharacter * pPC = *g_thePlayer;
	//DumpClass(item, 1024);

	//TESObjectBOOK* spell = DYNAMIC_CAST(CAHZPlayerInfo::GetTargetRef()->baseForm, TESForm, TESObjectBOOK);

	GetMagicItemDescription2(NULL, item, &temp);

	//CALL_MEMBER_FN(&spell->data.teaches.spell->description, Get)(&temp, spell, 'DESC');

	//MagicItem::EffectItem * pEffect = CALL_MEMBER_FN(item, GetCostliestEffectItem)(5, false);

	//SpellItem *fname = DYNAMIC_CAST(item, MagicItem, SpellItem);
	//CALL_MEMBER_FN(&fname->description, Get2)(&temp, NULL, 'DESC');

	for (int i = 0; i < item->effectItemList.count; i++)
	{


		float magnitude = item->effectItemList[i]->magnitude;
		float duration = item->effectItemList[i]->duration;
		wstring s = AHZT(item->effectItemList[i]->mgef->description.Get());

		std::wregex survivalRegex(L"\\[SURV=.+\\]");
		std::wsmatch survivalMatch;
		const wstring survivalConst = const_cast<wstring &>(s);
		if ((regex_search(survivalConst.begin(), survivalConst.end(), survivalMatch, survivalRegex)))
		{
			ReplaceStringInPlace(s, L"[SURV=", L"");
			size_t offset = (size_t)(s.length() - 1);
			size_t count = 1;
			s.erase(offset, count);

			canBeAdded = IsSurvivalMode();
		}
		else
		{
			canBeAdded = true;
		}

		if (canBeAdded)
		{
			swprintf_s(tempformatter, 1000, numberFormatter, magnitude);
			ReplaceStringInPlace(s, L"<mag>", tempformatter);
			swprintf_s(tempformatter, 1000, numberFormatter, duration);
			ReplaceStringInPlace(s, L"<dur>", tempformatter);

			std::wregex rgx(L"\\<\\d+?\\.?\\d*\\>|\\<\\w*\\>");
			std::wsmatch match;

			const wstring cs = const_cast<wstring &>(s);

			if (regex_search(cs.begin(), cs.end(), match, rgx))
			{
				for (int i = 0; i < match.size(); i++)
				{
					wstring temps = match[i];
					ReplaceStringInPlace(temps, L"<", L"");
					ReplaceStringInPlace(temps, L">", L"");
					wstring origMatch = match[i];

					swprintf_s(tempformatter, 1000, stringFormatter, temps.c_str());
					ReplaceStringInPlace(s, origMatch, tempformatter);
				}
			}

			outerString.append(s);
			outerString.append(L" ");
		}
	}


	description.append(outerString);
}