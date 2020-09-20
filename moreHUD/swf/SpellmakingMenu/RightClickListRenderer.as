import gfx.controls.ListItemRenderer;


class RightClickListRenderer extends ListItemRenderer
{
	/* INITIALIZATION */

	public function RightClickListRenderer()
	{
		super();
	}


	/* PRIVATE FUNCTIONS */

	// @override ListItemRenderer
	private function configUI(): Void
	{
		this["onPressAux"] = handleMousePress;
		this["onReleaseAux"] = handleMouseRelease;
		this["onDragOverAux"] = handleDragOver;
		this["onDragOutAux"] = handleDragOut;
		this["onReleaseOutsideAux"] = handleReleaseOutside;

		super.configUI();
	}
}
