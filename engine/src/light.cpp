#include "light.h"

#include "Game.h"
#include "helpers.h"
#include "animationManager.h"
#include "sfResources.h"
#include "view.h"

light::light(float posX, float posY, textureData sprite, textureData lightSprite)
	: objBasic(posX, posY, sprite), lightAnimations(lightSprite.resource.lock())
{
	lSpr.setTexture(*lightSprite.resource.lock()->get());

	lightColor = sf::Color::White;

	// for animated light
	if (!lightSprite.defaultFrameSize) {
		lightAnimations.generateFrameRects(lightSprite.frameSize.x, lightSprite.frameSize.y);
		lightAnimations.add("cycle", lightAnimations.getFrameList(), sf::seconds(0.5f));

		lightAnimations.play("cycle", true);
	}
}

void light::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	objBasic::draw(target, states);

	// change states
	sf::BlendMode negMode;
	negMode.alphaSrcFactor = sf::BlendMode::Zero;
	negMode.alphaDstFactor = sf::BlendMode::OneMinusSrcAlpha;
	negMode.alphaEquation = sf::BlendMode::Add;

	states.blendMode = negMode;

	// ignore target, use game->shadowTex
	theGame()->shadowTex.draw(lSpr, states);
}

void light::registerProperties()
{
	objBasic::registerProperties();

	registerProperty("lightR", prop(lightColor.r));
	registerProperty("lightG", prop(lightColor.g));
	registerProperty("lightB", prop(lightColor.b));
	registerProperty("lightIntensity", prop(lightColor.a));

	registerProperty("lightOffsetX", prop(lightOffset.x));
	registerProperty("lightOffsetY", prop(lightOffset.y));
}

void light::update(sf::Time elapsed)
{
	lightAnimations.update(lSpr, elapsed);

	objBasic::update(elapsed);

	// set up color
	lSpr.setColor(lightColor);
	lSpr.setOrigin(lSpr.getGlobalBounds().width / 2.f, lSpr.getGlobalBounds().height / 2.f);
	lSpr.setPosition(getCenter() - sf::Vector2f(theGame()->worldCamera->getPosition().x, theGame()->worldCamera->getPosition().y) + lightOffset);

	lSpr.setPosition(floor(lSpr.getPosition().x), floor(lSpr.getPosition().y));
}
