scriptname AHZConfigMenu extends SKI_ConfigBase


GlobalVariable Property AHZShowBottomWidget  Auto  
GlobalVariable Property AHZShowIngredientWidget  Auto
GlobalVariable Property AHZShowEffectsWidget  Auto
GlobalVariable Property AHZShowInventoryCount  Auto     
GlobalVariable Property AHZShowWeightClass  Auto  
GlobalVariable Property AHZShowBooksRead  Auto  
GlobalVariable Property AHZShowBookSkill  Auto  
GlobalVariable Property AHZShowTargetWeight  Auto 
GlobalVariable Property AHZShowVW  Auto  
GlobalVariable Property AHZShowEnemyLevel  Auto 
GlobalVariable Property AHZShowEnemySoulLevel  Auto   
GlobalVariable Property AHZEnemyLevelMax  Auto  
GlobalVariable Property AHZEnemyLevelMin  Auto  
GlobalVariable Property AHZShowEnchantmentKnown  Auto  

Quest Property AHZMainQuestREF  Auto  

; Default wiget positions for the sliders
int Property DefultEnemyLevelMin = 10 autoReadOnly
int Property DefultEnemyLevelMax = 10 autoReadOnly


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
int function GetVersion()
	return 1
endFunction


; PRIVATE VARIABLES -------------------------------------------------------------------------------

; --- Version 10 ---

; Lists

; OIDs (T:Text B:Toggle S:Slider M:Menu, C:Color, K:Key)

; General Settings --------------------
int			_toggle1OID_B				; Show Player Data Widget
int			_toggle2OID_B				; Show Target Data Widget (Ingredients)
int			_toggle3OID_B				; Show Target Data Widget (Effects)
int			_toggle5OID_B				; Show Target Inventory
int			_toggle8OID_B				; Show Books Read
int			_toggle9OID_B				; Show Armor Weight Class
int			_toggle10OID_B				; Show Book Skill
int			_toggle11OID_B				; Show Target Weight with Player's Weight
int			_toggle13OID_B				; Show VW
int			_toggle15OID_B				; Show Known Enchantment


; Enemy Level Settings ----------------
int			_toggle14OID_B				; Show Enemy Level
int			_sliderEnemyLevelMax_OID_S
int			_sliderEnemyLevelMin_OID_S
int			_toggle16OID_B				; Show Enemy Soul Level

;Private variables


; INITIALIZATION ----------------------------------------------------------------------------------

; @overrides SKI_ConfigBase
event OnConfigInit()
	Pages = new string[2]
	Pages[0] = "$mHUD_GeneralPage"
	Pages[1] = "$mHUD_EnemysLevelPage"	
endEvent

event OnConfigOpen()
	
endEvent

