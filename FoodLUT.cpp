#include <map>
#include "skse64/GameObjects.h"
#include "skse64/GameForms.h"
#include "skse64/GameRTTI.h"
#include "FoodLUT.h"
using namespace std;

CFoodLUT::CFoodLUT(void)
{	
	m_foodLUT[0x000D337F] = 0x00065C9F;	//SalmonFood
	m_foodLUT[0x000AD0CC] = 0x00065C9F;	//SalmonFood
}

CFoodLUT::~CFoodLUT(void)
{
}

AlchemyItem * CFoodLUT::GetFood(UInt32 formID)
{
	if (m_foodLUT.find(formID) != m_foodLUT.end())
	{
		UInt32 ingredientFormID = m_foodLUT.find(formID)->second;
		TESForm * form = LookupFormByID(ingredientFormID);
		return DYNAMIC_CAST(form, TESForm, AlchemyItem);
	}

	return NULL;
}

