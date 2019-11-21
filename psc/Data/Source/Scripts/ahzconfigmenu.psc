scriptname AHZConfigMenu extends SKI_ConfigBase

import FISSfactory

GlobalVariable Property AHZBottomWidgetXPercent  Auto  
GlobalVariable Property AHZBottomWidgetYPercent  Auto  
GlobalVariable Property AHZSideWidgetXPercent  Auto  
GlobalVariable Property AHZSideWidgetYPercent  Auto  
GlobalVariable Property AHZShowBottomWidget  Auto  
GlobalVariable Property AHZShowIngredientWidget  Auto
GlobalVariable Property AHZShowEffectsWidget  Auto
GlobalVariable Property AHZShowInventoryCount  Auto  
GlobalVariable Property AHZInventoryWidgetYPercent  Auto  
GlobalVariable Property AHZInventoryWidgetXPercent  Auto  
GlobalVariable Property AHZBottomWidgetRightAligned  Auto  
GlobalVariable Property AHZInventoryWidgetRightAligned  Auto  
GlobalVariable Property AHZEffectsWidgetStyle  Auto  
GlobalVariable Property AHZIngredientsWidgetStyle  Auto  
GlobalVariable Property AHZShowWeightClass  Auto  
GlobalVariable Property AHZShowBooksRead  Auto  
GlobalVariable Property AHZActivationMode  Auto 
GlobalVariable Property AHZToggleState Auto 
GlobalVariable Property AHZHotKey  Auto  
GlobalVariable Property AHZShowBookSkill  Auto  
GlobalVariable Property AHZShowTargetWeight  Auto 
GlobalVariable Property AHZHotkeyToggles  Auto  
GlobalVariable Property AHZShowVW  Auto 
GlobalVariable Property AHZBottomWidgetScale  Auto
GlobalVariable Property AHZInventoryWidgetScale  Auto 
GlobalVariable Property AHZSideWidgetScale  Auto  
GlobalVariable Property AHZShowEnemyLevel  Auto  
GlobalVariable Property AHZEnemyLevelMax  Auto  
GlobalVariable Property AHZEnemyLevelMin  Auto  
GlobalVariable Property AHZShowEnchantmentKnown  Auto  
GlobalVariable Property AHZDisplayDelay  Auto  

Quest Property AHZMainQuestREF  Auto  

; Default wiget positions for the sliders
float Property DefaultBottomXPercent  = 98.0 autoReadOnly
float Property DefaultBottomYPercent  = 100.0 autoReadOnly
float Property DefaultSideXPercent  = 80.0 autoReadOnly
float Property DefaultSideYPercent  = 68.0 autoReadOnly
float Property DefaultInventoryXPercent  = 2.0 autoReadOnly
float Property DefaultInventoryYPercent  = 100.0 autoReadOnly
float Property DefaultBottomScale  = 100.0 autoReadOnly
float Property DefaultInventoryScale  = 100.0 autoReadOnly
float Property DefaultSideScale  = 100.0 autoReadOnly
int Property DefaultEffectsStyle = 2 autoReadOnly
int Property DefaultIngredientsStyle = 0 autoReadOnly
int Property DefultEnemyLevelMin = 10 autoReadOnly
int Property DefultEnemyLevelMax = 10 autoReadOnly

string[] Property WidgetStyles2 Auto
string[] Property AlignmentStyles Auto
string[] Property ActivationModes Auto


; For preset
Int FissLoad
Int FissSave
bool ValidatePapyrusUtil=false
string[] saConfigs
string sConfigNameCurrent = "Default"
int selectedConfig = 0

; SCRIPT VERSION ----------------------------------------------------------------------------------
;
; NOTE:
; This is an example to show you how to update scripts after they have been deployed.
;
; History
;
; 1 - Initial version
; 2 - Cleanup, no actual changes to menu
; 3 - Added widget positioning options
; 4 - Added the option to show the inventory count
; 5 - Created separate pages for each widget
; 8 - Added Scaling
; 9 - Added A page for the enemy level
; 10 - Updated the widget style names
; 12 - Misspelling
; 13 - Added seconds translation
int function GetVersion()
	return 13
endFunction


; PRIVATE VARIABLES -------------------------------------------------------------------------------

; --- Version 10 ---

; Lists

; OIDs (T:Text B:Toggle S:Slider M:Menu, C:Color, K:Key)

; General Settings --------------------
int			_toggle1OID_B				; Show Player Data Widget
int			_toggle2OID_B				; Show Target Data Widget (Ingredients)
int			_toggle3OID_B				; Show Target Data Widget (Effects)
int			_toggle4OID_B				; Uninstall
int			_toggle5OID_B				; Show Target Inventory
int			_toggle8OID_B				; Show Books Read
int			_toggle9OID_B				; Show Armor Weight Class
int			_toggle10OID_B				; Show Book Skill
int			_toggle11OID_B				; Show Target Weight with Player's Weight
int			_toggle12OID_B				; Hotkey Toggles
int			_toggle13OID_B				; Show VW
int			_toggle15OID_B				; Show Known Enchantment
int         _activationModeOID_M		; Activate Mode Drop Down
int 		_activationKeyMapOID_K		; Activation Key Binding
int			_sliderDisplayDelay_OID_S	; Display Delay

; Side Widget Settings ------------------
int			_sliderSideX_OID_S
int			_sliderSideY_OID_S
int			_sliderSideScale_OID_S
int			_ingredientStyleOID_M		; Ingredient display syle
int			_effectsStyleOID_M			; Effects display style 

; Bottom Widget Settings ----------------
int			_sliderBottomX_OID_S
int			_sliderBottomY_OID_S
int         _bottomAlignmentOID_M
int			_sliderBottomScale_OID_S

; Inventory Count Widget ---------------
int			_sliderInventoryX_OID_S
int			_sliderInventoryY_OID_S
int			_sliderInventoryScale_OID_S
int         _inventoryAlignmentOID_M

; Enemy Level Settings ----------------
int			_toggle14OID_B				; Show Enemy Level
int			_sliderEnemyLevelMax_OID_S
int			_sliderEnemyLevelMin_OID_S

;Private variables
bool 		_uninstallMod			= false


; INITIALIZATION ----------------------------------------------------------------------------------

; @overrides SKI_ConfigBase
event OnConfigInit()
	Pages = new string[6]
	Pages[0] = "$mHUD_GeneralPage"
	Pages[1] = "$mHUD_PlayersDataPage"
	Pages[2] = "$mHUD_TargetsDataPage"
	Pages[3] = "$mHUD_TargetsCountPage"
	Pages[4] = "$mHUD_EnemysLevelPage"
	Pages[5] = "$mHUD_PresetPage"

	WidgetStyles2 = new String[4]
	WidgetStyles2[0] = "$mHUD_Bullets"
	WidgetStyles2[1] = "$mHUD_DialogNoBackground"
	WidgetStyles2[2] = "$mHUD_Dialog"
	WidgetStyles2[3] = "$mHUD_TextOnly"

	AlignmentStyles = new String[3]
	AlignmentStyles[0] = "$mHUD_Left"
	AlignmentStyles[1] = "$mHUD_Right"
	AlignmentStyles[2] = "$mHUD_Center"	

	ActivationModes = new String[5]
	ActivationModes[0] = "$mHUD_AutoAlways"
	ActivationModes[1] = "$mHUD_AutoNoCombat"
	ActivationModes[2] = "$mHUD_HotkeyOnly"		

