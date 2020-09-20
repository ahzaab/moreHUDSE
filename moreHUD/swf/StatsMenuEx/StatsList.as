import gfx.managers.FocusHandler;
import skyui.util.Translator;
import gfx.controls.ScrollBar;

class StatsList extends Shared.BSScrollingList
{
	public var selected:MovieClip;
	public var _focused:Number;
	var scrollbar: ScrollBar;
	
	function StatsList()
	{
		super();
		_focused = 0;
		
		if (scrollbar){
			scrollbar.focusTarget = this;
		}
		
	}
	
	public function onLoad(): Void
	{
		super.onLoad();
		this.selected._alpha = 0;
	}	
	
	// Override
	function get focused(): Number
	{
		return _focused;
	}	
	
	// Override
	function set focused(value): Void
	{
		if (_focused == value){
			return;
		}
		
		// On focus, also increase the alpha of the parent container
		if (value != 0)
		{
			FocusHandler.instance.setFocus(this, 0);
			this._parent._alpha = 100;
		}
		else
		{
			this._parent._alpha = 75;
		}
		_focused = value;
	}
	
	// Override
	function doSetSelectedIndex(aiNewIndex:Number, aiKeyboardOrMouse: Number): Void
	{
		super.doSetSelectedIndex(aiNewIndex, aiKeyboardOrMouse);
		
		// Forces Focus when an item is selected
		if (aiNewIndex != -1){
			focused = 1;	
		}
	}

	public function set scrollBar(dummy:Object): Void
	{
		
	}

	public function set dataProvider(elements:Array): Void
	{
		this.ClearList();
		this.scrollPosition = 0;
		this.selectedIndex = -1;
		for(var i: Number = 0; i < elements.length; i++) {
			this.entryList.push({text: Translator.translate(elements[i])});
		}
		this.InvalidateData();
	}
}
