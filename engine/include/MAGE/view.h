#pragma once;

// A view draws the entire part of the group it is assigned to that will fit onto the screen.
// it wraps sf::View to do this.
// It will do it with a transparent background unless you tell it otherwise
// It can be told to focus on a specific object and will stop trying to focus on this object when it stops existing.
// It can have a shader applied to everything it draws (not working yet) as an entire-view feature.

// The game has 2 it renders with by default - one for UI and one for the world.
// Both of these are autocreated and configured

// You can make your own view - just render it manually in onGameDraw or something

// unlike most objects cameras don't update independently of rendering - instead, their attributes are managed before every render

#include "group.h"
#include "shaders.h"

class MAGEDLL view : public sf::View, public shadable {
public:
	view();
	view(sf::Vector2f size, std::shared_ptr<groupBase> gr = nullptr);

	virtual void render(sf::RenderTarget& target, sf::Color bgCol = sf::Color::Transparent);

	sf::Vector2f getHalfSize();

	sf::Vector2f getPosition();
	void setPosition(sf::Vector2f pos); // when setCenter isn't enough for you

	void moveTowards(basic& target, float multiplier);

	sf::FloatRect bounds(); // find the boundary the view covers

	void setBaseSize(sf::Vector2f s);

	void setViewportBoundsIn(sf::RenderTarget& target, sf::FloatRect bounds); // set the position of the viewport in pixels rather than via percentages
	sf::FloatRect getViewportBoundsIn(sf::RenderTarget& target);

	void setZoomLevel(float zl);
	float getZoomLevel();

	float pythagorasWidth(); // lol

	sf::Vector2f relativePosition(sf::Vector2f mp);

private:
	void setDefaults();

protected:
	sf::RenderTexture internalRT;
	void resizeInternalRT(sf::Vector2u siz);

public:
	// things that affect rendering
	std::weak_ptr<groupBase> group;
	std::shared_ptr<sfShaderResource> shader;

	sf::Vector2f zoomZero;
	bool respectPixelGrid;
	sf::FloatRect cameraLimits;

	hook<view*> onRender;
};

// worldView can see the world objects in a group
class MAGEDLL worldView : public view {
public:
	worldView();
	worldView(sf::Vector2f size, std::shared_ptr<groupBase> gr = nullptr);

	void render(sf::RenderTarget& target, sf::Color bgCol = sf::Color::Transparent);
public:
	bool useStateBounds;
};

// uiView can see the ui objects in a group
class MAGEDLL uiView : public view {
public:
	uiView();
	uiView(sf::Vector2f size, std::shared_ptr<groupBase> gr = nullptr);

	void render(sf::RenderTarget& target, sf::Color bgCol = sf::Color::Transparent);
};