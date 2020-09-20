import gfx.core.UIComponent;
import gfx.io.GameDelegate;


class Meter extends UIComponent
{
	/* PUBLIC VARIABLES */

	public static var MIN_PCT: Number = 0;
	public static var MAX_PCT: Number = 1;

	private static var EMPTY: String = "empty";
	private static var FULL: String = "full";

	private static var ANIMATING: Number = -1;
	private static var STOPPED: Number = 0;


	/* PRIVATE VARIABLES */

	private var _emptyFrame: Number;
	private var _fullFrame: Number;
	private var _minFrame: Number;
	private var _maxFrame: Number;

	private var _opaqueFrame: Number;
	private var _translucentFrame: Number;
	private var _targetFrame: Number;

	private var _opaque: MovieClip;
	private var _translucent: MovieClip;

	private var _pctDelta: Number = 0.05;
	private var _frameDelta: Number = 12;
	private var _frameRate: Number = 60;

	private var _translucentTimer: Number = 0;

	/* STAGE ELEMENTS */

	public var opaqueHolder: MovieClip;
	public var translucentHolder: MovieClip;


	/* INITIALIZATION */

	public function Meter()
	{
		super();
	}


	// @override UIComponent
	private function configUI(): Void
	{
		_opaque = opaqueHolder;
		_translucent = translucentHolder;

		_opaque.gotoAndStop(EMPTY);
		_emptyFrame = _opaque._currentframe;
		_opaque.gotoAndStop(FULL);
		_fullFrame = _opaque._currentframe;

		_translucent.gotoAndStop(FULL);

		_opaqueFrame = _fullFrame;
		_translucentFrame = _fullFrame;
		_targetFrame = _fullFrame;

		_minFrame = _emptyFrame;
		_maxFrame = _fullFrame;

		getFramerate();
		_frameDelta = Math.floor(lerp(_emptyFrame, _fullFrame, _pctDelta));
	}


	/* PUBLIC FUNCTIONS */

	public function get percentDelta(): Number
	{
		return _pctDelta;
	}


	public function set percentDelta(a_percentDelta: Number): Void
	{
		_pctDelta = clamp(a_percentDelta / 100.0, MIN_PCT, MAX_PCT);
		_frameDelta = Math.floor(lerp(_minFrame, _maxFrame, _pctDelta));
	}


	public function setPercent(a_percent: Number, a_force: Boolean): Void
	{
		var targetPct: Number = clamp(a_percent / 100.0, MIN_PCT, MAX_PCT);
		_targetFrame = Math.floor(lerp(_emptyFrame, _fullFrame, targetPct));

		if (a_force) {
			_opaqueFrame = _targetFrame;
			_opaque.gotoAndStop(_opaqueFrame);

			_translucentFrame = _targetFrame;
			_translucent.gotoAndStop(_translucentFrame);
		}

		if (_opaqueFrame > _targetFrame) {	// if shrink
			_opaqueFrame = _targetFrame;
		}

		if (_translucentTimer == STOPPED) {
			_translucentTimer = _frameRate;
		}
	}


	/* PRIVATE FUNCTIONS */

	// @override UIComponent
	private function draw(): Void
	{
		drawOpaque();

		if (_translucentFrame <= _opaqueFrame) {
			_translucentTimer = STOPPED;
		}

		if (_translucentTimer > STOPPED) {
			--_translucentTimer;
		} else {
			drawTranslucent();
		}
	}


	private function drawOpaque(): Void
	{
		if (_opaqueFrame < _targetFrame) {	// if grow
			_opaqueFrame = Math.min(_targetFrame, _opaqueFrame + _frameDelta);
		}

		if (_opaque._currentframe != _opaqueFrame) {
			_opaque.gotoAndStop(_opaqueFrame);
		}
	}


	private function drawTranslucent(): Void
	{
		if (_translucentFrame > _opaqueFrame) {
			_translucentFrame = Math.max(_emptyFrame, _translucentFrame - _frameDelta);	// always shrink
		} else {
			_translucentFrame = _opaqueFrame;
		}

		if (_translucent._currentframe != _translucentFrame) {
			_translucent.gotoAndStop(_translucentFrame);
			_translucentTimer = ANIMATING;
		} else {
			_translucentTimer = STOPPED;
		}
	}


	private function getFramerate(): Void
	{
		GameDelegate.call("GetFramerate", [], this, "setFramerate");
	}


	private function setFramerate(a_frameRate: Number): Void
	{
		_frameRate = a_frameRate;
	}


	private function clamp(a_val: Number, a_min: Number, a_max: Number): Number
	{
		return Math.max(a_min, Math.min(a_max, a_val));
	}


	private function lerp(a_min: Number, a_max: Number, a_pct: Number): Number
	{
		return (1 - a_pct) * a_min + a_pct * a_max;
	}
}
