#include "SuperAlchemist.h"
#include "myLoadingScreen.h"

DeclareGame(SuperAlchemist);

SuperAlchemist::SuperAlchemist(int argc, char * argv[], sf::RenderWindow* windowToUse)
	: Game(argc, argv, windowToUse)
{
	// replace default window settings
	loadWindowSettings(sf::VideoMode::getDesktopMode(), "SUPER_ALCHEMIST Beta (r" + std::to_string(version()) + ")", true);

	// replace default loading screen
	setLoadingScreen(new myLoadingScreen());
}