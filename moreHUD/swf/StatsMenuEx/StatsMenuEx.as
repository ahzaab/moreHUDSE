import Components.Meter;
import Shared.GlobalFunc;
import gfx.controls.Button;
import gfx.events.EventDispatcher;
import gfx.events.EventTypes;
import gfx.io.GameDelegate;
import gfx.managers.FocusHandler;
import gfx.ui.InputDetails;
import gfx.ui.NavigationCode;

class StatsMenuEx extends MovieClip
{
	/* PRIVATE VARIABLES */

	// Meter indices
	var MAGICKA_METER: Number = 0;
	var HEALTH_METER: Number = 1;
	var STAMINA_METER: Number = 2;

	var STATE_MAIN:Number = 0;
	var STATE_CLASS:Number = 1;

	// Focus indices
	var ROOT_LIST: Number = 0;
	var TREE_LIST: Number = 1;
	var PERK_LIST: Number = 2;
	var RANK_LIST: Number = 3;
	var UNLOCK_BUTTON: Number = 4;

	var MeterText: Array;
	//Meters (Statistics).
	var MagickaMeter: Meter;
	var HealthMeter: Meter;
	var StaminaMeter: Meter;
	var SkillsMeter: Meter;
	//Movieclips (Statistics).
	var MagickaMeterBase: MovieClip;
	var HealthMeterBase: MovieClip;
	var StaminaMeterBase: MovieClip;

	private var requisites: MovieClip;
	private var unlocks: MovieClip;
	private var unlock: Button;
	private var PerksList_mc: MovieClip;
	private var listener: MovieClip;
	private var requisitesHeader:TextField;
	private var Root_mc:MovieClip;
	private var Trees_mc:MovieClip;
	private var Perks_mc:MovieClip;
	private var Ranks_mc:MovieClip;
	private var TreeList:MovieClip;
	private var PerkList:MovieClip;
	private var RankList:MovieClip;
	private var RootList:MovieClip;
	private var PlayerStats_mc:MovieClip;
	private var FirstLastLabel:TextField;
	private var PlayerStatsEx_mc: MovieClip;
	private var LevelMeter: Meter;
	private var PlayerAttributes_mc: MovieClip;
	private var PlayerRenown_mc: MovieClip;
	private var selectedClip:Array;
	private var selectedClipIdx:Number;
	private var changingSelection;
	private var UnlockButton_mc:MovieClip;
	private var SkillInfo_mc:MovieClip;
	private var PerkInfo_mc:MovieClip;
	private var ClassInfo_mc:MovieClip;
	private var perkPointsTextHolder:TextField;
	private var perkPoints:TextField;
	private var currentState:Number;
	private var MajorSkills:MovieClip;
	private var MajorSkillsList:MovieClip;
	private var MajorAttributesList:MovieClip;
	private var FactionInfo_mc:MovieClip;
	private var FactionsList:MovieClip;
	/* INITIALIZATION */

	public function StatsMenuEx()
	{
		super();
		Shared.GlobalFunc.MaintainTextFormat();
		Shared.GlobalFunc.AddReverseFunctions();
		EventDispatcher.initialize(this);
		GameDelegate.addCallBack("SetPlayerInfo", this, "SetPlayerInfo");
		GameDelegate.addCallBack("SetPlayerInfoEx", this, "SetPlayerInfoEx");
		GameDelegate.addCallBack("SetSkillInfo", this, "SetSkillInfo");
		GameDelegate.addCallBack("SetPerkCount", this, "SetPerkCount");
		GameDelegate.addCallBack("SetClassAndFactions", this, "SetClassAndFactions");
		GameDelegate.addCallBack("onLeftStickInput", this, "onLeftStickInput");

		currentState = STATE_MAIN;
		TreeList = Trees_mc.List_mc;
		PerkList = Perks_mc.List_mc;
		RankList = Ranks_mc.List_mc;
		RootList = Root_mc.List_mc;
		MajorSkills = ClassInfo_mc.MajorSkills_mc;
		MajorSkillsList = MajorSkills.List_mc;
		MajorAttributesList = ClassInfo_mc.MajorAttributes_mc.List_mc;
		FactionsList = FactionInfo_mc.Factions_mc.List_mc;

		unlock = PerkInfo_mc.UnlockButton_mc.button;
		unlocks = PerkInfo_mc.unlocks;
		requisites = PerkInfo_mc.requisites;

		selectedClipIdx = 0;
		selectedClip = [RootList, TreeList, PerkList, RankList, unlock];

	}

