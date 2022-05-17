import flash.display.BitmapData;
import Shared.GlobalFunc;
import flash.geom.Transform;
import flash.geom.ColorTransform;
import flash.geom.Matrix;
import flash.filters.DropShadowFilter;

class AHZVRActivateRollover extends VRActivateRollover
{
	//Widgets 
	public var AHZBottomBar_mc:MovieClip;
	public var content:MovieClip;
	public var WVTranslated:TextField;
	public var LevelTranslated:TextField;
	public var WarmthTranslated:TextField;
	public var txtMeasureInstance:TextField;
	public var BottomRolloverText:TextField;
	public var TopRolloverText:TextField;
	
	// Public vars

	// Options
	private var viewSideInfo:Boolean;
	private var viewEffectsInfo:Boolean;
	private var viewBottomInfo:Boolean;
	private var viewInventoryCount:Boolean;
	private var showBooksRead:Boolean;
	private var showWeightClass:Boolean;
	private var showBookSkill:Boolean;
	private var activationMode:Number;
	private var showTargetWeight:Boolean;
	private var showValueToWeight:Boolean;
	private var showknownEnchantment:Boolean;
	private var showTargetWarmth:Boolean;
	var PLAYER_CARD_WIDTH:Number = 651.0;
		
	// private variables
	private var locationWithoutPlayerData:Number = 0;
	private var locationWithPlayerData:Number = 0;
	private var savedAHZBottomBar_mcY:Number = 0;
	
	// Statics
	private static var hooksInstalled:Boolean = false;

	/* INITIALIZATION */
	
	public function AHZVRActivateRollover()
	{
		super();
		locationWithPlayerData = _root.VRActivateRolloverInstance._y;
		locationWithoutPlayerData = _root.VRActivateRolloverInstance._y + 30;	
		savedAHZBottomBar_mcY = AHZBottomBar_mc._y;
		// Start with the widgets hidden
		hideSideWidget();
		hideBottomWidget();

		BottomRolloverText = _root.VRActivateRolloverInstance.RolloverInfoInstance;
		TopRolloverText = _root.VRActivateRolloverInstance.RolloverNameInstance;

		if (! hooksInstalled)
		{			
			_global.skse.plugins.AHZmoreHUDPlugin.InstallHooks();
			hooksInstalled = true;
		}
		
		// Initialize variables
		viewSideInfo = false;
		viewEffectsInfo = false;
		viewBottomInfo = false;
		viewInventoryCount = false;
		showBooksRead = false;
		showWeightClass = false;
		showBookSkill = false;
		showTargetWeight = false;
		showValueToWeight = false;
		showknownEnchantment = false;
		showTargetWarmth = false;   // Always hide, not used in vr
	}

	function TransverseObject(target:Object):Void
	{
		_global.skse.plugins.AHZmoreHUDPlugin.AHZLog(">>>>>>>>>>>>>>>>>>>>>>>");	
		_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("Transversing : " + target.toString() + ":" + typeof target);	
		for (var o in target)
		{
			_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("   >>>>>>>>>>>>>>>>>>>>>>>");	
			_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("   " + o + ":" + target[o] + ":" + typeof target[o]);
			for (var o1 in target[o])
			{
				_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("      >>>>>>>>>>>>>>>>>>>>>>>");	
				_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("      " + o1 + ":" + target[o][o1] + ":" + typeof target[o][o1]);
				for (var o2 in target[o][o1])
				{
					_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("         " + o2 + ":" + target[o][o1][o2] + ":" + typeof target[o][o1][o2]);
				}
				_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("      <<<<<<<<<<<<<<<<<<<<<<<<");
			}
			_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("   <<<<<<<<<<<<<<<<<<<<<<<<");
		}	
		_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("<<<<<<<<<<<<<<<<<<<<<<<<<<<");	
		
	}

	function RefreshWidgets():Void
	{
		var outData:Object = {outObj:Object};
		var validTarget:Boolean = _global.skse.plugins.AHZmoreHUDPlugin.GetIsValidTarget(outData);
		var hudIsVisible:Boolean = (TopRolloverText._alpha > 0);	
		ProcessPlayerWidget(validTarget && hudIsVisible, (outData && outData.outObj && outData.outObj.canCarry));
		ProcessTargetAndInventoryWidget(validTarget && hudIsVisible);
	}

	function TurnOnWidgets():Void
	{
		_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("TurnOnWidgets");
	}

