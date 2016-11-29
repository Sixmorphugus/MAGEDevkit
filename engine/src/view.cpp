#include "view.h"

#include "sfResources.h"
#include "helpers.h"
#include "gameState.h"

view::view()
{
	setDefaults();
}

view::view(sf::Vector2f size, std::shared_ptr<groupBase> gr)
{
	setSize(size);
	resizeInternalRT(sf::Vector2u((unsigned int)getSize().x, (unsigned int)getSize().y));

	setDefaults();
	group = gr;
}

void view::render(sf::RenderTarget & target, sf::Color bgCol)
{
	// respect the pixel grid
	if (respectPixelGrid) {
		setPosition(sf::Vector2f(floor(getPosition().x), floor(getPosition().y)));

		// the size must be the same ratio as the viewport
		// (todo)
	}

	// respect the bounds rectangle unless the camera is too big for it
	if (getSize().x < cameraLimits.width && getSize().y < cameraLimits.height) {
		// find how far out the camera is
		auto camRect = bounds();

		if (camRect.left < cameraLimits.left) {
			float over = camRect.left - cameraLimits.left;
			move(-over, 0.f);
		}

		if (camRect.top < cameraLimits.top) {
			float over = camRect.top - cameraLimits.top;
			move(0.f, -over);
		}

		if (camRect.left + camRect.width > cameraLimits.left + cameraLimits.width) {
			float over = (camRect.left + camRect.width) - (cameraLimits.left + cameraLimits.width);
			move(-over, 0.f);
		}

		if (camRect.top + camRect.height > cameraLimits.top + cameraLimits.height) {
			float over = (camRect.top + camRect.height) - (cameraLimits.top + cameraLimits.height);
			move(0.f, -over);
		}
	}

	sf::View windowView = sf::View(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(target.getSize().x, target.getSize().y)));
	target.setView(windowView);

	resizeInternalRT(target.getSize());

	// we're drawing on the internal texture
	internalRT.setView(*this); // use this view

	if (bgCol != sf::Color::Transparent)
		internalRT.clear(bgCol); // this'll only actually happen in the viewport (i think...)
	
	onRender.notify(this);

	// the base view class doesn't actually draw anything.
	// we could have designed it so it drew EVERYTHING, but then:
	// - everything would be on the same layer
	// - ui would only be visible at the top left of the map if we didn't use multiple views
	// - viewports wouldn't work properly if we did that
	// so you have to use uiView or worldView if you want it to do something.
}

void view::resizeInternalRT(sf::Vector2u siz)
{
	if (internalRT.getSize() != siz) {
		internalRT.create(siz.x, siz.y);
	}

	internalRT.clear(sf::Color::Transparent);
}

sf::Vector2f view::getHalfSize()
{
	return sf::Vector2f(getSize().x / 2.f, getSize().y / 2.f);
}

sf::Vector2f view::getPosition()
{
	return sf::Vector2f(getCenter().x - getHalfSize().x, getCenter().y - getHalfSize().y);
}

void view::setPosition(sf::Vector2f pos)
{
	setCenter(pos.x + getHalfSize().x, pos.y + getHalfSize().y);
}

sf::FloatRect view::bounds()
{
	return sf::FloatRect(getPosition(), getSize());
}

void view::setBaseSize(sf::Vector2f s)
{
	float zl = getZoomLevel();
	zoomZero = s;

	setZoomLevel(zl);
}

void view::setViewportBoundsIn(sf::RenderTarget & target, sf::FloatRect bounds)
{
	auto s = target.getSize();

	// calculate the percentage
	sf::FloatRect boundsPrct(bounds.left / s.x, bounds.top / s.y, bounds.width / s.x, bounds.height / s.y);
	setViewport(boundsPrct);
}

sf::FloatRect view::getViewportBoundsIn(sf::RenderTarget & target)
{
	auto s = target.getSize();
	auto vp = getViewport();

	return sf::FloatRect(vp.left * s.x, vp.top * s.y, vp.width * s.x, vp.height * s.y);
}

void view::setZoomLevel(float zl)
{
	/*
	p::info("Zoom level is now " + std::to_string(zl));
	p::info("X " + std::to_string(zoomZero.x) + " to " + std::to_string(zoomZero.x * zl));
	*/
	setSize(zoomZero.x * zl, zoomZero.y * zl);
}

float view::getZoomLevel()
{
	/*
	p::info("Sx: " + std::to_string(getSize().x));
	p::info("Zx: " + std::to_string(zoomZero.x));
	p::info("R: " + std::to_string(getSize().x / zoomZero.x));
	*/
	return (getSize().x / zoomZero.x); // note: only uses x
}

