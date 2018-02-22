#include "AHZUtilities.h"

vector<string> CAHZUtilities::GetMHudFileList(string& folder)
{
   vector<string> names;
   string search_path = folder + "/*.*";
   WIN32_FIND_DATA fd;
   HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
   if (hFind != INVALID_HANDLE_VALUE) {
      do {
         // read all (real) files in current folder
         // , delete '!' read other 2 default folder . and ..
         if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            string fileName(fd.cFileName);
            transform(fileName.begin(), fileName.end(), fileName.begin(), toupper);
            size_t found = fileName.find(".MHUD");
            if (found != string::npos)
            {
               names.push_back(fd.cFileName);
            }
         }
      } while (::FindNextFile(hFind, &fd));
      ::FindClose(hFind);
   }
   return names;
}

vector<string> CAHZUtilities::SplitString(string& str, string& token) {
   vector<string>result;
   while (str.size()) {
      int index = str.find(token);
      if (index != string::npos) {
         result.push_back(str.substr(0, index));
         str = str.substr(index + token.size());
         if (str.size() == 0)result.push_back(str);
      }
      else {
         result.push_back(str);
         str = "";
      }
   }
   return result;
}

AHZLUTObject CAHZUtilities::ParseLUTObject(string & stringValue)
{
   vector<string> items = SplitString(string(stringValue), string(","));
   AHZLUTObject lutObject;

   if (items.size() == 2)
   {
      vector<string> baseItem = SplitString(string(items[0]), string("|"));
      vector<string> targetItem = SplitString(string(items[1]), string("|"));

      if (baseItem.size() == 2 && targetItem.size() == 2)
      {
         baseItem[0] = trim(baseItem[0]);
         baseItem[1] = trim(baseItem[1]);

         targetItem[0] = trim(targetItem[0]);
         targetItem[1] = trim(targetItem[1]);

         lutObject.BaseMod = baseItem[0];
         lutObject.BaseFormID = stoi(baseItem[1], nullptr, 16);
         lutObject.TargetMod = targetItem[0];
         lutObject.TargetFormID = stoi(targetItem[1], nullptr, 16);

         return lutObject;
      }
   }

   _MESSAGE("The line %s could not be parsed", stringValue.c_str());
   return AHZLUTObject();
}

string CAHZUtilities::GetSkyrimDataPath()
{
   HMODULE hModule = GetModuleHandle(NULL);
   if (hModule != NULL)
   {
      char skyrimPath[_MAX_PATH];
      char skyrimDir[_MAX_DIR];
      char skyrimDrive[_MAX_DRIVE];
      // Use GetModuleFileName() with module handle to get the path
      GetModuleFileName(hModule, skyrimPath, (sizeof(skyrimPath)));

      _splitpath_s(
         (const char*)skyrimPath,
         &skyrimDrive[0],
         (size_t)sizeof(skyrimDrive),
         &skyrimDir[0],
         (size_t)sizeof(skyrimDir),
         NULL,
         0,
         NULL,
         0
      );

      string dataPath(skyrimDrive);
      dataPath.append(skyrimDir);
      dataPath.append("Data\\");
      return dataPath;
   }
   return string();
}

// trim from end of string (right)
string& CAHZUtilities::rtrim(string& s)
{
   s.erase(s.find_last_not_of(" \t\n\r\f\v") + 1);
   return s;
}

// trim from beginning of string (left)
string& CAHZUtilities::ltrim(string& s)
{
   s.erase(0, s.find_first_not_of(" \t\n\r\f\v"));
   return s;
}

// trim from both ends of string (left & right)
string& CAHZUtilities::trim(string& s)
{
   return ltrim(rtrim(s));
}



CAHZUtilities::CAHZUtilities()
{
}


CAHZUtilities::~CAHZUtilities()
{
}
