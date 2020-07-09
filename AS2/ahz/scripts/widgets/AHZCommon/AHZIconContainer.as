import flash.utils.*;
import gfx.events.EventDispatcher;
import flash.display.BitmapData;

class AHZIconContainer
{
  /* CONSTANTS */
  	private static var MAX_CONCURRENT_ICONS:Number = 8;
	private static var ICON_WIDTH:Number = 20;
	private static var ICON_HEIGHT:Number = 20;
	private static var ICON_XOFFSET:Number = 3;
	private static var ICON_YOFFSET:Number = 3;

	/* Static */
	private static var eventObject: Object;
  	private static var managerSetup:Boolean = false;		
		
  /* INITIALIATZION */
  
  	public var IconContainer_mc:MovieClip;
  	private var iconLoader:MovieClipLoader;
  	private var loadedIcons:Array;
	private var loadedClips:Array;
    private var _imageSubs:Array;
	private var _currentImageIndex:Number;
  	private var _tf:TextField;
  	private var _lastX:Number;
	private var _textFormat:TextFormat;
	
	public function Reset():Void
	{
		_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("~ Reset Start ~ ");
		_textFormat = _tf.getTextFormat();
		_tf.setImageSubstitutions(null);
_tf.border = true;
        for (var i:Number = 0; i < loadedClips.length; i++)
		{
			loadedClips[i]._x = _tf._x;
			loadedClips[i]._y = _tf._y;
		}
		
		for (var i:Number = 0; i < loadedIcons.length; i++)
		{
			loadedIcons[i].gotoAndStop("ahzEmpty");
			loadedIcons[i]._x = 0;
			loadedIcons[i]._y = 0;
		}				
		this._alpha = 0;	
		_currentImageIndex = 0;
		_imageSubs = new Array();		
		_lastX = (_tf.getLineMetrics(0).x + _tf._x);
		_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("~ Reset End ~ ");
	}
			
	public function set _alpha(alphaValue:Number):Void{
		for (var i = 0; i <= _currentImageIndex; i++)
		{
			loadedIcons[i]._alpha = alphaValue;
		}			
	}
		

  	public function AHZIconContainer()
	{	
	}
	
	function appendHtml(newHtml:String):Void
	{
		_tf.appendHtml(newHtml);
		updatePosition();
	}
	
	function updatePosition ():Void {
		
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("    ~updatePosition~ ", false);
		if (!loadedIcons.length)
		{
			return;
		}
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("    ~updatePosition Icons Loaded~ ", false);
						
		var newLineMetrics = _tf.getLineMetrics(1);
		var xDelta = _lastX - (newLineMetrics.x + _tf._x);
		for (var i = 0; i < _currentImageIndex; i++)
		{
			//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("old loadedIcons["+i+"]._x: " + loadedIcons[i]._x, false);
			loadedIcons[i]._x = loadedIcons[i]._x - (xDelta);
			//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("new loadedIcons["+i+"]._x: " + loadedIcons[i]._x, false);
			loadedIcons[_currentImageIndex]._y = (_tf._y + _tf._height) - newLineMetrics.height + ICON_YOFFSET;
		}		
		_lastX = (newLineMetrics.x + _tf._x);
	}
	
	public function Load(textField:TextField, s_filePath:String, a_scope: Object, a_loadedCallBack: String, a_errorCallBack: String):Void
	{		
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("~ LOAD ~ '" + s_filePath + "'" , false);
		_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("~ LOAD ~ '" + s_filePath + "'" );
		if (managerSetup){
			return;
		}
		loadedClips = new Array();
		loadedIcons = new Array();
		_imageSubs = new Array();		
		managerSetup = true;
		_currentImageIndex = 0;
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
								
	public function onLoadInit(a_mc: MovieClip): Void
	{		
		_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("~ onLoadInit ~ " + loadedIcons.length.toString());
		a_mc._quality = "BEST";
		a_mc.gotoAndStop("ahzEmpty");		
		loadedIcons.push(a_mc);
		if (loadedIcons.length == MAX_CONCURRENT_ICONS)
		{
			_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("~ dispatchEvent ~ iconsLoaded");
			eventObject.dispatchEvent({type: "iconsLoaded", tf: this});	
		}
	}
	
	public function onLoadError(a_mc:MovieClip, a_errorCode: String): Void
	{
		_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("~ onLoadError ~ ");
		eventObject.dispatchEvent({type: "iconLoadError", error: a_errorCode});	
	}
	
	public function GetImageSub(a_imageName:String):Object
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
	
	function appendImage(a_imageName:String):Void
	{		
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("~AppendImage~ " + a_imageName, false);
		var loadedImage:BitmapData;
		if (loadedIcons.length)
		{
			_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("Append Dummy");
	 		loadedImage = BitmapData.loadBitmap("ahzEmpty");
		}
		else
		{		
			//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("OLD LOADING", false);
			_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("OLD LOADING");
			loadedImage = BitmapData.loadBitmap(a_imageName);
		}
		
		//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("loadedImage: " + loadedImage, false);
		if (loadedImage)
		{
			if (loadedIcons.length || !GetImageSub(a_imageName))   // Already exists
			{
				_imageSubs.push({ subString:"[" + a_imageName + "]", image:loadedImage, width:ICON_WIDTH, height:ICON_WIDTH, id:"id" + a_imageName });
			}	
		}
				
		if (_imageSubs.length)
		{
			//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("_tf.html: " + _tf.html, false);
			
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
				//_global.skse.plugins.AHZmoreHUDInventory.AHZLog("SHIFT WITH THIS VALUE: " + _tf.text, false);
				loadedIcons[_currentImageIndex].gotoAndStop(a_imageName);
				_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("GOTO: " + a_imageName);
				loadedIcons[_currentImageIndex]._quality = "BEST";
				loadedIcons[_currentImageIndex]._height = ICON_HEIGHT;
				loadedIcons[_currentImageIndex]._width = ICON_WIDTH;
				loadedIcons[_currentImageIndex]._x = (currentLineMetrics.x + currentLineMetrics.width) + ICON_XOFFSET + _tf._x ;
				loadedIcons[_currentImageIndex]._y = (_tf._y + _tf._height) - currentLineMetrics.height + ICON_YOFFSET;
				updatePosition();
				
				if (_currentImageIndex < loadedIcons.length - 1)
				{
					_currentImageIndex++;			
				}
			}
		}	
	}	
}