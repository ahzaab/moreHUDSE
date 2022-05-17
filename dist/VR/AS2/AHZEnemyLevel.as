import flash.display.BitmapData;
import gfx.io.GameDelegate;
import Shared.GlobalFunc;
import skyui.util.Debug;
import flash.geom.Transform;
import flash.geom.ColorTransform;
import flash.geom.Matrix;
import flash.filters.DropShadowFilter;

class AHZEnemyLevel extends MovieClip
{
	//Widgets 
	public var EnemyLevel_mc:MovieClip;
	public var txtMeasureInstance:TextField;
	public var SoulLevelInstance:TextField;
	public var AHZEnemyLevel_mc:MovieClip;
	// Public vars
	public var prevEnemyPercent:Number;

	// Options
	private var showEnemyLevel:Boolean;
	private var showEnemyLevelMax:Number;
	private var showEnemyLevelMin:Number;
	private var showEnemySoulLevel:Boolean;
		
	// private variables
	private var savedEnemyTextInfo:String;
	private var savedEnemyHtmlTextInfo:String;
	private var savedEnemyLevelValue:String;
	private var savedEnemyLevelNumber:Number = 0;
	private var savedPlayerLevelNumber:Number = 0;
	private var tempfunc:Function;
	private var firstRun:Boolean = true;
	
	// Rects
	private var maxXY:Object;
	private var minXY:Object;
	
	
	// Statics
	private static var hooksInstalled:Boolean = false;

	/* INITIALIZATION */
	
	public function AHZEnemyLevel()
	{
		super();
		firstRun = true;
		// Changing scale to compensate for the size change of the nif
		_root.VREnemyMetersInstance._yscale = _root.VREnemyMetersInstance._yscale * 0.565289527; //0.434710473; //0.565289527;
		_root.VREnemyMetersInstance._xscale = _root.VREnemyMetersInstance._xscale * 0.769005;

		var mc:MovieClip = MovieClip(_root.VREnemyMetersInstance.EnemyHealth_mc.BracketsInstance);
		AHZEnemyLevel_mc = mc.duplicateMovieClip("AHZEnemyLevel_mc", this.getNextHighestDepth());
		_root.VREnemyMetersInstance.EnemyHealth_mc.BracketsInstance._y = 1000;
		AHZEnemyLevel_mc.gotoAndStop(100);
		AHZEnemyLevel_mc.RolloverNameInstance.textAutoSize="shrink";	
		SoulLevelInstance.textAutoSize="shrink";
		firstRun = false;

		if (! hooksInstalled)
		{	
			// Apply hooks to hook events
			//hookFunction(_root.VREnemyMetersInstance.EnemyHealthMeter,"Update",this,"Update");
			_global.skse.plugins.AHZmoreHUDPlugin.InstallHooks();
			hooksInstalled = true;
		}
			
		// Initialize variables
		showEnemyLevel = false;
		showEnemyLevelMax = 10;
		showEnemyLevelMin = 10;
		showEnemySoulLevel = false;
	}
	
	function appendHtmlToEnd(htmlText:String, appendedHtml:String):String
	{
		var stringIndex:Number;
		stringIndex = htmlText.lastIndexOf("</P></TEXTFORMAT>");
		var firstText:String = htmlText.substr(0,stringIndex);
		var secondText:String = htmlText.substr(stringIndex,htmlText.length - stringIndex);						
        return firstText + appendedHtml + secondText;
	}

	function appendImageToEnd(textField:TextField, imageName:String, width:Number, height:Number)
	{
		if (textField.text.indexOf("[" + imageName + "]") < 0)
		{
			var b1 = BitmapData.loadBitmap(imageName); 
			if (b1)
			{
				var a = new Array; 
				a[0] = { subString:"[" + imageName + "]", image:b1, width:width, height:height, id:"id" + imageName };  //baseLineY:0, 
				textField.setImageSubstitutions(a);
				textField.htmlText = 
				appendHtmlToEnd(textField.htmlText, " " + "[" + imageName + "]");
			}
		}
	}
	
