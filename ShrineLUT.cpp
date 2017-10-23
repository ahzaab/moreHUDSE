#include <map>
#include "skse64/GameObjects.h"
#include "skse64/GameForms.h"
#include "skse64/GameRTTI.h"
#include "ShrineLUT.h"
using namespace std;

CShrineLUT::CShrineLUT(void)
{	

	//ShrineofArkay "Shrine of Arkay"[ACTI:00071854]   AltarArkaySpell "Blessing of Arkay"[SPEL:000FB994]
	//	ShrineOfZenithar "Shrine of Zenithar"[ACTI:000D987B] AltarZenitharSpell "Blessing of Zenithar"[SPEL:000FB99B]
	//	ShrineofStendarr "Shrine of Stendarr"[ACTI:000D987D]  AltarStendarrSpell "Blessing of Stendarr"[SPEL:000FB999]
	//	ShrineOfKynareth "Shrine of Kynareth"[ACTI:000D987F] AltarKynarethSpell "Blessing of Kynareth"[SPEL:000FB997]
	//	ShrineofDibella "Shrine of Dibella"[ACTI:000D9881][SPEL:000FB997] AltarDibellaSpell "Blessing of Dibella"[SPEL:000FB995]
	//	ShrineofAkatosh "Shrine of Akatosh"[ACTI:000D9883] AltarAkatoshSpell "Blessing of Akatosh"[SPEL:000FB988]
	//	ShrineofJulianos "Shrine of Julianos"[ACTI:000D9885] AltarJulianosSpell "Blessing of Julianos"[SPEL:000FB996]
	//	ShrineofMara "Shrine of Mara"[ACTI:000D9887] AltarMaraSpell "Blessing of Mara"[SPEL:000FB998]
	//	ShrineofTalos "Shrine of Talos"[ACTI:00100780] AltarTalosSpell "Blessing of Talos"[SPEL:000FB99A]


	m_shrineLUT[0x00071854] = 0x000FB994;
	m_shrineLUT[0x000D987B] = 0x000FB99B;
	m_shrineLUT[0x000D987D] = 0x000FB999;
	m_shrineLUT[0x000D987F] = 0x000FB997;
	m_shrineLUT[0x000D9881] = 0x000FB995;
	m_shrineLUT[0x000D9883] = 0x000FB988;
	m_shrineLUT[0x000D9885] = 0x000FB996;
	m_shrineLUT[0x000D9887] = 0x000FB998;
	m_shrineLUT[0x00100780] = 0x000FB99A;

}

CShrineLUT::~CShrineLUT(void)
{
}

SpellItem * CShrineLUT::GetBlessing(UInt32 formID)
{
	if (m_shrineLUT.find(formID) != m_shrineLUT.end())
	{
		UInt32 spellFormID = m_shrineLUT.find(formID)->second;
		TESForm * form = LookupFormByID(spellFormID);
		return DYNAMIC_CAST(form, TESForm, SpellItem);
	}

	return NULL;
}

