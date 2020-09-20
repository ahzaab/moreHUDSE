import RightClickList;
import gfx.controls.Button;
import gfx.controls.DropdownMenu;
import gfx.controls.ScrollingList;
import gfx.controls.Slider;
import gfx.controls.TextInput;
import gfx.events.EventTypes;
import gfx.io.GameDelegate;


class GlobalListener extends MovieClip
{
	/* PRIVATE VARIABLES */

	private var _available : ScrollingList;
	private var _selected: RightClickList;
	private var _range: DropdownMenu;
	private var _type: DropdownMenu;
	private var _magnitude: Slider;
	private var _duration: Slider;
	private var _area: Slider;
	private var _nameInput: TextInput;
	private var _craft: Button;


	/* INITIALIZATION */

	public function GlobalListener()
	{
		super();
	}


	public function onLoad(): Void
	{
		_available = _root.available;
		if (_available != undefined) {
			_available.addEventListener(EventTypes.ITEM_PRESS, this, "handleAvailablePress");
		} else {
			log("could not get available");
		}

		_selected = _root.selected;
		if (_selected != undefined) {
			_selected.addEventListener(EventTypes.ITEM_PRESS, this, "handleSelectedPress");
		} else {
			log("could not get selected");
		}
		
		_type = _root.type;
		if (_type == undefined) {
			log("could not get type");
		}

		_range = _root.range;
		if (_range == undefined) {
			log("could not get range");
		}

		_magnitude = _root.magnitude;
		if (_magnitude != undefined) {
			_magnitude.addEventListener(EventTypes.CHANGE, this, "handleMagnitudeChange");
			_magnitude.thumb.addEventListener(EventTypes.PRESS, this, "handleMagnitudeDragBegin");
			_magnitude.thumb.addEventListener(EventTypes.CLICK, this, "handleMagnitudeDragEnd");
			_magnitude.thumb.addEventListener(EventTypes.RELEASE_OUTSIDE, this, "handleMagnitudeDragEnd");
		} else {
			log("could not get magnitude");
		}

		_duration = _root.duration;
		if (_duration != undefined) {
			_duration.addEventListener(EventTypes.CHANGE, this, "handleDurationChange");
			_duration.thumb.addEventListener(EventTypes.PRESS, this, "handleDurationDragBegin");
			_duration.thumb.addEventListener(EventTypes.CLICK, this, "handleDurationDragEnd");
			_duration.thumb.addEventListener(EventTypes.RELEASE_OUTSIDE, this, "handleDurationDragEnd");
		} else {
			log("could not get duration");
		}

		_area = _root.area;
		if (_area != undefined) {
			_area.addEventListener(EventTypes.CHANGE, this, "handleAreaChange");
			_area.thumb.addEventListener(EventTypes.PRESS, this, "handleAreaDragBegin");
			_area.thumb.addEventListener(EventTypes.CLICK, this, "handleAreaDragEnd");
			_area.thumb.addEventListener(EventTypes.RELEASE_OUTSIDE, this, "handleAreaDragEnd");
		} else {
			log("could not get area");
		}

		_nameInput = _root.name;
		if (_nameInput != undefined) {
			// working on it...
			//_nameInput.maxChars = 50;
			//_nameInput.defaultText = "Name your spell";
			_nameInput.addEventListener(EventTypes.TEXT_CHANGE, this, "handleTextChange");
			log("added text change listener");
			log("" + _nameInput.defaultText + " " + String(_nameInput.disabled)+" "+ String(_nameInput.editable) +" "+  String(_nameInput.enabled));
			log("" + _nameInput.text);
			_nameInput.text = "bruh";
			log("" + _nameInput.text);
		} else {
			log("could not get name");
		}

		_craft = _root.craft;
		if (_craft != undefined) {
			_craft.addEventListener(EventTypes.PRESS, this, "handleCraftPress");
		} else {
			log("could not get craft");
		}
		log("loaded");
	}


	/* PRIVATE FUNCTIONS */
	private function handleTextChange(a_event: Object): Void
	{
		log("text changed");
		GameDelegate.call("OnTextChange", [_nameInput.text]);
	}

	private function handleAvailablePress(a_event: Object): Void
	{
		var index: Number = a_event.index;
		GameDelegate.call("OnAvailablePress", [index]);
	}


	private function handleSelectedPress(a_event: Object): Void
	{
		var index: Number = a_event.index;

		var button: Number = a_event.button;
		var remove: Boolean = button != undefined && button == 1;

		GameDelegate.call("OnSelectedPress", [index, remove]);
	}


	private function handleMagnitudeChange(a_event: Object): Void
	{
		GameDelegate.call("OnMagnitudeChange", []);
	}


	private function handleMagnitudeDragBegin(a_event: Object): Void
	{
		GameDelegate.call("OnMagnitudeDragBegin", []);
	}


	private function handleMagnitudeDragEnd(a_event: Object): Void
	{
		GameDelegate.call("OnMagnitudeDragEnd", []);
	}


	private function handleDurationChange(a_event: Object): Void
	{
		GameDelegate.call("OnDurationChange", []);
	}


	private function handleDurationDragBegin(a_event: Object): Void
	{
		GameDelegate.call("OnDurationDragBegin", []);
	}


	private function handleDurationDragEnd(a_event: Object): Void
	{
		GameDelegate.call("OnDurationDragEnd", []);
	}


	private function handleAreaChange(a_event: Object): Void
	{
		GameDelegate.call("OnAreaChange", []);
	}


	private function handleAreaDragBegin(a_event: Object): Void
	{
		GameDelegate.call("OnAreaDragBegin", []);
	}


	private function handleAreaDragEnd(a_event: Object): Void
	{
		GameDelegate.call("OnAreaDragEnd", []);
	}


	private function handleCraftPress(a_event: Object): Void
	{
		GameDelegate.call("CraftSpell", []);
	}


	private function log(a_message: String): Void
	{
		GameDelegate.call("Log", [a_message]);
	}
}
