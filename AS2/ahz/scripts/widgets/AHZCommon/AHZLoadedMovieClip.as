
class AHZLoadedMovieClip
{  
  	public var Loaded: MovieClip;

	public function loadWithExisting(a_existingClip:MovieClip):Void{
		Loaded = a_existingClip;
	}
	
	public function loadFromFile(a_path:String):Void{
		var loader = new MovieClipLoader();
		loader.addListener(this);
		loader.loadClip(a_path, Loaded);
	}	
	
  /* PRIVATE FUNCTIONS */

	// @implements MovieClipLoader
	private function onLoadInit(a_mc: MovieClip): Void
	{
		a_mc.gotoAndStop(0);
	}
}