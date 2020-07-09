Scriptname AHZMainQuest extends Quest Hidden

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
GlobalVariable Property AHZEnemyLevelPercent  Auto  
GlobalVariable Property AHZEnemyLevelMax  Auto  
GlobalVariable Property AHZEnemyLevelMin  Auto  
GlobalVariable Property AHZShowEnchantmentKnown  Auto
GlobalVariable Property AHZDisplayDelay  Auto  

; Keeps track of the revision
int Property iVersion Auto
int Property iToggleOn Auto

; Constants -------------------------------------------------------------------------------------------------
; <--- Edit These value when updating
int Property I_THIS_VERSION_MAJOR = 3 autoReadOnly
int Property I_THIS_VERSION_MINOR = 6 autoReadOnly
int Property I_THIS_VERSION_BUILD = 4 autoReadOnly
String Property WidgetRoot = "_root.AHZWidgetContainer.AHZWidget" autoReadOnly

; SKSE oldest supported release index
int I_MIN_SKSE_RELEASE_IDX = 53

; Public Functions ------------------------------------------------------------------------------------------
Function Unregister()
	debug.notification("Uninstalling moreHUD")
	iVersion = 0

    ; Hide the widget 
    UpdateSettings(true)

    ; Unregister for any key event
    UnregisterForAllKeys();

	; Stop the quest
	Stop()
EndFunction

Function Maintenance()
    ; Detect SKSE
    int skseRelease = SKSE.GetVersionRelease()
    bool isSKSEInstalled = false
    if skseRelease == 0
    	Debug.MessageBox("moreHUD: SKSE is not Detected!")
    	isSKSEInstalled = false
    elseIf skseRelease < I_MIN_SKSE_RELEASE_IDX
       	Debug.MessageBox("moreHUD: The SKSE revision is out of date.  Please install the latest SKSE")
        isSKSEInstalled = false
    ;elseIf (SKSE.GetScriptVersionRelease() != skseRelease)
   ;     Debug.MessageBox("moreHUD: The SKSE scripts are out of date.  Please make sure the latest scripts are installed. \n(Script Revision: " + SKSE.GetScriptVersionRelease() + ", SKSE Revision: " + skseRelease + ")")
   ;     isSKSEInstalled = false
    else
		isSKSEInstalled = true
    endIf

    if (isSKSEInstalled == true)
		If iVersion < ((I_THIS_VERSION_MAJOR * 100) + (I_THIS_VERSION_MINOR * 10) + I_THIS_VERSION_BUILD)
			iVersion = (I_THIS_VERSION_MAJOR * 100) + (I_THIS_VERSION_MINOR * 10) + I_THIS_VERSION_BUILD
			Debug.Notification("moreHUD version: " + I_THIS_VERSION_MAJOR + "." + I_THIS_VERSION_MINOR + "." + I_THIS_VERSION_BUILD)
		EndIf
	Endif
	; Other maintenance code that only needs to run once per save load

    UpdateSettings(false)

    ; Register for this event to update the settings again incase they did not update
    ; the first time.  Since 1.5.3 Some users have reported that the widgets did not
    ; load correctly on startup  
    RegisterForModEvent("SKIWF_widgetManagerReady", "OnWidgetManagerReady")
EndFunction

function RefreshWidgets()
    UI.Invoke("HUD Menu", WidgetRoot + ".RefreshWidgets")
EndFunction

