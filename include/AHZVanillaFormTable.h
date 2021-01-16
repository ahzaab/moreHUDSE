#pragma once


//using namespace std;
using AHZFormEntry = std::tuple<std::string, uint32_t>;
using VMScriptEntries = std::vector<std::string>;
using AHZLookupMap = std::vector<AHZFormEntry>;

class CAHZVanillaFormTable
{
public:
    static AHZLookupMap    CreateACTIBaseMap();
    static AHZLookupMap    CreateACTITargetMap();
    static VMScriptEntries CreateVMVariables();
    static void            LoadACTIForms(CAHZFormLookup& loader);
    static void            LoadVMVariables(CAHZFormLookup& loader);

private:
    CAHZVanillaFormTable() = default;
    ~CAHZVanillaFormTable() = default;
};
