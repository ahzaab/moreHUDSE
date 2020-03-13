Scriptname AHZMaintenanceAliasScript extends ReferenceAlias  

Event OnPlayerLoadGame()
	(self.GetOwningQuest() as AHZMainQuest).Maintenance()
EndEvent


Event OnItemAdded(Form akBaseItem, Int aiItemCount, ObjectReference akItemReference, ObjectReference akSourceContainer)
	(self.GetOwningQuest() as AHZMainQuest).RefreshWidgets()
EndEvent

Event OnItemRemoved(Form akBaseItem, Int aiItemCount, ObjectReference akItemReference, ObjectReference akDestContainer)  
	(self.GetOwningQuest() as AHZMainQuest).RefreshWidgets()
EndEvent