#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include "AHZLUTObject.h"
//using namespace std;

class CAHZUtilities
{
public:
    static std::vector<std::string> GetMHudFileList(std::string& folder);
    static std::vector<std::string> SplitString(std::string& str, std::string& token);
    static AHZLUTObject   ParseLUTObject(std::string& string);
    static std::string&        GetSkyrimDataPath();
    static std::string&        GetPluginPath();
    static std::string&        trim(std::string& s);
    static std::string         GetConfigOption(const char* section, const char* key);

    CAHZUtilities() = default;
    ~CAHZUtilities() = default;

private:
    static std::string& rtrim(std::string& s);
    static std::string& ltrim(std::string& s);
};
