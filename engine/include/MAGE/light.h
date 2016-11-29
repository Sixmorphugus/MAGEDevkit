#pragma once
#include "basic.h"

// casts light
class MAGEDLL light : public objBasic
{
public:
	light(float posX, float posY, textureData sprite, textureData lightSprite);

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
	virtual void registerProperties();

	virtual void update(sf::Time elapsed);

	CLONEABLE(light);
public:
	sf::Color lightColor;
	sf::Vector2f lightOffset;

	animator lightAnimations;

	sf::Sprite lSpr;
};
