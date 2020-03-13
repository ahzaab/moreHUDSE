import gfx.io.GameDelegate;

class ahz.scripts.widgets.AHZCommon.AHZBottomBar extends MovieClip
{
	var PlayerInfoCard_mc: MovieClip;
	
	var PlayerInfoObj: Object;
	var iLastItemType: Number;
	var iLeftOffset: Number;
	var playerCardWith: Number;
	
	var PLAYER_CARD_WIDTH:Number = 651.0;

	function AHZBottomBar()
	{
		super();
		PlayerInfoCard_mc = this;
		iLastItemType = AHZInventoryDefines.ICT_NONE;
	}

	function PositionElements(aiLeftOffset: Number, aiRightOffset: Number): Void
	{
		//iLeftOffset = aiLeftOffset;
		//PlayerInfoCard_mc._x = aiRightOffset - PlayerInfoCard_mc._width;
	}

	function ShowPlayerInfo(): Void
	{
		PlayerInfoCard_mc._alpha = 100;
	}

	function HidePlayerInfo(): Void
	{
		PlayerInfoCard_mc._alpha = 0;
	}

	function UpdatePerItemInfo(aItemUpdateObj: Object, aligned:Number): Void
	{
		var iItemType: Number = aItemUpdateObj.type;
		if (iItemType == undefined) {
			iItemType = iLastItemType;
			if (aItemUpdateObj == undefined)
				aItemUpdateObj = {type: iLastItemType};
		} else {
			iLastItemType = iItemType;
		}
		if (PlayerInfoObj != undefined && aItemUpdateObj != undefined) {
			switch(iItemType) {
				case AHZInventoryDefines.ICT_ARMOR:
					PlayerInfoCard_mc.gotoAndStop("Armor");
					var strArmor: String = Math.floor(PlayerInfoObj.armor).toString();
					if (aItemUpdateObj.armorChange != undefined) {
						var iArmorDelta = Math.round(aItemUpdateObj.armorChange);
						if (iArmorDelta > 0) 
							strArmor = strArmor + " <font color=\'#189515\'>(+" + iArmorDelta.toString() + ")</font>";
						else if (iArmorDelta < 0) 
							strArmor = strArmor + " <font color=\'#FF0000\'>(" + iArmorDelta.toString() + ")</font>";
					}
					PlayerInfoCard_mc.ArmorRatingValue.textAutoSize = "shrink";
					PlayerInfoCard_mc.ArmorRatingValue.html = true;
					PlayerInfoCard_mc.ArmorRatingValue.SetText(strArmor, true);
					
					var strWarmth = aItemUpdateObj.warmth == undefined?"0":Math.floor(aItemUpdateObj.warmth).toString();
					if(aItemUpdateObj.warmthChange != undefined)
					{
					  var iWarmthDelta = Math.round(aItemUpdateObj.warmthChange);
					  if(iWarmthDelta > 0)
					  {
						 strWarmth = strWarmth + " <font color=\'#189515\'>(+" + iWarmthDelta.toString() + ")</font>";
					  }
					  else if(iWarmthDelta < 0)
					  {
						 strWarmth = strWarmth + " <font color=\'#FF0000\'>(" + iWarmthDelta.toString() + ")</font>";
					  }
					}
					this.PlayerInfoCard_mc.WarmthRatingLabel._visible = aItemUpdateObj.warmth != undefined;
					this.PlayerInfoCard_mc.WarmthRatingValue._visible = aItemUpdateObj.warmth != undefined;
					this.PlayerInfoCard_mc.WarmthRatingValue.textAutoSize = "shrink";
					this.PlayerInfoCard_mc.WarmthRatingValue.html = true;
					this.PlayerInfoCard_mc.WarmthRatingValue.SetText(strWarmth,true);						
					break;
					
				case AHZInventoryDefines.ICT_WEAPON:
					PlayerInfoCard_mc.gotoAndStop("Weapon");
					var strDamage: String = Math.floor(PlayerInfoObj.damage).toString();
					if (aItemUpdateObj.damageChange != undefined) {
						var iDamageDelta = Math.round(aItemUpdateObj.damageChange);
						if (iDamageDelta > 0) 
							strDamage = strDamage + " <font color=\'#189515\'>(+" + iDamageDelta.toString() + ")</font>";
						else if (iDamageDelta < 0) 
							strDamage = strDamage + " <font color=\'#FF0000\'>(" + iDamageDelta.toString() + ")</font>";
					}
					PlayerInfoCard_mc.DamageValue.textAutoSize = "shrink";
					PlayerInfoCard_mc.DamageValue.html = true;
					PlayerInfoCard_mc.DamageValue.SetText(strDamage, true);
					break;
					
				case AHZInventoryDefines.ICT_DEFAULT:
				default:
					PlayerInfoCard_mc.gotoAndStop("Default");
			}
			
			//PlayerInfoCard_mc.CarryWeightValue.textAutoSize = "shrink";
			
			if (aItemUpdateObj.objWeight > 0)
			{
				var totalWeight:Number = PlayerInfoObj.encumbrance + aItemUpdateObj.objWeight; //aItemUpdateObj.objWeight,1);	
				if (totalWeight > PlayerInfoObj.maxEncumbrance)
				{
					PlayerInfoCard_mc.CarryWeightValue.html = true;
					PlayerInfoCard_mc.CarryWeightValue.htmlText = Math.ceil(PlayerInfoObj.encumbrance).toString() +
					   	" <font color=\'#FF0000\'>(" + Math.ceil(totalWeight).toString() + ")</font>/" + Math.floor(PlayerInfoObj.maxEncumbrance).toString();
				}
				else
				{
					PlayerInfoCard_mc.CarryWeightValue.html = true;
					PlayerInfoCard_mc.CarryWeightValue.htmlText = Math.ceil(PlayerInfoObj.encumbrance).toString() +
						" <font color=\'#189515\'>(" + Math.ceil(totalWeight).toString() + ")</font>/" + Math.floor(PlayerInfoObj.maxEncumbrance).toString();					
				}
			}
			else
			{
				PlayerInfoCard_mc.CarryWeightValue.html = false;
				PlayerInfoCard_mc.CarryWeightValue.SetText(Math.ceil(PlayerInfoObj.encumbrance).toString() + "/" + Math.floor(PlayerInfoObj.maxEncumbrance).toString());
			}
			PlayerInfoCard_mc.PlayerGoldValue.SetText(PlayerInfoObj.gold.toString());
			if (aligned == 0) //Left
			{				
				PlayerInfoCard_mc.CarryWeightValue.autoSize = "left";
				PlayerInfoCard_mc.CarryWeightLabel.autoSize = "left";
				PlayerInfoCard_mc.PlayerGoldValue.autoSize = "left";
				PlayerInfoCard_mc.PlayerGoldLabel.autoSize = "left";
				PlayerInfoCard_mc.ArmorRatingValue.autoSize = "left";
				PlayerInfoCard_mc.ArmorRatingLabel.autoSize = "left";
				PlayerInfoCard_mc.WarmthRatingValue.autoSize = "left";
				PlayerInfoCard_mc.WarmthRatingLabel.autoSize = "left";
				PlayerInfoCard_mc.DamageValue.autoSize = "left";
				PlayerInfoCard_mc.DamageLabel.autoSize = "left";
				
				PlayerInfoCard_mc.PlayerGoldLabel._x = 0;	
				PlayerInfoCard_mc.PlayerGoldValue._x = PlayerInfoCard_mc.PlayerGoldLabel._x + PlayerInfoCard_mc.PlayerGoldLabel.getLineMetrics(0).width + 3;
				PlayerInfoCard_mc.CarryWeightLabel._x = PlayerInfoCard_mc.PlayerGoldValue._x + PlayerInfoCard_mc.PlayerGoldValue.getLineMetrics(0).width + 8;
				PlayerInfoCard_mc.CarryWeightValue._x = PlayerInfoCard_mc.CarryWeightLabel._x + PlayerInfoCard_mc.CarryWeightLabel.getLineMetrics(0).width + 3;
				if (iItemType === AHZInventoryDefines.ICT_ARMOR) {
					PlayerInfoCard_mc.ArmorRatingLabel._x = PlayerInfoCard_mc.CarryWeightValue._x + PlayerInfoCard_mc.CarryWeightValue.getLineMetrics(0).width + 8;
					PlayerInfoCard_mc.ArmorRatingValue._x = PlayerInfoCard_mc.ArmorRatingLabel._x + PlayerInfoCard_mc.ArmorRatingLabel.getLineMetrics(0).width + 3;
					PlayerInfoCard_mc.WarmthRatingLabel._x = PlayerInfoCard_mc.ArmorRatingValue._x + PlayerInfoCard_mc.ArmorRatingValue.getLineMetrics(0).width + 8;
					PlayerInfoCard_mc.WarmthRatingValue._x = PlayerInfoCard_mc.WarmthRatingLabel._x + PlayerInfoCard_mc.WarmthRatingLabel.getLineMetrics(0).width + 3;    	
				} else if (iItemType === AHZInventoryDefines.ICT_WEAPON) {
					PlayerInfoCard_mc.DamageLabel._x = PlayerInfoCard_mc.CarryWeightValue._x + PlayerInfoCard_mc.CarryWeightValue.getLineMetrics(0).width + 8;
					PlayerInfoCard_mc.DamageValue._x = PlayerInfoCard_mc.DamageLabel._x + PlayerInfoCard_mc.DamageLabel.getLineMetrics(0).width + 3;			
				}
			}			
			else if (aligned == 1) // Right
			{				
				PlayerInfoCard_mc.CarryWeightValue.autoSize = "right";
				PlayerInfoCard_mc.CarryWeightLabel.autoSize = "right";
				PlayerInfoCard_mc.PlayerGoldValue.autoSize = "right";
				PlayerInfoCard_mc.PlayerGoldLabel.autoSize = "right";
				PlayerInfoCard_mc.ArmorRatingValue.autoSize = "right";
				PlayerInfoCard_mc.ArmorRatingLabel.autoSize = "right";
				PlayerInfoCard_mc.DamageValue.autoSize = "right";
				PlayerInfoCard_mc.DamageLabel.autoSize = "right";				
				
				
				PlayerInfoCard_mc.PlayerGoldValue._x = PLAYER_CARD_WIDTH - PlayerInfoCard_mc.PlayerGoldValue._width;
			

				PlayerInfoCard_mc.PlayerGoldLabel._x = PlayerInfoCard_mc.PlayerGoldValue._x + PlayerInfoCard_mc.PlayerGoldValue.getLineMetrics(0).x - PlayerInfoCard_mc.PlayerGoldLabel._width;
				PlayerInfoCard_mc.CarryWeightValue._x = PlayerInfoCard_mc.PlayerGoldLabel._x + PlayerInfoCard_mc.PlayerGoldLabel.getLineMetrics(0).x - PlayerInfoCard_mc.CarryWeightValue._width - 5;
				PlayerInfoCard_mc.CarryWeightLabel._x = PlayerInfoCard_mc.CarryWeightValue._x + PlayerInfoCard_mc.CarryWeightValue.getLineMetrics(0).x - PlayerInfoCard_mc.CarryWeightLabel._width;
				if (iItemType === AHZInventoryDefines.ICT_ARMOR) {
					PlayerInfoCard_mc.ArmorRatingValue._x = PlayerInfoCard_mc.CarryWeightLabel._x + PlayerInfoCard_mc.CarryWeightLabel.getLineMetrics(0).x - PlayerInfoCard_mc.ArmorRatingValue._width - 5;
					PlayerInfoCard_mc.ArmorRatingLabel._x = PlayerInfoCard_mc.ArmorRatingValue._x + PlayerInfoCard_mc.ArmorRatingValue.getLineMetrics(0).x - PlayerInfoCard_mc.ArmorRatingLabel._width;
					PlayerInfoCard_mc.WarmthRatingValue._x = PlayerInfoCard_mc.ArmorRatingLabel._x + PlayerInfoCard_mc.ArmorRatingLabel.getLineMetrics(0).x - PlayerInfoCard_mc.WarmthRatingValue._width - 5;
                    PlayerInfoCard_mc.WarmthRatingLabel._x = PlayerInfoCard_mc.WarmthRatingValue._x + PlayerInfoCard_mc.WarmthRatingValue.getLineMetrics(0).x - PlayerInfoCard_mc.WarmthRatingLabel._width;
				} else if (iItemType === AHZInventoryDefines.ICT_WEAPON) {
					PlayerInfoCard_mc.DamageValue._x = PlayerInfoCard_mc.CarryWeightLabel._x + PlayerInfoCard_mc.CarryWeightLabel.getLineMetrics(0).x - PlayerInfoCard_mc.DamageValue._width - 5;
					PlayerInfoCard_mc.DamageLabel._x = PlayerInfoCard_mc.DamageValue._x + PlayerInfoCard_mc.DamageValue.getLineMetrics(0).x - PlayerInfoCard_mc.DamageLabel._width;
				}		
			}
			else if (aligned == 2) // Center
			{				
				PlayerInfoCard_mc.CarryWeightValue.autoSize = "right";
				PlayerInfoCard_mc.CarryWeightLabel.autoSize = "right";
				PlayerInfoCard_mc.PlayerGoldValue.autoSize = "right";
				PlayerInfoCard_mc.PlayerGoldLabel.autoSize = "right";
				PlayerInfoCard_mc.ArmorRatingValue.autoSize = "right";
				PlayerInfoCard_mc.ArmorRatingLabel.autoSize = "right";
				PlayerInfoCard_mc.WarmthRatingValue.autoSize = "right";
				PlayerInfoCard_mc.WarmthRatingLabel.autoSize = "right";
				PlayerInfoCard_mc.DamageValue.autoSize = "right";
				PlayerInfoCard_mc.DamageLabel.autoSize = "right";				
				
				
				PlayerInfoCard_mc.PlayerGoldValue._x = PLAYER_CARD_WIDTH - PlayerInfoCard_mc.PlayerGoldValue._width;
			

				PlayerInfoCard_mc.PlayerGoldLabel._x = PlayerInfoCard_mc.PlayerGoldValue._x + PlayerInfoCard_mc.PlayerGoldValue.getLineMetrics(0).x - PlayerInfoCard_mc.PlayerGoldLabel._width;
				PlayerInfoCard_mc.CarryWeightValue._x = PlayerInfoCard_mc.PlayerGoldLabel._x + PlayerInfoCard_mc.PlayerGoldLabel.getLineMetrics(0).x - PlayerInfoCard_mc.CarryWeightValue._width - 5;
				PlayerInfoCard_mc.CarryWeightLabel._x = PlayerInfoCard_mc.CarryWeightValue._x + PlayerInfoCard_mc.CarryWeightValue.getLineMetrics(0).x - PlayerInfoCard_mc.CarryWeightLabel._width;
				if (iItemType === AHZInventoryDefines.ICT_ARMOR) {
					PlayerInfoCard_mc.ArmorRatingValue._x = PlayerInfoCard_mc.CarryWeightLabel._x + PlayerInfoCard_mc.CarryWeightLabel.getLineMetrics(0).x - PlayerInfoCard_mc.ArmorRatingValue._width - 5;
					PlayerInfoCard_mc.ArmorRatingLabel._x = PlayerInfoCard_mc.ArmorRatingValue._x + PlayerInfoCard_mc.ArmorRatingValue.getLineMetrics(0).x - PlayerInfoCard_mc.ArmorRatingLabel._width;
					PlayerInfoCard_mc.WarmthRatingValue._x = PlayerInfoCard_mc.ArmorRatingLabel._x + PlayerInfoCard_mc.ArmorRatingLabel.getLineMetrics(0).x - PlayerInfoCard_mc.WarmthRatingValue._width - 5;
                    PlayerInfoCard_mc.WarmthRatingLabel._x = PlayerInfoCard_mc.WarmthRatingValue._x + PlayerInfoCard_mc.WarmthRatingValue.getLineMetrics(0).x - PlayerInfoCard_mc.WarmthRatingLabel._width;
				} else if (iItemType === AHZInventoryDefines.ICT_WEAPON) {
					PlayerInfoCard_mc.DamageValue._x = PlayerInfoCard_mc.CarryWeightLabel._x + PlayerInfoCard_mc.CarryWeightLabel.getLineMetrics(0).x - PlayerInfoCard_mc.DamageValue._width - 5;
					PlayerInfoCard_mc.DamageLabel._x = PlayerInfoCard_mc.DamageValue._x + PlayerInfoCard_mc.DamageValue.getLineMetrics(0).x - PlayerInfoCard_mc.DamageLabel._width;
				}
				
				var deltaVal:Number = 0.0;
				if (iItemType === AHZInventoryDefines.ICT_ARMOR) 
				{
					if (PlayerInfoCard_mc.WarmthRatingLabel._visible)
					{
						deltaVal = ((PLAYER_CARD_WIDTH - PlayerInfoCard_mc.WarmthRatingLabel._x) / 2.0) + PlayerInfoCard_mc.WarmthRatingLabel._x;
						deltaVal -= (PLAYER_CARD_WIDTH / 2.0);
						PlayerInfoCard_mc.WarmthRatingLabel._x -= deltaVal;
						PlayerInfoCard_mc.WarmthRatingValue._x -= deltaVal;	
					}
					else
					{
						deltaVal = ((PLAYER_CARD_WIDTH - PlayerInfoCard_mc.ArmorRatingLabel._x) / 2.0) + PlayerInfoCard_mc.ArmorRatingLabel._x;
						deltaVal -= (PLAYER_CARD_WIDTH / 2.0);
					}
					PlayerInfoCard_mc.ArmorRatingLabel._x -= deltaVal;
					PlayerInfoCard_mc.ArmorRatingValue._x -= deltaVal;					
				} 
				else if (iItemType === AHZInventoryDefines.ICT_WEAPON) 
				{
					deltaVal = ((PLAYER_CARD_WIDTH - PlayerInfoCard_mc.DamageLabel._x) / 2.0) + PlayerInfoCard_mc.DamageLabel._x;
					deltaVal -= (PLAYER_CARD_WIDTH / 2.0);
					//deltaVal = PlayerInfoCard_mc.DamageLabel._x - deltaVal;
					PlayerInfoCard_mc.DamageLabel._x -= deltaVal;
					PlayerInfoCard_mc.DamageValue._x -= deltaVal;
				}		
				else
				{
					deltaVal = ((PLAYER_CARD_WIDTH - PlayerInfoCard_mc.CarryWeightLabel._x) / 2.0) + PlayerInfoCard_mc.CarryWeightLabel._x;
					deltaVal -= (PLAYER_CARD_WIDTH / 2.0);
				}

				PlayerInfoCard_mc.CarryWeightLabel._x -= deltaVal;
				PlayerInfoCard_mc.CarryWeightValue._x -= deltaVal;
				PlayerInfoCard_mc.PlayerGoldLabel._x -= deltaVal;
				PlayerInfoCard_mc.PlayerGoldValue._x -= deltaVal;	
			}
		}
	}

	function UpdatePlayerInfo(aPlayerUpdateObj: Object, aItemUpdateObj: Object, bottomAligned:Number): Void
	{
		PlayerInfoObj = aPlayerUpdateObj;
		UpdatePerItemInfo(aItemUpdateObj,bottomAligned);
	}
}