endEvent

event OnConfigOpen()
	_uninstallMod = false;
	
	ValidatePapyrusUtil = PapyrusUtil.getVersion()>31
	if ValidatePapyrusUtil
		saConfigs = JSONUtil.JsonInFolder("..\\moreHUD\\")
		int i=saConfigs.length
		while i
			i-=1
			if StringUtil.find(saConfigs[i], ".json")!=-1
				saConfigs[i] = StringUtil.subString(saConfigs[i], 0, StringUtil.getLength(saConfigs[i]) - 5)
			endIf
		endWhile
	endIf
	
endEvent

event OnConfigClose()
	if (_uninstallMod == true)
		(AHZMainQuestREF as AHZMainQuest).Unregister()
		Stop()
	else
		(AHZMainQuestREF as AHZMainQuest).UpdateSettings(false)
	endIf
endEvent

; @implements SKI_QuestBase
event OnVersionUpdate(int a_version)
	{Called when a version update of this script has been detected}
	OnConfigInit()							
endEvent


; EVENTS ------------------------------------------------------------------------------------------

; @implements SKI_ConfigBase
event OnPageReset(string a_page)
	{Called when a new page is selected, including the initial empty page}

	if (a_page == "")
		LoadCustomContent("exported/AHZmoreHUDLogo.dds", 150,100)
		return
	else
		UnloadCustomContent()
		SetCursorFillMode(TOP_TO_BOTTOM)
	endif

	if (a_page == "$mHUD_GeneralPage")
		AddHeaderOption("$mHUD_ActivationOptions")
		_activationModeOID_M 	= AddMenuOption("$mHUD_ActivationMode", ActivationModes[AHZActivationMode.GetValueInt()])
		_activationKeyMapOID_K 	= AddKeyMapOption("$mHUD_ActivationHotKey",AHZHotKey.GetValueInt(),  OPTION_FLAG_WITH_UNMAP);
		_toggle12OID_B			= AddToggleOption("$mHUD_HotKeyToggles", AHZHotkeyToggles.GetValueInt())
		_sliderDisplayDelay_OID_S = AddSliderOption("$mHUD_DisplayDelay", AHZDisplayDelay.GetValue(), "{2} S")
		AddEmptyOption()

		AddHeaderOption("$mHUD_WidgetExtras")		
		_toggle8OID_B			= AddToggleOption("$mHUD_ShowBooksRead", AHZShowBooksRead.GetValueInt())
		_toggle9OID_B			= AddToggleOption("$mHUD_ShowArmorWeightClass", AHZShowWeightClass.GetValueInt())
		_toggle10OID_B			= AddToggleOption("$mHUD_ShowBookSkill", AHZShowBookSkill.GetValueInt())
		_toggle13OID_B			= AddToggleOption("$mHUD_ShowValueToWeightRatio", AHZShowVW.GetValueInt())
		_toggle15OID_B			= AddToggleOption("$mHUD_ShowKnownEnchantment", AHZShowEnchantmentKnown.GetValueInt())
		AddEmptyOption()

		SetCursorPosition(1)
		AddHeaderOption("$mHUD_Maintenance")	
		_toggle4OID_B			= AddToggleOption("$mHUD_Uninstall", _uninstallMod)
		
	elseif (a_page == "$mHUD_PlayersDataPage")
		AddHeaderOption("$mHUD_PlayersDataWidget")
		_toggle1OID_B			= AddToggleOption("$mHUD_Visible", AHZShowBottomWidget.GetValueInt())
		_bottomAlignmentOID_M 	= AddMenuOption("$mHUD_Alignment", AlignmentStyles[AHZBottomWidgetRightAligned.GetValueInt()])
		AddEmptyOption()
		_toggle11OID_B			= AddToggleOption("$mHUD_DisplayTargetWeight", AHZShowTargetWeight.GetValueInt())

		SetCursorPosition(1)
		AddHeaderOption("$mHUD_Position")
		_sliderBottomX_OID_S 	= AddSliderOption("$mHUD_XPosition", AHZBottomWidgetXPercent.GetValue(), "{2}%")
		_sliderBottomY_OID_S 	= AddSliderOption("$mHUD_YPosition", AHZBottomWidgetYPercent.GetValue(), "{2}%")

		AddEmptyOption()
		AddHeaderOption("$mHUD_Scale")	
		_sliderBottomScale_OID_S = AddSliderOption("$mHUD_Scale", AHZBottomWidgetScale.GetValue(), "{2}%")
	elseif (a_page == "$mHUD_TargetsDataPage")
		AddHeaderOption("$mHUD_TargetsDataWidget")
		_toggle2OID_B			= AddToggleOption("$mHUD_ShowIngredientEffects", AHZShowIngredientWidget.GetValueInt())
		_toggle3OID_B			= AddToggleOption("$mHUD_ShowOtherEffects", AHZShowEffectsWidget.GetValueInt())
		AddEmptyOption()
		AddHeaderOption("$mHUD_WidgetStyle")
		_ingredientStyleOID_M = AddMenuOption("$mHUD_Ingredients", WidgetStyles2[AHZIngredientsWidgetStyle.GetValueInt()])
		_effectsStyleOID_M = AddMenuOption("$mHUD_OtherEffects", WidgetStyles2[AHZEffectsWidgetStyle.GetValueInt()])

		SetCursorPosition(1)
		AddHeaderOption("$mHUD_Position")		
		_sliderSideX_OID_S 		= AddSliderOption("$mHUD_XPosition", AHZSideWidgetXPercent.GetValue(), "{2}%")
		_sliderSideY_OID_S 		= AddSliderOption("$mHUD_YPosition", AHZSideWidgetYPercent.GetValue(), "{2}%")

		AddEmptyOption()
		AddHeaderOption("$mHUD_Scale")	
		_sliderSideScale_OID_S = AddSliderOption("$mHUD_Scale", AHZSideWidgetScale.GetValue(), "{2}%")					
	elseif ((a_page == "$mHUD_TargetsInventory") || (a_page == "$mHUD_TargetsCountPage"))
		AddHeaderOption("$mHUD_TargetsCountWidget")
		_toggle5OID_B 			= AddToggleOption("$mHUD_Visible", AHZShowInventoryCount.GetValueInt())
		_inventoryAlignmentOID_M = AddMenuOption("$mHUD_Alignment", AlignmentStyles[AHZInventoryWidgetRightAligned.GetValueInt()])

		AddEmptyOption()

		SetCursorPosition(1)
		AddHeaderOption("$mHUD_Position")	
		_sliderInventoryX_OID_S = AddSliderOption("$mHUD_XPosition", AHZInventoryWidgetXPercent.GetValue(), "{2}%")
		_sliderInventoryY_OID_S = AddSliderOption("$mHUD_YPosition", AHZInventoryWidgetYPercent.GetValue(), "{2}%")

		AddEmptyOption()
		AddHeaderOption("$mHUD_Scale")	
		_sliderInventoryScale_OID_S = AddSliderOption("$mHUD_Scale", AHZInventoryWidgetScale.GetValue(), "{2}%")		
	elseif (a_page == "$mHUD_EnemysLevelPage")
		AddHeaderOption("$mHUD_GeneralEnemyLevel")
		_toggle14OID_B			= AddToggleOption("$mHUD_ShowEnemyLevel", AHZShowEnemyLevel.GetValueInt())
		AddHeaderOption("$mHUD_ColorScale")
		_sliderEnemyLevelMin_OID_S = AddSliderOption("$mHUD_LevelBelowPlayer", AHZEnemyLevelMin.GetValue(), "{0}")
		_sliderEnemyLevelMax_OID_S = AddSliderOption("$mHUD_LevelAbovePlayer", AHZEnemyLevelMax.GetValue(), "{0}")	
	elseIf a_page == "$mHUD_PresetPage"
		SetCursorFillMode(TOP_TO_BOTTOM)
		SetCursorPosition(0)
		AddHeaderOption("$mHUD_PresetFiss")
		AddTextOptionST("FISS_SAVE_PRESET", "$mHUD_SavePreset", "$mHUD_GO", 0)
		AddTextOptionST("FISS_LOAD_PRESET", "$mHUD_LoadPreset", "$mHUD_GO", 0)
		AddHeaderOption("$mHUD_PresetPapyrusUtil")
		if ValidatePapyrusUtil
			AddInputOptionST("CurrentConfigIN", "$mHUD_PresetName", sConfigNameCurrent)
			AddTextOptionST("SaveCurrentConfigBN", "$mHUD_SavePreset", "$mHUD_GO")
		else
			AddEmptyOption()
		endIf
		if saConfigs.length>0
			AddMenuOptionST("ConfigToLoadMN", "$mHUD_PresetToload", saConfigs[selectedConfig])
			AddTextOptionST("LoadSelectedConfigBN", "$mHUD_LoadPreset", "$mHUD_GO")
		else
			AddMenuOptionST("ConfigToLoadMN", "$mHUD_PresetToload", "$mHUD_NoPresetsFound", OPTION_FLAG_DISABLED)
			AddTextOptionST("LoadSelectedConfigBN", "$mHUD_LoadPreset", "$mHUD_GO", OPTION_FLAG_DISABLED)
		endIf	
	endif