	public function onLoad(): Void
	{
		TreeList.ClearList();
		TreeList.InvalidateData();
		PerkList.ClearList();
		PerkList.InvalidateData();
		RankList.ClearList();
		RankList.InvalidateData();
		requisites.ClearList();
		requisites.InvalidateData();
		unlocks.ClearList();
		unlocks.InvalidateData();
		MajorSkillsList.ClearList();
		MajorAttributesList.ClearList();
		FactionsList.ClearList();
		
		trace("ClassInfo_mc: "+ ClassInfo_mc);
		trace("ClassInfo_mc.MajorSkills_mc: "+ ClassInfo_mc.MajorSkills_mc);
		trace("MajorSkills: "+ MajorSkills)
		trace("ClassInfo_mc.MajorSkills_mc.List_mc: "+ MajorSkills.List_mc);
		
		trace("PerkInfo_mc: "+ PerkInfo_mc);
		trace("PerkInfo_mc.descriptionLabel: "+ PerkInfo_mc.descriptionLabel);
		
		PerkInfo_mc.descriptionLabel.textAutoSize = "shrink";
		PerkInfo_mc.descText.textAutoSize = "shrink";
		PerkInfo_mc.perkLabel.textAutoSize = "shrink";

		RootList.addEventListener("selectionChange",this,"handleRootsSelect");

		TreeList.addEventListener("selectionChange",this,"handleTreesSelect");

		PerkList.addEventListener("selectionChange",this,"handlePerksSelect");
		PerkList.addEventListener("itemPress",this,"handlePerksPress");

		RankList.addEventListener("selectionChange",this,"handleRanksSelect");
		RankList.addEventListener("itemPress",this,"handleRanksPress");

		if (requisites != undefined)
		{
			requisites.addEventListener(EventTypes.ITEM_PRESS, this, "handleRequisitesPress");
		}
		else {
			log("could not get requisites");
		}
		if (unlocks != undefined)
		{
			unlocks.addEventListener(EventTypes.ITEM_PRESS, this, "handleUnlocksPress");
		}
		else {
			log("could not get unlocks");
		}

		if (unlock != undefined)
		{
			unlock.addEventListener(EventTypes.PRESS, this, "handleUnlockPress");
		}
		else {
			log("could not get unlock");
		}

		// Code taken from vanilla stats menu to setup the meters
		var playerInfoCard:MovieClip = PlayerStats_mc.PlayerInfoCardInstance;
		this.MagickaMeterBase = playerInfoCard.MagickaMeterInstance;
		this.HealthMeterBase = playerInfoCard.HealthMeterInstance;
		this.StaminaMeterBase = playerInfoCard.StaminaMeterInstance;
		this.MagickaMeter = new Components.Meter(this.MagickaMeterBase.MagickaMeter_mc);
		this.HealthMeter = new Components.Meter(this.HealthMeterBase.HealthMeter_mc);
		this.StaminaMeter = new Components.Meter(this.StaminaMeterBase.StaminaMeter_mc);
		this.MagickaMeterBase.Magicka.gotoAndStop("Pause");
		this.HealthMeterBase.Health.gotoAndStop("Pause");
		this.StaminaMeterBase.Stamina.gotoAndStop("Pause");
		this.MeterText = [playerInfoCard.magicValue,playerInfoCard.healthValue,playerInfoCard.enduranceValue];
		SetMeter(this.MAGICKA_METER, 50, 100);
		SetMeter(this.HEALTH_METER, 75, 100);
		SetMeter(this.STAMINA_METER, 25, 100);

		// Not vary scalable, but for now we have 8 attributes
		for (var i:Number = 0; i < 8; i++)
		{
			PlayerAttributes_mc["Entry" + i.toString()].label.textAutoSize = "shrink";
			PlayerAttributes_mc["Entry" + i.toString()].base.textAutoSize = "shrink";
			PlayerAttributes_mc["Entry" + i.toString()].mod.textAutoSize = "shrink";
		}

		for (var i:Number = 0; i < 2; i++)
		{
			PlayerRenown_mc["Entry" + i.toString()].label.textAutoSize = "shrink";
			PlayerRenown_mc["Entry" + i.toString()].value.textAutoSize = "shrink";
		}

		// Make sure this gets initial focus
		FocusHandler.instance.setFocus(this,0);

		Selection.setFocus(RootList);

		SkillInfo_mc.skillLabel.textAutoSize = "shrink";
		SkillInfo_mc.maxValue.textAutoSize = "shrink";
		SkillInfo_mc.minValue.textAutoSize = "shrink";
		SkillsMeter = new Components.Meter(SkillInfo_mc.meter);		
	}

