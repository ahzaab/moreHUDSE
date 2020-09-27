#include "PCH.h"
#include "AHZUtilities.h"
#include <io.h>

vector<string> CAHZUtilities::GetMHudFileList(string& folder)
{
    vector<string>  names;
    string          search_path = folder + "/*.MHUD";
    WIN32_FIND_DATA fd;
    logger::info("Search the '{}' directory...", folder.c_str());
    HANDLE hFind = ::FindFirstFile(search_path.c_str(), &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            // read all (real) files in current folder
            // , delete '!' read other 2 default folder . and ..
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                string fileName = fd.cFileName;

                std::for_each(fileName.begin(), fileName.end(), [](char& c) {
                    c = static_cast<char>(::toupper(static_cast<int32_t>(c)));
                });

                logger::info("--> FOUND '{}'", fileName.c_str());
                names.push_back(fd.cFileName);
            }
        } while (::FindNextFile(hFind, &fd));
        ::FindClose(hFind);
    }
    return names;
}

vector<string> CAHZUtilities::SplitString(string& str, string& token)
{
    vector<string> result;
    while (str.size()) {
        auto index = str.find(token);
        if (index != string::npos) {
            result.push_back(str.substr(0, index));
            str = str.substr(index + token.size());
            if (str.size() == 0)
                result.push_back(str);
        } else {
            result.push_back(str);
            str = "";
        }
    }
    return result;
}

AHZLUTObject CAHZUtilities::ParseLUTObject(string& stringValue)
{
    string         split = ",";
    vector<string> items = SplitString(stringValue, split);
    string         pipe = "|";

    AHZLUTObject lutObject;

    if (items.size() == 2) {
        vector<string> baseItem = SplitString(items[0], pipe);
        vector<string> targetItem = SplitString(items[1], pipe);

        if (baseItem.size() == 2 && targetItem.size() == 2) {
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

    logger::info("The line {} could not be parsed", stringValue.c_str());
    return AHZLUTObject();
}

string& CAHZUtilities::GetSkyrimDataPath()
{
    static string s_dataPath;

    if (s_dataPath.empty()) {
        HMODULE hModule = GetModuleHandle(nullptr);
        if (hModule != nullptr) {
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
                nullptr,
                0,
                nullptr,
                0);

            s_dataPath.append(skyrimDrive);
            s_dataPath.append(skyrimDir);
            s_dataPath.append("Data\\");
        }
    }
    return s_dataPath;
}

string& CAHZUtilities::GetPluginPath()
{
    static string s_pluginPath;

    if (s_pluginPath.empty()) {
        s_pluginPath.append(GetSkyrimDataPath().c_str());
        s_pluginPath.append("SKSE\\Plugins\\");
    }
    return s_pluginPath;
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

string CAHZUtilities::GetConfigOption(const char* section, const char* key)
{
    string        result;
    static string s_pluginPath;

    if (s_pluginPath.empty()) {
        s_pluginPath.append(GetPluginPath().c_str());
        s_pluginPath.append("AHZmoreHUDPlugin");
        s_pluginPath.append(".ini");
    }

    char resultBuf[256];
    resultBuf[0] = 0;

    if (_access_s(s_pluginPath.c_str(), 0) == 0) {
        uint32_t resultLen = GetPrivateProfileString(section, key, NULL, resultBuf, sizeof(resultBuf), s_pluginPath.c_str());
        result = resultBuf;
    }
    return result;
}