endEvent

; @implements SKI_ConfigBase
event OnOptionSelect(int a_option)
	{Called when the user selects a non-dialog option}
	
	; Toggle books read
	if (a_option == _toggle8OID_B)
		ToggleGlobalInt(AHZShowBooksRead)
		SetToggleOptionValue(_toggle8OID_B, AHZShowBooksRead.GetValueInt())
	endif

	; Toggle weight class
	if (a_option == _toggle9OID_B)
		ToggleGlobalInt(AHZShowWeightClass)
		SetToggleOptionValue(_toggle9OID_B, AHZShowWeightClass.GetValueInt())
	endif

	; Toggle book skill
	if (a_option == _toggle10OID_B)
		ToggleGlobalInt(AHZShowBookSkill)
		SetToggleOptionValue(_toggle10OID_B, AHZShowBookSkill.GetValueInt())
	endif

	; Toggle VW
	if (a_option == _toggle13OID_B)
		ToggleGlobalInt(AHZShowVW)
		SetToggleOptionValue(_toggle13OID_B, AHZShowVW.GetValueInt())
	endif

	; Toggle Known Enchantment
	if (a_option == _toggle15OID_B)
		ToggleGlobalInt(AHZShowEnchantmentKnown)
		SetToggleOptionValue(_toggle15OID_B, AHZShowEnchantmentKnown.GetValueInt())
	endif

	; Toggle Enemy Level
	if (a_option == _toggle14OID_B)
		ToggleGlobalInt(AHZShowEnemyLevel)
		SetToggleOptionValue(_toggle14OID_B, AHZShowEnemyLevel.GetValueInt())
	endif

	; Toggle Target+Players Weight
	if (a_option == _toggle11OID_B)
		ToggleGlobalInt(AHZShowTargetWeight)
		SetToggleOptionValue(_toggle11OID_B, AHZShowTargetWeight.GetValueInt())
	endif

	; Hotkey Toggles
	if (a_option == _toggle12OID_B)
		ToggleGlobalInt(AHZHotkeyToggles)
		SetToggleOptionValue(_toggle12OID_B, AHZHotkeyToggles.GetValueInt())
	endif

	; Toggle bottom widget
	if (a_option == _toggle1OID_B)
		ToggleGlobalInt(AHZShowBottomWidget)
		SetToggleOptionValue(_toggle1OID_B, AHZShowBottomWidget.GetValueInt())
	endif
	
	; Toggle Inventory Count
	if (a_option == _toggle5OID_B)
		ToggleGlobalInt(AHZShowInventoryCount)
		SetToggleOptionValue(_toggle5OID_B, AHZShowInventoryCount.GetValueInt())
	endif

	; Toggle side widget for ingredient effects
	if (a_option == _toggle2OID_B)
		ToggleGlobalInt(AHZShowIngredientWidget)
		SetToggleOptionValue(_toggle2OID_B, AHZShowIngredientWidget.GetValueInt())
	endif	
	
	; Toggle side widget for effects
	if (a_option == _toggle3OID_B)
		ToggleGlobalInt(AHZShowEffectsWidget)
		SetToggleOptionValue(_toggle3OID_B, AHZShowEffectsWidget.GetValueInt())
	endif	

	; Toggle uninstall
	if (a_option == _toggle4OID_B)
		_uninstallMod = !_uninstallMod
		if (_uninstallMod == true)
			if (ShowMessage("$mHUD_UnistallQuestion", true))
			else
				_uninstallMod = false
			endIf
		endif
		SetToggleOptionValue(_toggle4OID_B, _uninstallMod)
	endif		

endEvent

