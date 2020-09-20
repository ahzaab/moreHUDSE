import gfx.controls.ScrollBar;

class PerksList extends StatsList
{
	private static var STATE_KNOWN:Number = 0;
	private static var STATE_AVAILABLE:Number = 1;
	private static var STATE_UNAVAILABLE:Number = 2;
	
	function PerksList()
	{
		super();
	}

	public function set dataProvider(elements:Array): Void
	{
		this.ClearList();
		this.scrollPosition = 0;
		for (var i: Number = 0; i < elements.length; i++)
		{
			this.entryList.push({text:elements[i].text, state:elements[i].state});
		}
		this.InvalidateData();
	}
	
	/* Override */
	function SetEntry(aEntryClip: MovieClip, aEntryObject: Object): Void
	{		
		if (aEntryClip != undefined) {
			if (aEntryObject == selectedEntry){
				switch (aEntryObject.state)
				{
					case STATE_UNAVAILABLE:
					{
						aEntryClip.gotoAndStop("Selected_unavail");
					}
					break;
					case STATE_AVAILABLE:
					{
						aEntryClip.gotoAndStop("Selected_avail");
					}
					break;	
					case STATE_KNOWN:
					{
						aEntryClip.gotoAndStop("Selected_known");
					}
					break;
					default:
					{
						aEntryClip.gotoAndStop("Selected");
					}
					break;
				}
			}
			else {
				switch (aEntryObject.state)
				{
					case STATE_UNAVAILABLE:
					{
						aEntryClip.gotoAndStop("Normal_unavail");
					}
					break;
					case STATE_AVAILABLE:
					{
						aEntryClip.gotoAndStop("Normal_avail");
					}
					break;	
					case STATE_KNOWN:
					{
						aEntryClip.gotoAndStop("Normal_known");
					}
					break;
					default:
					{
						aEntryClip.gotoAndStop("Normal");
					}
					break;
				}				
			}
			SetEntryText(aEntryClip, aEntryObject);
		}
	}
}
