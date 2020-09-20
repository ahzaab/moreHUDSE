#pragma once
#include <string.h>
using namespace std;

class AHZLUTObject {

public:
   string BaseMod;
   UInt32 BaseFormID;
   string TargetMod;
   UInt32 TargetFormID;

   bool IsEmpty()
   {
      return !(BaseMod.length() && BaseFormID > 0 && TargetMod.length() && TargetFormID > 0);
   }
};

