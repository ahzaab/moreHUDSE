import gfx.controls.ScrollingList;


class RightClickList extends ScrollingList
{
	/* INITIALIZATION */

	public function RightClickList()
	{
		super();
	}


	/* PRIVATE FUNCTIONS */

	// @override ScrollingList
	private function dispatchItemEvent(event: Object): Void
	{
		var type: String;
		switch (event.type) {
			case "press":
				type = "itemPress";
				break;
			case "click":
				type = "itemClick";
				break;
			case "rollOver":
				type = "itemRollOver";
				break;
			case "rollOut":
				type = "itemRollOut";
				break;
			case "doubleClick":
				type = "itemDoubleClick";
				break;
			default:
				return;
		}

		var newEvent: Object = {
			target: this,
			type: type,
			item: event.target.data,
			renderer: event.target,
			index: event.target.index,
			controllerIdx: event.controllerIdx,
			button: event.button
		};

		dispatchEventAndSound(newEvent);
	}
}
