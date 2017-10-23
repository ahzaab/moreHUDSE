#pragma once
class CShrineLUT
{
public:
	CShrineLUT(void);
	~CShrineLUT(void);

	SpellItem *GetBlessing(UInt32 formID);

private:
	std::map<UInt32,UInt32> m_shrineLUT;

};

