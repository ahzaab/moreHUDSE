Scriptname AHZMaintenanceAliasScript extends ReferenceAlias  

Event OnPlayerLoadGame()
	(self.GetOwningQuest() as AHZMainQuest).Maintenance()
EndEvent