	function TurnOffWidgets():Void
	{
	
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
	function SetCrosshairTarget(abActivate, aName, abShowButton, abTextOnly, abFavorMode, aWeight, aCost, aFieldValue, aFieldText)
	//function SetCrosshairTarget(abActivate:Boolean,aName:String,abShowButton:Boolean,abTextOnly:Boolean,abFavorMode:Boolean,abShowCrosshair:Boolean,aWeight:Number,aCost:Number,aFieldValue:Number,aFieldText):Void
	{			
	    super.SetCrosshairTarget(abActivate, aName, abShowButton, abTextOnly, abFavorMode, aWeight, aCost, aFieldValue, aFieldText);
		
		var validTarget:Boolean = false;
		var activateWidgets:Boolean = false;
		var outData:Object = {outObj:Object};
		
			
		//showEquippedWidget(1);
		if (abActivate)
		{
			validTarget = _global.skse.plugins.AHZmoreHUDPlugin.GetIsValidTarget(outData);
				
			_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("outData.outObj.canCarry: " + outData.outObj.canCarry);				
				
				
			// Process the bottom player widget
			ProcessPlayerWidget(validTarget, (outData.outObj && outData.outObj.canCarry));	
			// Process the side widget and inventory
			ProcessTargetAndInventoryWidget(validTarget);					
		}
		else
		{
			hideSideWidget();
			hideBottomWidget();	
		}
		
		// Always show regardless of activation mode
		ProcessValueToWeight(validTarget);
		ProcessTargetWarmth(validTarget);
		ProcessKnownEnchantment(validTarget);
		ProcessBookSkill(validTarget);
		ProcessWeightClass(validTarget);
		ProcessReadBook(validTarget);
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
			}
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
				if (TopRolloverText._alpha > 0 && TopRolloverText.htmlText!="" && targetData.inventoryObj.inventoryCount)
				{
					TopRolloverText.html=true;
					
					// Inventory Icon
					appendImageToEnd(TopRolloverText, "Inv.png", 17, 17);
	            	var invCount:String = "<FONT FACE=\"$EverywhereMediumFont\"SIZE=\"15\"COLOR=\"#999999\"KERNING=\"0\">" + targetData.inventoryObj.inventoryCount.toString() + " <\\FONT>"		
		
					TopRolloverText.htmlText = 
						appendHtmlToEnd(TopRolloverText.htmlText, invCount);
					
				}

			}
			
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

	}

	// @Papyrus
	public function setBottomWidgetScale(percent:Number):Void
	{
	}

	// @Papyrus
	public function setInventoryWidgetPosition(xPercent:Number,yPercent:Number):Void
	{
	}
	
	// @Papyrus
	public function setInventoryWidgetScale(percent:Number):Void
	{
	}	
	
	// @Papyrus
	public function setSideWidgetPosition(xPercent:Number,yPercent:Number):Void
	{
	}

	// @Papyrus
	public function setSideWidgetScale(percent:Number):Void
	{
	}	

	// @Papyrus
	public function updateSettings(sideView:Number, 
								   effectsView:Number, 
								   bottomView:Number, 
								   inventoryCount:Number, 
								   showWeightClassValue:Number,
								   showBooksReadValue:Number,
								   showBookSkillValue:Number,
								   showTargetWeightValue:Number,
								   showValueToWeightValue:Number,
								   showknownEnchantmentValue:Number):Void 
	{				
		viewSideInfo = (sideView>=1);
		viewEffectsInfo = (effectsView>=1);
		viewBottomInfo = (bottomView>=1);
		viewInventoryCount = (inventoryCount>=1);
		showWeightClass = (showWeightClassValue>=1);
		showBooksRead = (showBooksReadValue>=1);
		showBookSkill = (showBookSkillValue>=1);
		showTargetWeight = (showTargetWeightValue>=1);
		showValueToWeight = (showValueToWeightValue>=1);
		showknownEnchantment = (showknownEnchantmentValue>=1);
		RefreshWidgets();
	}

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
					2);


			AHZBottomBar_mc._alpha = 100;
			
			// No need to keep it higher if no details are shown
			if (_root.VRActivateRolloverInstance.RolloverGrayBar_mc._alpha){
				_root.VRActivateRolloverInstance._y = locationWithPlayerData; 
				AHZBottomBar_mc._y = savedAHZBottomBar_mcY;
			}
			else{
				_root.VRActivateRolloverInstance._y = locationWithoutPlayerData;
				AHZBottomBar_mc._y = savedAHZBottomBar_mcY - 15;
			}
		}
		else
		{
			hideBottomWidget();
		}
	}

	// @Papyrus
	public function hideBottomWidget():Void
	{
		AHZBottomBar_mc._alpha = 0;
		_root.VRActivateRolloverInstance._y = locationWithoutPlayerData; 
	}

	// @Papyrus
	public function hideSideWidget():Void
	{
		content.gotoAndStop("DEFAULT");
	}


	// @Papyrus
	public function showSideWidget(a_val:Object):Void
	{
		if (viewEffectsInfo&&a_val.effectsDescription!=undefined&&a_val.effectsDescription!=null&&a_val.effectsDescription!="")
		{
			content.gotoAndStop("EFFECTS");
				
			//content.ApparelEnchantedLabel.border = true;
			content.ApparelEnchantedLabel.verticalAutoSize="buttom"	
			content.ApparelEnchantedLabel.textAutoSize="shrink";		
			content.ApparelEnchantedLabel.verticalAlign = "bottom"

			content.ApparelEnchantedLabel.html=true;
			content.ApparelEnchantedLabel.htmlText=a_val.effectsDescription;

		} else if (viewSideInfo && a_val.effect1 != undefined) {
			content.gotoAndStop("INGREDIENTS");

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
					content["IngredientBullet" + i]._alpha=50;
					content["Ingredient" + i]._alpha=50;
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
}