; @implements SKI_ConfigBase
event OnOptionSliderOpen(int a_option)
	{Called when the user selects a slider option}

	if (a_option == _sliderBottomX_OID_S)
		SetSliderDialogStartValue(AHZBottomWidgetXPercent.GetValue())
		SetSliderDialogDefaultValue(DefaultBottomXPercent)
		SetSliderDialogRange(0, 125)
		SetSliderDialogInterval(0.01)
	endIf

	if (a_option == _sliderBottomY_OID_S)
		SetSliderDialogStartValue(AHZBottomWidgetYPercent.GetValue())
		SetSliderDialogDefaultValue(DefaultBottomYPercent)
		SetSliderDialogRange(0, 125)
		SetSliderDialogInterval(0.01)
	endIf	

	if (a_option == _sliderSideX_OID_S)
		SetSliderDialogStartValue(AHZSideWidgetXPercent.GetValue())
		SetSliderDialogDefaultValue(DefaultSideXPercent)
		SetSliderDialogRange(0, 125)
		SetSliderDialogInterval(0.01)
	endIf		

	if (a_option == _sliderSideY_OID_S)
		SetSliderDialogStartValue(AHZSideWidgetYPercent.GetValue())
		SetSliderDialogDefaultValue(DefaultSideYPercent)
		SetSliderDialogRange(0, 125)
		SetSliderDialogInterval(0.01)
	endIf	

	if (a_option == _sliderInventoryX_OID_S)
		SetSliderDialogStartValue(AHZInventoryWidgetXPercent.GetValue())
		SetSliderDialogDefaultValue(DefaultInventoryXPercent)
		SetSliderDialogRange(0, 125)
		SetSliderDialogInterval(0.01)
	endIf		

	if (a_option == _sliderInventoryY_OID_S)
		SetSliderDialogStartValue(AHZInventoryWidgetYPercent.GetValue())
		SetSliderDialogDefaultValue(DefaultInventoryYPercent)
		SetSliderDialogRange(0, 125)
		SetSliderDialogInterval(0.01)
	endIf		

	if (a_option == _sliderBottomScale_OID_S)
		SetSliderDialogStartValue(AHZBottomWidgetScale.GetValue())
		SetSliderDialogDefaultValue(DefaultBottomScale)
		SetSliderDialogRange(25, 200)
		SetSliderDialogInterval(0.01)
	endIf	

	if (a_option == _sliderSideScale_OID_S)
		SetSliderDialogStartValue(AHZSideWidgetScale.GetValue())
		SetSliderDialogDefaultValue(DefaultSideScale)
		SetSliderDialogRange(25, 200)
		SetSliderDialogInterval(0.01)
	endIf

	if (a_option == _sliderInventoryScale_OID_S)
		SetSliderDialogStartValue(AHZInventoryWidgetScale.GetValue())
		SetSliderDialogDefaultValue(DefaultInventoryScale)
		SetSliderDialogRange(25, 200)
		SetSliderDialogInterval(0.01)
	endIf	

	if (a_option == _sliderEnemyLevelMin_OID_S)
		SetSliderDialogStartValue(AHZEnemyLevelMin.GetValue())
		SetSliderDialogDefaultValue(DefultEnemyLevelMin)
		SetSliderDialogRange(0, 100)
		SetSliderDialogInterval(1)
	endif

	if (a_option == _sliderEnemyLevelMax_OID_S)
		SetSliderDialogStartValue(AHZEnemyLevelMax.GetValue())
		SetSliderDialogDefaultValue(DefultEnemyLevelMax)
		SetSliderDialogRange(0, 100)
		SetSliderDialogInterval(1)
	endif	

	if (a_option == _sliderDisplayDelay_OID_S)
		SetSliderDialogStartValue(AHZDisplayDelay.GetValue())
		SetSliderDialogDefaultValue(0)
		SetSliderDialogRange(0, 2.00)
		SetSliderDialogInterval(0.01)
	endif	

endEvent

; @implements SKI_ConfigBase
event OnOptionSliderAccept(int a_option, float a_value)

	if (a_option == _sliderBottomX_OID_S)
		AHZBottomWidgetXPercent.SetValue(a_value)
		SetSliderOptionValue(a_option, a_value, "{2}%")
	endIf

	if (a_option == _sliderBottomY_OID_S)
		AHZBottomWidgetYPercent.SetValue(a_value)
		SetSliderOptionValue(a_option, a_value, "{2}%")
	endIf	

	if (a_option == _sliderSideX_OID_S)
		AHZSideWidgetXPercent.SetValue(a_value)
		SetSliderOptionValue(a_option, a_value, "{2}%")
	endIf		

	if (a_option == _sliderSideY_OID_S)
		AHZSideWidgetYPercent.SetValue(a_value)
		SetSliderOptionValue(a_option, a_value, "{2}%")
	endIf	

	if (a_option == _sliderInventoryX_OID_S)
		AHZInventoryWidgetXPercent.SetValue(a_value)
		SetSliderOptionValue(a_option, a_value, "{2}%")
	endIf		

	if (a_option == _sliderInventoryY_OID_S)
		AHZInventoryWidgetYPercent.SetValue(a_value)
		SetSliderOptionValue(a_option, a_value, "{2}%")
	endIf	

	if (a_option == _sliderBottomScale_OID_S)
		AHZBottomWidgetScale.SetValue(a_value)
		SetSliderOptionValue(a_option, a_value, "{2}%")
	endIf

	if (a_option == _sliderSideScale_OID_S)
		AHZSideWidgetScale.SetValue(a_value)
		SetSliderOptionValue(a_option, a_value, "{2}%")
	endIf

	if (a_option == _sliderInventoryScale_OID_S)
		AHZInventoryWidgetScale.SetValue(a_value)
		SetSliderOptionValue(a_option, a_value, "{2}%")
	endIf	

	if (a_option == _sliderEnemyLevelMin_OID_S)
		AHZEnemyLevelMin.SetValue(a_value)
		SetSliderOptionValue(a_option, a_value, "{0}")
	endif	

	if (a_option == _sliderEnemyLevelMax_OID_S)
		AHZEnemyLevelMax.SetValue(a_value)
		SetSliderOptionValue(a_option, a_value, "{0}")
	endif	

	if (a_option == _sliderDisplayDelay_OID_S)
		AHZDisplayDelay.SetValue(a_value)
		SetSliderOptionValue(a_option, a_value, "{2} S")
	endif	

endEvent

; @implements SKI_ConfigBase
event OnOptionMenuOpen(int a_option)
	{Called when the user selects a menu option}
	if (a_option == _ingredientStyleOID_M)
		SetMenuDialogStartIndex(AHZIngredientsWidgetStyle.GetValueInt())
		SetMenuDialogDefaultIndex(DefaultIngredientsStyle)
		SetMenuDialogOptions(WidgetStyles2)
	endIf

	if (a_option == _effectsStyleOID_M)
		SetMenuDialogStartIndex(AHZEffectsWidgetStyle.GetValueInt())
		SetMenuDialogDefaultIndex(DefaultEffectsStyle)
		SetMenuDialogOptions(WidgetStyles2)
	endIf	

	if (a_option == _bottomAlignmentOID_M)
		SetMenuDialogStartIndex(AHZBottomWidgetRightAligned.GetValueInt())
		SetMenuDialogDefaultIndex(1)
		SetMenuDialogOptions(AlignmentStyles)
	endif

	if (a_option == _inventoryAlignmentOID_M)
		SetMenuDialogStartIndex(AHZInventoryWidgetRightAligned.GetValueInt())
		SetMenuDialogDefaultIndex(0)
		SetMenuDialogOptions(AlignmentStyles)
	endif

	if (a_option == _activationModeOID_M)
		SetMenuDialogStartIndex(AHZActivationMode.GetValueInt())
		SetMenuDialogDefaultIndex(0)
		SetMenuDialogOptions(ActivationModes)
	endif

