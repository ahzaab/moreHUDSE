#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include "AHZLUTObject.h"
using namespace std;

class CAHZUtilities
{
public:
   static vector<string> GetMHudFileList(string& folder);
   static vector<string> SplitString(string& str, string& token);
   static AHZLUTObject ParseLUTObject(string &string);
   static string& GetSkyrimDataPath();
   static string& GetPluginPath();
   static string& trim(string& s);
   static string GetConfigOption(const char * section, const char * key);

   CAHZUtilities();
   ~CAHZUtilities();

private:
   static string& rtrim(string& s);
   static string& ltrim(string& s);


};

