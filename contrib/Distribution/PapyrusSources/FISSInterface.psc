Scriptname FISSInterface extends Quest

Function beginSave(string filename, string modname)
EndFunction

Function saveInt(string name, int i)
EndFunction

Function saveString(string name, string s)
EndFunction

Function saveFloat(string name, float f)
EndFunction

Function saveBool(string name, bool b)
EndFunction

string Function endSave()
EndFunction

string Function saveTextToTxtFile(string filename, string text)
EndFunction

Function beginLoad(string filename)
EndFunction

int Function loadInt(string name)
EndFunction

string Function loadString(string name)
EndFunction

float Function loadFloat(string name)
EndFunction

bool Function loadBool(string name)
EndFunction

string Function getModName()
EndFunction

string Function endLoad()
EndFunction

float Function getVersion()
EndFunction

float Function getInterfaceVersion()
	return 1.0
EndFunction

string Function getAllFilenamesInFolder(string path)
EndFunction

string Function requestUserInput(string titleMessage)
EndFunction

string Function requestFilename()
EndFunction

Function setTheme(string fissInputTheme, float fissInputScale, float fissInputAplha)
EndFunction

Function blockInput(bool block)
EndFunction

Function forceClose()
EndFunction

Function setTitle(string title)
EndFunction

Function setText(string text)
EndFunction

Function setInfoText(string text)
EndFunction

Function hideInfoText(bool hide)
EndFunction