	function interpolate(pBegin:Number, pEnd:Number, pMax:Number, pStep:Number):Number {
		return pBegin + Math.floor((pEnd - pBegin) * pStep / pMax);
	}

	function measureStringWidth(str:String):Number {
		txtMeasureInstance._alpha = 0;
    	txtMeasureInstance.text = str;
    	return txtMeasureInstance.textWidth;
	}

	function adjustBracketWidth():Void 
	{
		var widthValue:Number = 0.66;
		widthValue = widthValue * AHZEnemyLevel_mc.RolloverNameInstance.textWidth;
		widthValue = Math.floor(widthValue) + 5;
		widthValue = Math.min(100, Math.max(0, widthValue));
		
		AHZEnemyLevel_mc.gotoAndStop(widthValue);
	}

	function updateDisplayText():Void
	{
		// Could use the extension method SetText, but just to be sure
		if (_root.VREnemyMetersInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.html){
			AHZEnemyLevel_mc.RolloverNameInstance.html = true;
			AHZEnemyLevel_mc.RolloverNameInstance.htmlText = _root.VREnemyMetersInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.htmlText;
		}
		else{
			AHZEnemyLevel_mc.RolloverNameInstance.html = false;
			AHZEnemyLevel_mc.RolloverNameInstance.text = _root.VREnemyMetersInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.text;
		}
	}

