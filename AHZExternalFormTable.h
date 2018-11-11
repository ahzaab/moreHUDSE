#pragma once
#include "AHZFormLookup.h"
#include "AHZUtilities.h"
#include <vector>
#include <map>

using namespace std;

class CAHZExternalFormTable
{
public:

   static void LoadACTIForms(CAHZFormLookup& loader, vector<string>& mHudFiles)
   {
      string skyrimDataPath = CAHZUtilities::GetSkyrimDataPath();

      // Foreach mhud file, load in the lookup table entries
      vector<string>::iterator p;
      for (p = mHudFiles.begin(); p != mHudFiles.end(); p++) {
         _MESSAGE("Loading ACTI Forms from '%s'", (*p).c_str());
         gLog.Indent();
         string fullPath = skyrimDataPath + *p;

         // Get the number of entries
         int iNumOfEntries = GetPrivateProfileInt("LookupTable", "iNumOfEntries", 0, fullPath.c_str());
         _MESSAGE("Loading %d ACTI Form Entries", iNumOfEntries);
         cout << iNumOfEntries << endl;

         // Get each entry and load into the lookup table
         for (int i = 0; i < iNumOfEntries; i++)
         {
            char value[32] = "";
            char returnValue[1024] = "";
            sprintf_s(value, (size_t)32, "%d", i + 1);
            string entrName("oEntry");
            entrName.append(value);
            GetPrivateProfileString("LookupTable", entrName.c_str(), "", returnValue, size_t(1024), fullPath.c_str());
            if (strlen(returnValue))
            {
               AHZLUTObject lutObject = CAHZUtilities::ParseLUTObject(string(returnValue));

               if (!lutObject.IsEmpty())
               {
                  loader.AddFormID(lutObject.BaseMod, lutObject.BaseFormID, lutObject.TargetMod, lutObject.TargetFormID);
               }
               else
               {
                  _MESSAGE("Could not entry Entry: %s", returnValue);
               }
            }
         }

         gLog.Outdent();
      }
   };

   static void LoadVMVariables(CAHZFormLookup& loader, vector<string> & mHudFiles)
   {
      string skyrimDataPath = CAHZUtilities::GetSkyrimDataPath();

      vector<string>::iterator p;
      for (p = mHudFiles.begin(); p != mHudFiles.end(); p++) {
         _MESSAGE("Loading Script Variables from '%s'", (*p).c_str());
         gLog.Indent();
         string fullPath = skyrimDataPath + *p;
         // Get the number of entries
         int iNumOfEntries = GetPrivateProfileInt("ScriptVariables", "iNumOfVariables", 0, fullPath.c_str());
         _MESSAGE("Loading %d Script Variable(s)", iNumOfEntries);
         cout << iNumOfEntries << endl;

         // Get each entry and load into the lookup table
         for (int i = 0; i < iNumOfEntries; i++)
         {
            char value[32] = "";
            char returnValue[1024] = "";
            sprintf_s(value, (size_t)32, "%d", i + 1);
            string entrName("sVariable");
            entrName.append(value);
            GetPrivateProfileString("ScriptVariables", entrName.c_str(), "", returnValue, size_t(1024), fullPath.c_str());

            if (strlen(returnValue))
            {
               loader.AddScriptVarable(string(returnValue));
            }
            else
            {
               _MESSAGE("Could not load script variable: %s", returnValue);
            }
         }
         gLog.Outdent();
      }
   };

private:
   CAHZExternalFormTable() {};
   ~CAHZExternalFormTable() {};
};