	public function handleInput(details: InputDetails, pathToFocus: Array): Boolean
	{
		var inputHandled:Boolean = false;

		var nextClip = pathToFocus.shift();
		if (nextClip && nextClip.handleInput(details, pathToFocus))
			return true;

		if (GlobalFunc.IsKeyPressed(details))
		{
			if (details.navEquivalent == NavigationCode.RIGHT)
			{
				if (currentState == STATE_MAIN)
				{
					if (selectedClipIdx < selectedClip.length - 1 && (selectedClip[selectedClipIdx+1] == unlock || selectedClip[selectedClipIdx+1].entryList.length))
					{
						selectedClipIdx++;
					}
	
					// Allow selection of the unlock button if allowed to unlock
					if (selectedClipIdx == UNLOCK_BUTTON && !canUnlockPerk())
					{
						selectedClipIdx--;
					}
	
					if (selectedClipIdx == UNLOCK_BUTTON)
					{
						unlock.selected = true;
						unlock.displayFocus = true;
					}
					else
					{
						selectedClip[selectedClipIdx].focused = 1;
					}
					inputHandled = true;
					GameDelegate.call("PlaySound", ["UIMenuBladeOpenSD"]);
				}
			}
			else if (details.navEquivalent == NavigationCode.LEFT)
			{
				if (currentState == STATE_MAIN)
				{
					if (selectedClipIdx > 0 && selectedClip[selectedClipIdx-1].maxEntries)
					{
						selectedClipIdx--;
					}
					unlock.displayFocus = false;
					unlock.selected = false;
					selectedClip[selectedClipIdx].focused = 1;
					GameDelegate.call("PlaySound", ["UIMenuBladeOpenSD"]);
					inputHandled = true;
				}
			}
			else if (details.navEquivalent == NavigationCode.TAB)
			{
				GameDelegate.call("CloseMenu", []);
			}
		}

		if (inputHandled)
			return true;

		return selectedClip[selectedClipIdx].handleInput(details, pathToFocus);
	}

	// @APIs
	function SetPerkCount(): Void
	{
		if (arguments[0])
		{
			perkPoints.SetText(perkPointsTextHolder.text + "  " + arguments[0]);
			perkPoints._visible = true;
		}
		else
		{
			perkPoints._visible = false;
		}
	}

	function SetPlayerInfoEx(): Void
	{
		if (arguments[0] != undefined)
		{
			this.PlayerStatsEx_mc.ClassvalueLabel.SetText(arguments[0]);
		}
		var attributes:Array = arguments[1];

		for (var i:Number = 0; i < attributes.length; i++)
		{
			var entry =  this.PlayerAttributes_mc["Entry" + i.toString()];
			entry.label.SetText(attributes[i].label);
			entry.base.SetText(attributes[i].base);

			if (attributes[i].mod == 0)
			{
				entry.mod.SetText("<font color=\'" + attributes[i].modColor + "\'></font>", true);
			}
			else
			{
				entry.mod.SetText("<font color=\'" + attributes[i].modColor + "\'>(" + attributes[i].mod + ")</font>", true);
			}
		}

		this.PlayerRenown_mc["Entry0"].label.SetText("$BOUNTY");
		this.PlayerRenown_mc["Entry0"].value.SetText(arguments[2]);
		this.PlayerRenown_mc["Entry1"].label.SetText("$REPUTATION");
		this.PlayerRenown_mc["Entry1"].value.SetText(arguments[3]);

	}

	function SetPlayerInfo(): Void
	{
		// Matches the code from the vanilla menu
		this.FirstLastLabel.textAutoSize = "shrink";
		this.FirstLastLabel.SetText(arguments[0]);
		this.PlayerStatsEx_mc.LevelNumberLabel.SetText(arguments[1]);
		if (LevelMeter == undefined)
		{
			LevelMeter = new Meter(this.PlayerStatsEx_mc.animate);
		}
		LevelMeter.SetPercent(arguments[2]);
		this.PlayerStatsEx_mc.RacevalueLabel.SetText(arguments[3]);
		SetMeter(0, arguments[4], arguments[5], arguments[6]);
		SetMeter(1, arguments[7], arguments[8], arguments[9]);
		SetMeter(2, arguments[10], arguments[11], arguments[12]);
	}

