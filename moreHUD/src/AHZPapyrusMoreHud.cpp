#include "PCH.h"
#include "AHZPapyrusMoreHud.h"
#include <mutex>


	typedef std::map<uint32_t, RE::BSFixedString> AhzIconItemCache;
	typedef std::map<std::string, RE::BGSListForm*> AhzIconFormListCache;
	static AhzIconItemCache s_ahzRegisteredIcons;
	static AhzIconFormListCache s_ahzRegisteredIconFormLists;
	static std::recursive_mutex mtx;

	uint32_t PapyrusMoreHud::GetVersion(RE::StaticFunctionTag* base)
	{
        return 0;  //PLUGIN_VERSION;
	}

	void PapyrusMoreHud::RegisterIconFormList(RE::StaticFunctionTag* base, RE::BSFixedString iconName, RE::BGSListForm* list)
	{
		//_MESSAGE("AddIconItem %d, %s", itemID, iconName.c_str());
		std::lock_guard <recursive_mutex> lock(mtx);

		if (!list)
			return;

		if (!IsIconFormListRegistered(base, iconName))
		{
			s_ahzRegisteredIconFormLists.insert(AhzIconFormListCache::value_type(iconName.c_str(), list));
		}
	}

	void PapyrusMoreHud::UnRegisterIconFormList(RE::StaticFunctionTag* base, RE::BSFixedString iconName)
	{
		//_MESSAGE("AddIconItem %d, %s", itemID, iconName.c_str());
		std::lock_guard <recursive_mutex> lock(mtx);

		if (IsIconFormListRegistered(base, iconName))
		{
			s_ahzRegisteredIconFormLists.erase(iconName.c_str());
		}
	}

	bool PapyrusMoreHud::IsIconFormListRegistered_Internal(std::string iconName)
	{
		//_MESSAGE("IsIconItemRegistered %d", itemID);
		std::lock_guard <recursive_mutex> lock(mtx);
		// Create an iterator of map
		AhzIconFormListCache::iterator it;

		if (s_ahzRegisteredIconFormLists.empty())
			return false;

		// Find the element with key itemID
		it = s_ahzRegisteredIconFormLists.find(iconName);

		// Check if element exists in map or not
		return (it != s_ahzRegisteredIconFormLists.end());
	}

	bool PapyrusMoreHud::IsIconFormListRegistered(RE::StaticFunctionTag* base, RE::BSFixedString iconName)
	{
		return IsIconFormListRegistered_Internal(iconName.c_str());
	}

	bool PapyrusMoreHud::HasForm(std::string iconName, uint32_t formId)
	{
		std::lock_guard <recursive_mutex> lock(mtx);
		if (IsIconFormListRegistered_Internal(iconName))
		{
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

	bool PapyrusMoreHud::IsIconItemRegistered(RE::StaticFunctionTag* base, uint32_t itemID)
	{
		//_MESSAGE("IsIconItemRegistered %d", itemID);
		std::lock_guard <recursive_mutex> lock(mtx);
		// Create an iterator of map
		AhzIconItemCache::iterator it;

		// Find the element with key itemID
		it = s_ahzRegisteredIcons.find(itemID);

		// Check if element exists in map or not
		return (it != s_ahzRegisteredIcons.end());
	}



	void PapyrusMoreHud::AddIconItem(RE::StaticFunctionTag* base, uint32_t itemID, RE::BSFixedString iconName)
	{
		//_MESSAGE("AddIconItem %d, %s", itemID, iconName.c_str());
		std::lock_guard <recursive_mutex> lock(mtx);
		if (!IsIconItemRegistered(base, itemID))
		{
			s_ahzRegisteredIcons.insert(AhzIconItemCache::value_type(itemID, iconName));
		}
	}

	void PapyrusMoreHud::RemoveIconItem(RE::StaticFunctionTag* base, uint32_t itemID)
	{
		//_MESSAGE("RemoveIconItem %d", itemID);
		std::lock_guard <recursive_mutex> lock(mtx);
		if (IsIconItemRegistered(base, itemID))
		{
			s_ahzRegisteredIcons.erase(itemID);
		}
	}

	void PapyrusMoreHud::AddIconItems(RE::StaticFunctionTag* base, std::vector<uint32_t> itemIDs, std::vector<RE::BSFixedString> iconNames)
	{
		std::lock_guard <recursive_mutex> lock(mtx);
		if (itemIDs.size() != iconNames.size())
		{
			return;
		}

		for (uint32_t i = 0; i < itemIDs.size(); i++)
		{
            uint32_t      itemID;
			RE::BSFixedString iconName;
            itemID = itemIDs[i];
            iconName = iconNames[i];
			AddIconItem(base, itemID, iconName);
		}
	}

	void PapyrusMoreHud::RemoveIconItems(RE::StaticFunctionTag* base, std::vector<uint32_t> itemIDs)
	{
		std::lock_guard <recursive_mutex> lock(mtx);
        for (uint32_t i = 0; i < itemIDs.size(); i++)
		{
            uint32_t itemID;
            itemID = itemIDs[i];
			if (itemID)
			{
				RemoveIconItem(base, itemID);
			}
		}
	}

	string PapyrusMoreHud::GetIconName(uint32_t itemID)
	{
		string iconName("");
		std::lock_guard <recursive_mutex> lock(mtx);

		if (IsIconItemRegistered(NULL, itemID))
		{
			iconName.append(s_ahzRegisteredIcons[itemID].c_str());
		}

		return iconName;
	}


void PapyrusMoreHud::RegisterFunctions(RE::BSScript::IVirtualMachine* a_vm)
    {
        a_vm->RegisterFunction("GetVersion", "AhzMoreHud", GetVersion);
        a_vm->RegisterFunction("IsIconItemRegistered", "AhzMoreHud", IsIconItemRegistered);
        a_vm->RegisterFunction("AddIconItem", "AhzMoreHud", AddIconItem);
        a_vm->RegisterFunction("RemoveIconItem", "AhzMoreHud", RemoveIconItem);
        a_vm->RegisterFunction("AddIconItems", "AhzMoreHud", AddIconItems);
        a_vm->RegisterFunction("RemoveIconItems", "AhzMoreHud", RemoveIconItems);
        a_vm->RegisterFunction("RegisterIconFormList", "AhzMoreHud", RegisterIconFormList);
        a_vm->RegisterFunction("UnRegisterIconFormList", "AhzMoreHud", UnRegisterIconFormList);
        a_vm->RegisterFunction("IsIconFormListRegistered", "AhzMoreHud", IsIconFormListRegistered);
    }