endEvent

; @implements SKI_ConfigBase
event OnOptionMenuAccept(int a_option, int a_index)
	{Called when the user accepts a new menu entry}

	if (a_option == _ingredientStyleOID_M)
		AHZIngredientsWidgetStyle.SetValueInt(a_index)
		SetMenuOptionValue(a_option, WidgetStyles2[a_index])
	endIf

	if (a_option == _effectsStyleOID_M)
		AHZEffectsWidgetStyle.SetValueInt(a_index)
		SetMenuOptionValue(a_option, WidgetStyles2[a_index])
	endIf	

	if (a_option == _bottomAlignmentOID_M)
		AHZBottomWidgetRightAligned.SetValueInt(a_index)
		SetMenuOptionValue(a_option, AlignmentStyles[a_index])		
	endif	

	if (a_option == _inventoryAlignmentOID_M)
		AHZInventoryWidgetRightAligned.SetValueInt(a_index)
		SetMenuOptionValue(a_option, AlignmentStyles[a_index])		
	endif	

	if (a_option == _activationModeOID_M)
		AHZActivationMode.SetValueInt(a_index)
		SetMenuOptionValue(a_option, ActivationModes[a_index])		
	endif	
endEvent

; @implements SKI_ConfigBase
event OnOptionDefault(int a_option)
	{Called when resetting an option to its default value}
	if (a_option == _activationKeyMapOID_K)
		AHZHotKey.SetValueInt(-1);
		SetKeymapOptionValue(a_option, -1)
	endif
endEvent

; @implements SKI_ConfigBase
event OnOptionColorOpen(int a_option)
	{Called when a color option has been selected}
endEvent

; @implements SKI_ConfigBase
event OnOptionColorAccept(int a_option, int a_color)
	{Called when a new color has been accepted}
endEvent

; @implements SKI_ConfigBase
event OnOptionKeyMapChange(int a_option, int a_keyCode, string a_conflictControl, string a_conflictName)
	{Called when a key has been remapped}

	if (a_option == _activationKeyMapOID_K)

		bool continue = true

		if (a_conflictControl != "" && a_keyCode != -1)
			string msg

			if (a_conflictName != "")
				msg = "This key is already mapped to:\n'" + a_conflictControl + "'\n(" + a_conflictName + ")\n\nAre you sure you want to continue?"
			else
				msg = "This key is already mapped to:\n'" + a_conflictControl + "'\n\nAre you sure you want to continue?"
			endIf

			continue = ShowMessage(msg, true, "$mHUD_Yes", "$mHUD_No")
		endIf

		if (continue)
			AHZHotKey.SetValueInt(a_keyCode);
			SetKeymapOptionValue(a_option, a_keyCode)
		endIf
	endIf
endEvent

; @implements SKI_ConfigBase
event OnOptionHighlight(int a_option)
	{Called when the user highlights an option}
	
	; TODO: localization
	if (a_option == _toggle8OID_B)
		SetInfoText("$mHUD_ShowsIconBook")
	endif

	if (a_option == _toggle9OID_B)
		SetInfoText("$mHUD_ShowsArmor")
	endif

	if (a_option == _toggle10OID_B)
		SetInfoText("$mHUD_ShowsSkill")
	endif

	if (a_option == _toggle13OID_B)
		SetInfoText("$mHUD_ShowsValue")
	endif

	if (a_option == _toggle14OID_B)
		SetInfoText("$mHUD_ShowsEnemyslevel")
	endif

	if (a_option == _toggle15OID_B)
		SetInfoText("$mHUD_ShowsIconDisenchanted")
	endif

	if (a_option == _toggle11OID_B)
		SetInfoText("$mHUD_ShowsTargetsWeight")
	endif

	if (a_option == _toggle12OID_B)
		SetInfoText("$mHUD_ActivationKey")
	endif

	if (a_option == _toggle1OID_B)
		SetInfoText("$mHUD_ShowsBottomBar")
	endif		
	
	if (a_option == _toggle2OID_B)
		SetInfoText("$mHUD_ShowsIngredientEffects")
	endif		
	
	if (a_option == _toggle3OID_B)
		SetInfoText("$mHUD_ShowsEffectsEnchanted")
	endif

	if (a_option == _toggle5OID_B)
		SetInfoText("$mHUD_ShowsCurrentNumber")
	endif	

	if (a_option == _sliderBottomX_OID_S)
		SetInfoText("$mHUD_SetsXPositionBottom")
	endIf

	if (a_option == _sliderBottomY_OID_S)
		SetInfoText("$mHUD_SetsYPositionBottom")
	endIf	

	if (a_option == _sliderSideX_OID_S)
		SetInfoText("$mHUD_SetsXPositionIngredient")
	endIf		

	if (a_option == _sliderSideY_OID_S)
		SetInfoText("$mHUD_SetsYPositionIngredient")
	endIf

	if (a_option == _sliderInventoryX_OID_S)
		SetInfoText("$mHUD_SetsXPositionInventory")
	endIf		

	if (a_option == _sliderInventoryY_OID_S)
		SetInfoText("$mHUD_SetsYPositionInventory")
	endIf

	if (a_option == _ingredientStyleOID_M)
		SetInfoText("$mHUD_SetsStyleIngredient")
	endIf

	if (a_option == _effectsStyleOID_M)
		SetInfoText("$mHUD_SetsStyleEffects")
	endIf	

	if (a_option == _bottomAlignmentOID_M)
		SetInfoText("$mHUD_SetsAlignmentPlayer")
	endif

	if (a_option == _inventoryAlignmentOID_M)
		SetInfoText("$mHUD_SetsAlignmentInventory")
	endif

	if (a_option == _activationKeyMapOID_K)
		SetInfoText("$mHUD_SetsHotkey")
	endif

	if (a_option == _activationModeOID_M)
		SetInfoText("$mHUD_SetsActivationMode")
	endif

	if (a_option == _sliderBottomScale_OID_S)
		SetInfoText("$mHUD_SetsScaleBottom")
	endIf

	if (a_option == _sliderSideScale_OID_S)
		SetInfoText("$mHUD_SetsScaleIngredient")
	endIf

	if (a_option == _sliderInventoryScale_OID_S)
		SetInfoText("$mHUD_SetsScaleInventory")
	endIf

	if (a_option == _sliderEnemyLevelMin_OID_S)
		SetInfoText("$mHUD_EnemiesFullyGreen")
	endif	

	if (a_option == _sliderEnemyLevelMax_OID_S)
		SetInfoText("$mHUD_EnemiesFullyRed")
	endif	

	if (a_option == _sliderDisplayDelay_OID_S)
		SetInfoText("$mHUD_DisplayDelayDescription")
	endif	

