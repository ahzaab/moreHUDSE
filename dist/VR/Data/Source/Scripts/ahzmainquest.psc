Scriptname AHZMainQuest extends Quest Hidden

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

; Keeps track of the revision
int Property iVersion Auto

; Constants -------------------------------------------------------------------------------------------------
; <--- Edit These value when updating
int Property I_THIS_VERSION_MAJOR = 1 autoReadOnly
int Property I_THIS_VERSION_MINOR = 1 autoReadOnly
int Property I_THIS_VERSION_BUILD = 0 autoReadOnly
; Main Rollover Menu
String Property VRActivateRolloverInstance = "_root.VRActivateRolloverInstance" autoReadOnly
String Property WSActivateRollover = "WSActivateRollover" autoReadOnly

; Enemy Menu
String Property AHZEnemyLevelInstance = "_root.AHZEnemyLevelInstance.EnemyLevel_mc" autoReadOnly
String Property WSEnemyMeters = "WSEnemyMeters" autoReadOnly

; SKSE oldest supported release index
int I_MIN_SKSE_RELEASE_IDX = 59

; Public Functions ------------------------------------------------------------------------------------------
Function Maintenance()
    ; Detect SKSE
    int skseRelease = SKSE.GetVersionRelease()
    bool isSKSEInstalled = false
    if skseRelease == 0
    	Debug.MessageBox("moreHUD VR: SKSE is not Detected!")
    	isSKSEInstalled = false
    elseIf skseRelease < I_MIN_SKSE_RELEASE_IDX
       	Debug.MessageBox("moreHUD VR: The SKSE revision is out of date.  Please install the latest SKSE")
        isSKSEInstalled = false
    else
		isSKSEInstalled = true
    endIf

    if (isSKSEInstalled == true)
		If iVersion < ((I_THIS_VERSION_MAJOR * 100) + (I_THIS_VERSION_MINOR * 10) + I_THIS_VERSION_BUILD)
			iVersion = (I_THIS_VERSION_MAJOR * 100) + (I_THIS_VERSION_MINOR * 10) + I_THIS_VERSION_BUILD
			Debug.Notification("moreHUD VR version: " + I_THIS_VERSION_MAJOR + "." + I_THIS_VERSION_MINOR + "." + I_THIS_VERSION_BUILD)
		EndIf
	Endif
	; Other maintenance code that only needs to run once per save load

    ; UpdateSettings(false)

    ; Register for this event to update the settings again incase they did not update
    ; the first time.  Since 1.5.3 Some users have reported that the widgets did not
    ; load correctly on startup  
    RegisterForModEvent("SKIWF_widgetManagerReady", "OnWidgetManagerReady")

   RegisterForMenu(WSActivateRollover)
   RegisterForMenu(WSEnemyMeters)
EndFunction

function RefreshWidgets()
    UI.Invoke(WSActivateRollover, VRActivateRolloverInstance + ".RefreshWidgets")
EndFunction

function UpdateSettings(bool disable)
    int[] intargs_200 = new int[10]
    int[] enemyArgs = new int[4]

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
    else
        intargs_200[0] = AHZShowIngredientWidget.GetValueInt()
        intargs_200[1] = AHZShowEffectsWidget.GetValueInt()
        intargs_200[2] = AHZShowBottomWidget.GetValueInt()
        intargs_200[3] = AHZShowInventoryCount.GetValueInt()
        intargs_200[4] = AHZShowWeightClass.GetValueInt()
        intargs_200[5] = AHZShowBooksRead.GetValueInt() 
        intargs_200[6] = AHZShowBookSkill.GetValueInt() 
        intargs_200[7] = AHZShowTargetWeight.GetValueInt() 
        intargs_200[8] = AHZShowVW.GetValueInt()  
        intargs_200[9] = AHZShowEnchantmentKnown.GetValueInt()     
    endif


    ; Show the widgets with the passed parameters
    UI.InvokeIntA(WSActivateRollover, VRActivateRolloverInstance + ".updateSettings", intargs_200)

    ; When disabling, set all to 0 to hide all widgets
    if (disable == true)
        ; This could be a loop, but...
        enemyArgs[0] = 0
        enemyArgs[1] = 0
        enemyArgs[2] = 0
        enemyArgs[3] = 0
    else
        enemyArgs[0] = AHZShowEnemyLevel.GetValueInt()
        enemyArgs[1] = AHZEnemyLevelMax.GetValueInt()
        enemyArgs[2] = AHZEnemyLevelMin.GetValueInt()
        enemyArgs[3] = AHZShowEnemySoulLevel.GetValueInt() 
    endif

    UI.InvokeIntA(WSEnemyMeters, AHZEnemyLevelInstance + ".updateSettings", enemyArgs)
endfunction

; Events ------------------------------------------------------------------------------------------

Event OnInit()
    Maintenance()
EndEvent

event OnWidgetManagerReady(string a_eventName, string a_strArg, float a_numArg, Form a_sender)
    ; Try to update the settings again
    ; UpdateSettings(false)
EndEvent
 
Event OnMenuOpen(String MenuName)
	If MenuName == WSEnemyMeters
		; Update The menu with the global variables the first time it is loaded
		UpdateSettings(false)  
		UnregisterForMenu(WSEnemyMeters)   
	EndIf

	If MenuName == WSActivateRollover
		; Update The menu with the global variables the first time it is loaded
		UpdateSettings(false)
		UnregisterForMenu(WSActivateRollover)
	EndIf
EndEvent

; Private Functions-------------------------------------------------------------------------------------