	function SetSkillInfo(): Void
	{
		var skillLabelValue = arguments[0] + " <font color=\'" + arguments[1] + "\'>" + arguments[2] + "</font>"
		SkillInfo_mc.skillLabel.SetText(skillLabelValue, true);
		SkillInfo_mc.maxValue.SetText(arguments[3]);
		SkillInfo_mc.minValue.SetText(arguments[4]);
		SkillsMeter.SetPercent(arguments[5]);
	}

	function onLeftStickInput(afX: Number, afY: Number): Void
	{
		if (currentState == STATE_MAIN)
		{
			var list = selectedClip[selectedClipIdx];
			if (afY < 0 && list != unlock)
			{
				list.moveSelectionDown();
			}
			else if (afY > 0 && list != unlock)
			{
				list.moveSelectionUp();
			}
			else if (afX < 0)
			{
				if (selectedClipIdx > 0 && selectedClip[selectedClipIdx-1].entryList.length)
				{
					selectedClipIdx--;
				}
				unlock.displayFocus = false;
				unlock.selected = false;
				selectedClip[selectedClipIdx].focused = 1;
	
				if (selectedClip[selectedClipIdx].selectedIndex == -1)
				{
					selectedClip[selectedClipIdx].selectedIndex = 0;
				}
				else
				{
					GameDelegate.call("PlaySound", ["UIMenuBladeOpenSD"]);
				}
			}
			else if (afX > 0)
			{
				if (selectedClipIdx < selectedClip.length - 1 && (selectedClip[selectedClipIdx+1] == unlock || selectedClip[selectedClipIdx+1].entryList.length))
				{
					selectedClipIdx++;
				}
				// Allow selection of the unlock button if allowed to unloc
				if (selectedClipIdx == UNLOCK_BUTTON && !canUnlockPerk())
				{
					selectedClipIdx--;
				}
				if (selectedClipIdx == UNLOCK_BUTTON)
				{
					unlock.selected = true;
					unlock.displayFocus = true;
				}
				else
				{
					selectedClip[selectedClipIdx].focused = 1;
	
					if (selectedClip[selectedClipIdx].selectedIndex == -1)
					{
						selectedClip[selectedClipIdx].selectedIndex = 0;
					}
					else
					{
						GameDelegate.call("PlaySound", ["UIMenuBladeOpenSD"]);
					}
	
				}
				GameDelegate.call("PlaySound", ["UIMenuBladeOpenSD"]);
			}
		}
	}
	/* API */
	function SetClassAndFactions(): Void
	{
		var c:MovieClip = ClassInfo_mc;	
		c.classLabel.SetText(arguments[0]);
		c.birthsignValue.SetText(arguments[1]);
		var skillsArr:Array = arguments[2];
		MajorSkillsList.dataProvider = skillsArr;
		var factionsArr:Array = arguments[3];
		FactionsList.dataProvider = factionsArr;
		c.specializationValue.SetText(arguments[4]);
		var attrArr:Array =  arguments[5]
		MajorAttributesList.dataProvider = attrArr;
		
	}

	/* PRIVATE FUNCTIONS */

	private function SetMeter(aMeter: Number, aCurrentValue: Number, aMaxValue: Number, aColor: String): Void
	{
		if (aMeter >= this.MAGICKA_METER && aMeter <= this.STAMINA_METER)
		{
			var meterPercent: Number = 100 * (Math.max(0, Math.min(aCurrentValue, aMaxValue)) / aMaxValue);

			switch (aMeter)
			{
				case this.MAGICKA_METER:
					this.MagickaMeter.SetPercent(meterPercent);
					break;
				case this.HEALTH_METER:
					this.HealthMeter.SetPercent(meterPercent);
					break;
				case this.STAMINA_METER:
					this.StaminaMeter.SetPercent(meterPercent);
					break;
			}

			this.MeterText[aMeter].html = true;
			if (aColor == undefined)
			{
				this.MeterText[aMeter].SetText("<font color=\'#F9EDD5\'>" + aCurrentValue + "/" + aMaxValue + "</font>", true);
			}
			else
			{

				if (aColor == '#FFFFFF')
				{
					aColor = '#F9EDD5';
				}

				this.MeterText[aMeter].SetText("<font color=\'" + aColor + "\'>" + aCurrentValue + "/" + aMaxValue + "</font>", true);
			}
			this.MagickaMeter.Update();
			this.HealthMeter.Update();
			this.StaminaMeter.Update();
		}
	}

