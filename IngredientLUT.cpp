#include <map>
#include "skse64/GameObjects.h"
#include "skse64/GameForms.h"
#include "skse64/GameRTTI.h"
#include "IngredientLUT.h"
using namespace std;

CIngredientLUT::CIngredientLUT(void)
{
	// 000E1FB2		00059B86	//Nirnroot
	// 000B6FB9		000B701A	//Crimson Nirnroot
	// 000A9169		000A9195    // Bee
	// CritterDragonfly01 "Dragonfly" [ACTI:000D95C5]		DragonflyOrange "Orange Dartwing" [INGR:000BB956]
	// CritterDragonFly02 "Dragonfly" [ACTI:000D95C7]		DragonflyBlue "Blue Dartwing" [INGR:000E4F0C]
	// CritterFirefly "Torchbug" [ACTI:000C2D3F]			FireflyThorax "Torchbug Thorax" [INGR:0004DA73]
	// CritterMothBlue "Blue Butterfly" [ACTI:00022219]		MothWingBlue "Blue Butterfly Wing" [INGR:000727DE]
	// CritterMothLuna "Luna Moth" [ACTI:0002221E]			MothWingLuna "Luna Moth Wing" [INGR:000727DF]
	// CritterMothMonarch "Monarch Butterfly" [ACTI:000B97AF]	MothWingMonarch "Butterfly Wing" [INGR:000727E0]
	// CritterPondFish01 "Silverside Perch" [ACTI:00106D28]	CritterPondFish01Ingredient "Silverside Perch" [INGR:00106E1C]
	// CritterPondFish02 "Abacean Longfin" [ACTI:00106D29]	CritterPondFish02Ingredient "Abecean Longfin" [INGR:00106E1B]
	// CritterPondFish03 "River Betty" [ACTI:00106D2A]		CritterPondFish03Ingredient "River Betty" [INGR:00106E1A]
	// CritterPondFish04 "Cyrodilic Spadetail" [ACTI:00106D2B]	CritterPondFish04Ingredient "Cyrodilic Spadetail" [INGR:00106E19]
	// CritterPondFish05 "Histcarp" [ACTI:00106D2C]			CritterPondFish05Ingredient "Histcarp" [INGR:00106E18]

	// 0x000E1FB2	0x00059B86	//Nirnroot
	// 0x000B6FB9	0x000B701A	//Crimson Nirnroot
	// 0x000A9169	0x000A9195    // Bee
	// 0x000D95C5	0x000BB956
	// 0x000D95C7	0x000E4F0C
	// 0x000C2D3F	0x0004DA73
	// 0x00022219	0x000727DE
	// 0x0002221E	0x000727DF
	// 0x000B97AF	0x000727E0
	// 0x00106D28	0x00106E1C
	// 0x00106D29	0x00106E1B
	// 0x00106D2A	0x00106E1A
	// 0x00106D2B	0x00106E19
	// 0x00106D2C	0x00106E18	
	
	m_ingrLUT[0x000E1FB2] = 0x00059B86;	//Nirnroot
	m_ingrLUT[0x000B6FB9] = 0x000B701A;	//Crimson Nirnroot
	m_ingrLUT[0x000A9169] = 0x000A9195;    // Bee
	m_ingrLUT[0x000D95C5] = 0x000BB956;
	m_ingrLUT[0x000D95C7] = 0x000E4F0C;
	m_ingrLUT[0x000C2D3F] = 0x0004DA73;
	m_ingrLUT[0x00022219] = 0x000727DE;
	m_ingrLUT[0x0002221E] = 0x000727DF;
	m_ingrLUT[0x000B97AF] = 0x000727E0;
	m_ingrLUT[0x00106D28] = 0x00106E1C;
	m_ingrLUT[0x00106D29] = 0x00106E1B;
	m_ingrLUT[0x00106D2A] = 0x00106E1A;
	m_ingrLUT[0x00106D2B] = 0x00106E19;
	m_ingrLUT[0x00106D2C] = 0x00106E18;
}

CIngredientLUT::~CIngredientLUT(void)
{
}

IngredientItem * CIngredientLUT::GetIngredient(UInt32 formID)
{
	if (m_ingrLUT.find(formID) != m_ingrLUT.end())
	{
		UInt32 ingredientFormID = m_ingrLUT.find(formID)->second;
		TESForm * form = LookupFormByID(ingredientFormID);
		return DYNAMIC_CAST(form, TESForm, IngredientItem);
	}

	return NULL;
}

