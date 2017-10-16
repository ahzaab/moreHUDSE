#pragma once
class CIngredientLUT
{
public:
	CIngredientLUT(void);
	~CIngredientLUT(void);

	IngredientItem *GetIngredient(UInt32 formID);

private:
	std::map<UInt32,UInt32> m_ingrLUT;

};

