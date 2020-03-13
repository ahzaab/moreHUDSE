import flash.display.BitmapData;
import gfx.io.GameDelegate;
import Shared.GlobalFunc;
import skyui.util.Debug;
import flash.geom.Transform;
import flash.geom.ColorTransform;
import flash.geom.Matrix;
import flash.filters.DropShadowFilter;

class ahz.scripts.widgets.AHZHudInfoWidget extends MovieClip
{
	//Widgets 
	public var AHZBottomBar_mc:MovieClip;
	public var Inventory_mc:MovieClip;
	public var content:MovieClip;
	public var WVTranslated:TextField;
	public var LevelTranslated:TextField;
	public var WarmthTranslated:TextField;
	public var txtMeasureInstance:TextField;
	public var BottomRolloverText:TextField;
	public var TopRolloverText:TextField;
	public var EnemySoul:TextField;
	
	// Public vars
	public var ToggleState:Number;
	public var prevEnemyPercent:Number;

	// Options
	private var viewSideInfo:Boolean;
	private var viewEffectsInfo:Boolean;
	private var viewBottomInfo:Boolean;
	private var viewBottomInfoAlways:Boolean;
	private var viewInventoryCount:Boolean;
	private var bottomAligned:Number;
	private var inventoryAligned:Number;
	private var ingredientWidgetStyle:Number;// 1, 2, 3
	private var effectsWidgetStyle:Number;// 1, 2, 3
	private var showBackgroundForEffects:Boolean;
	private var showBackgroundForIngredients:Boolean;
	private var showBooksRead:Boolean;
	private var showWeightClass:Boolean;
	private var showBookSkill:Boolean;
	private var activationMode:Number;
	private var showTargetWeight:Boolean;
	private var showValueToWeight:Boolean;
	private var showEnemyLevel:Boolean;
	private var showEnemyLevelMax:Number;
	private var showEnemyLevelMin:Number;
	private var showknownEnchantment:Boolean;
	private var widgetDisplayDelayMS:Number;
	private var savedCrossHairData:Object;
	private var displayActive:Boolean;
	private var showEnemySoulLevel:Boolean;
	private var showTargetWarmth:Boolean;
	var PLAYER_CARD_WIDTH:Number = 651.0;
		
	// private variables
	private var savedRolloverInfoText:String;
	private var savedEnemyTextInfo:String;
	private var savedEnemyHtmlTextInfo:String;
	private var savedEnemyLevelValue:String;
	private var savedEnemyLevelNumber:Number;
	private var savedPlayerLevelNumber:Number

	private var _mcLoader:MovieClipLoader;
	private var alphaTimer:Number;
	private var widgetDelayTimer:Number;
	
	// Rects
	private var maxXY:Object;
	private var minXY:Object;

	
	// Statics
	private static var hooksInstalled:Boolean = false;

	/* INITIALIZATION */
	
	public function AHZHudInfoWidget()
	{
		super();
		savedCrossHairData = {outObj:Object,validTarget:Boolean};
		// Get the rec of the parent
		maxXY = {x:Stage.visibleRect.x,y:Stage.visibleRect.y};
		minXY = {x:Stage.visibleRect.x + Stage.visibleRect.width,y:Stage.visibleRect.y + Stage.visibleRect.height};
		this._parent.globalToLocal(maxXY);
		this._parent.globalToLocal(minXY);
		txtMeasureInstance._alpha = 0;
		
		// Anchor this widget to the top left corner
		this._y = maxXY.y;
		this._x = maxXY.x;

		// Start with the widgets hidden
		hideInventoryWidget();
		hideSideWidget();
		hideBottomWidget();
		hideInventoryWidget();

		InitEnemySoulTextField();

		//EnemySoul.border = true;

		if (_root.HUDMovieBaseInstance.RolloverInfoInstance)
		{
			BottomRolloverText = _root.HUDMovieBaseInstance.RolloverInfoInstance;
		}
		else
		{
			// SkyHUD
			BottomRolloverText = _root.HUDMovieBaseInstance.RolloverInfo_mc.RolloverInfoInstance
		}
		
		if (_root.HUDMovieBaseInstance.RolloverNameInstance)
		{
			TopRolloverText = _root.HUDMovieBaseInstance.RolloverNameInstance;
		}
		else
		{
			// SkyHUD
			TopRolloverText = _root.HUDMovieBaseInstance.RolloverName_mc.RolloverNameInstance
		}
		
		if (! hooksInstalled)
		{
			// Apply hooks to hook events
			hookFunction(_root.HUDMovieBaseInstance,"SetCrosshairTarget",this,"SetCrosshairTarget");
			hookFunction(_root.HUDMovieBaseInstance,"ShowElements",this,"ShowElements");
			hookFunction(_root.HUDMovieBaseInstance,"SetCompassAngle",this,"SetCompassAngle");
			_global.skse.plugins.AHZmoreHUDPlugin.InstallHooks();
			hooksInstalled = true;
		}
			
		// Initialize variables
		viewSideInfo = false;
		viewEffectsInfo = false;
		viewBottomInfo = false;
		viewBottomInfoAlways = false;
		viewInventoryCount = false;
		bottomAligned = 1;
		inventoryAligned = 0;
		ingredientWidgetStyle = 0;
		effectsWidgetStyle = 0;
		showBackgroundForEffects = false;
		showBackgroundForIngredients = false;
		showBooksRead = false;
		showWeightClass = false;
		showBookSkill = false;
		activationMode = 0;
		ToggleState = 0;
		savedRolloverInfoText = "";
		showTargetWeight = false;
		showValueToWeight = false;
		showEnemyLevel = false;
		showEnemyLevelMax = 10;
		showEnemyLevelMin = 10;
		showknownEnchantment = false;
		showTargetWarmth = true;   // Always show. This matches the Games logic if survival mode is enabled
		widgetDisplayDelayMS = 0;
		displayActive = false;
		showEnemySoulLevel = false;
	}

