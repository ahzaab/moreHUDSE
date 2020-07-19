Scriptname AHZdbmTest extends Quest  

FormList Property dbmNew  Auto  
FormList Property dbmDisp  Auto  


Event OnInit()
	Maintenance() ; OnPlayerLoadGame will not fire the first time
EndEvent


Function Maintenance()
	debug.notification("Registering FormLists")
	AhzMoreHudIE.RegisterIconFormList("dbmNew", dbmNew)
	AhzMoreHudIE.RegisterIconFormList("dbmDisp", dbmDisp)
	AhzMoreHudIE.AddIconItem(0x37B9BB51, "iEquipQL.png")  ; 0002E4E2	Petty Soul Gem	
	AhzMoreHudIE.AddIconItem(0xA0C4693E, "iEquipQB.png")  ; 0002E4E6	Common Soul Gem

	AhzMoreHud.RegisterIconFormList("dbmNew", dbmNew)
	AhzMoreHud.RegisterIconFormList("dbmDisp", dbmDisp)
	AhzMoreHud.AddIconItem(0x37B9BB51, "iEquipQL.png")  ; 0002E4E2	Petty Soul Gem	
	AhzMoreHud.AddIconItem(0xA0C4693E, "iEquipQB.png")  ; 0002E4E6	Common Soul Gem

	;UnRegisterIconFormList
	;IsIconFormListRegistered

EndFunction