import Shared.GlobalFunc;
import gfx.controls.Button;
import skyui.util.Translator;

class UnlockButtonHolder extends MovieClip
{
	public var button:Button;
	private static var SELECTION_INDICATOR_WIDTH: Number = 50;
	private static var textValue:String = "$Unlock"
	function UnlockButtonHolder()
	{
		super();
	}

	function onLoad()
	{
		super.onLoad();
		
		// Code pulled from the message box menu to format the button
		
		_global.gfxExtensions = true;
		Selection.alwaysEnableArrowKeys = true;
		Selection.alwaysEnableKeyboardPress = true;
		Selection.disableFocusAutoRelease = true;
		Shared.GlobalFunc.MaintainTextFormat();
		var buttonText:TextField = button.textField;
		buttonText.autoSize = "center";
		buttonText.verticalAlign = "center";
		buttonText.verticalAutoSize = "center";
		buttonText.html = true;
		//trace("button :" + button);
		//trace("button.ButtonText :" + button.ButtonText);
		//trace("Setting the button text to :" + Translator.translate(textValue));
		var unlock:String = Translator.translate(textValue);
		buttonText.SetText(unlock, true);
		button.SelectionIndicatorHolder.SelectionIndicator._width = buttonText._width + SELECTION_INDICATOR_WIDTH;
		button.SelectionIndicatorHolder.SelectionIndicator._y = buttonText._y + buttonText._height / 2;
		buttonText.noTranslate = true;		
		button.label = unlock;
	}

}
