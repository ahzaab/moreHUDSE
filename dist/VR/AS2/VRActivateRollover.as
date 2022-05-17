import flash.display.BitmapData;
import Shared.GlobalFunc;
import flash.geom.Transform;
import flash.geom.ColorTransform;
import flash.geom.Matrix;
import flash.filters.DropShadowFilter;

class VRActivateRollover extends Shared.PlatformChangeUser
{
    var RolloverNameInstance, RolloverText, ActivateButton_tf, RolloverButton_tf, RolloverInfoInstance, RolloverInfoText, GrayBarInstance, RolloverGrayBar_mc, ValueTranslated, WeightTranslated;
    function VRActivateRollover()
    {
        super();
        Shared.GlobalFunc.MaintainTextFormat();
        Shared.GlobalFunc.AddReverseFunctions();
        RolloverText = RolloverNameInstance;
        RolloverButton_tf = ActivateButton_tf;
        RolloverInfoText = RolloverInfoInstance;
        RolloverGrayBar_mc = GrayBarInstance;
        RolloverGrayBar_mc._alpha = 0;
        RolloverInfoText.html = true;
        RolloverText.All = true;
        RolloverInfoText.All = true;
        RolloverGrayBar_mc.All = true;
        RolloverButton_tf.All = true;
        RolloverText.Favor = true;
        RolloverInfoText.Favor = true;
        RolloverGrayBar_mc.Favor = true;
        RolloverButton_tf.Favor = true;
        RolloverText.StealthMode = true;
        RolloverButton_tf.StealthMode = true;
        RolloverInfoText.StealthMode = true;
        RolloverGrayBar_mc.StealthMode = true;
        RolloverText.Swimming = true;
        RolloverInfoText.Swimming = true;
        RolloverGrayBar_mc.Swimming = true;
        RolloverButton_tf.Swimming = true;
    } // End of the function
    function InitExtensions()
    {
        RolloverText.verticalAutoSize = "top";
        RolloverText.html = true;
    } // End of the function
    function SetPlatform(aiPlatform, abPS3Switch)
    {
    } // End of the function
    function SetCrosshairTarget(abActivate, aName, abShowButton, abTextOnly, abFavorMode, aWeight, aCost, aFieldValue, aFieldText)
    {
        var _loc5 = abFavorMode ? ("Favor") : ("NoTarget");
        var _loc10 = abFavorMode ? ("Favor") : ("Target");
        if (!abActivate && SavedRolloverText.length > 0)
        {
            RolloverText.SetText(SavedRolloverText, true);
            RolloverText._alpha = 100;
            RolloverButton_tf._alpha = 0;
        }
        else if (abTextOnly || abActivate)
        {
            RolloverText.SetText(aName, true);
            RolloverText._alpha = 100;
            RolloverButton_tf._alpha = abShowButton ? (100) : (0);
            RolloverButton_tf._x = RolloverText._x + RolloverText.getLineMetrics(0).x - 115;
        }
        else
        {
            RolloverText.SetText(" ", true);
            RolloverText._alpha = 0;
            RolloverButton_tf._alpha = 0;
        } // end else if
        var _loc2 = "";
        if (aCost != undefined)
        {
            _loc2 = ValueTranslated.text + " <font face=\'$EverywhereBoldFont\' size=\'24\' color=\'#FFFFFF\'>" + Math.round(aCost) + "</font>" + _loc2;
        } // end if
        if (aWeight != undefined)
        {
            _loc2 = WeightTranslated.text + " <font face=\'$EverywhereBoldFont\' size=\'24\' color=\'#FFFFFF\'>" + Shared.GlobalFunc.RoundDecimal(aWeight, 1) + "</font>      " + _loc2;
        } // end if
        if (aFieldValue != undefined)
        {
            var _loc3 = new TextField();
            _loc3.text = aFieldText.toString();
            _loc2 = _loc3.text + " <font face=\'$EverywhereBoldFont\' size=\'24\' color=\'#FFFFFF\'>" + Math.round(aFieldValue) + "</font>      " + _loc2;
        } // end if
        if (_loc2.length > 0)
        {
            RolloverGrayBar_mc._alpha = 100;
        }
        else
        {
            RolloverGrayBar_mc._alpha = 0;
        } // end else if
        RolloverInfoText.htmlText = _loc2;
    } // End of the function
    function RefreshActivateButtonArt(astrButtonName)
    {
        if (astrButtonName == undefined)
        {
            RolloverButton_tf.SetText(" ", true);
        }
        else
        {
            var _loc2 = flash.display.BitmapData.loadBitmap(astrButtonName + ".png");
            if (_loc2 != undefined && _loc2.height > 0)
            {
                var _loc3 = 26;
                var _loc5 = Math.floor(_loc3 / _loc2.height * _loc2.width);
                RolloverButton_tf.SetText("<img src=\'" + astrButtonName + ".png\' height=\'" + _loc3 + "\' width=\'" + _loc5 + "\'>", true);
            }
            else
            {
                RolloverButton_tf.SetText(" ", true);
            } // end else if
        } // end else if
    } // End of the function
    var SavedRolloverText = "";
} // End of Class
