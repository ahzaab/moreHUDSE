import flash.utils.*;
import gfx.events.EventDispatcher;
import flash.display.BitmapData;

class AHZIconContainer
{
  /* CONSTANTS */
  	private static var MAX_CONCURRENT_ICONS:Number = 16;
	//private static var ICON_WIDTH:Number = 20;
	//private static var ICON_HEIGHT:Number = 20;
	private static var MIN_ICON_SIZE:Number = 16;
	private static var ICON_XOFFSET:Number = -5;
	private static var ICON_YOFFSET:Number = -5;

	/* Static */
	private static var eventObject: Object;
  	private static var managerSetup:Boolean = false;		
		
  /* INITIALIATZION */
  
  	public var IconContainer_mc:MovieClip;
  	private var iconLoader:MovieClipLoader;
  	private var loadedIcons:Array;
	private var loadedClips:Array;
    private var _imageSubs:Array;
	private var loadedIconNames:Array;
  	private var _tf:TextField;
  	private var _lastX:Number;
	private var _textFormat:TextFormat;
	private var _iconSize:Number;
	private var _iconScale:Number;
	
  	public function AHZIconContainer()
	{	
	}

	
	public function Load(textField:TextField, 
						 s_filePath:String, 
						 a_scope: Object, 
						 a_loadedCallBack: String, 
						 a_errorCallBack: String,
						 a_size:Number,
						 a_scale:Number):Void
	{		
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("~ LOAD ~ '" + s_filePath + "'" , false);
		//_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("~ LOAD ~ '" + s_filePath + "'" );
		if (managerSetup){
			return;
		}
		_iconSize = a_size;
		if (_iconSize < MIN_ICON_SIZE)
		{
			_iconSize = MIN_ICON_SIZE;
		}
		_iconScale = a_scale;
		if (!_iconScale)
		{
			_iconScale = 1.0;
		}
		loadedClips = new Array();
		loadedIcons = new Array();
		loadedIconNames = new Array();
		_imageSubs = new Array();		
		managerSetup = true;
		eventObject = {};
		EventDispatcher.initialize(eventObject);
		eventObject.addEventListener("iconsLoaded", a_scope, a_loadedCallBack);
		eventObject.addEventListener("iconLoadError", a_scope, a_errorCallBack);		
		_tf = textField;
		_textFormat = _tf.getTextFormat();
		for (var i:Number = 0; i < MAX_CONCURRENT_ICONS; i++)
		{
			var clip = MovieClip(_tf._parent).createEmptyMovieClip("clip" + i, MovieClip(_tf._parent).getNextHighestDepth());
			clip._y = _tf._y;
			clip._x = _tf._x;
			loadedClips.push(clip);
			iconLoader = new MovieClipLoader();
			iconLoader.addListener(this);
			iconLoader.loadClip(s_filePath, clip);
		}
	}

	public function appendImage(a_imageName:String):Void
	{		
		// Cannot load any more icons
		if (loadedIconNames.length >= MAX_CONCURRENT_ICONS)
		{
			return;
		}
	
		// check for duplicates
		for (var i:Number = 0; i < loadedIconNames.length; i++) {
			if (loadedIconNames[i] == a_imageName)
			{
				return;
			}
		}
	
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("~AppendImage~ " + a_imageName, false);
		var loadedImage:BitmapData;
		if (loadedIcons.length)
		{
			_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("Append Dummy");
	 		loadedImage = BitmapData.loadBitmap("ahzEmpty");
		}
		
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("loadedImage: " + loadedImage, false);
		if (loadedImage)
		{
			if (loadedIcons.length || !GetImageSub(a_imageName))   // Already exists
			{
				_imageSubs.push({ subString:"[" + a_imageName + "]", image:loadedImage, width:(_iconSize * _iconScale), height:20, id:"id" + a_imageName });
			}	
		}
				
		if (_imageSubs.length)
		{			
			// get the line metrics before addeding the next image
			var currentLineMetrics = _tf.getLineMetrics(1);
			if (_tf.html) 
			{
				_tf.appendHtml(getDefaultHtml("[" + a_imageName + "]"));
			}
			else
			{
				_tf.text += "[" + a_imageName + "]";
			}			
			
			_tf.setImageSubstitutions(_imageSubs);
			
			if (loadedIcons.length){	
				
				// Goto the frame including the icon 
				loadedIcons[loadedIconNames.length].gotoAndStop(a_imageName);
				_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("GOTO: " + a_imageName);
				loadedIcons[loadedIconNames.length]._quality = "BEST";
				loadedIcons[loadedIconNames.length]._height = (_iconSize * _iconScale);
				loadedIcons[loadedIconNames.length]._width = (_iconSize * _iconScale);
				loadedIcons[loadedIconNames.length]._x = (currentLineMetrics.x + currentLineMetrics.width) + ICON_XOFFSET + _tf._x ;
				loadedIcons[loadedIconNames.length]._y = (_tf._y + _tf._height) - (_iconSize * _iconScale) + ICON_YOFFSET;
				loadedIcons._alpha = _tf._alpha;
				
				// preposition the previously added icons
				updatePosition();
				
				loadedIconNames.push(a_imageName);
			}
		}	
	}	
	
