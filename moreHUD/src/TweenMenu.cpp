#include "PCH.h"
#include "tweenmenu.h"


namespace Scaleform
{
	// Start the animation
	REL::Relocation<TweenMenu::StartCloseMenu_t> TweenMenu::_StartCloseMenu(REL::ID(51838));

	// Close the Tween Menu, This function needs to be called to allow the animation to finish;
	REL::Relocation<TweenMenu::CloseMenu_t> TweenMenu::_CloseMenu(REL::ID(51839));

	void TweenMenu::CloseMenu()
	{
		_CloseMenu();
	}

	void TweenMenu::StartCloseMenu()
	{
		_StartCloseMenu();
	}
}