	function InitEnemySoulTextField():Void{
		var enemy_mc = _root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance;
		EnemySoul = enemy_mc.createTextField("EnemySoul", 
							enemy_mc.getNextHighestDepth(), 
							 enemy_mc.RolloverNameInstance._x, 
							 enemy_mc.RolloverNameInstance._height - 22, 
							 enemy_mc.RolloverNameInstance._width, 
							 enemy_mc.RolloverNameInstance._height);
		EnemySoul.setNewTextFormat(enemy_mc.RolloverNameInstance.getTextFormat());
		
		var filter:DropShadowFilter = new DropShadowFilter(2,45,0,100,2,2,1.5);
		var filterArray:Array = new Array();
  		filterArray.push(filter);
		EnemySoul.filters = filterArray;
		EnemySoul._alpha = 0;	
	}

	function ShowElements(aMode:String,abShow:Boolean):Void
	{
		/*hudModes[0] = "All"
		hudModes[1] = "StealthMode"
		hudModes[2] = "Favor"
		hudModes[3] = "Swimming"
		hudModes[4] = "HorseMode"
		hudModes[5] = "WarHorseMode"*/

		if (aMode == "BookMode")
		{
			// Leaving book mode
			if (! abShow)
			{
				var outData:Object = {outObj:Object};
				ProcessReadBook(_global.skse.plugins.AHZmoreHUDPlugin.GetIsValidTarget(outData));
			}
		}

		var hudmode:String = _root.HUDMovieBaseInstance.HUDModes[_root.HUDMovieBaseInstance.HUDModes.length - 1];
		
		if (hudmode == "All" ||
			hudmode == "StealthMode" || 
			hudmode == "Favor" || 
			hudmode == "Swimming" || 
			hudmode == "HorseMode" || 
			hudmode == "WarHorseMode")
		{
			this._visible = true;
		}
		else
		{
			this._visible = false;
		}

	}

	public function checkForClearedHud():Void
	{
		clearInterval(alphaTimer);
		if (TopRolloverText._alpha < 50)
		{
			hideSideWidget();	
			hideInventoryWidget();
			//Book_mc._alpha = 0;
			
			if (ToggleState == 0)
			{
				hideBottomWidget();
			}
		}
	}

	function RefreshWidgets():Void
	{
		if (ToggleState > 0)
		{
			var outData:Object = {outObj:Object};
			var validTarget:Boolean = _global.skse.plugins.AHZmoreHUDPlugin.GetIsValidTarget(outData);
			var hudIsVisible:Boolean = (TopRolloverText._alpha > 0);	
			ProcessPlayerWidget(validTarget && hudIsVisible, (outData && outData.outObj && outData.outObj.canCarry));
			ProcessTargetAndInventoryWidget(validTarget && hudIsVisible);
		}	
		else if (viewBottomInfo && viewBottomInfoAlways){
			var outData:Object = {outObj:Object};
			var validTarget:Boolean = _global.skse.plugins.AHZmoreHUDPlugin.GetIsValidTarget(outData);
			ProcessPlayerWidget(validTarget, (outData && outData.outObj && outData.outObj.canCarry));
		}
	}