event OnConfigClose()
	(AHZMainQuestREF as AHZMainQuest).UpdateSettings(false)
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
		AddHeaderOption("$mHUD_WidgetExtras")		
		_toggle8OID_B			= AddToggleOption("$mHUD_ShowBooksRead", AHZShowBooksRead.GetValueInt())
		_toggle9OID_B			= AddToggleOption("$mHUD_ShowArmorWeightClass", AHZShowWeightClass.GetValueInt())
		_toggle10OID_B			= AddToggleOption("$mHUD_ShowBookSkill", AHZShowBookSkill.GetValueInt())
		_toggle13OID_B			= AddToggleOption("$mHUD_ShowValueToWeightRatio", AHZShowVW.GetValueInt())
		_toggle15OID_B			= AddToggleOption("$mHUD_ShowKnownEnchantment", AHZShowEnchantmentKnown.GetValueInt())

		SetCursorPosition(1)
		AddHeaderOption("$mHUD_PlayersDataWidget")
		_toggle1OID_B			= AddToggleOption("$mHUD_Visible", AHZShowBottomWidget.GetValueInt())		
		_toggle11OID_B			= AddToggleOption("$mHUD_DisplayTargetWeight", AHZShowTargetWeight.GetValueInt())
		AddHeaderOption("$mHUD_TargetsDataWidget")
		_toggle2OID_B			= AddToggleOption("$mHUD_ShowIngredientEffects", AHZShowIngredientWidget.GetValueInt())
		_toggle3OID_B			= AddToggleOption("$mHUD_ShowOtherEffects", AHZShowEffectsWidget.GetValueInt())		
		AddHeaderOption("$mHUD_TargetsCountWidget")
		_toggle5OID_B 			= AddToggleOption("$mHUD_Visible", AHZShowInventoryCount.GetValueInt())
		
	elseif (a_page == "$mHUD_EnemysLevelPage")
		AddHeaderOption("$mHUD_GeneralEnemyLevel")
		_toggle14OID_B = AddToggleOption("$mHUD_ShowEnemyLevel", AHZShowEnemyLevel.GetValueInt())
		_toggle16OID_B = AddToggleOption("$mHUD_ShowEnemySoulLevel", AHZShowEnemySoulLevel.GetValueInt())	
		AddHeaderOption("$mHUD_ColorScale")
		_sliderEnemyLevelMin_OID_S = AddSliderOption("$mHUD_LevelBelowPlayer", AHZEnemyLevelMin.GetValue(), "{0}")
		_sliderEnemyLevelMax_OID_S = AddSliderOption("$mHUD_LevelAbovePlayer", AHZEnemyLevelMax.GetValue(), "{0}")	
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

	; Toggle Enemy Soul Level
	if (a_option == _toggle16OID_B)
		ToggleGlobalInt(AHZShowEnemySoulLevel)
		SetToggleOptionValue(_toggle16OID_B, AHZShowEnemySoulLevel.GetValueInt())
	endif

	; Toggle Target+Players Weight
	if (a_option == _toggle11OID_B)
		ToggleGlobalInt(AHZShowTargetWeight)
		SetToggleOptionValue(_toggle11OID_B, AHZShowTargetWeight.GetValueInt())
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

endEvent

; @implements SKI_ConfigBase
event OnOptionSliderOpen(int a_option)
	{Called when the user selects a slider option}

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

endEvent

; @implements SKI_ConfigBase
event OnOptionSliderAccept(int a_option, float a_value)	
	
	if (a_option == _sliderEnemyLevelMin_OID_S)
		AHZEnemyLevelMin.SetValue(a_value)
		SetSliderOptionValue(a_option, a_value, "{0}")
	endif	

	if (a_option == _sliderEnemyLevelMax_OID_S)
		AHZEnemyLevelMax.SetValue(a_value)
		SetSliderOptionValue(a_option, a_value, "{0}")
	endif	

endEvent

; @implements SKI_ConfigBase
event OnOptionMenuOpen(int a_option)
	{Called when the user selects a menu option}

endEvent

; @implements SKI_ConfigBase
event OnOptionMenuAccept(int a_option, int a_index)
	{Called when the user accepts a new menu entry}
		
endEvent

; @implements SKI_ConfigBase
event OnOptionDefault(int a_option)
	{Called when resetting an option to its default value}

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

	if (a_option == _toggle16OID_B)
		SetInfoText("$mHUD_ShowsEnemysSoulLevel")
	endif

	if (a_option == _toggle15OID_B)
		SetInfoText("$mHUD_ShowsIconDisenchanted")
	endif

	if (a_option == _toggle11OID_B)
		SetInfoText("$mHUD_ShowsTargetsWeight")
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

	if (a_option == _sliderEnemyLevelMin_OID_S)
		SetInfoText("$mHUD_EnemiesFullyGreen")
	endif	

	if (a_option == _sliderEnemyLevelMax_OID_S)
		SetInfoText("$mHUD_EnemiesFullyRed")
	endif	

endEvent

; Private Functions ------------------------------------------------------------------------------------------
Function ToggleGlobalInt(GlobalVariable var)
	{Utility function to toggle global variables}
	if (var.GetValueInt() > 0)
		var.SetValueInt(0)
	else
		var.SetValueInt(1)
	endif	
EndFunction