	private function handleRootsSelect(a_event: Object): Void
	{
		trace("handleRootsSelect");
		if (a_event.index == 0)
		{
			trace("class");
			this.gotoAndStop("class");
		}
		else
		{
			trace("main");
			this.gotoAndStop("main");
		}

		PerkList.ClearList();
		PerkList.InvalidateData();

		RankList.ClearList();
		RankList.InvalidateData();

		var index: Number = a_event.index;
		GameDelegate.call("OnRootPress", [index]);

		if (a_event.index != -1)
		{
			GameDelegate.call("PlaySound", ["UIMenuFocus"]);
		}

		if (!changingSelection)
		{
			selectedClipIdx = ROOT_LIST;
		}

	}

	private function handleTreesSelect(a_event: Object): Void
	{
		RankList.ClearList();
		RankList.InvalidateData();

		var index: Number = a_event.index;

		GameDelegate.call("OnTreePress", [index]);

		if (a_event.index != -1)
		{
			GameDelegate.call("PlaySound", ["UIMenuFocus"]);
		}

		if (!changingSelection)
		{
			selectedClipIdx = TREE_LIST;
		}
	}

	private function handlePerksSelect(a_event: Object): Void
	{
		var perkIndex: Number = a_event.index;
		var treeIndex: Number = TreeList.selectedIndex;
		GameDelegate.call("OnPerkPress", [perkIndex, treeIndex]);

		if (a_event.index != -1)
		{
			GameDelegate.call("PlaySound", ["UIMenuFocus"]);
		}

		if (!changingSelection)
		{
			selectedClipIdx = PERK_LIST;
		}
	}

	private function handleRanksSelect(a_event: Object): Void
	{
		var rankIndex: Number = a_event.index;
		var treeIndex: Number = TreeList.selectedIndex;
		GameDelegate.call("OnRankPress", [rankIndex, treeIndex]);

		if (a_event.index != -1)
		{
			GameDelegate.call("PlaySound", ["UIMenuFocus"]);
		}

		if (!changingSelection)
		{
			selectedClipIdx = RANK_LIST;
		}
	}

	private function handlePerksPress(a_event: Object): Void
	{
		var rankIndex: Number = RankList.selectedIndex;
		var perkIndex: Number = a_event.index;
		var treeIndex: Number = TreeList.selectedIndex;

		if (a_event.index != -1 && perkIndex != -1 && treeIndex != -1 && unlock.visible && !unlock.disabled)
		{
			GameDelegate.call("UnlockPerk", [rankIndex, perkIndex, treeIndex]);
		}
	}

	private function handleRanksPress(a_event: Object): Void
	{
		var rankIndex: Number = a_event.index;
		var perkIndex: Number = PerkList.selectedIndex;
		var treeIndex: Number = TreeList.selectedIndex;

		if (a_event.index != -1 && perkIndex != -1 && treeIndex != -1 && unlock.visible && !unlock.disabled)
		{
			GameDelegate.call("UnlockPerk", [rankIndex, perkIndex, treeIndex]);
		}
	}

	private function handleRequisitesPress(a_event: Object): Void
	{
		var requisiteIndex: Number = a_event.index;
		var treeIndex: Number = TreeList.selectedIndex;
		GameDelegate.call("OnRequisitePress", [requisiteIndex, treeIndex]);
	}

	private function handleUnlocksPress(a_event: Object): Void
	{
		var unlockIndex: Number = a_event.index;
		var treeIndex: Number = TreeList.selectedIndex;
		GameDelegate.call("OnUnlockPress", [unlockIndex, treeIndex]);
	}

	private function handleUnlockPress(a_event: Object): Void
	{
		var rankIndex: Number = RankList.selectedIndex;
		var perkIndex: Number = PerkList.selectedIndex;
		var treeIndex: Number = TreeList.selectedIndex;
		GameDelegate.call("PlaySound",["UIMenuOK"]);
		GameDelegate.call("UnlockPerk", [rankIndex, perkIndex, treeIndex]);
	}

	private function log(a_message: String): Void
	{
		GameDelegate.call("Log", [a_message]);
	}

	private function canUnlockPerk():Boolean
	{
		return (unlock.visible && !unlock.disabled && PerkInfo_mc._visible);
	}

}