endEvent

function FISS_SAVE()

	fissinterface fiss = FISSfactory.getFISS()
	if !fiss
		debug.MessageBox("$mHUD_FissMiss")
		return 
	endIf
	fiss.beginSave("MoreHUD.xml", "MoreHUD")
	fiss.saveBool("fissExists", true)
	fiss.saveInt("AHZActivationMode", AHZActivationMode.GetValueInt())
	fiss.saveInt("AHZIngredientsWidgetStyle", AHZIngredientsWidgetStyle.GetValueInt())
	fiss.saveInt("AHZEffectsWidgetStyle", AHZEffectsWidgetStyle.GetValueInt())
	fiss.saveInt("AHZBottomWidgetRightAligned", AHZBottomWidgetRightAligned.GetValueInt())
	fiss.saveInt("AHZInventoryWidgetRightAligned", AHZInventoryWidgetRightAligned.GetValueInt())
	fiss.saveInt("AHZHotKey", AHZHotKey.GetValueInt())
	fiss.saveInt("AHZHotkeyToggles", AHZHotkeyToggles.GetValueInt())
	fiss.saveInt("AHZShowBooksRead", AHZShowBooksRead.GetValueInt())
	fiss.saveInt("AHZShowWeightClass", AHZShowWeightClass.GetValueInt())
	fiss.saveInt("AHZShowBookSkill", AHZShowBookSkill.GetValueInt())
	fiss.saveInt("AHZShowBottomWidget", AHZShowBottomWidget.GetValueInt())
	fiss.saveInt("AHZShowIngredientWidget", AHZShowIngredientWidget.GetValueInt())
	fiss.saveInt("AHZShowEffectsWidget", AHZShowEffectsWidget.GetValueInt())
	fiss.saveInt("AHZShowInventoryCount", AHZShowInventoryCount.GetValueInt())	
	fiss.saveInt("AHZShowTargetWeight", AHZShowTargetWeight.GetValueInt())	
	fiss.saveInt("AHZShowVW", AHZShowVW.GetValueInt())
	fiss.saveInt("AHZBottomWidgetScale", AHZBottomWidgetScale.GetValueInt())
	fiss.saveInt("AHZInventoryWidgetScale", AHZInventoryWidgetScale.GetValueInt())
	fiss.saveInt("AHZSideWidgetScale", AHZSideWidgetScale.GetValueInt())
	fiss.saveInt("AHZEnemyLevelMax", AHZEnemyLevelMax.GetValueInt())
	fiss.saveInt("AHZEnemyLevelMin", AHZEnemyLevelMin.GetValueInt())
	fiss.saveInt("AHZShowEnemyLevel", AHZShowEnemyLevel.GetValueInt())
	fiss.saveInt("AHZShowEnchantmentKnown", AHZShowEnchantmentKnown.GetValueInt())
	fiss.saveFloat("AHZDisplayDelay", AHZDisplayDelay.GetValue())
	fiss.saveFloat("AHZBottomWidgetXPercent", AHZBottomWidgetXPercent.GetValue())
	fiss.saveFloat("AHZBottomWidgetYPercent", AHZBottomWidgetYPercent.GetValue())
	fiss.saveFloat("AHZSideWidgetXPercent", AHZSideWidgetXPercent.GetValue())
	fiss.saveFloat("AHZSideWidgetYPercent", AHZSideWidgetYPercent.GetValue())
	fiss.saveFloat("AHZInventoryWidgetXPercent", AHZInventoryWidgetXPercent.GetValue())
	fiss.saveFloat("AHZInventoryWidgetYPercent", AHZInventoryWidgetYPercent.GetValue())
	String saveResult = fiss.endSave()
	if saveResult != " "
		debug.Trace(saveResult, 0)
	endIf
endFunction

function FISS_LOAD()

	fissinterface fiss = fissfactory.getFISS()
	if !fiss
		debug.MessageBox("$mHUD_FissMiss")
		return 
	endIf
	fiss.beginLoad("MoreHUD.xml")
	if fiss.loadBool("fissExists")
		AHZActivationMode.SetValueInt(fiss.loadInt("AHZActivationMode"))
		AHZIngredientsWidgetStyle.SetValueInt(fiss.loadInt("AHZIngredientsWidgetStyle"))
		AHZEffectsWidgetStyle.SetValueInt(fiss.loadInt("AHZEffectsWidgetStyle"))
		AHZBottomWidgetRightAligned.SetValueInt(fiss.loadInt("AHZBottomWidgetRightAligned"))
		AHZInventoryWidgetRightAligned.SetValueInt(fiss.loadInt("AHZInventoryWidgetRightAligned"))
		AHZHotKey.SetValueInt(fiss.loadInt("AHZHotKey"))
		AHZHotkeyToggles.SetValueInt(fiss.loadInt("AHZHotkeyToggles"))
		AHZShowBooksRead.SetValueInt(fiss.loadInt("AHZShowBooksRead"))
		AHZShowWeightClass.SetValueInt(fiss.loadInt("AHZShowWeightClass"))
		AHZShowBookSkill.SetValueInt(fiss.loadInt("AHZShowBookSkill"))
		AHZShowBottomWidget.SetValueInt(fiss.loadInt("AHZShowBottomWidget"))
		AHZShowIngredientWidget.SetValueInt(fiss.loadInt("AHZShowIngredientWidget"))
		AHZShowEffectsWidget.SetValueInt(fiss.loadInt("AHZShowEffectsWidget"))
		AHZShowInventoryCount.SetValueInt(fiss.loadInt("AHZShowInventoryCount"))
		AHZBottomWidgetXPercent.SetValue(fiss.loadFloat("AHZBottomWidgetXPercent"))
		AHZBottomWidgetYPercent.SetValue(fiss.loadFloat("AHZBottomWidgetYPercent"))
		AHZSideWidgetXPercent.SetValue(fiss.loadFloat("AHZSideWidgetXPercent"))
		AHZSideWidgetYPercent.SetValue(fiss.loadFloat("AHZSideWidgetYPercent"))
		AHZInventoryWidgetXPercent.SetValue(fiss.loadFloat("AHZInventoryWidgetXPercent"))
		AHZInventoryWidgetYPercent.SetValue(fiss.loadFloat("AHZInventoryWidgetYPercent"))		
		AHZShowTargetWeight.SetValueInt(fiss.loadInt("AHZShowTargetWeight"))
		AHZShowVW.SetValueInt(fiss.loadInt("AHZShowVW"))
		AHZBottomWidgetScale.SetValueInt(fiss.loadInt("AHZBottomWidgetScale"))
		AHZInventoryWidgetScale.SetValueInt(fiss.loadInt("AHZInventoryWidgetScale"))
		AHZSideWidgetScale.SetValueInt(fiss.loadInt("AHZSideWidgetScale"))
		AHZEnemyLevelMax.SetValueInt(fiss.loadInt("AHZEnemyLevelMax"))
		AHZEnemyLevelMin.SetValueInt(fiss.loadInt("AHZEnemyLevelMin"))
		AHZShowEnemyLevel.SetValueInt(fiss.loadInt("AHZShowEnemyLevel"))
		AHZShowEnchantmentKnown.SetValueInt(fiss.loadInt("AHZShowEnchantmentKnown"))
		AHZDisplayDelay.SetValue(fiss.loadFloat("AHZDisplayDelay"))
	endIf
	String loadResult = fiss.endLoad()
	if loadResult != " "
		debug.Trace(loadResult, 0)
	endIf
