
class TextFieldContainer extends MovieClip
{
	private var descText:TextField;
	private var initialFontSize:Number=20;
	
	function TextFieldContainer()
	{
		super();
	}

	function onLoad()
	{
		super.onLoad();
		initialFontSize = descText.getTextFormat().size;
	}

	// Wrapper to override the Saleform multiline shrinking, because it's broken
	function set text(textValue):Void 
	{
		descText.text = textValue;
		ShrinkToFit(descText);
	}
	
	function get text():String 
	{
		return descText.text;
	}	
	
		// The Scaleform Extension is broken for center justify for Shrink so I rolled my own
	private function ShrinkToFit(tf:TextField):Void
	{
		tf.multiline = true;
		tf.wordWrap = true
		var tfText:String = tf.htmlText;
		var fontSize:Number = initialFontSize;
		var htmlSize = "SIZE=\"" + fontSize.toString() + "\""
		tf.textAutoSize = "none";
		tf.SetText(tfText, true);
		tf.textAutoSize = "none";
		var tfHeight:Number = tf.getLineMetrics(0).height * tf.numLines;
		while (tfHeight > tf._height && fontSize > 5)
		{
			var beforeHtmlSize = "SIZE=\"" + fontSize.toString() + "\"";
			fontSize -= 1;
			htmlSize = "SIZE=\"" + fontSize.toString() + "\"";
			tfText = stringReplace(tfText, beforeHtmlSize, htmlSize);
			tf.textAutoSize = "none";
			tf.SetText(tfText, true);
			tf.textAutoSize = "none";
			tfHeight = tf.getLineMetrics(0).height * tf.numLines;
		}
	}
	
	private function stringReplace(block:String, findStr:String, replaceStr:String):String
	{
		return block.split(findStr).join(replaceStr);
	}
	
}
