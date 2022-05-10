#include "pch.h"
#include "AHZPapyrusMoreHud.h"
#include "version.h"
#include <mutex>

using AhzIconItemCache = std::map<uint32_t, RE::BSFixedString>;
using AhzIconFormListCache = std::map<std::string, RE::BGSListForm*>;
static AhzIconItemCache     s_ahzRegisteredIcons;
static AhzIconFormListCache s_ahzRegisteredIconFormLists;
static std::recursive_mutex mtx;

auto PapyrusMoreHud::GetVersion([[maybe_unused]] RE::StaticFunctionTag* base) -> uint32_t
{
    auto version = Version::ASINT;
    logger::trace("GetVersion: {}", version);
    return version;
}

void PapyrusMoreHud::RegisterIconFormList(RE::StaticFunctionTag* base, RE::BSFixedString iconName, RE::BGSListForm* list)
{
    logger::trace("RegisterIconFormList");
    std::lock_guard<std::recursive_mutex> lock(mtx);

    if (!list)
        return;

    if (!IsIconFormListRegistered(base, iconName)) {
        s_ahzRegisteredIconFormLists.insert(AhzIconFormListCache::value_type(iconName.c_str(), list));
    }
}

void PapyrusMoreHud::UnRegisterIconFormList(RE::StaticFunctionTag* base, RE::BSFixedString iconName)
{
    logger::trace("UnRegisterIconFormList");
    std::lock_guard<std::recursive_mutex> lock(mtx);

    if (IsIconFormListRegistered(base, iconName)) {
        s_ahzRegisteredIconFormLists.erase(iconName.c_str());
    }
}

std::vector<std::string_view> PapyrusMoreHud::GetFormIcons(RE::FormID formId)
{
    std::lock_guard<std::recursive_mutex> lock(mtx);
    std::vector<std::string_view>         results;
    for (auto& kvp : s_ahzRegisteredIconFormLists) {
        auto list = s_ahzRegisteredIconFormLists[kvp.first];

        if (list && list->HasForm(formId)) {
            results.emplace_back(kvp.first);
        }
    }
    return results;
}

auto PapyrusMoreHud::IsIconFormListRegistered_Internal(std::string iconName) -> bool
{
    logger::trace("IsIconFormListRegistered_Internal");
    std::lock_guard<std::recursive_mutex> lock(mtx);
    // Create an iterator of map
    AhzIconFormListCache::iterator it;

    if (s_ahzRegisteredIconFormLists.empty())
        return false;

    // Find the element with key itemID
    it = s_ahzRegisteredIconFormLists.find(iconName);

    // Check if element exists in map or not
    return (it != s_ahzRegisteredIconFormLists.end());
}

auto PapyrusMoreHud::IsIconFormListRegistered([[maybe_unused]] RE::StaticFunctionTag* base, RE::BSFixedString iconName) -> bool
{
    return IsIconFormListRegistered_Internal(iconName.c_str());
}

auto PapyrusMoreHud::HasForm(std::string iconName, uint32_t formId) -> bool
{
    logger::trace("HasForm");
    std::lock_guard<std::recursive_mutex> lock(mtx);
    if (IsIconFormListRegistered_Internal(iconName)) {
        auto formList = s_ahzRegisteredIconFormLists[iconName];

        if (!formId)
            return false;

        auto formFromId = RE::TESForm::LookupByID(formId);

        if (!formFromId)
            return false;

        return formList->HasForm(formFromId);
    }
    return false;
}

auto PapyrusMoreHud::IsIconItemRegistered([[maybe_unused]] RE::StaticFunctionTag* base, uint32_t itemID) -> bool
{
    logger::trace("IsIconItemRegistered");
    std::lock_guard<std::recursive_mutex> lock(mtx);
    // Create an iterator of map
    AhzIconItemCache::iterator it;

    // Find the element with key itemID
    it = s_ahzRegisteredIcons.find(itemID);

    // Check if element exists in map or not
    return (it != s_ahzRegisteredIcons.end());
}

void PapyrusMoreHud::AddIconItem(RE::StaticFunctionTag* base, uint32_t itemID, RE::BSFixedString iconName)
{
    logger::trace("AddIconItem");
    std::lock_guard<std::recursive_mutex> lock(mtx);
    if (!IsIconItemRegistered(base, itemID)) {
        s_ahzRegisteredIcons.insert(AhzIconItemCache::value_type(itemID, iconName));
    }
}

void PapyrusMoreHud::RemoveIconItem(RE::StaticFunctionTag* base, uint32_t itemID)
{
    logger::trace("RemoveIconItem");
    std::lock_guard<std::recursive_mutex> lock(mtx);
    if (IsIconItemRegistered(base, itemID)) {
        s_ahzRegisteredIcons.erase(itemID);
    }
}

void PapyrusMoreHud::AddIconItems(RE::StaticFunctionTag* base, std::vector<uint32_t> itemIDs, std::vector<RE::BSFixedString> iconNames)
{
    logger::trace("AddIconItems");
    std::lock_guard<std::recursive_mutex> lock(mtx);
    if (itemIDs.size() != iconNames.size()) {
        return;
    }

    for (uint32_t i = 0; i < itemIDs.size(); i++) {
        uint32_t          itemID;
        RE::BSFixedString iconName;
        itemID = itemIDs[i];
        iconName = iconNames[i];
        AddIconItem(base, itemID, iconName);
    }
}

void PapyrusMoreHud::RemoveIconItems(RE::StaticFunctionTag* base, std::vector<uint32_t> itemIDs)
{
    logger::trace("RemoveIconItem");
    std::lock_guard<std::recursive_mutex> lock(mtx);
    for (uint32_t i = 0; i < itemIDs.size(); i++) {
        uint32_t itemID;
        itemID = itemIDs[i];
        if (itemID) {
            RemoveIconItem(base, itemID);
        }
    }
}

auto PapyrusMoreHud::GetIconName(uint32_t itemID) -> std::string
{
    logger::trace("GetIconName");
    std::string                           iconName("");
    std::lock_guard<std::recursive_mutex> lock(mtx);

    if (IsIconItemRegistered(nullptr, itemID)) {
        iconName.append(s_ahzRegisteredIcons[itemID].c_str());
    }

    return iconName;
}

auto PapyrusMoreHud::RegisterFunctions(RE::BSScript::IVirtualMachine* a_vm) -> bool
{
    a_vm->RegisterFunction("GetVersion", "AhzMoreHud", GetVersion, true);
    a_vm->RegisterFunction("IsIconItemRegistered", "AhzMoreHud", IsIconItemRegistered);
    a_vm->RegisterFunction("AddIconItem", "AhzMoreHud", AddIconItem);
    a_vm->RegisterFunction("RemoveIconItem", "AhzMoreHud", RemoveIconItem);
    a_vm->RegisterFunction("AddIconItems", "AhzMoreHud", AddIconItems);
    a_vm->RegisterFunction("RemoveIconItems", "AhzMoreHud", RemoveIconItems);
    a_vm->RegisterFunction("RegisterIconFormList", "AhzMoreHud", RegisterIconFormList);
    a_vm->RegisterFunction("UnRegisterIconFormList", "AhzMoreHud", UnRegisterIconFormList);
    a_vm->RegisterFunction("IsIconFormListRegistered", "AhzMoreHud", IsIconFormListRegistered);
    return true;
}
