import gfx.managers.FocusHandler;
import skyui.util.Translator;
import gfx.controls.ScrollBar;

class FactionsList extends StatsList
{
	public var selected:MovieClip;
	public var _focused:Number;
	var scrollbar: ScrollBar;
	
	function FactionsList()
	{
		super();
	}
	
	public function onLoad(): Void
	{
		super.onLoad();

	}	

	/* override */
	public function set dataProvider(elements:Array): Void
	{
		this.ClearList();
		this.scrollPosition = 0;
		this.selectedIndex = -1;
		for(var i: Number = 0; i < elements.length; i++) {
			this.entryList.push({text: elements[i].factionName, value: elements[i].rankName});
		}
		this.InvalidateData();
	}
	
	/* override */
	function SetEntryText(aEntryClip: MovieClip, aEntryObject: Object): Void
	{
		super.SetEntryText(aEntryClip, aEntryObject);
		aEntryClip.valueText.textAutoSize = "shrink";
		if (aEntryObject.text != undefined) {
			aEntryClip.valueText.SetText(aEntryObject.value.toString());
			return;
		}
		aEntryClip.valueText.SetText(" ");
	}	
}
