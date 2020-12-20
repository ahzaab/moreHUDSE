scriptname AhzdbmTestStartup   extends ReferenceAlias

 ; EVENTS -----------------------------------------------------------------------------------------

event OnPlayerLoadGame()
	(GetOwningQuest() as AHZdbmTest).Maintenance()
endEvent