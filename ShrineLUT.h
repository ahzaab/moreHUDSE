#pragma once
class CShrineLUT
{
public:

   static CShrineLUT GetSingleton();
	SpellItem *GetBlessing(UInt32 formID);

   ~CShrineLUT(void);
private:
   CShrineLUT(void);
   std::map<UInt32, UInt32> m_shrineLUT;
   UInt32 s_dawnguardIndex;
};