function UpdateSettings(bool disable)
    int[] intargs_200 = new int[20]
    float[] argsF = new float[2]
    float argF = 100.0

    ; When disabling, set all to 0 to hide all widgets
    if (disable == true)
        ; This could be a loop, but...
        intargs_200[0] = 0
        intargs_200[1] = 0
        intargs_200[2] = 0
        intargs_200[3] = 0
        intargs_200[4] = 0
        intargs_200[5] = 0
        intargs_200[6] = 0
        intargs_200[7] = 0
        intargs_200[8] = 0
        intargs_200[9] = 0
        intargs_200[10] = 0
        intargs_200[11] = 0
        intargs_200[12] = 0
        intargs_200[13] = 0
        intargs_200[14] = 0
        intargs_200[15] = 0
        intargs_200[16] = 0        
	    intargs_200[17] = 0   
        intargs_200[18] = 0
        intargs_200[19] = 0 
    else
        intargs_200[0] = AHZShowIngredientWidget.GetValueInt()
        intargs_200[1] = AHZShowEffectsWidget.GetValueInt()
        intargs_200[2] = AHZShowBottomWidget.GetValueInt()
        intargs_200[3] = AHZShowInventoryCount.GetValueInt()
        intargs_200[4] = AHZBottomWidgetRightAligned.GetValueInt()
        intargs_200[5] = AHZInventoryWidgetRightAligned.GetValueInt()
        intargs_200[6] = AHZIngredientsWidgetStyle.GetValueInt()
        intargs_200[7] = AHZEffectsWidgetStyle.GetValueInt()
        intargs_200[8] = AHZShowWeightClass.GetValueInt()
        intargs_200[9] = AHZShowBooksRead.GetValueInt() 
        intargs_200[10] = AHZActivationMode.GetValueInt()
        intargs_200[11] = AHZToggleState.GetValueInt() 
        intargs_200[12] = AHZShowBookSkill.GetValueInt() 
        intargs_200[13] = AHZShowTargetWeight.GetValueInt() 
        intargs_200[14] = AHZShowVW.GetValueInt() 
        intargs_200[15] = AHZShowEnemyLevel.GetValueInt()
        intargs_200[16] = AHZEnemyLevelMax.GetValueInt()
	    intargs_200[17] = AHZEnemyLevelMin.GetValueInt()   
        intargs_200[18] = AHZShowEnchantmentKnown.GetValueInt()   
        intargs_200[19] = ((AHZDisplayDelay.GetValue() * 1000.0) as Int)     
    endif

    argsF[0] = AHZBottomWidgetXPercent.GetValue()
    argsF[1] = AHZBottomWidgetYPercent.GetValue()
    UI.InvokeFloatA("HUD Menu", WidgetRoot + ".setBottomWidgetPosition", argsF)

    argsF[0] = AHZSideWidgetXPercent.GetValue()
    argsF[1] = AHZSideWidgetYPercent.GetValue()
    UI.InvokeFloatA("HUD Menu", WidgetRoot + ".setSideWidgetPosition", argsF)

    argsF[0] = AHZInventoryWidgetXPercent.GetValue()
    argsF[1] = AHZInventoryWidgetYPercent.GetValue()
    UI.InvokeFloatA("HUD Menu", WidgetRoot + ".setInventoryWidgetPosition", argsF)

    argF = AHZBottomWidgetScale.GetValue()
    UI.InvokeFloat("HUD Menu", WidgetRoot + ".setBottomWidgetScale", argF)

    argF = AHZInventoryWidgetScale.GetValue()
    UI.InvokeFloat("HUD Menu", WidgetRoot + ".setInventoryWidgetScale", argF)

    argF = AHZSideWidgetScale.GetValue()
    UI.InvokeFloat("HUD Menu", WidgetRoot + ".setSideWidgetScale", argF)

    ; Show the widgets with the passed parameters
    UI.InvokeIntA("HUD Menu", WidgetRoot + ".updateSettings", intargs_200)

    ; Register for key press event for toggling
    if (AHZHotKey.GetValueInt() == -1)
        UnregisterForAllKeys() 
    else
        RegisterForKey(AHZHotKey.GetValueInt())
    Endif

    iToggleOn = AHZToggleState.GetValueInt()
endfunction

; Events ------------------------------------------------------------------------------------------

Event OnInit()
    Maintenance()
EndEvent

event OnWidgetManagerReady(string a_eventName, string a_strArg, float a_numArg, Form a_sender)
    ; Try to update the settings again
    UpdateSettings(false)
EndEvent

Event OnKeyUp(Int KeyCode, Float HoldTime)
    If KeyCode == AHZHotKey.GetValueInt()
        ; If toggling then, toggle the widget state on a release
        if (AHZHotkeyToggles.GetValueInt() > 0)
            toggleWidget()
        else
            ; If not toggling, then hide the widgets on release
            iToggleOn = 0
            UI.Invoke("HUD Menu", WidgetRoot + ".TurnOffWidgets") 
        endif

        ; Update the toggle state for saved games
        AHZToggleState.SetValueInt(iToggleOn)
    EndIf
EndEvent

Event OnKeyDown(Int KeyCode)
    If KeyCode == AHZHotKey.GetValueInt()
        ; If not toggling then show widgets on press
        if (AHZHotkeyToggles.GetValueInt() == 0)
            iToggleOn = 1
            UI.Invoke("HUD Menu", WidgetRoot + ".TurnOnWidgets")  
        endif

        ; Update the toggle state for saved games
        AHZToggleState.SetValueInt(iToggleOn)
    EndIf
EndEvent

; Private Functions-------------------------------------------------------------------------------------

Function toggleWidget()
    if (iToggleOn > 0)
        iToggleOn = 0
        UI.Invoke("HUD Menu", WidgetRoot + ".TurnOffWidgets")
    else
        iToggleOn = 1
        UI.Invoke("HUD Menu", WidgetRoot + ".TurnOnWidgets")            
    endif  
EndFunction



