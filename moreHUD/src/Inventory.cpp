#include "pch.h"
#include "Inventory.h"

namespace Scaleform
{
	void Inventory::RegisterForInventory([[maybe_unused]] RE::GFxMovieView* a_view, [[maybe_unused]] RE::GFxValue* a_object, [[maybe_unused]] RE::InventoryEntryData* a_item)
	{
		// TODO: Could probably just get rid of this.  Turns out I did not need to add any more extensions.  There was aleady the keywords from SKSE that I needed :)
	}
}
