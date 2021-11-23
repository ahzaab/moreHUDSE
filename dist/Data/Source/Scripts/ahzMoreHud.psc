Scriptname AhzMoreHud Hidden

; Gets the version e.g 10008 for 1.0.8
int Function GetVersion() global native


;iEquip Functions ---------------------------------------

; Returns true if the Item ID is registered
bool Function IsIconItemRegistered(int aItemId) global native

; Add an Item ID with the icon that you want to display
Function AddIconItem(int aItemId, string aIconName) global native

; Add an Item ID
Function RemoveIconItem(int aItemId) global native

; Adds an array of Item ID's with the icon that you want to display
Function AddIconItems(int[] aItemIds, string[] aIconNames) global native

; Removes an array of Item ID's 
Function RemoveIconItems(int[] aItemIds) global native


;Relic Functions ---------------------------------------

; Returns true if the form list with the key
Function RegisterIconFormList(string aKey, FormList alist) global native

; Unregistered a form list with this key
Function UnRegisterIconFormList(string aKey) global native

; Returns true if the form list is registered
bool Function IsIconFormListRegistered(string aKey) global native

string Function IsBetaPlugin(int aVersion) global
    return ((aVersion % 100) > 0)
EndFunction

; Gets the version as a string for viewing
string Function GetVersionString() global
    int iVersion = GetVersion()

    if (iVersion == 0)
        return ""
    endif

    int iMajor = iVersion / 1000000
    int iMinor = (iVersion / 10000) % 100
    int iBug = (iVersion / 100) % 100
    int iBeta = iVersion % 100
    string aVersion = iMajor + "." + iMinor + "." + iBug

    if (iBeta > 0)
        aVersion += " Beta " + iBeta
    endif
    return aVersion
EndFunction
