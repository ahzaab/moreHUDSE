#pragma once
class CFoodLUT
{
public:
	CFoodLUT(void);
	~CFoodLUT(void);

	AlchemyItem *GetFood(UInt32 formID);

private:
	std::map<UInt32,UInt32> m_foodLUT;

};