	function onEnterFrame()
    {		
		AHZEnemyLevel_mc._alpha = _root.VREnemyMetersInstance.EnemyHealth_mc.BracketsInstance._alpha;		
		this._alpha = _root.VREnemyMetersInstance._alpha;
		if (!AHZEnemyLevel_mc._alpha)
		{
			SoulLevelInstance._alpha = 0;
		}
		
		if (!_root.VREnemyMetersInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.text ||
			!_root.VREnemyMetersInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.text.length)		
			{
				// I f the text is empty then it was forced by this mod, indicating that no update
				// from the engine was made, so leave right away.  We don't want to be updating in every frame
				return;
			}
			
		var outData:Object = {outObj:Object};
	
		if (showEnemySoulLevel && _root.VREnemyMetersInstance.EnemyHealth_mc.BracketsInstance._alpha)
		{
			_global.skse.plugins.AHZmoreHUDPlugin.GetEnemyInformation(outData, "");
			if (outData && outData.outObj && outData.outObj.Soul){
				SoulLevelInstance._alpha = _root.VREnemyMetersInstance.EnemyHealth_mc.BracketsInstance._alpha;
				SoulLevelInstance.text = outData.outObj.Soul;
			}
			else
			{
				SoulLevelInstance._alpha = 0;
			}
		}
		else
		{
			SoulLevelInstance._alpha = 0;
			// stting to null will flag the code further down to read the data again for the level
			outData = null;
		}
	
		// This function is hooked and gets fired every frame
		if (_root.VREnemyMetersInstance.EnemyHealth_mc.BracketsInstance._alpha && showEnemyLevel)
		{			
			//_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("T");
			var levelText:String;
			// If the data was not aquired from reading the soul level then read it here
			if (!outData)
			{
				outData = {outObj:Object};
				_global.skse.plugins.AHZmoreHUDPlugin.GetEnemyInformation(outData, "");			
			}
			
			if (outData && outData.outObj)
			{				
				savedEnemyLevelNumber = outData.outObj.EnemyLevel;
				savedPlayerLevelNumber = outData.outObj.PlayerLevel;
			}
											
			if (showEnemyLevelMax && showEnemyLevelMin && savedEnemyLevelNumber && savedPlayerLevelNumber)
			{	

				// Get the delta of level from player
				var deltaLevelFromPlayer = savedEnemyLevelNumber-savedPlayerLevelNumber;
				var maxPercent:Number = showEnemyLevelMax;
				var minPercent:Number = showEnemyLevelMin * -1.0;
							
				var R:Number;
				var G:Number;
				var B:Number;
				var RGB:Number;
				var fontColor:String;
				if (deltaLevelFromPlayer < 0){
					if (deltaLevelFromPlayer < minPercent)
					{
						deltaLevelFromPlayer = minPercent;
					}
					
					// Start with the same green that is used throughout the menus
					R = interpolate(0xFF,0x18,minPercent, deltaLevelFromPlayer);
					G = interpolate(0xFF,0x95,minPercent, deltaLevelFromPlayer);
					B = interpolate(0xFF,0x15,minPercent, deltaLevelFromPlayer);
					RGB = (R * 65536) + (G * 256) + B;
					fontColor = RGB.toString(16);
				}
				else if (deltaLevelFromPlayer > 0){
					if (deltaLevelFromPlayer > maxPercent)
					{
						deltaLevelFromPlayer = maxPercent;
					}
					R = interpolate(0xFF,0xFF,maxPercent, deltaLevelFromPlayer);
					G = interpolate(0xFF,0x00,maxPercent, deltaLevelFromPlayer);
					B = interpolate(0xFF,0x00,maxPercent, deltaLevelFromPlayer);
					RGB = (R * 65536) + (G * 256) + B;
					fontColor = RGB.toString(16);						
				}
				else
				{
					fontColor = "FFFFFF";
				}
			
				
				levelText = " (<font color=\'#" + fontColor + "\'>" + savedEnemyLevelNumber.toString() + "</font>)";
				_root.VREnemyMetersInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.html = true;
				// Append the level
				AHZEnemyLevel_mc.RolloverNameInstance.html = true;
				AHZEnemyLevel_mc.RolloverNameInstance.htmlText = 
					appendHtmlToEnd(_root.VREnemyMetersInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.htmlText, levelText);
				adjustBracketWidth();
			}
			// No coloring, turn off html
			else if (savedEnemyLevelNumber)
			{
				AHZEnemyLevel_mc.RolloverNameInstance.html = false;
				_root.VREnemyMetersInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.html = false;
				AHZEnemyLevel_mc.RolloverNameInstance.text = _root.VREnemyMetersInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.text + " (" + savedEnemyLevelNumber.toString() + ")";	
				adjustBracketWidth();
			}
			else
			{
				AHZEnemyLevel_mc.gotoAndStop(_root.VREnemyMetersInstance.EnemyHealth_mc.BracketsInstance._currentframe);
				updateDisplayText();
			}
		}
		else if (_root.VREnemyMetersInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.text &&
				 _root.VREnemyMetersInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.text.length)
		{
			AHZEnemyLevel_mc.gotoAndStop(_root.VREnemyMetersInstance.EnemyHealth_mc.BracketsInstance._currentframe);
			updateDisplayText();
		}
	
		_root.VREnemyMetersInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.htmlText = "";	
		_root.VREnemyMetersInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.html = false;	
		_root.VREnemyMetersInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.text = "";	
	}
	// @override WidgetBase
	public function onLoad():Void
	{
		super.onLoad();
	}
	// @Papyrus
	public function updateSettings(showEnemyLevelValue:Number,
								   showEnemyLevelMaxValue:Number,
								   showEnemyLevelMinValue:Number,
								   showEnemySoulLevelValue:Number):Void 
	{				
		showEnemyLevel = (showEnemyLevelValue>=1);
		showEnemyLevelMax = showEnemyLevelMaxValue;
		showEnemyLevelMin = showEnemyLevelMinValue;
		showEnemySoulLevel = (showEnemySoulLevelValue>=1);
	}

	public static function hookFunction(a_scope:Object, a_memberFn:String, a_hookScope:Object, a_hookFn:String):Boolean {
		var memberFn:Function = a_scope[a_memberFn];
		if (memberFn == null || a_scope[a_memberFn] == null) {
			return false;
		}

		a_scope[a_memberFn] = function () {
			memberFn.apply(a_scope,arguments);
			a_hookScope[a_hookFn].apply(a_hookScope,arguments);
		};
		return true;
	}
}