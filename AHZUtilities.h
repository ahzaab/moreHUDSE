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
   static string GetSkyrimDataPath();
   static string& trim(string& s);

   CAHZUtilities();
   ~CAHZUtilities();

private:
   static string& rtrim(string& s);
   static string& ltrim(string& s);


};