endFunction

;-- State -------------------------------------------
state FISS_LOAD_PRESET

	function OnSelectST()

		if self.ShowMessage("$mHUD_LoadPreset", true, "$mHUD_Yes", "$mHUD_No")
			self.FISS_LOAD()
			self.ShowMessage("$mHUD_Done", true, "$mHUD_Yes", "$mHUD_No")
		endIf
	endFunction

	function OnHighlightST()

		self.SetInfoText("$mHUD_LoadInfoFiss")
	endFunction
endState

;-- State -------------------------------------------
state FISS_SAVE_PRESET

	function OnSelectST()

		if self.ShowMessage("$mHUD_SavePreset", true, "$mHUD_Yes", "$mHUD_No")
			self.FISS_SAVE()
		endIf
	endFunction

	function OnHighlightST()

		self.SetInfoText("$mHUD_SaveInfoFiss")
	endFunction
endState

state CurrentConfigIN
	event OnInputOpenST()
		SetInputDialogStartText(sConfigNameCurrent)
	endEvent

	event OnInputAcceptST(string txt)
		if txt!=""
			sConfigNameCurrent = txt
			SetInputOptionValueST(txt)
		endIf
	endEvent
	
	event OnHighlightST()
		SetInfoText("$mHUD_PresetNameInfo")
	endEvent
endState

state SaveCurrentConfigBN
	event OnSelectST()
		string file = "..\\moreHUD\\" + sConfigNameCurrent
			JSONUtil.SetPathIntValue(file, ".!AHZActivationMode", AHZActivationMode.GetValueInt())
			JSONUtil.SetPathIntValue(file, ".!AHZHotKey", AHZHotKey.GetValueInt())
			JSONUtil.SetPathIntValue(file, ".!AHZHotkeyToggles", AHZHotkeyToggles.GetValueInt())
			JSONUtil.SetPathIntValue(file, ".!AHZShowBooksRead", AHZShowBooksRead.GetValueInt())
			JSONUtil.SetPathIntValue(file, ".!AHZShowWeightClass", AHZShowWeightClass.GetValueInt())
			JSONUtil.SetPathIntValue(file, ".!AHZShowBookSkill", AHZShowBookSkill.GetValueInt())
			JSONUtil.SetPathIntValue(file, ".!AHZShowVW", AHZShowVW.GetValueInt())
			JSONUtil.SetPathIntValue(file, ".!AHZShowEnchantmentKnown", AHZShowEnchantmentKnown.GetValueInt())	
			JSONUtil.SetPathIntValue(file, ".!AHZShowBottomWidget", AHZShowBottomWidget.GetValueInt())			
			JSONUtil.SetPathIntValue(file, ".!AHZBottomWidgetRightAligned", AHZBottomWidgetRightAligned.GetValueInt())
			JSONUtil.SetPathIntValue(file, ".!AHZShowTargetWeight", AHZShowTargetWeight.GetValueInt())			
			JSONUtil.SetPathFloatValue(file, ".!AHZBottomWidgetXPercent", AHZBottomWidgetXPercent.GetValue())
			JSONUtil.SetPathFloatValue(file, ".!AHZBottomWidgetYPercent", AHZBottomWidgetYPercent.GetValue())
			JSONUtil.SetPathFloatValue(file, ".!AHZBottomWidgetScale", AHZBottomWidgetScale.GetValue())			
			JSONUtil.SetPathIntValue(file, ".!AHZShowIngredientWidget", AHZShowIngredientWidget.GetValueInt())
			JSONUtil.SetPathIntValue(file, ".!AHZShowEffectsWidget", AHZShowEffectsWidget.GetValueInt())
			JSONUtil.SetPathIntValue(file, ".!AHZIngredientsWidgetStyle", AHZIngredientsWidgetStyle.GetValueInt())
			JSONUtil.SetPathIntValue(file, ".!AHZEffectsWidgetStyle", AHZEffectsWidgetStyle.GetValueInt())			
			JSONUtil.SetPathFloatValue(file, ".!AHZSideWidgetXPercent", AHZSideWidgetXPercent.GetValue())
			JSONUtil.SetPathFloatValue(file, ".!AHZSideWidgetYPercent", AHZSideWidgetYPercent.GetValue())
			JSONUtil.SetPathIntValue(file, ".!AHZSideWidgetScale", AHZSideWidgetScale.GetValueInt())			
			JSONUtil.SetPathIntValue(file, ".!AHZShowInventoryCount", AHZShowInventoryCount.GetValueInt())
			JSONUtil.SetPathIntValue(file, ".!AHZInventoryWidgetRightAligned", AHZInventoryWidgetRightAligned.GetValueInt())			
			JSONUtil.SetPathFloatValue(file, ".!AHZInventoryWidgetXPercent", AHZInventoryWidgetXPercent.GetValue())
			JSONUtil.SetPathFloatValue(file, ".!AHZInventoryWidgetYPercent", AHZInventoryWidgetYPercent.GetValue())
			JSONUtil.SetPathFloatValue(file, ".!AHZInventoryWidgetScale", AHZInventoryWidgetScale.GetValue())		
			JSONUtil.SetPathIntValue(file, ".!AHZShowEnemyLevel", AHZShowEnemyLevel.GetValueInt())
			JSONUtil.SetPathFloatValue(file, ".!AHZEnemyLevelMax", AHZEnemyLevelMax.GetValue())
			JSONUtil.SetPathFloatValue(file, ".!AHZEnemyLevelMin", AHZEnemyLevelMin.GetValue())	
			JSONUtil.SetPathFloatValue(file, ".!AHZDisplayDelay", AHZDisplayDelay.GetValue())
			
		JSONUtil.save(file)
		SetTextOptionValueST("$mHUD_Done")
	endEvent
	
	event OnHighlightST()
		SetInfoText("$mHUD_SaveInfoPU")
	endEvent
endState

