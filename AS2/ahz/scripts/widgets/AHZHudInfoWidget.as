import flash.display.BitmapData;
import gfx.io.GameDelegate;
import Shared.GlobalFunc;
import skyui.util.Debug;
import flash.geom.Transform;
import flash.geom.ColorTransform;
import flash.geom.Matrix;
import flash.filters.DropShadowFilter;
import Components.Meter;
import mx.managers.DepthManager;

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
	public var EnemyMagicka_mc:MovieClip;
	public var EnemyStamina_mc:MovieClip;
	public var LoadedEnemyMagicka_mc:MovieClip;
	public var LoadedEnemyStamina_mc:MovieClip;	
	public var EnemyMagickaMeter: Meter;
	public var EnemyStaminaMeter: Meter;	
	public var HealthStats_mc: MovieClip;
	public var MagickaStats_mc: MovieClip;
	public var StaminaStats_mc: MovieClip;
	public var AHZBracketInstance: MovieClip;
	
	// Public vars
	public var ToggleState:Number;
	public var prevEnemyPercent:Number;

	// Options
	private var viewSideInfo:Boolean;
	private var viewEffectsInfo:Boolean;
	private var viewBottomInfo:Boolean;
	private var viewBottomInfoAlways:Boolean;
	private var viewInventoryCount:Boolean;
	private var viewInventoryCountWithZero:Boolean;
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
	private var showEnemyMagickaMeter:Boolean;
	private var showEnemyStaminaMeter:Boolean;
	private var showEnemyStaminaStats:Boolean;
	private var showEnemyMagickaStats:Boolean;
	private var showEnemyHealthStats:Boolean;
	
	var PLAYER_CARD_WIDTH:Number = 651.0;
	var EXPORTED_PREFIX:String = "exported/"	
		
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
	private var orginalBracketLocationY:Number;
	private var ENEMY_METER_HEIGHT:Number = 11.5;
	private var enemyMeterHeightScaled:Number;
	private var firstMagickaMeterUpdate:Boolean = true;
	private var firstStaminaMeterUpdate:Boolean = true;
	private var alphaChanged:Boolean = true;
	private var swapped:Boolean = false;
	private var _config:Object;
	private var staminaMeterBaseY:Number = 0;
	private var baseX:Number = 0;
	private var baseY:Number = 0;
	private var metersToLoad:Array;
	private var mcLoader:MovieClipLoader;
	
	
	// Rects
	private var stageRect:Object;

	// Statics
	private static var hooksInstalled:Boolean = false;
	static var IconContainer:AHZIconContainer = new AHZIconContainer();

	/* INITIALIZATION */
	
	public function AHZHudInfoWidget()
	{
		super();
		
		mcLoader = new MovieClipLoader();
		mcLoader.addListener(this);
		metersToLoad = new Array();
		
		savedCrossHairData = {outObj:Object,validTarget:Boolean};
		// Get the rec of the parent
		stageRect = {x:Stage.visibleRect.x,y:Stage.visibleRect.y};
		
		txtMeasureInstance._alpha = 0;
		//_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("This:");
		// Anchor this widget to the top left corner
		this._y = stageRect.y;
		this._x = stageRect.x;

		// Start with the widgets hidden
		hideInventoryWidget();
		hideSideWidget();
		hideBottomWidget();
		hideInventoryWidget();

		_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("AHZConfigManager.loadConfig");
		AHZConfigManager.loadConfig(this, "configLoaded", "configError");

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
					
		// Initialize variables
		viewSideInfo = false;
		viewEffectsInfo = false;
		viewBottomInfo = false;
		viewBottomInfoAlways = false;
		viewInventoryCount = false;
		viewInventoryCountWithZero = false;
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
		showEnemyMagickaMeter = false;
		showEnemyStaminaMeter = false;	
		showEnemyHealthStats = false;
		showEnemyStaminaStats = false;
		showEnemyMagickaStats = false;
	}

	function configLoaded(event:Object):Void{
		_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("configLoaded: " + event);
		_config = event.config;
		prepareConfigs();
		initializeClips();
	}

	function configError(event:Object):Void{
		_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("configError: " + event);
		prepareConfigs();
		initializeClips();
	}

	function prepareConfigs() :Void{
		if (!_config)
			_config = {};
		
		if (!_config[AHZDefines.CFG_ENEMY_MAGICKA_METER_XOFFSET])
			_config[AHZDefines.CFG_ENEMY_MAGICKA_METER_XOFFSET] = 0;
		
		if (!_config[AHZDefines.CFG_ENEMY_MAGICKA_METER_YOFFSET])
			_config[AHZDefines.CFG_ENEMY_MAGICKA_METER_YOFFSET] = 0;		
		
		if (!_config[AHZDefines.CFG_ENEMY_MAGICKA_METER_XSCALE])
			_config[AHZDefines.CFG_ENEMY_MAGICKA_METER_XSCALE] = 1.0;				
		
		if (!_config[AHZDefines.CFG_ENEMY_MAGICKA_METER_YSCALE])
			_config[AHZDefines.CFG_ENEMY_MAGICKA_METER_YSCALE] = 1.0;						

		if (!_config[AHZDefines.CFG_ENEMY_MAGICKA_METER_ALPHA])
			_config[AHZDefines.CFG_ENEMY_MAGICKA_METER_ALPHA] = 100;	

		if (!_config[AHZDefines.CFG_ENEMY_MAGICKA_METER_NUMBERS_XOFFSET])
			_config[AHZDefines.CFG_ENEMY_MAGICKA_METER_NUMBERS_XOFFSET] = 0;			

		if (!_config[AHZDefines.CFG_ENEMY_MAGICKA_METER_NUMBERS_YOFFSET])
			_config[AHZDefines.CFG_ENEMY_MAGICKA_METER_NUMBERS_YOFFSET] = 0;		

		if (!_config[AHZDefines.CFG_ENEMY_MAGICKA_METER_NUMBERS_XSCALE])
			_config[AHZDefines.CFG_ENEMY_MAGICKA_METER_NUMBERS_XSCALE] = 1.0;		

		if (!_config[AHZDefines.CFG_ENEMY_MAGICKA_METER_NUMBERS_YSCALE])
			_config[AHZDefines.CFG_ENEMY_MAGICKA_METER_NUMBERS_YSCALE] = 1.0;	
			
		if (!_config[AHZDefines.CFG_ENEMY_MAGICKA_METER_NUMBERS_ALPHA])
			_config[AHZDefines.CFG_ENEMY_MAGICKA_METER_NUMBERS_ALPHA] = 100;				
			
		if (!_config[AHZDefines.CFG_ENEMY_STAMINA_METER_XOFFSET])
			_config[AHZDefines.CFG_ENEMY_STAMINA_METER_XOFFSET] = 0;
		
		if (!_config[AHZDefines.CFG_ENEMY_STAMINA_METER_YOFFSET])
			_config[AHZDefines.CFG_ENEMY_STAMINA_METER_YOFFSET] = 0;		
		
		if (!_config[AHZDefines.CFG_ENEMY_STAMINA_METER_XSCALE])
			_config[AHZDefines.CFG_ENEMY_STAMINA_METER_XSCALE] = 1.0;				
		
		if (!_config[AHZDefines.CFG_ENEMY_STAMINA_METER_YSCALE])
			_config[AHZDefines.CFG_ENEMY_STAMINA_METER_YSCALE] = 1.0;						

		if (!_config[AHZDefines.CFG_ENEMY_STAMINA_METER_ALPHA])
			_config[AHZDefines.CFG_ENEMY_STAMINA_METER_ALPHA] = 100;	

		if (!_config[AHZDefines.CFG_ENEMY_STAMINA_METER_NUMBERS_XOFFSET])
			_config[AHZDefines.CFG_ENEMY_STAMINA_METER_NUMBERS_XOFFSET] = 0;			

		if (!_config[AHZDefines.CFG_ENEMY_STAMINA_METER_NUMBERS_YOFFSET])
			_config[AHZDefines.CFG_ENEMY_STAMINA_METER_NUMBERS_YOFFSET] = 0;		

		if (!_config[AHZDefines.CFG_ENEMY_STAMINA_METER_NUMBERS_XSCALE])
			_config[AHZDefines.CFG_ENEMY_STAMINA_METER_NUMBERS_XSCALE] = 1.0;		

		if (!_config[AHZDefines.CFG_ENEMY_STAMINA_METER_NUMBERS_YSCALE])
			_config[AHZDefines.CFG_ENEMY_STAMINA_METER_NUMBERS_YSCALE] = 1.0;	
	
		if (!_config[AHZDefines.CFG_ENEMY_STAMINA_METER_NUMBERS_ALPHA])
			_config[AHZDefines.CFG_ENEMY_STAMINA_METER_NUMBERS_ALPHA] = 100;	
	
		if (!_config[AHZDefines.CFG_ENEMY_HEALTH_METER_NUMBERS_XOFFSET])
			_config[AHZDefines.CFG_ENEMY_HEALTH_METER_NUMBERS_XOFFSET] = 0;			

		if (!_config[AHZDefines.CFG_ENEMY_HEALTH_METER_NUMBERS_YOFFSET])
			_config[AHZDefines.CFG_ENEMY_HEALTH_METER_NUMBERS_YOFFSET] = 0;		

		if (!_config[AHZDefines.CFG_ENEMY_HEALTH_METER_NUMBERS_XSCALE])
			_config[AHZDefines.CFG_ENEMY_HEALTH_METER_NUMBERS_XSCALE] = 1.0;		

		if (!_config[AHZDefines.CFG_ENEMY_HEALTH_METER_NUMBERS_YSCALE])
			_config[AHZDefines.CFG_ENEMY_HEALTH_METER_NUMBERS_YSCALE] = 1.0;	
			
		if (!_config[AHZDefines.CFG_ENEMY_HEALTH_METER_NUMBERS_ALPHA])
			_config[AHZDefines.CFG_ENEMY_HEALTH_METER_NUMBERS_ALPHA] = 100;			
			
		if (!_config[AHZDefines.CFG_ENEMY_METER_USE_STACKING])
			_config[AHZDefines.CFG_ENEMY_METER_USE_STACKING] = true;
			
		if (!_config[AHZDefines.CFG_ENEMY_METER_HEIGHT])
			_config[AHZDefines.CFG_ENEMY_METER_HEIGHT] = ENEMY_METER_HEIGHT;				
	}
	
	function initializeClips():Void {	
		_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("initializeClips");
		if (_config[AHZDefines.CFG_ENEMY_STAMINA_METER_PATH])
		{			
			_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("Loading: " + _config[AHZDefines.CFG_ENEMY_STAMINA_METER_PATH]);
			metersToLoad.push(_config[AHZDefines.CFG_ENEMY_STAMINA_METER_PATH])
			LoadedEnemyStamina_mc = this.createEmptyMovieClip("LoadedEnemyStamina_mc", EnemyStamina_mc.getDepth());
			mcLoader.loadClip(AHZConfigManager.ResolvePath(_config[AHZDefines.CFG_ENEMY_STAMINA_METER_PATH]), LoadedEnemyStamina_mc);					
			
			// Make the built-in movieclip disapear, it is being replaced, pending any loading errors
			EnemyStamina_mc._alpha = 0;
			EnemyStamina_mc._xscale = 0;
			EnemyStamina_mc._yscale = 0;
		}
		else
		{
			LoadedEnemyStamina_mc = EnemyStamina_mc;
		}
		
		if (_config[AHZDefines.CFG_ENEMY_MAGICKA_METER_PATH])
		{
			
			_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("Loading: " + _config[AHZDefines.CFG_ENEMY_MAGICKA_METER_PATH]);
			metersToLoad.push(_config[AHZDefines.CFG_ENEMY_MAGICKA_METER_PATH])
			LoadedEnemyMagicka_mc = this.createEmptyMovieClip("LoadedEnemyMagicka_mc", EnemyMagicka_mc.getDepth());
			mcLoader.loadClip(AHZConfigManager.ResolvePath(_config[AHZDefines.CFG_ENEMY_MAGICKA_METER_PATH]), LoadedEnemyMagicka_mc);		
			
			// Make the built-in movieclip disapear, it is being replaced, pending any loading errorss
			EnemyMagicka_mc._alpha = 0;
			EnemyMagicka_mc._xscale = 0;
			EnemyMagicka_mc._yscale = 0;
		}
		else
		{
			LoadedEnemyMagicka_mc = EnemyMagicka_mc;
		}		
				
		if (!metersToLoad.length)
		{
			loadIcons();
		}
	}

	public function iconsLoaded(event:Object):Void
	{
		_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("~ iconsLoaded event ~");
		IconContainer.Reset();
		clipsReady();
	}

	public function iconsLoadedError(event:Object):Void
	{
		_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("~ iconsLoadedError event ~");
		IconContainer.Reset();
		clipsReady();
	}

	public function loadIcons(): Void
	{
		_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("Loading icons");
		if (_config[AHZDefines.CFG_ICONS_PATH])
		{
			_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("Loading: " + _config[AHZDefines.CFG_ICONS_PATH]);
			IconContainer.Load(TopRolloverText, AHZConfigManager.ResolvePath(_config[AHZDefines.CFG_ICONS_PATH]), this, "iconsLoaded", "iconsLoadedError");
		}
		else
		{
			_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("Could not load: " + _config[AHZDefines.CFG_ICONS_PATH]);
			clipsReady();
		}							  
	}

	function clipsReady()
	{
		if (!LoadedEnemyMagicka_mc)
			LoadedEnemyMagicka_mc = EnemyMagicka_mc;			
		if (!LoadedEnemyStamina_mc)
			LoadedEnemyStamina_mc = EnemyStamina_mc;
										
		_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("Loading COMPLETED");
		LoadedEnemyMagicka_mc._alpha = 0;
		LoadedEnemyStamina_mc._alpha = 0;
		LoadedEnemyMagicka_mc._xscale = (_root.HUDMovieBaseInstance.EnemyHealth_mc._xscale ) * _config[AHZDefines.CFG_ENEMY_MAGICKA_METER_XSCALE];
		LoadedEnemyMagicka_mc._yscale = (_root.HUDMovieBaseInstance.EnemyHealth_mc._yscale ) * _config[AHZDefines.CFG_ENEMY_MAGICKA_METER_YSCALE];
		LoadedEnemyStamina_mc._xscale = (_root.HUDMovieBaseInstance.EnemyHealth_mc._xscale ) * _config[AHZDefines.CFG_ENEMY_STAMINA_METER_XSCALE];
		LoadedEnemyStamina_mc._yscale =  (_root.HUDMovieBaseInstance.EnemyHealth_mc._yscale) * _config[AHZDefines.CFG_ENEMY_STAMINA_METER_YSCALE];
		HealthStats_mc._xscale =  (_root.HUDMovieBaseInstance.EnemyHealth_mc._xscale) * _config[AHZDefines.CFG_ENEMY_HEALTH_METER_NUMBERS_XSCALE];	
		HealthStats_mc._yscale =  (_root.HUDMovieBaseInstance.EnemyHealth_mc._yscale) * _config[AHZDefines.CFG_ENEMY_HEALTH_METER_NUMBERS_YSCALE];			
		MagickaStats_mc._xscale =  (_root.HUDMovieBaseInstance.EnemyHealth_mc._xscale) * _config[AHZDefines.CFG_ENEMY_MAGICKA_METER_NUMBERS_XSCALE];			
		MagickaStats_mc._yscale =  (_root.HUDMovieBaseInstance.EnemyHealth_mc._yscale) * _config[AHZDefines.CFG_ENEMY_MAGICKA_METER_NUMBERS_YSCALE];							
		StaminaStats_mc._xscale =  (_root.HUDMovieBaseInstance.EnemyHealth_mc._xscale) * _config[AHZDefines.CFG_ENEMY_STAMINA_METER_NUMBERS_XSCALE];		
		StaminaStats_mc._yscale =  (_root.HUDMovieBaseInstance.EnemyHealth_mc._yscale) * _config[AHZDefines.CFG_ENEMY_STAMINA_METER_NUMBERS_YSCALE];				
		enemyMeterHeightScaled = _config[AHZDefines.CFG_ENEMY_METER_HEIGHT] * (_root.HUDMovieBaseInstance.EnemyHealth_mc._yscale/100);

		// Not stacking so height does not matter
		if (!_config[AHZDefines.CFG_ENEMY_METER_USE_STACKING])
		{
			enemyMeterHeightScaled = 1.0;
		}
		
		orginalBracketLocationY = _root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance._y;
		baseX = ((_root.HUDMovieBaseInstance.EnemyHealth_mc._parent._x + _root.HUDMovieBaseInstance.EnemyHealth_mc._x) - stageRect.x);
		baseY = ((_root.HUDMovieBaseInstance.EnemyHealth_mc._parent._y + _root.HUDMovieBaseInstance.EnemyHealth_mc._y) - stageRect.y);
		
		LoadedEnemyMagicka_mc._y = baseY + enemyMeterHeightScaled + _config[AHZDefines.CFG_ENEMY_MAGICKA_METER_YOFFSET];
		LoadedEnemyMagicka_mc._x = baseX + _config[AHZDefines.CFG_ENEMY_MAGICKA_METER_XOFFSET];
		LoadedEnemyStamina_mc._x = baseX  + _config[AHZDefines.CFG_ENEMY_STAMINA_METER_XOFFSET];
		staminaMeterBaseY = baseY + enemyMeterHeightScaled + _config[AHZDefines.CFG_ENEMY_STAMINA_METER_YOFFSET];
		
		EnemyMagickaMeter = new Meter(LoadedEnemyMagicka_mc);
		EnemyStaminaMeter = new Meter(LoadedEnemyStamina_mc);
		EnemyMagickaMeter.SetPercent(0);
		EnemyStaminaMeter.SetPercent(0);		
		LoadedEnemyMagicka_mc._alpha = 0;
		LoadedEnemyStamina_mc._alpha = 0;
		HealthStats_mc._alpha = 0;
		MagickaStats_mc._alpha = 0;
		StaminaStats_mc._alpha = 0;	
		
		HealthStats_mc._x = baseX + _config[AHZDefines.CFG_ENEMY_HEALTH_METER_NUMBERS_XOFFSET];
		HealthStats_mc._y = baseY + _config[AHZDefines.CFG_ENEMY_HEALTH_METER_NUMBERS_YOFFSET];
		
		var mc:MovieClip = MovieClip(_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance);
		AHZBracketInstance = mc.duplicateMovieClip("AHZBracketInstance", this.getNextHighestDepth());
		AHZBracketInstance.gotoAndStop(100);
		AHZBracketInstance.RolloverNameInstance.textAutoSize="shrink";	
		
		AHZBracketInstance._y = _root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance._y;
		AHZBracketInstance._x = _root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance._x;
		
		// Juat move it out of view.  I want the alpha logic to stay in place
		_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance._xscale = 0;
		_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance._yscale = 0;
		_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance._y = -999999;
	
		var enemy_mc = AHZBracketInstance;
		EnemySoul = enemy_mc.createTextField("EnemySoul", 
							enemy_mc.getNextHighestDepth(), 
							 enemy_mc.RolloverNameInstance._x, 
							 enemy_mc.RolloverNameInstance._height - 28, 
							 enemy_mc.RolloverNameInstance._width, 
							 enemy_mc.RolloverNameInstance._height);
		var format = enemy_mc.RolloverNameInstance.getTextFormat();
		format.size = 15;
		EnemySoul.setNewTextFormat(format);
		
		var filter:DropShadowFilter = new DropShadowFilter(2,45,0,100,2,2,1.5);
		var filterArray:Array = new Array();
  		filterArray.push(filter);
		EnemySoul.filters = filterArray;
		EnemySoul._alpha = 0;			
		EnemySoul.textAutoSize="shrink";		

		if (! hooksInstalled)
		{
			// Apply hooks to hook events
			hookFunction(_root.HUDMovieBaseInstance,"SetCrosshairTarget",this,"SetCrosshairTarget");
			hookFunction(_root.HUDMovieBaseInstance,"ShowElements",this,"ShowElements");
			hookFunction(_root.HUDMovieBaseInstance,"SetCompassAngle",this,"SetCompassAngle");
			_global.skse.plugins.AHZmoreHUDPlugin.InstallHooks();
			hooksInstalled = true;
		}		
	}
	
	function ProcessThirdPartyIcons()
	{
		if (_global.skse.plugins.AHZmoreHUDPlugin.IsTargetInFormList("dbmDisp"))
		{
			IconContainer.appendImage("dbmDisp");
		}
	
		var informList = _global.skse.plugins.AHZmoreHUDPlugin.IsTargetInFormList("dbmNew");
		_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("informList: " + informList);
		if (informList)
		{
			_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("Append dbmNew");
			IconContainer.appendImage("dbmNew");
		}
		
		if (_global.skse.plugins.AHZmoreHUDPlugin.IsTargetInFormList("dbmFound"))
		{
			IconContainer.appendImage("dbmFound");
		}		
		
		if (_global.skse.plugins.AHZmoreHUDPlugin.IsTargetInIconList("iEquipQ.png"))
		{
			IconContainer.appendImage("iEquipQ.png");
		}		
		
		if (_global.skse.plugins.AHZmoreHUDPlugin.IsTargetInIconList("iEquipQB.png"))
		{
			IconContainer.appendImage("iEquipQB.png");
		}
		
		if (_global.skse.plugins.AHZmoreHUDPlugin.IsTargetInIconList("iEquipQL.png"))
		{
			IconContainer.appendImage("iEquipQL.png");
		}	
		
		if (_global.skse.plugins.AHZmoreHUDPlugin.IsTargetInIconList("iEquipQR.png"))
		{
			IconContainer.appendImage("iEquipQR.png");
		}		
	}

	function UpdateEnemyMeters(magickaPct:Number, staminaPct:Number, updateRequired:Boolean):Void{
		
		if (magickaPct < 0 && staminaPct < 0)
		{
			EnemyMagickaMeter.SetPercent(0);
			EnemyStaminaMeter.SetPercent(0);
			LoadedEnemyMagicka_mc._alpha = 0;
			LoadedEnemyStamina_mc._alpha = 0;
		}	
		else if (magickaPct < 0)
		{
			EnemyMagickaMeter.SetPercent(0);
			LoadedEnemyMagicka_mc._alpha = 0;
			if (updateRequired || firstStaminaMeterUpdate)
			{
				EnemyStaminaMeter.SetPercent(staminaPct);
				firstStaminaMeterUpdate = false;
			}
			else
			{
				EnemyStaminaMeter.SetTargetPercent(staminaPct);
			}
			LoadedEnemyStamina_mc._alpha = _config[AHZDefines.CFG_ENEMY_STAMINA_METER_ALPHA];
		}
		else if (staminaPct < 0)
		{
			if (updateRequired || firstMagickaMeterUpdate)
			{
				EnemyMagickaMeter.SetPercent(magickaPct);
				firstMagickaMeterUpdate = false;
			}
			else
			{
				EnemyMagickaMeter.SetTargetPercent(magickaPct);
			}
			LoadedEnemyMagicka_mc._alpha = _config[AHZDefines.CFG_ENEMY_MAGICKA_METER_ALPHA];			
			EnemyStaminaMeter.SetPercent(0);
			LoadedEnemyStamina_mc._alpha = 0;
		}
		else
		{
			if (updateRequired || firstMagickaMeterUpdate || firstStaminaMeterUpdate)
			{
				EnemyMagickaMeter.SetPercent(magickaPct);
				EnemyStaminaMeter.SetPercent(staminaPct);
				firstStaminaMeterUpdate = false;
				firstMagickaMeterUpdate = false;
			}
			else
			{
				EnemyMagickaMeter.SetTargetPercent(magickaPct);
				EnemyStaminaMeter.SetTargetPercent(staminaPct);
			}
			LoadedEnemyMagicka_mc._alpha = _config[AHZDefines.CFG_ENEMY_MAGICKA_METER_ALPHA];	
			LoadedEnemyStamina_mc._alpha = _config[AHZDefines.CFG_ENEMY_STAMINA_METER_ALPHA];
		}		
		
		if (!_config[AHZDefines.CFG_ENEMY_METER_USE_STACKING])
		{
			LoadedEnemyStamina_mc._y = staminaMeterBaseY;
		}
		else
		{
			if (magickaPct < 0 && staminaPct < 0)
			{
				AHZBracketInstance._y = orginalBracketLocationY;
			}	
			else if (magickaPct < 0)
			{
				LoadedEnemyStamina_mc._y = staminaMeterBaseY;
				AHZBracketInstance._y = (orginalBracketLocationY + _config[AHZDefines.CFG_ENEMY_METER_HEIGHT]);
			}
			else if (staminaPct < 0)
			{
				AHZBracketInstance._y = (orginalBracketLocationY + _config[AHZDefines.CFG_ENEMY_METER_HEIGHT]);
			}
			else
			{
				LoadedEnemyStamina_mc._y = staminaMeterBaseY + enemyMeterHeightScaled;
				AHZBracketInstance._y = (orginalBracketLocationY + (_config[AHZDefines.CFG_ENEMY_METER_HEIGHT] * 2));
			}	
		}
		
		
		if (!_root.HUDMovieBaseInstance.EnemyHealth_mc._alpha || !_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance._alpha)
		{
			EnemyMagickaMeter.SetPercent(0);
			EnemyStaminaMeter.SetPercent(0);			
			LoadedEnemyMagicka_mc._alpha = 0;
			LoadedEnemyStamina_mc._alpha = 0;
		}

	}

	function UpdateEnemyStats(enemy:Object):Void{
		
		if (!enemy){
			MagickaStats_mc._alpha = 0;
			StaminaStats_mc._alpha = 0;
			return;
		}
		
		MagickaStats_mc._alpha = (showEnemyMagickaStats) ? _config[AHZDefines.CFG_ENEMY_MAGICKA_METER_NUMBERS_ALPHA] : 0;
		StaminaStats_mc._alpha = (showEnemyStaminaStats) ? _config[AHZDefines.CFG_ENEMY_STAMINA_METER_NUMBERS_ALPHA] : 0;
		MagickaStats_mc.Stats.text = enemy.magicka.toString() + "/" + enemy.maxMagicka.toString();
		StaminaStats_mc.Stats.text = enemy.stamina.toString() + "/" + enemy.maxStamina.toString();		
		
		if (!_config[AHZDefines.CFG_ENEMY_METER_USE_STACKING])
		{
			MagickaStats_mc._x = baseX + _config[AHZDefines.CFG_ENEMY_MAGICKA_METER_NUMBERS_XOFFSET];
			MagickaStats_mc._y = baseY + _config[AHZDefines.CFG_ENEMY_MAGICKA_METER_NUMBERS_YOFFSET];
			StaminaStats_mc._x = baseX + _config[AHZDefines.CFG_ENEMY_STAMINA_METER_NUMBERS_XOFFSET];
			StaminaStats_mc._y = baseY + _config[AHZDefines.CFG_ENEMY_STAMINA_METER_NUMBERS_YOFFSET];
		}
		else
		{
			MagickaStats_mc._alpha = (showEnemyMagickaStats) ? _config[AHZDefines.CFG_ENEMY_MAGICKA_METER_NUMBERS_ALPHA] : 0;
			StaminaStats_mc._alpha = (showEnemyStaminaStats) ? _config[AHZDefines.CFG_ENEMY_STAMINA_METER_NUMBERS_ALPHA] : 0;
			MagickaStats_mc.Stats.text = enemy.magicka.toString() + "/" + enemy.maxMagicka.toString();
			StaminaStats_mc.Stats.text = enemy.stamina.toString() + "/" + enemy.maxStamina.toString();
			MagickaStats_mc._x = LoadedEnemyMagicka_mc._x + _config[AHZDefines.CFG_ENEMY_MAGICKA_METER_NUMBERS_XOFFSET];
			MagickaStats_mc._y = LoadedEnemyMagicka_mc._y + _config[AHZDefines.CFG_ENEMY_MAGICKA_METER_NUMBERS_YOFFSET];
			StaminaStats_mc._x = LoadedEnemyStamina_mc._x + _config[AHZDefines.CFG_ENEMY_STAMINA_METER_NUMBERS_XOFFSET];
			StaminaStats_mc._y = LoadedEnemyStamina_mc._y + _config[AHZDefines.CFG_ENEMY_STAMINA_METER_NUMBERS_YOFFSET];
		}
		
		
		if (!_root.HUDMovieBaseInstance.EnemyHealth_mc._alpha || !_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance._alpha)
		{			
			MagickaStats_mc._alpha = 0;
			StaminaStats_mc._alpha = 0;
		}	
	}

	function UpdateEnemyHealthStats(enemy:Object):Void{
		
		if (!enemy){
			HealthStats_mc._alpha = 0;
			return;
		}
		
		if (showEnemyHealthStats)
		{
			if (!_root.HUDMovieBaseInstance.EnemyHealth_mc._alpha || !_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance._alpha)
			{
				HealthStats_mc._alpha = 0;
			}
			else
			{
				HealthStats_mc._alpha = _config[AHZDefines.CFG_ENEMY_HEALTH_METER_NUMBERS_ALPHA];
			}
		}
		else
		{
			HealthStats_mc._alpha = 0;
		}
		
		HealthStats_mc.Stats.text = enemy.health.toString() + "/" + enemy.maxHealth.toString();
	}

	function ShowElements(aMode:String,abShow:Boolean):Void
	{
		var newHUDMode:String = "All";
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

		if (abShow) {
			newHUDMode = aMode;
		} else {
			if (_root.HUDMovieBaseInstance.HUDModes.length > 0) {
				newHUDMode = String(_root.HUDMovieBaseInstance.HUDModes[_root.HUDMovieBaseInstance.HUDModes.length - 1]);
			}
		}
		
		if (newHUDMode == "All" ||
			newHUDMode == "StealthMode" || 
			newHUDMode == "Favor" || 
			newHUDMode == "Swimming" || 
			newHUDMode == "HorseMode" || 
			newHUDMode == "WarHorseMode")
		{
			//_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("hudmode: " + newHUDMode + ": visible");
			this._visible = true;
		}
		else
		{
			//_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("hudmode: " + newHUDMode + ": hidden");
			this._visible = false;
		}

	}

	public function checkForClearedHud():Void
	{
		clearInterval(alphaTimer);
		if (TopRolloverText._alpha < 50)
		{
			IconContainer._alpha = 0;
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
			IconContainer._alpha = TopRolloverText._alpha;
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
		IconContainer._alpha = TopRolloverText._alpha;
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

	// Hooks the main huds function
	function SetCrosshairTarget(abActivate:Boolean,aName:String,abShowButton:Boolean,abTextOnly:Boolean,abFavorMode:Boolean,abShowCrosshair:Boolean,aWeight:Number,aCost:Number,aFieldValue:Number,aFieldText):Void
	{			
		var validTarget:Boolean = false;
		var activateWidgets:Boolean = false;
		var outData:Object = {outObj:Object};
		IconContainer.Reset();
		
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
		ProcessThirdPartyIcons();
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
	
	function interpolate(pBegin:Number, pEnd:Number, pMax:Number, pStep:Number):Number {
		return pBegin + Math.floor((pEnd - pBegin) * pStep / pMax);
	}
	
	function adjustBracketWidth():Void 
	{
		var widthValue:Number = 0.66;
		widthValue = widthValue * AHZBracketInstance.RolloverNameInstance.textWidth;
		widthValue = Math.floor(widthValue) + 5;
		widthValue = Math.min(100, Math.max(0, widthValue));
		
		AHZBracketInstance.gotoAndStop(widthValue);
	}

	function updateDisplayText():Void
	{
		// Could use the extension method SetText, but just to be sure
		if (_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.html){
			AHZBracketInstance.RolloverNameInstance.html = true;
			AHZBracketInstance.RolloverNameInstance.htmlText = _root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.htmlText;
		}
		else{
			AHZBracketInstance.RolloverNameInstance.html = false;
			AHZBracketInstance.RolloverNameInstance.text = _root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.text;
		}
		adjustBracketWidth();
	}


	function isEnemyLevelUpdateRequired():Boolean
	{
		//_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("Checking if visible");
		
		// Check the enemy movie is even visible
		if (_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance._alpha == 0 ||
			!_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.text ||
			!_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.text.length){
			return false;
		}
		
		return true;
	}

	function measureStringWidth(str:String):Number {
		txtMeasureInstance._alpha = 0;
    	txtMeasureInstance.text = str;
    	return txtMeasureInstance.textWidth;
	}

    function GetRootMovies(targetMovie:MovieClip):Array
    {
        var arr:Array = new Array();
        for (var i in targetMovie)
        {
			if (targetMovie[i] instanceof MovieClip)
            {
				var target:MovieClip = MovieClip(targetMovie[i]);
				//_global.skse.plugins.AHZmoreHUDPlugin.AHZLog(i + ": " + target + ": " + target.getDepth());
            }
        }
        return arr;
    }


	function SetCompassAngle(aPlayerAngle: Number, aCompassAngle: Number, abShowCompass: Boolean)
	{			
		var outData:Object = {enemy:Object, player:Object};
		var dataRead:Boolean = false;
		var magickaPct:Number = -1;
		var staminaPct:Number = -1;
		var updateRequired:Boolean = false;
		var levelText:String;
		
		// The enemy meter is not visible so leave
		if (!AHZBracketInstance._alpha)
		{
			return;
		}
		
		//_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("_root.FloatingContainer: " + _root.FloatingContainer);
		
		if (!swapped && _root.FloatingContainer){
			//_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("Before:");
			//GetRootMovies(_root);

			// I'm patching FloatingContriner because the swapDepth is moving HUDMovieBaseInstance above the as2 reserved depth which is the highest allowed
			// as long as _root.FloatingContainer is below the z order of HUDMovieBaseInstance then we are good
			_root.HUDMovieBaseInstance.setDepthTo(_root.FloatingContainer.getDepth() +1);
			
			//_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("After:");
			//GetRootMovies(_root);
			swapped = true;
		}
		
		if (showEnemySoulLevel)
		{
			_global.skse.plugins.AHZmoreHUDPlugin.GetEnemyInformation(outData, LevelTranslated.htmlText);
			dataRead = true;
			if (outData && outData.enemy && outData.enemy.soul){
				EnemySoul._alpha = _root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance._alpha;
				EnemySoul.text = outData.enemy.soul;
			}
			else
			{
				EnemySoul._alpha = 0;
			}
		}
		else
		{
			EnemySoul._alpha = 0;
		}
	
		if (showEnemyMagickaMeter || showEnemyStaminaMeter)
		{
			if (!dataRead)
			{
				_global.skse.plugins.AHZmoreHUDPlugin.GetEnemyInformation(outData, LevelTranslated.htmlText);
				dataRead = true;
			}
	
			if (outData && outData.enemy && showEnemyMagickaMeter){
				magickaPct = outData.enemy.magickaPct;
			}
			if (outData && outData.enemy && showEnemyStaminaMeter){
				staminaPct = outData.enemy.staminaPct;
			}	
			UpdateEnemyMeters(magickaPct, staminaPct, outData.enemy.targetChanged || alphaChanged);		
			UpdateEnemyStats(outData.enemy);
			UpdateEnemyHealthStats(outData.enemy);
		}
		else
		{
			UpdateEnemyMeters(-1, -1, true);
			UpdateEnemyStats(null);
			
			if (showEnemyHealthStats)
			{
				if (!dataRead)
				{
					_global.skse.plugins.AHZmoreHUDPlugin.GetEnemyInformation(outData, LevelTranslated.htmlText);
					dataRead = true;
				}
				UpdateEnemyHealthStats(outData.enemy);
			}
			else
			{
				UpdateEnemyHealthStats(null);
			}
		}
		
		
		//_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("T");
		
		if (showEnemyLevel)
		{
			// If the data was not aquired from reading the soul level then read it here
			if (!dataRead)
			{
				_global.skse.plugins.AHZmoreHUDPlugin.GetEnemyInformation(outData, LevelTranslated.htmlText);		
				dataRead = true;
			}
			
			if (outData && outData.enemy && outData.player)
			{				
				savedEnemyLevelNumber = outData.enemy.level;
				savedPlayerLevelNumber = outData.player.level;
				
				if (!savedPlayerLevelNumber || !savedEnemyLevelNumber)  // Don't show level if the level is 0
				{
					updateDisplayText();
					alphaChanged = false;
					return;
				}
			}
			else
			{
				updateDisplayText();
				alphaChanged = false;
				return;
			}
											
			if (showEnemyLevelMax && showEnemyLevelMin)
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
				_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.html = true;
				// Append the level
				AHZBracketInstance.RolloverNameInstance.html = true;
				AHZBracketInstance.RolloverNameInstance.htmlText = 
					appendHtmlToEnd(_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.htmlText, levelText);
				_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.html = false;
				adjustBracketWidth();					
			}
			// No coloring, turn off html
			else if (savedEnemyLevelNumber)
			{
				AHZBracketInstance.RolloverNameInstance.html = false;
				_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.html = false;
				AHZBracketInstance.RolloverNameInstance.text = _root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance.RolloverNameInstance.text + " (" + savedEnemyLevelNumber.toString() + ")";	
				adjustBracketWidth();
			}
			else
			{
				updateDisplayText();
			}
		}
		else
		{
			updateDisplayText();
		}
		
		alphaChanged = false;
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
					IconContainer._alpha = TopRolloverText._alpha;
					IconContainer.appendImage("ahzKnown");
				}
				
				// The item is enchanted, but the player cannot learn the enchantment
				if (knownEnchantment == 2){
					IconContainer.appendImage("ahzEnch")
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
									 AHZDefines.kNone,
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
								   viewBottomInfoAlwaysValue:Number,
								   showEnemyMagickaMeterValue:Number,
								   showEnemyStaminaMeterValue:Number,
								   showEnemyHealthStatsValue:Number,
								   showEnemyStaminaStatsValue:Number,
								   showEnemyMagickaStatsValue:Number,
								   viewInventoryCountWithZeroValue:Number):Void 
	{				
		viewSideInfo = (sideView>=1);
		viewBottomInfo = (bottomView>=1);
		viewInventoryCount = (inventoryCount>=1); 
		viewInventoryCountWithZero = (viewInventoryCountWithZeroValue>=1);
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
		showEnemyMagickaMeter = (showEnemyMagickaMeterValue>=1);
		showEnemyStaminaMeter = (showEnemyStaminaMeterValue>=1);
		showEnemyHealthStats = (showEnemyHealthStatsValue>=1);
		showEnemyStaminaStats = (showEnemyStaminaStatsValue>=1);
		showEnemyMagickaStats = (showEnemyMagickaStatsValue>=1);
		//_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("updateSettings");
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
			if (formType == AHZDefines.kWeapon || formType == AHZDefines.kAmmo || formType == AHZDefines.kProjectile)
			{
				tempType = AHZDefines.ICT_WEAPON;
			}
			else if (formType == AHZDefines.kArmor)
			{
				tempType = AHZDefines.ICT_ARMOR;
			}
			else
			{
				tempType = AHZDefines.ICT_DEFAULT;
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
		if (viewInventoryCount && (inventoryCount > 0 || viewInventoryCountWithZero))
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
				IconContainer.appendImage("ahzEye");
			}
		}
	}

	function onEnterFrame(): Void
	{
		var previousAlpha = AHZBracketInstance._alpha;
		if (!_root.HUDMovieBaseInstance.EnemyHealth_mc._parent._alpha || !_root.HUDMovieBaseInstance.EnemyHealth_mc._alpha || !_root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance._alpha)
		{	
			AHZBracketInstance._alpha = 0;
		}
		else if (_root.HUDMovieBaseInstance.EnemyHealth_mc._parent._alpha || _root.HUDMovieBaseInstance.EnemyHealth_mc._alpha || _root.HUDMovieBaseInstance.EnemyHealth_mc.BracketsInstance._alpha)
		{
			AHZBracketInstance._alpha = 100;
		}
		
		// Detect visibility change
		if (previousAlpha != AHZBracketInstance._alpha)
		{
			alphaChanged = true;
		}
		
		EnemyMagickaMeter.Update();
		EnemyStaminaMeter.Update();
	
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
	
	private function removePendingClip(s_mc:MovieClip):Void{
		var index = metersToLoad.indexOf(s_mc);
		if (index >= 0){
			metersToLoad.splice(index, 1);
		}	
	}
	
	public function onLoadInit(s_mc: MovieClip): Void
	{
		_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("onLoadInit: " + s_mc);

		removePendingClip(s_mc);
		if (!metersToLoad.length)
		{
			
			loadIcons();
		}
	}
	
	public function onLoadError(s_mc:MovieClip, a_errorCode: String): Void
	{
		// Even on error, we need to pull it from the pending clips.  If the
		// clip could not load it will revert back to the built-in clip
		_global.skse.plugins.AHZmoreHUDPlugin.AHZLog("Error Loading: " + s_mc + " Error: " + a_errorCode);
		
		removePendingClip(s_mc);
		if (!metersToLoad.length)
		{
			loadIcons();
		}		
	}	
}