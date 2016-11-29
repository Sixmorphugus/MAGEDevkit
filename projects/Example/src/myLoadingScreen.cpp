#include "myLoadingScreen.h"

#include "MAGE/Game.h"
#include "MAGE/uiPrimitives.h"
#include "MAGE/view.h"

myLoadingScreen::myLoadingScreen()
	: loadingScreen()
{
}

void myLoadingScreen::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	// draw the @ symbol in the corner
	float screenHeight = theGame()->uiCamera->getSize().y;

	pDrawInfoSimple(target, states, sf::Vector2f(48.f, screenHeight - 48.f + (sin(theGame()->getSimTime().asSeconds() * 2.f) * 2.f)), "Loading!");
}
