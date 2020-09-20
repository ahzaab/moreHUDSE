import gfx.controls.ScrollBar;
import skyui.util.Translator;

class PerkInfoList extends Shared.BSScrollingList
{
	var scrollbar: ScrollBar;

	function PerkInfoList()
	{
		super();
		scrollbar.focusTarget = this;
	}

	public function set dataProvider(elements:Array): Void
	{
		this.ClearList();
		this.scrollPosition = 0;
		this.selectedIndex = -1;
		for (var i: Number = 0; i < elements.length; i++)
		{
			this.entryList.push({text:Translator.translate(elements[i])});
		}
		this.InvalidateData();
	}

}