	public function Show():Void
	{
		this._alpha = _tf._alpha;
	}
	
	public function Hide():Void
	{
		this._alpha = 0;
	}
	
	public function Reset(a_size:Number):Void
	{
		_iconSize = a_size;
		if (_iconSize < MIN_ICON_SIZE)
		{
			_iconSize = MIN_ICON_SIZE;
		}		
		
		// re-ackquire the text field forms
		_textFormat = _tf.getTextFormat();
		
		// CLear subsitutions in the text field
		_tf.setImageSubstitutions(null);
		
		// reset the parent movie clips
        for (var i:Number = 0; i < loadedClips.length; i++)
		{
			loadedClips[i]._x = _tf._x;
			loadedClips[i]._y = _tf._y;
		}
		
		//reset all movie clips to their default positions
		for (var i:Number = 0; i < loadedIcons.length; i++)
		{
			loadedIcons[i].gotoAndStop("ahzEmpty");
			loadedIcons[i]._x = 0;
			loadedIcons[i]._y = 0;
		}				
		
		// Hide all visible movies
		this._alpha = 0;	
		
		// Reset the load image names
		delete loadedIconNames;
		loadedIconNames = new Array();
		
		// Reset substitutions
		delete _imageSubs;
		_imageSubs = new Array();		
		
		_lastX = (_tf.getLineMetrics(0).x + _tf._x);
	}
	
	public function set _alpha(alphaValue:Number):Void{
		for (var i = 0; i < MAX_CONCURRENT_ICONS; i++)
		{
			loadedIcons[i]._alpha = alphaValue;
		}			
	}
	
	private function appendHtml(newHtml:String):Void
	{
		_tf.appendHtml(newHtml);
		updatePosition();
	}
	
	private function updatePosition ():Void {
		
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("    ~updatePosition~ ", false);
		if (!loadedIcons.length)
		{
			return;
		}
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("    ~updatePosition Icons Loaded~ ", false);
						
		var newLineMetrics = _tf.getLineMetrics(1);
		var xDelta = _lastX - (newLineMetrics.x + _tf._x);
		for (var i = 0; i < loadedIconNames.length; i++)
		{
			//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("old loadedIcons["+i+"]._x: " + loadedIcons[i]._x, false);
			loadedIcons[i]._x = loadedIcons[i]._x - (xDelta);
			//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("new loadedIcons["+i+"]._x: " + loadedIcons[i]._x, false);
			loadedIcons[i]._y = (_tf._y + _tf._height) - (_iconSize * _iconScale) + ICON_YOFFSET;
		}		
		_lastX = (newLineMetrics.x + _tf._x);
	}
								
	private function onLoadInit(a_mc: MovieClip): Void
	{		
		//_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("~ onLoadInit ~ " + loadedIcons.length.toString());
		a_mc._quality = "BEST";
		a_mc.gotoAndStop("ahzEmpty");		
		loadedIcons.push(a_mc);
		if (loadedIcons.length == MAX_CONCURRENT_ICONS)
		{
			//_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("~ dispatchEvent ~ iconsLoaded");
			eventObject.dispatchEvent({type: "iconsLoaded", tf: this});	
		}
	}
	
	private function onLoadError(a_mc:MovieClip, a_errorCode: String): Void
	{
		//_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("~ onLoadError ~ ");
		eventObject.dispatchEvent({type: "iconLoadError", error: a_errorCode});	
	}
	
	private function GetImageSub(a_imageName:String):Object
	{
		var i:Number;
        for (i = 0; i < _imageSubs.length; i++)
		{
			if (_imageSubs[i].subString && _imageSubs[i].subString == "[" + a_imageName + "]")
			{
				return _imageSubs[i];
			}
		}
		
		return null;
	}

	private function getDefaultHtml(textValue:String):String
	{
		var returnValue = "<font face=\'"+_textFormat.font+"\' size=\'"+_textFormat.size+"\' color=\'#"+_textFormat.color.toString(16)+"\'>" + textValue + "</font>";
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("returnValue: " + returnValue, false);
		return returnValue;
	}
	
	
}