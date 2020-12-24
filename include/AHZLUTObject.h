#pragma once
#include <string.h>
//using namespace std;

class AHZLUTObject
{
public:
    std::string   BaseMod;
    uint32_t BaseFormID;
    std::string   TargetMod;
    uint32_t TargetFormID;

    bool IsEmpty()
    {
        return !(BaseMod.length() && BaseFormID > 0 && TargetMod.length() && TargetFormID > 0);
    }
};