float view::pythagorasWidth()
{
	return sqrtf((getSize().x*getSize().x) + (getSize().y*getSize().y));
}

sf::Vector2f view::relativePosition(sf::Vector2f mp)
{
	return sf::Vector2f(getPosition() + (mp * getZoomLevel()));
}

void view::setDefaults()
{
	zoomZero = getSize();

	setPosition(sf::Vector2f(0, 0));

	shader = nullptr;
	respectPixelGrid = true;

	setZoomLevel(ZOOMDEFAULT);
}

void view::moveTowards(basic & target, float multiplier)
{
	auto ourPos = getCenter();
	auto tPos = target.getCenter();

	auto dist = tPos - ourPos;

	move(dist * multiplier);
}

// worldView
worldView::worldView()
{
	useStateBounds = true;
}

worldView::worldView(sf::Vector2f size, std::shared_ptr<groupBase> gr)
	: view(size, gr)
{
	useStateBounds = true;
}

void worldView::render(sf::RenderTarget & target, sf::Color bgCol)
{
	// if the group we're viewing is a gameState, we can use its bounds (though this can be overidden)
	if (useStateBounds) {
		auto gs = std::dynamic_pointer_cast<gameState>(group.lock());

		if (gs) {
			cameraLimits = gs->mapBounds;
		}
	}

	view::render(target, bgCol); // this is called here so the pixel grid respect still works after moving the camera

	// draw all the objects in the group
	sf::RenderStates rs;
	applyShader(rs, sf::Vector2f(internalRT.getSize().x, internalRT.getSize().y));

	if (!group.expired()) {
		auto groupLck = group.lock();

		groupLck->drawWorldObjects(internalRT, sf::RenderStates());

		internalRT.display();

		sf::Sprite irtSpr((internalRT.getTexture()));
		target.draw(irtSpr, rs);
	}
}

// uiView
uiView::uiView()
{
}

uiView::uiView(sf::Vector2f size, std::shared_ptr<groupBase> gr)
	: view(size, gr)
{
}

void uiView::render(sf::RenderTarget & target, sf::Color bgCol)
{
	view::render(target, bgCol);

	// draw all the objects in the group
	sf::RenderStates rs;
	applyShader(rs, sf::Vector2f(internalRT.getSize().x, internalRT.getSize().y));

	if (!group.expired()) {
		auto groupLck = group.lock();

		groupLck->drawUiObjects(internalRT, sf::RenderStates());

		internalRT.display();

		sf::Sprite irtSpr(internalRT.getTexture());

		target.draw(irtSpr, rs);
	}
}

// SE Bind
DeclareScriptingType(view);
DeclareScriptingBaseClass(sf::View, view);
DeclareScriptingBaseClass(shadable, worldView);
DeclareScriptingFunction(&view::bounds, "bounds");
DeclareScriptingFunction(&view::getHalfSize, "getHalfSize");
DeclareScriptingFunction(&view::getViewportBoundsIn, "getViewportBoundsIn");
DeclareScriptingFunction(&view::getZoomLevel, "getZoomLevel");
DeclareScriptingFunction(&view::getPosition, "getPosition");
DeclareScriptingFunction(&view::pythagorasWidth, "pythagorasWidth");
DeclareScriptingFunction(&view::render, "render");
DeclareScriptingFunction(&view::respectPixelGrid, "respectPixelGrid");
DeclareScriptingFunction(&view::setViewportBoundsIn, "setViewportBoundsIn");
DeclareScriptingFunction(&view::setZoomLevel, "setZoomLevel");
DeclareScriptingFunction(&view::setPosition, "setPosition");
DeclareScriptingFunction(&view::shader, "shader");
DeclareScriptingFunction(&view::group, "group");
DeclareScriptingFunction(&view::zoomZero, "zoomZero");

DeclareScriptingType(worldView);
DeclareScriptingBaseClass(sf::View, worldView);
DeclareScriptingBaseClass(shadable, worldView);
DeclareScriptingBaseClass(view, worldView);
DeclareScriptingConstructor(worldView(), "worldView");
DeclareScriptingConstructor(worldView(sf::Vector2f, std::shared_ptr<groupBase>), "worldView");
DeclareScriptingFunction(&worldView::useStateBounds, "useStateBounds");

DeclareScriptingType(uiView);
DeclareScriptingBaseClass(sf::View, uiView);
DeclareScriptingBaseClass(shadable, uiView);
DeclareScriptingBaseClass(view, uiView);
DeclareScriptingConstructor(uiView(), "uiView");
DeclareScriptingConstructor(uiView(sf::Vector2f, std::shared_ptr<groupBase>), "uiView");

DeclareScriptingHook("viewRenderHook", view*);