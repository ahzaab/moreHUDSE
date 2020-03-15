
intrinsic class skse.plugins.AHZmoreHUDPlugin
{
	static function GetTargetObjectData(data:Object):Void;
	static function GetPlayerData(data:Object):Void;
	static function GetIsPlayerInCombat():Boolean;
	static function GetTargetEffects(data:Object, calculateVentory:Boolean):Void;
	static function InstallHooks():Void;
	static function GetIsBookAndWasRead():Boolean;
	static function GetIsValidTarget(data:Object):Boolean;
	static function GetArmorWeightClassString():String;
	static function GetBookSkillString():String;
	static function GetValueToWeightString(data:String, VWTranslation:String):String;
	static function GetEnemyInformation(data:Object, LevelTranslation:String):Void;
	static function IsAKnownEnchantedItem():Number;
	static function GetTargetWarmthRating():Number;
	static function AHZLog(data:String):String;
}