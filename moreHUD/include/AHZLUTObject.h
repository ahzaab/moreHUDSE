#pragma once
#include <string.h>
using namespace std;

class AHZLUTObject {

public:
   string BaseMod;
   uint32_t BaseFormID;
   string TargetMod;
   uint32_t TargetFormID;

   bool IsEmpty()
   {
      return !(BaseMod.length() && BaseFormID > 0 && TargetMod.length() && TargetFormID > 0);
   }
};

