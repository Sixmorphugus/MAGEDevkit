#include "WelcomeGame.h"
#include "myLoadingScreen.h"

// You don't really need to know C++ to use MAGE.
// You can just put DeclareGame(Game) to have MAGE use a base (unmodified) Game class to start up, or you can write your own main() function however you want.
// Using things written in C++ in your Game is demonstrated in shared files, however, and shown in the other examples.

DeclareGame(WelcomeGame);

WelcomeGame::WelcomeGame(int argc, char * argv[], sf::RenderWindow* windowToUse)
	: Game(argc, argv, windowToUse)
{
	// replace default loading screen with our own
	setLoadingScreen(new myLoadingScreen());
}