state ConfigToLoadMN
	event OnMenuOpenST()
		SetMenuDialogStartIndex(selectedConfig)
		SetMenuDialogDefaultIndex(0)
		SetMenuDialogOptions(saConfigs)
	endEvent
	
	event OnMenuAcceptST(int index)
		if index>-1 && index<saConfigs.length
			selectedConfig=index
			SetMenuOptionValueST(saConfigs[selectedConfig])
			sConfigNameCurrent = saConfigs[selectedConfig]
			SetInputOptionValueST(sConfigNameCurrent, false, "CurrentConfigIN")
		endIf
	endEvent
endState

state LoadSelectedConfigBN
	event OnSelectST()
		string file = "..\\moreHUD\\" + saConfigs[selectedConfig]
		if !JSONUtil.IsGood(file)
			ShowMessage("$mHUD_PresetBadInfo \n" + JSONUtil.getErrors(file), false, "$mHUD_PresetBad")
			return
		endIf
		SetTextOptionValueST("$mHUD_PresetApply")
			AHZActivationMode.SetValueInt(JSONUtil.GetPathIntValue(file, ".!AHZActivationMode", AHZActivationMode.GetValueInt()))
			AHZHotKey.SetValueInt(JSONUtil.GetPathIntValue(file, ".!AHZHotKey", AHZHotKey.GetValueInt()))
			AHZHotkeyToggles.SetValueInt(JSONUtil.GetPathIntValue(file, ".!AHZHotkeyToggles", AHZHotkeyToggles.GetValueInt()))
			AHZShowBooksRead.SetValueInt(JSONUtil.GetPathIntValue(file, ".!AHZShowBooksRead", AHZShowBooksRead.GetValueInt()))
			AHZShowWeightClass.SetValueInt(JSONUtil.GetPathIntValue(file, ".!AHZShowWeightClass", AHZShowWeightClass.GetValueInt()))
			AHZShowBookSkill.SetValueInt(JSONUtil.GetPathIntValue(file, ".!AHZShowBookSkill", AHZShowBookSkill.GetValueInt()))
			AHZShowVW.SetValueInt(JSONUtil.GetPathIntValue(file, ".!AHZShowVW", AHZShowVW.GetValueInt()))
			AHZShowEnchantmentKnown.SetValueInt(JSONUtil.GetPathIntValue(file, ".!AHZShowEnchantmentKnown", AHZShowEnchantmentKnown.GetValueInt()))	
			AHZDisplayDelay.SetValue(JSONUtil.GetPathFloatValue(file, ".!AHZDisplayDelay", AHZDisplayDelay.GetValue()))		
			AHZShowBottomWidget.SetValueInt(JSONUtil.GetPathIntValue(file, ".!AHZShowBottomWidget", AHZShowBottomWidget.GetValueInt()))			
			AHZBottomWidgetRightAligned.SetValueInt(JSONUtil.GetPathIntValue(file, ".!AHZBottomWidgetRightAligned", AHZBottomWidgetRightAligned.GetValueInt()))
			AHZShowTargetWeight.SetValueInt(JSONUtil.GetPathIntValue(file, ".!AHZShowTargetWeight", AHZShowTargetWeight.GetValueInt()))			
			AHZBottomWidgetXPercent.SetValue(JSONUtil.GetPathFloatValue(file, ".!AHZBottomWidgetXPercent", AHZBottomWidgetXPercent.GetValue()))
			AHZBottomWidgetYPercent.SetValue(JSONUtil.GetPathFloatValue(file, ".!AHZBottomWidgetYPercent", AHZBottomWidgetYPercent.GetValue()))
			AHZBottomWidgetScale.SetValue(JSONUtil.GetPathFloatValue(file, ".!AHZBottomWidgetScale", AHZBottomWidgetScale.GetValue()))			
			AHZShowIngredientWidget.SetValueInt(JSONUtil.GetPathIntValue(file, ".!AHZShowIngredientWidget", AHZShowIngredientWidget.GetValueInt()))
			AHZShowEffectsWidget.SetValueInt(JSONUtil.GetPathIntValue(file, ".!AHZShowEffectsWidget", AHZShowEffectsWidget.GetValueInt()))
			AHZIngredientsWidgetStyle.SetValueInt(JSONUtil.GetPathIntValue(file, ".!AHZIngredientsWidgetStyle", AHZIngredientsWidgetStyle.GetValueInt()))
			AHZEffectsWidgetStyle.SetValueInt(JSONUtil.GetPathIntValue(file, ".!AHZEffectsWidgetStyle", AHZEffectsWidgetStyle.GetValueInt()))			
			AHZSideWidgetXPercent.SetValue(JSONUtil.GetPathFloatValue(file, ".!AHZSideWidgetXPercent", AHZSideWidgetXPercent.GetValue()))
			AHZSideWidgetYPercent.SetValue(JSONUtil.GetPathFloatValue(file, ".!AHZSideWidgetYPercent", AHZSideWidgetYPercent.GetValue()))
			AHZSideWidgetScale.SetValueInt(JSONUtil.GetPathIntValue(file, ".!AHZSideWidgetScale", AHZSideWidgetScale.GetValueInt()))			
			AHZShowInventoryCount.SetValueInt(JSONUtil.GetPathIntValue(file, ".!AHZShowInventoryCount", AHZShowInventoryCount.GetValueInt()))
			AHZInventoryWidgetRightAligned.SetValueInt(JSONUtil.GetPathIntValue(file, ".!AHZInventoryWidgetRightAligned", AHZInventoryWidgetRightAligned.GetValueInt()))			
			AHZInventoryWidgetXPercent.SetValue(JSONUtil.GetPathFloatValue(file, ".!AHZInventoryWidgetXPercent", AHZInventoryWidgetXPercent.GetValue()))
			AHZInventoryWidgetYPercent.SetValue(JSONUtil.GetPathFloatValue(file, ".!AHZInventoryWidgetYPercent", AHZInventoryWidgetYPercent.GetValue()))
			AHZInventoryWidgetScale.SetValue(JSONUtil.GetPathFloatValue(file, ".!AHZInventoryWidgetScale", AHZInventoryWidgetScale.GetValue()))		
			AHZShowEnemyLevel.SetValueInt(JSONUtil.GetPathIntValue(file, ".!AHZShowEnemyLevel", AHZShowEnemyLevel.GetValueInt()))
			AHZEnemyLevelMax.SetValue(JSONUtil.GetPathFloatValue(file, ".!AHZEnemyLevelMax", AHZEnemyLevelMax.GetValue()))
			AHZEnemyLevelMin.SetValue(JSONUtil.GetPathFloatValue(file, ".!AHZEnemyLevelMin", AHZEnemyLevelMin.GetValue()))
				
	endEvent
	
	event OnHighlightST()
		SetInfoText("$mHUD_LoadInfoPU")
	endEvent
endState 

; Private Functions ------------------------------------------------------------------------------------------
Function ToggleGlobalInt(GlobalVariable var)
	{Utility function to toggle global variables}
	if (var.GetValueInt() > 0)
		var.SetValueInt(0)
	else
		var.SetValueInt(1)
	endif	
EndFunction