	function TurnOnWidgets():Void
	{
		ToggleState = 1;
		var outData:Object = {outObj:Object};
		var validTarget:Boolean = _global.skse.plugins.AHZmoreHUDPlugin.GetIsValidTarget(outData);
		var hudIsVisible:Boolean = (TopRolloverText._alpha > 0);
		ProcessPlayerWidget(validTarget && hudIsVisible, (outData && outData.outObj && outData.outObj.canCarry));
		ProcessTargetAndInventoryWidget(validTarget && hudIsVisible, (outData && outData.outObj && outData.outObj.canCarry));
	}

	function TurnOffWidgets():Void
	{
		ToggleState = 0;
		ProcessPlayerWidget(false, false);
		ProcessTargetAndInventoryWidget(false);
		hideBottomWidget();
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

	// Hooks the main huds function
	function SetCrosshairTarget(abActivate:Boolean,aName:String,abShowButton:Boolean,abTextOnly:Boolean,abFavorMode:Boolean,abShowCrosshair:Boolean,aWeight:Number,aCost:Number,aFieldValue:Number,aFieldText):Void
	{			
		var validTarget:Boolean = false;
		var activateWidgets:Boolean = false;
		var outData:Object = {outObj:Object};
		
		// Always reset the delay timer to reset when the cross hair changes
		if (widgetDelayTimer)
		{
			clearInterval(widgetDelayTimer);
		}			
		
		//showEquippedWidget(1);
		if (abActivate)
		{
			validTarget = _global.skse.plugins.AHZmoreHUDPlugin.GetIsValidTarget(outData);
			savedCrossHairData.outObj = outData.outObj;
			savedCrossHairData.validTarget = validTarget;
			
			if (alphaTimer != null)
			{
				clearInterval(alphaTimer);
			}
			// Set an interval to disable hide the widgets.  This is for less intrusive hud 
			alphaTimer = setInterval(this,"checkForClearedHud",6000);	
		}
		else
		{
			displayActive = false;
		}
		
		if ((abActivate && activationMode == 0) ||
			(abActivate && activationMode == 1 && ! _global.skse.plugins.AHZmoreHUDPlugin.GetIsPlayerInCombat()) ||
			(abActivate && activationMode == 2 && ToggleState == 1))
		{
			activateWidgets = true;
		}

		// Only use visibulity delay when not toggling
		if (activateWidgets && activationMode != 2 && widgetDisplayDelayMS>0 && !displayActive)
		{	
			// Set an interval to disable hide the widgets.  This is for less intrusive hud 
			widgetDelayTimer = setInterval(this,"delayedDisplay",widgetDisplayDelayMS);					
		}
		else
		{
			// Process the bottom player widget
			ProcessPlayerWidget(validTarget && activateWidgets, (outData && outData.outObj && outData.outObj.canCarry));	
			// Process the side widget and inventory
			ProcessTargetAndInventoryWidget(validTarget && activateWidgets);
		}
		
		// Always show regardless of activation mode
		ProcessValueToWeight(validTarget);
		ProcessTargetWarmth(validTarget);
		ProcessKnownEnchantment(validTarget);
		ProcessBookSkill(validTarget);
		ProcessWeightClass(validTarget);
		ProcessReadBook(validTarget);
	}
	
	function delayedDisplay():Void
	{
		clearInterval(widgetDelayTimer);
		if (savedCrossHairData)
		{
			// Process the bottom player widget
			ProcessPlayerWidget(savedCrossHairData.validTarget, (savedCrossHairData.outObj && savedCrossHairData.outObj.canCarry));	
			// Process the side widget and inventory
			ProcessTargetAndInventoryWidget(savedCrossHairData.validTarget);
			
			displayActive = savedCrossHairData.validTarget;
		}	
	}
	
	function UpdateEnemyLevelValues()
	{
		var outData:Object = {outObj:Object};
		_global.skse.plugins.AHZmoreHUDPlugin.GetEnemyInformation(outData, LevelTranslated.htmlText);			
		if (outData && outData.outObj)
		{				
			savedEnemyLevelNumber = outData.outObj.EnemyLevel;
			savedPlayerLevelNumber = outData.outObj.PlayerLevel;	
			
			//_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("Update: " + savedEnemyLevelNumber.toString() + ", " + savedPlayerLevelNumber.toString());
		}	
	}

	function interpolate(pBegin:Number, pEnd:Number, pMax:Number, pStep:Number):Number {
		return pBegin + Math.floor((pEnd - pBegin) * pStep / pMax);
	}

	function isEnemyLevelUpdateRequired():Boolean
	{
		//_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("Checking if visible");
		
		// Check the enemy movie is even visible
		if (_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance._alpha == 0 || !showEnemyLevel){
			return false;
		}
		

		var enemyNameText = _root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.text;
		//_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("Checking if (x) is in '" + enemyNameText + "'");
		var startIndex:Number = enemyNameText.lastIndexOf("(");
		var lastIndex:Number = enemyNameText.lastIndexOf(")");	
	
		//_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("SI: " + startIndex.toString());
		//_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("EI: " + lastIndex.toString());
	
		// Look for (x) in the text  (This would be a lot easier if AS2 supported RegExp -_-)
		if (startIndex < 0 || lastIndex < 0 || (lastIndex < startIndex) || ((lastIndex - startIndex) < 2))	
		{
			return true;
		}
	
		// Check for A number in the parentheses
		var levelString:String = enemyNameText.substring(startIndex + 1, lastIndex);
		
		//_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("D: " + (lastIndex - startIndex).toString());
		//_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("Level String: '" + levelString + "'");
		
		if (levelString == "" || parseInt(levelString) == NaN){
			return true;
		}
	
		return false;
	
	}

	function measureStringWidth(str:String):Number {
		txtMeasureInstance._alpha = 0;
    	txtMeasureInstance.text = str;
    	return txtMeasureInstance.textWidth;
	}

	function SetCompassAngle(aPlayerAngle: Number, aCompassAngle: Number, abShowCompass: Boolean)
	{			
		var outData:Object = {outObj:Object};
	
		if (showEnemySoulLevel && _root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance._alpha)
		{
			_global.skse.plugins.AHZmoreHUDPlugin.GetEnemyInformation(outData, LevelTranslated.htmlText);
			if (outData && outData.outObj && outData.outObj.Soul){
				EnemySoul._alpha = _root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance._alpha;
				EnemySoul.text = outData.outObj.Soul;
			}
			else
			{
				EnemySoul._alpha = 0;
			}
		}
		else
		{
			EnemySoul._alpha = 0;
			// stting to null will flag the code further down to read the data again for the level
			outData = null;
		}
	
		// This function is hooked and gets fired every frame
		if (isEnemyLevelUpdateRequired(outData))
		{			
			//_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("T");
			var levelText:String;
			// If the data was not aquired from reading the soul level then read it here
			if (!outData)
			{
				outData = {outObj:Object};
				_global.skse.plugins.AHZmoreHUDPlugin.GetEnemyInformation(outData, LevelTranslated.htmlText);			
			}
			
			if (outData && outData.outObj)
			{				
				savedEnemyLevelNumber = outData.outObj.EnemyLevel;
				savedPlayerLevelNumber = outData.outObj.PlayerLevel;
				
				if (savedPlayerLevelNumber == 0 && savedEnemyLevelNumber == 0)
				{
					return;
				}
			}
			else
			{
				return;
			}
											
			if (showEnemyLevelMax > 0 && showEnemyLevelMin > 0 )
			{	
				var oldTextWidth:Number = measureStringWidth(_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.text);

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
			
				_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.html = true;	
				levelText = " (<font color=\'#" + fontColor + "\'>" + savedEnemyLevelNumber.toString() + "</font>)";
				
				// Append the level
				_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.htmlText = 
					appendHtmlToEnd(_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.htmlText, levelText);
			}
			// No coloring, turn off html
			else
			{
				_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.html = false;
				levelText = _root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.text;									
				levelText = levelText + " (" + savedEnemyLevelNumber.toString() + ")";
				_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.html = false;
				_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.text = levelText;
			}
			
			// Calculate the new position for the brackets
			var newTextWidth:Number = measureStringWidth(_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.text);
			
			var widthPercentChange = (newTextWidth - oldTextWidth) / oldTextWidth;
			var adjustedBracketWidth = _root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance._currentframe + 
					(_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance._currentframe * widthPercentChange)
			
			adjustedBracketWidth = Math.min(100, Math.max(adjustedBracketWidth, 0));
			var iMeterFrame: Number = Math.floor(adjustedBracketWidth);
			
			_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance.gotoAndStop(iMeterFrame);						
		}
	}
	
	function ProcessTargetWarmth(isValidTarget:Boolean):Void
	{		
		if (showTargetWarmth && isValidTarget)
		{
			// Shor Warmth
			if (BottomRolloverText._alpha > 0 && BottomRolloverText.htmlText != "")
			{
				var targetWarmthRating:Number = _global.skse.plugins.AHZmoreHUDPlugin.GetTargetWarmthRating();
				if (targetWarmthRating > 0)
				{			
					var warmthStr:String;
	            	warmthStr = "<FONT FACE=\"$EverywhereMediumFont\"SIZE=\"15\"COLOR=\"#999999\"KERNING=\"0\">     " + WarmthTranslated.text + " <\\FONT>"		
							+ "<FONT FACE=\"$EverywhereBoldFont\"SIZE=\"24\"COLOR=\"#FFFFFF\"KERNING=\"0\">" + targetWarmthRating.toString() + "<\\FONT>";
		
					BottomRolloverText.htmlText = 
						appendHtmlToEnd(BottomRolloverText.htmlText, warmthStr); 
				}
			}
		}
	}	
	
	function ProcessValueToWeight(isValidTarget:Boolean):Void
	{		
		if (showValueToWeight && isValidTarget)
		{
			// Show weight class if its armor
			if (BottomRolloverText._alpha > 0 && BottomRolloverText.htmlText != "")
			{
				var valueToWeight:String = _global.skse.plugins.AHZmoreHUDPlugin.GetValueToWeightString(BottomRolloverText.text, WVTranslated.text);
				if (valueToWeight != "")
				{					
					BottomRolloverText.htmlText = 
						appendHtmlToEnd(BottomRolloverText.htmlText, valueToWeight); 
				}
			}
		}
	}

	function ProcessKnownEnchantment(isValidTarget:Boolean):Void
	{
		if (showknownEnchantment && isValidTarget)
		{
			var knownEnchantment:Number=_global.skse.plugins.AHZmoreHUDPlugin.IsAKnownEnchantedItem();

			if (knownEnchantment && TopRolloverText._alpha > 0 && TopRolloverText.htmlText!="")
			{
				TopRolloverText.html=true;
				
				// Player knows the enchantment
				if (knownEnchantment == 1){
					appendImageToEnd(TopRolloverText, "ahzknown.png", 17, 17);
				}
				
				// The item is enchanted, but the player cannot learn the enchantment
				if (knownEnchantment == 2){
					appendImageToEnd(TopRolloverText, "ahzEnch.png", 17, 17);
				}	
			}
		}
	}

	function ProcessWeightClass(isValidTarget:Boolean):Void
	{
		if (showWeightClass && isValidTarget)
		{
			// Show weight class if its armor
			if (BottomRolloverText._alpha > 0 && BottomRolloverText.htmlText != "")
			{
				var weightClass:String = _global.skse.plugins.AHZmoreHUDPlugin.GetArmorWeightClassString();
				if (weightClass != "")
				{
					// Insert the weight class into the rolloverinfo textfield
					BottomRolloverText.htmlText = 
						appendHtmlToEnd(BottomRolloverText.htmlText, weightClass.toUpperCase());
				}
			}
		}
	}

	function ProcessBookSkill(isValidTarget:Boolean):Void
	{
		if (showBookSkill && isValidTarget)
		{			
			// Show book skill
			if (BottomRolloverText._alpha > 0 && BottomRolloverText.htmlText != "")
			{
				var bookSkill:String = _global.skse.plugins.AHZmoreHUDPlugin.GetBookSkillString();
				if (bookSkill != "")
				{
					// Insert the book skill into the rolloverinfo textfield
					BottomRolloverText.htmlText = 
						appendHtmlToEnd(BottomRolloverText.htmlText, bookSkill.toUpperCase());					
				}
				else
				{
					savedRolloverInfoText = "";
				}
			}
			else
			{
				savedRolloverInfoText = "";
			}
		}
		else
		{
			savedRolloverInfoText = "";
		}
	}

	function ProcessTargetAndInventoryWidget(isValidTarget:Boolean):Void
	{
		var sideWidgetDataExists:Boolean = false;

		if (isValidTarget)
		{
			var targetData:Object = {effectsObj:Object, ingredientObj:Object, inventoryObj:Object};
			
			if (viewEffectsInfo || viewSideInfo || viewInventoryCount)
			{
				// Get the target effects
				_global.skse.plugins.AHZmoreHUDPlugin.GetTargetEffects(targetData, viewInventoryCount);	
			}
						
			if (viewEffectsInfo)
			{
				// If effects exist
				if (targetData.effectsObj != undefined && targetData.effectsObj != null)
				{
					sideWidgetDataExists = true;
					showSideWidget(targetData.effectsObj);
				}
			}
			
			if (viewSideInfo && !sideWidgetDataExists)
			{
				// If the target is an ingredient
				if (targetData.ingredientObj != undefined && targetData.ingredientObj != null)
				{
					sideWidgetDataExists = true;
					showSideWidget(targetData.ingredientObj);
				}
			}
			
			if (viewInventoryCount && targetData.inventoryObj)
			{
				showInventoryWidget(targetData.inventoryObj.inventoryName,targetData.inventoryObj.inventoryCount);
			}
			else
			{
				hideInventoryWidget();
			}			
		}
		else
		{
			hideInventoryWidget();
		}
		
		// If There is no side widget data, then make sure the widget is hidden
		if (! sideWidgetDataExists)
		{
			hideSideWidget();
		}
	}
	
	function ProcessPlayerWidget(isValidTarget:Boolean, canCarry:Boolean):Void
	{
		if (viewBottomInfo)
		{
			var targetData:Object = {targetObj:Object};
			var playerData:Object = {playerObj:Object};

			if (isValidTarget && canCarry)
			{
				// Get player data against the current target
				_global.skse.plugins.AHZmoreHUDPlugin.GetTargetObjectData(targetData);
				_global.skse.plugins.AHZmoreHUDPlugin.GetPlayerData(playerData);

				if (targetData.targetObj != undefined && targetData.targetObj != null && playerData.playerObj != undefined && playerData.playerObj != null)
				{
					// SHow the bottom widget data.  TODO: pass the object directly
					showBottomWidget(
									 targetData.targetObj.ratingOrDamage,
									 targetData.targetObj.difference,
									 playerData.playerObj.encumbranceNumber,
									 playerData.playerObj.maxEncumbranceNumber,
									 playerData.playerObj.goldNumber,
									 targetData.targetObj.objWeight,
									 targetData.targetObj.formType,
									 targetData.targetObj.totalWarmthRating,
									 targetData.targetObj.warmthDifference,
									 targetData.targetObj.isSurvivalMode);
				}
				else
				{
					hideBottomWidget();
				}
			}
			else if (ToggleState > 0 || viewBottomInfoAlways)
			{
				// Only show player data
				_global.skse.plugins.AHZmoreHUDPlugin.GetPlayerData(playerData);
				if (playerData.playerObj != undefined && playerData.playerObj != null)
				{
					showBottomWidget(
									 0,
									 0,
									 playerData.playerObj.encumbranceNumber,
									 playerData.playerObj.maxEncumbranceNumber,
									 playerData.playerObj.goldNumber,
									 0.0,
									 AHZInventoryDefines.kNone,
									 undefined,
									 undefined,
									 undefined);
				}
				else
				{
					hideBottomWidget();
				}
			}
			else
			{
				hideBottomWidget();
			}
		}
		else
		{
			hideBottomWidget();
		}
	}

	// @override WidgetBase
	public function onLoad():Void
	{
		super.onLoad();
	}
	
	// @Papyrus
	public function setBottomWidgetPosition(xPercent:Number,yPercent:Number):Void
	{
		var tempVar:Number;
		var inverse:Number;

		inverse = 1.0/(xPercent/100.0);

		tempVar = (Stage.visibleRect.width/inverse)-(PLAYER_CARD_WIDTH/inverse);
		AHZBottomBar_mc._x = tempVar;

		inverse = 1.0/(yPercent/100.0);
		tempVar = (Stage.visibleRect.height/inverse)-(AHZBottomBar_mc._height/inverse);
		AHZBottomBar_mc._y = tempVar;
	}

	// @Papyrus
	public function setBottomWidgetScale(percent:Number):Void
	{
		AHZBottomBar_mc._yscale  = (percent / 1.0);
		AHZBottomBar_mc._xscale  = (percent / 1.0);
	}

	// @Papyrus
	public function setInventoryWidgetPosition(xPercent:Number,yPercent:Number):Void
	{
		var tempVar:Number;
		var inverse:Number;

		inverse = 1.0/(xPercent/100.0);

		tempVar = (Stage.visibleRect.width/inverse)-(381.0/inverse);
		Inventory_mc._x = tempVar;

		inverse = 1.0/(yPercent/100.0);
		tempVar = (Stage.visibleRect.height/inverse)-(Inventory_mc._height/inverse);
		Inventory_mc._y = tempVar;
	}
	
	// @Papyrus
	public function setInventoryWidgetScale(percent:Number):Void
	{
		Inventory_mc._yscale  = (percent / 1.0);
		Inventory_mc._xscale  = (percent / 1.0);
	}	
	
	// @Papyrus
	public function setSideWidgetPosition(xPercent:Number,yPercent:Number):Void
	{
		var tempVar:Number;
		var inverse:Number;

		inverse = 1.0/(xPercent/100.0);

		tempVar = (Stage.visibleRect.width/inverse)-(content.SizeHolder_mc._width/inverse);
		content._x = tempVar;

		inverse = 1.0/(yPercent/100.0);
		tempVar = (Stage.visibleRect.height/inverse)-(content.SizeHolder_mc._height/inverse);
		content._y = tempVar;
	}

	// @Papyrus
	public function setSideWidgetScale(percent:Number):Void
	{
		content._yscale  = (percent / 1.0);
		content._xscale  = (percent / 1.0);
	}	

	// @Papyrus
	public function updateSettings(sideView:Number, 
								   effectsView:Number, 
								   bottomView:Number, 
								   inventoryCount:Number, 
								   bottomAlignedValue:Number, 
								   inventoryAlignedValue:Number, 
								   ingredientWidgetStyleValue:Number, 
								   effectsWidgetStyleValue:Number,
								   showWeightClassValue:Number,
								   showBooksReadValue:Number,
								   activationModeValue:Number,
								   ToggleStateValue:Number,
								   showBookSkillValue:Number,
								   showTargetWeightValue:Number,
								   showValueToWeightValue:Number,
								   showEnemyLevelValue:Number,
								   showEnemyLevelMaxValue:Number,
								   showEnemyLevelMinValue:Number,
								   showknownEnchantmentValue:Number,
								   widgetDisplayDelayMSValue:Number,
								   showEnemySoulLevelValue:Number,
								   viewBottomInfoAlwaysValue:Number):Void 
	{				
		viewSideInfo = (sideView>=1);
		viewBottomInfo = (bottomView>=1);
		viewInventoryCount = (inventoryCount>=1);
		bottomAligned = bottomAlignedValue;
		inventoryAligned = inventoryAlignedValue;
		viewEffectsInfo = (effectsView>=1);
		effectsWidgetStyle = effectsWidgetStyleValue;
		ingredientWidgetStyle = ingredientWidgetStyleValue;
		showBooksRead = (showBooksReadValue>=1);
		showWeightClass = (showWeightClassValue>=1);
		ToggleState = ToggleStateValue;
		activationMode = activationModeValue;
		showBookSkill = (showBookSkillValue>=1);
		showTargetWeight = (showTargetWeightValue>=1);
		showValueToWeight = (showValueToWeightValue>=1);
		showEnemyLevel = (showEnemyLevelValue>=1);
		showEnemyLevelMax = showEnemyLevelMaxValue;
		showEnemyLevelMin = showEnemyLevelMinValue;
		showknownEnchantment = (showknownEnchantmentValue>=1);
		widgetDisplayDelayMS = widgetDisplayDelayMSValue;
		showEnemySoulLevel = (showEnemySoulLevelValue>=1);
		viewBottomInfoAlways = (viewBottomInfoAlwaysValue>=1);
		RefreshWidgets();
	}

	// @Papyrus
	public function showBottomWidget(
						ratingOrDamage:Number,
						difference:Number,
						encumbranceNumber:Number,
						maxEncumbranceNumber:Number,
						goldNumber:Number,weightValue:Number,
						formType:Number,
						warmthRating:Number,
						warmthDifference:Number,
						isSurvivalMode:Boolean):Void
	{		
		if (viewBottomInfo)
		{
			var tempType:Number;
			if (formType == AHZInventoryDefines.kWeapon || formType == AHZInventoryDefines.kAmmo || formType == AHZInventoryDefines.kProjectile)
			{
				tempType = AHZInventoryDefines.ICT_WEAPON;
			}
			else if (formType == AHZInventoryDefines.kArmor)
			{
				tempType = AHZInventoryDefines.ICT_ARMOR;
			}
			else
			{
				tempType = AHZInventoryDefines.ICT_DEFAULT;
			}
			
			// Set to 0 to disable
			if (!showTargetWeight)
			{
				weightValue = 0;
			}
			
			if (!isSurvivalMode)
			{
				warmthRating = undefined;
				warmthDifference = undefined;
			}
			
			AHZBottomBar_mc.UpdatePlayerInfo(
					{
						damage:ratingOrDamage,
					 	armor:ratingOrDamage,
					 	gold:goldNumber,
					 	encumbrance:encumbranceNumber,
					 	maxEncumbrance:maxEncumbranceNumber
					},
					{
						type:tempType,
					   	damageChange:difference,
					   	armorChange:difference,
					   	objWeight:weightValue,
					   	warmthChange:warmthDifference,
					   	warmth:warmthRating
					},
					bottomAligned);


			AHZBottomBar_mc._alpha = 100;
		}
		else
		{
			AHZBottomBar_mc._alpha = 0;
		}
	}

	// @Papyrus
	public function hideBottomWidget():Void
	{
		if (!viewBottomInfoAlways){
			AHZBottomBar_mc._alpha = 0;
		}
		else
		{
			
		}
	}

	// @Papyrus
	public function hideSideWidget():Void
	{
		content.gotoAndStop("DEFAULT");
	}

	public function hideInventoryWidget():Void
	{
		Inventory_mc._alpha = 0;
	}

	public function showInventoryWidget(inventoryName:String,inventoryCount:Number)
	{
		if (viewInventoryCount && inventoryCount > 0)
		{
			Inventory_mc.InventoryCount.SetText(inventoryCount.toString());
			Inventory_mc.InventoryName.SetText(inventoryName);

			if (inventoryAligned == 1)
			{
				// Right Aligned
				Inventory_mc.InventoryCount.autoSize = "right";
				Inventory_mc.InventoryName.autoSize = "right";

				Inventory_mc.InventoryCount._x = 381.0 - Inventory_mc.InventoryCount._width;
				Inventory_mc.InventoryName._x = Inventory_mc.InventoryCount._x + Inventory_mc.InventoryCount.getLineMetrics(0).x - Inventory_mc.InventoryName._width;
			}
			else if (inventoryAligned == 2)
			{
				// Center aligned
				Inventory_mc.InventoryCount.autoSize = "right";
				Inventory_mc.InventoryName.autoSize = "right";
				Inventory_mc.InventoryCount._x = 381.0 - Inventory_mc.InventoryCount._width;
				Inventory_mc.InventoryName._x = Inventory_mc.InventoryCount._x + Inventory_mc.InventoryCount.getLineMetrics(0).x - Inventory_mc.InventoryName._width;

				// Calculate the amount to move to adjust to the center of the Inventory_mc movie clip
				var deltaVal:Number = ((381.0 - Inventory_mc.InventoryName._x) / 2.0) + Inventory_mc.InventoryName._x;
				deltaVal -= (381.0 / 2.0);

				// Shift into position
				Inventory_mc.InventoryName._x-=deltaVal;
				Inventory_mc.InventoryCount._x-=deltaVal;

			}
			else
			{
				//Default left
				Inventory_mc.InventoryCount.autoSize="left";
				Inventory_mc.InventoryName.autoSize="left";

				Inventory_mc.InventoryName._x=0;
				Inventory_mc.InventoryCount._x=Inventory_mc.InventoryName._x+Inventory_mc.InventoryName.getLineMetrics(0).width+8;
			}

			Inventory_mc._alpha=100;
		}
		else
		{
			hideInventoryWidget();
		}
	}

	// @Papyrus
	public function showSideWidget(a_val:Object):Void
	{
		if (viewEffectsInfo&&a_val.effectsDescription!=undefined&&a_val.effectsDescription!=null&&a_val.effectsDescription!="")
		{

			switch (effectsWidgetStyle)
			{
				case 0 :
					content.gotoAndStop("ST1_EFFECTS");
					break;
				case 1 :
					content.gotoAndStop("ST2_EFFECTS");
					break;
				case 2 :
					content.gotoAndStop("ST2_EFFECTS_BG");
					break;
				case 3 :
					content.gotoAndStop("ST3_EFFECTS");
					break;
				default :
					content.gotoAndStop("DEFAULT");
					break;
			}
				
			//content.ApparelEnchantedLabel.border = true;
			content.ApparelEnchantedLabel.verticalAutoSize="center"	
			content.ApparelEnchantedLabel.textAutoSize="shrink";		

			content.ApparelEnchantedLabel.html=true;
			content.ApparelEnchantedLabel.htmlText=a_val.effectsDescription;

		} else if (viewSideInfo && a_val.effect1 != undefined) {
			switch (ingredientWidgetStyle) {
				case 0 :
					content.gotoAndStop("ST1_INGREDIENTS");
					break;
				case 1 :
					content.gotoAndStop("ST2_INGREDIENTS");
					break;
				case 2 :
					content.gotoAndStop("ST2_INGREDIENTS_BG");
					break;
				case 3 :
					content.gotoAndStop("ST3_INGREDIENTS");
					break;
				default :
					content.gotoAndStop("DEFAULT");
					break;
			}

			for (var i = 1; i <=4; i++) 
			{
				content["Ingredient" + i].html=true;

				if (a_val["effect" + i] == undefined)
				{
					content["Ingredient" + i].htmlText="";
					content["IngredientBullet" + i]._alpha=0;
				}
				else if (a_val["effect" + i]=="")
				{
					content["Ingredient" + i].htmlText="$UNKNOWN";
					content["IngredientBullet" + i]._alpha=25;
					content["Ingredient" + i]._alpha=25;
				}
				else
				{
					content["Ingredient" + i]._alpha=100;
					content["IngredientBullet" + i]._alpha=100;
					content["Ingredient" + i].htmlText=a_val["effect"+i];
				}
			}
		}
		else
		{
			content.gotoAndStop("DEFAULT");
		}
	}

	public function ProcessReadBook(isValidTarget:Boolean):Void
	{
		if (showBooksRead&&isValidTarget)
		{
			var bookRead:Boolean=_global.skse.plugins.AHZmoreHUDPlugin.GetIsBookAndWasRead();

			if (bookRead && TopRolloverText._alpha>0 && TopRolloverText.htmlText!="")
			{
				TopRolloverText.html=true;
				appendImageToEnd(TopRolloverText, "eyeImage.png", 17, 17);				
			}
		}
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