import gfx.controls.ScrollBar;

class RootList extends StatsList
{
	function RootList()
	{
		super();
	}

	function UpdateList(): Void
	{
		super.UpdateList();
		var item: MovieClip = GetClipByIndex(1);
		item._visible = false;
	}

	function set selectedIndex(aiNewIndex: Number): Void
	{
		var prevSelection = super.selectedIndex;
		// skip over the divider
		if (aiNewIndex == 1)
		{
			if (aiNewIndex > prevSelection)
			{
				aiNewIndex++;
			}
			else
			{
				aiNewIndex--;
			}
		}
		super.selectedIndex = aiNewIndex;
	}
}
