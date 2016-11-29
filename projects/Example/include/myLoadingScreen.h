#pragma once
#include "MAGE/uiPrimitives.h"

class myLoadingScreen :
	public loadingScreen
{
public:
	myLoadingScreen();
	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};

