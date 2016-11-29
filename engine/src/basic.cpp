#include "basic.h"

#include "Game.h"
#include "platform.h"
#include "mixer.h"
#include "resources.h"
#include "view.h"
#include "gameState.h"
#include "constants.h"
#include "scriptingEngine.h"
#include "helpers.h"
#include "uiPrimitives.h"
#include "prefabs.h"
#include "sfResources.h"

// BASIC PROPERTY CLASS
// ----------------------------------------------------------------------------
basic::prop::prop() {
	typ = NA;
}

basic::prop::prop(int &intProp) {
	iRef = &intProp;
	typ = INTPROP;
}

basic::prop::prop(float &floatProp) {
	fRef = &floatProp;
	typ = FLOATPROP;
}

basic::prop::prop(std::string &stringProp) {
	sRef = &stringProp;
	typ = STRINGPROP;
}

basic::prop::prop(bool &boolProp) {
	bRef = &boolProp;
	typ = BOOLPROP;
}

basic::prop::prop(sf::Uint8& colProp) {
	cRef = &colProp;
	typ = COLPROP;
}

void basic::prop::update(int iUpdate) {
	if (typ == INTPROP) {
		// just use the int
		*iRef = iUpdate;
	}
	else if (typ == FLOATPROP) {
		// int to float
		*fRef = (float)iUpdate;
	}
	else if (typ == STRINGPROP) {
		// toString the int
		*sRef = std::to_string(iUpdate);
	}
	else if (typ == BOOLPROP) {
		*bRef = (iUpdate != 0);
	}
	else if (typ == COLPROP) {
		*cRef = ((sf::Uint8) iUpdate);
	}
}

void basic::prop::update(float fUpdate) {
	if (typ == INTPROP) {
		// float to int (values were lost that day)
		*iRef = (int)fUpdate;
	}
	else if (typ == FLOATPROP) {
		// float is float.
		*fRef = fUpdate;
	}
	else if (typ == STRINGPROP) {
		// toString the int
		*sRef = std::to_string(fUpdate);
	}
	else if (typ == BOOLPROP) {
		*bRef = (fUpdate != 0.f);
	}
	else if (typ == COLPROP) {
		*cRef = ((sf::Uint8) fUpdate);
	}
}

void basic::prop::update(std::string sUpdate) {
	if (typ == INTPROP) {
		// atoi() the string.
		*iRef = atoi(sUpdate.c_str());
	}
	else if (typ == FLOATPROP) {
		// atof() the string.
		*fRef = (float)atof(sUpdate.c_str());
	}
	else if (typ == STRINGPROP) {
		*sRef = sUpdate;
	}
	else if (typ == BOOLPROP) {
		*bRef = (sUpdate == "1");
	}
	else if (typ == COLPROP) {
		*cRef = (sf::Uint8)atoi(sUpdate.c_str());
	}
}

std::string basic::prop::read()
{
	if (typ == INTPROP) {
		// int
		return std::to_string(*iRef);
	}
	else if (typ == FLOATPROP) {
		// float
		return std::to_string(*fRef);
	}
	else if (typ == STRINGPROP) {
		// string
		return *sRef;
	}
	else if (typ == BOOLPROP) {
		return std::to_string(*bRef);
	}
	else if (typ == COLPROP) {
		return std::to_string(*cRef);
	}

	return "";
}

// textureData
// ----------------------------------------------------------------------------
basic::textureData::textureData()
{
}

basic::textureData::textureData(std::shared_ptr<sfTextureResource> spriteResource)
{
	if(spriteResource)
		resource = spriteResource;

	defaultFrameSize = true;
}

basic::textureData::textureData(std::shared_ptr<sfTextureResource> spriteResource, unsigned int frameWidth, unsigned int frameHeight)
{
	if (spriteResource)
		resource = spriteResource;

	defaultFrameSize = false;
	frameSize.x = frameWidth;
	frameSize.y = frameHeight;
}

basic::spriteData::spriteData(basic* owner, textureData &texData)
	: animations(texData.resource.lock())
{
	texture = texData;

	if (!texture.defaultFrameSize) {
		if (texture.resource.expired()) {
			p::warn("Texture resource absent from textureData with frameSize");
		}

		animations.generateFrameRects(texData.frameSize.x, texData.frameSize.y);
	}
}

// BASIC OF BASICS.
// ----------------------------------------------------------------------------
basic::basic(float x, float y, textureData sd)
{
	setPosition(sf::Vector2f(x, y));

	curSprite = -1;

	addSprite(sd);
	setCurrentSprite(0);

	gr = nullptr;
	initializedCalled = false;
	enforcePixelGrid = true;
	moved = false;

	hackRot = 0.f;
	rotCheck = 0.f;

	sfSync();
}

basic::~basic()
{
	onDestroyed.notify(this);
}

basic::basic(const basic & cp)
{
	copyFrom(cp);
}

basic & basic::operator=(basic assignedCopy)
{
	copyFrom(assignedCopy);
	return *this;
}

void basic::copyFrom(const basic & cp)
{
	sprites = cp.sprites;

	sfSprite = cp.sfSprite;
	curSprite = cp.curSprite;

	onCreated = cp.onCreated;
	onPreUpdate = cp.onPreUpdate;
	onUpdate = cp.onUpdate;
	onDestroyed = cp.onDestroyed;
	onDrawn = cp.onDrawn;
	onFirstUpdate = cp.onFirstUpdate;

	prefabSource = cp.prefabSource;
	enforcePixelGrid = cp.enforcePixelGrid;

	hackPos = cp.hackPos;
	hackOrigin = cp.hackOrigin;
	hackRot = cp.hackRot;
	hackScale = cp.hackScale;

	posCheck = cp.posCheck;
	orCheck = cp.orCheck;
	rotCheck = cp.rotCheck;
	scaleCheck = cp.scaleCheck;

	gr = nullptr;
	initializedCalled = false;
	moved = false;

	sfSync();
}

bool basic::isCloneSafe()
{
	return (typeid(*this) == typeid(*clone()));
}

void basic::registerProperties() {
	onCreated.notify(this);

	// legacy x and y
	registerProperty("x", prop(hackPos.x));
	registerProperty("y", prop(hackPos.y));
	deleteProperty("x");
	deleteProperty("y");

	// new X and Y (always at top)
	registerProperty("_X", prop(hackPos.x));
	registerProperty("_Y", prop(hackPos.y));

	registerProperty("originX", prop(hackOrigin.x));
	registerProperty("originY", prop(hackOrigin.y));
	registerProperty("scaleX", prop(hackScale.x));
	registerProperty("scaleY", prop(hackScale.y));
	registerProperty("angle", prop(hackRot));

	registerProperty("uiName", prop(uiName));
	registerProperty("enforcePixelGrid", prop(enforcePixelGrid));
}

void basic::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	onDrawn.notify(this, &target);
	applyShader(states, getTexSizeF());

	target.draw(sfSprite, states);
}

void basic::preUpdate(sf::Time elapsed) {
	moved = false;

	// messy position change tracking
	if (hackPos != posCheck) {
		posCheck = hackPos;
		setPosition(hackPos);

		moved = true;
	}

	if (getPosition() != posCheck) {
		posCheck = getPosition();
		hackPos = getPosition();

		moved = true;
	}

	// messy origin change tracking
	if (hackOrigin != orCheck) {
		orCheck = hackOrigin;
		setOrigin(hackOrigin);

		moved = true;
	}

	if (getOrigin() != orCheck) {
		orCheck = getOrigin();
		hackOrigin = getOrigin();

		moved = true;
	}

	// messy rotation change tracking
	if (hackRot != rotCheck) {
		rotCheck = hackRot;
		setRotation(hackRot);

		enforcePixelGrid = false;
	}

	if (getRotation() != rotCheck) {
		rotCheck = getRotation();
		hackRot = getRotation();
	}

	// messy scale change tracking
	if (hackScale != scaleCheck) {
		scaleCheck = hackScale;
		setScale(hackScale);

		enforcePixelGrid = false;
		moved = true;
	}

	if (getScale() != scaleCheck) {
		scaleCheck = getScale();
		hackScale = getScale();

		moved = true;
	}

	doPixelGridCorrection(false);

	// set sfSprite to the not found image if needed
	if (curSprite < 0) {
		if (theGame()->worldObjectsFrozen) {
			sfSprite = sf::Sprite(*theGame()->resources->getAs<sfTextureResource>("ui_notfound")->get().get());
		}
		else {
			sfSprite = sf::Sprite();
		}
	}
	else if (getCurrentSprite()->texture.resource.lock()->isLoaded() && spriteLoadFailed) {
		sfSprite.setTexture(*getCurrentSprite()->texture.resource.lock()->get());
		spriteLoadFailed = false;
	}

	// update transform
	sfSync();

	if (!initializedCalled && !theGame()->worldObjectsFrozen) {
		onFirstUpdate.notify(this);
		initializedCalled = true;
	}
	else if (initializedCalled && theGame()->worldObjectsFrozen) {
		initializedCalled = false; // if the game is frozen and unfrozen the function is called again
	}

	onPreUpdate.notify(this);
}

void basic::update(sf::Time elapsed) {
	onUpdate.notify(this);

	// update animations
	if (!theGame()->worldObjectsFrozen && curSprite > -1)
		getCurrentSprite()->animations.update(sfSprite, elapsed);
}

std::string basic::serialize()
{
	std::stringstream saveFile;

	auto pList = getPropertyList();
	for (unsigned int j = 0; j < pList.size(); j++) {
		if (getProperty(pList[j]).hidden) {
			continue;
		}

		saveFile << pList[j] << "=\"" << getProperty(pList[j]).read() << "\",";
	}

	return saveFile.str();
}

bool basic::deserialize(std::string data)
{
	// load properties!
	auto valueStrings = splitString(data, ',', '"');

	for (unsigned int i = 0; i < valueStrings.size(); i++) {
		auto pair = splitString(valueStrings[i], '=', '"');

		if (pair.size() != 2)
			continue;

		if (isProperty(pair[0]))
			getProperty(pair[0]).update(strFilter(pair[1], '"'));
	}

	return true;
}

unsigned int basic::addSprite(textureData texture) {
	if (texture.resource.expired())
		return -1;

	spriteData newSprite(this, texture);

	// add to list
	sprites.push_back(newSprite);

	return sprites.size() - 1;
}

int basic::getCurrentSpriteIndex() const
{
	return curSprite;
}

basic::spriteData* basic::getCurrentSprite()
{
	if (curSprite < 0) {
		return nullptr;
	}

	return &sprites[curSprite];
}

const basic::spriteData* basic::getCurrentSprite() const
{
	if (curSprite < 0) {
		return nullptr;
	}

	return &sprites[curSprite];
}

void basic::setCurrentSprite(int spr)
{
	if (sprites.size() <= spr)
		return;

	if (curSprite == spr)
		return;

	curSprite = spr;

	auto texturePointer = sprites[curSprite].texture.resource.lock();

	if (!texturePointer->isLoaded()) {
		p::warn("Object's texture was set to that of a resource that ISN'T LOADED!");
		spriteLoadFailed = true;
	}
	else {
		sfSprite.setTexture(*texturePointer->get());
		spriteLoadFailed = false;
	}
	
	sprites[curSprite].animations.setFrame(sfSprite, 0);
}

void basic::replaceCurrentSprite(textureData texture)
{
	if (texture.resource.expired()) {
		p::warn("No valid replacement texture given!");
		return;
	}

	bool ad = false;

	if (curSprite == -1) {
		setCurrentSprite(0);

		if (sprites.size() == 0) {
			sprites.push_back(spriteData(this, texture));
			ad = true;
		}
	}

	if(!ad)
		sprites[curSprite] = spriteData(this, texture);

	auto ocs = curSprite;
	curSprite = -1;

	setCurrentSprite(ocs);
}

void basic::sfSync()
{
	sfSprite.setPosition(getPosition());
	sfSprite.setRotation(getRotation());
	sfSprite.setScale(getScale());
	sfSprite.setOrigin(getOrigin());
}

void basic::doPixelGridCorrection(bool force)
{
	// force this object to stay aligned? (nullifies rotation + scale)
	if (enforcePixelGrid || force) {
		setPosition(floor(getPosition().x), floor(getPosition().y));
		setRotation(0.f); // pixel locked objects can't rotate
		setScale(1.f, 1.f); // ..or scale
	}
}

void basic::destroy()
{
	if (gr) {
		gr->detach(this);
	}
}

void basic::scheduleDestroy()
{
	theGame()->scripting->schedule(sf::seconds(0.f), std::bind(&basic::destroy, this)); // destroy this object as soon as we're not in the middle of other things
}

sf::Vector2f basic::getCenter() const {
	sf::Vector2f pos(sfSprite.getGlobalBounds().left, sfSprite.getGlobalBounds().top);

	pos.x += sfSprite.getGlobalBounds().width / 2.f;
	pos.y += sfSprite.getGlobalBounds().height / 2.f;

	return pos;
}

sf::Vector2u basic::getFrameSize() const
{
	auto cs = getCurrentSprite();

	if (cs)
		return sf::Vector2u(cs->texture.frameSize.x, cs->texture.frameSize.y);
	else
		return sf::Vector2u(sfSprite.getGlobalBounds().width, sfSprite.getGlobalBounds().height); // fallback
}

sf::Vector2f basic::getRealPosition() const
{
	return getPosition() - sf::Vector2f(getOrigin().x * getScale().x, getOrigin().y * getScale().y);
}

sf::Vector2u basic::getTexSize() const
{
	if(sfSprite.getTexture())
		return sfSprite.getTexture()->getSize();
	else {
		return sf::Vector2u(0U, 0U);
	}
}

sf::Vector2f basic::getTexSizeF() const
{
	return sf::Vector2f(getTexSize().x, getTexSize().y);
}

sf::Vector2f basic::getSize() const
{
	return sf::Vector2f(sfSprite.getGlobalBounds().width, sfSprite.getGlobalBounds().height);
}

sf::FloatRect basic::getMainBounds() const
{
	return sf::FloatRect(getRealPosition(), getSize());
}

void basic::setRealPosition(sf::Vector2f p)
{
	setPosition(p + getOrigin());
}

std::shared_ptr<basic> basic::shareThis()
{
	if (getGroup()) {
		return getGroup()->getObject(getGroup()->indexOf(this));
	}

	p::fatal("shareThis() was called on a basic that does not yet have shared ownership.");
	return nullptr;
}

void basic::registerProperty(std::string name, prop p)
{
	properties[name] = p;
}

void basic::deleteProperty(std::string name)
{
	for (auto i = properties.begin(); i != properties.end(); i++) {
		if (i->first == name) {
			//properties.erase(i);
			i->second.hidden = true; // property can still be saved but shouldn't be changable
			break;
		}
	}
}

bool basic::isProperty(std::string name)
{
	for (auto i = properties.begin(); i != properties.end(); i++) {
		if (i->first == name) {
			return true;
		}
	}

	return false;
}

basic::prop& basic::getProperty(std::string name)
{
	for (auto i = properties.begin(); i != properties.end(); i++) {
		if (i->first == name) {
			return i->second;
		}
	}

	std::string whatThereIs;

	for (auto i = properties.begin(); i != properties.end(); i++) {
		whatThereIs += i->first + ", ";
	}

	platform::fatal("getProperty: " + name + " isn't defined!\nThe following do exist: " + whatThereIs);

	// code execution never reaches this point but the compiler requires this
	int dum;
	prop* dummyProp = new prop(dum);

	return *dummyProp;
}

std::vector<std::string> basic::getPropertyList()
{
	std::vector<std::string> ls;

	for (auto i = properties.begin(); i != properties.end(); i++) {
		ls.push_back(i->first);
	}

	return ls;
}

basic::prop& basic::operator[](std::string name)
{
	return getProperty(name);
}

// BASIC WORLD OBJECT
// ----------------------------------------------------------------------------
objBasic::objBasic(float x, float y, textureData sd)
	: basic(x, y, sd)
{
	drawBottomOffset = 0;
	uiName = "";
}

void objBasic::registerProperties()
{
	basic::registerProperties();

	// reg props
	registerProperty("drawBottomOffset", prop(drawBottomOffset));
	deleteProperty("drawBottomOffset"); // drawBottomOffset is legacy

	registerProperty("_Z", prop(drawBottomOffset));
}

void objBasic::update(sf::Time elapsed)
{
	basic::update(elapsed);

	if (hasMoved() && getGroup()) {
		getGroup()->objectListDirty = true;
	}

	if (!pulledCamera.expired()) {
		auto l = pulledCamera.lock();
		l->moveTowards(*this, pulledCameraMultiplier);
	}
}

void objBasic::drawShadows(sf::RenderTarget & target) const
{
	
}

void objBasic::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	basic::draw(target, states);
}

void objBasic::drawCollisionBoxes(sf::RenderTarget & target, sf::RenderStates states) const
{
	for (unsigned int i = 0; i < collisionBoxes.size(); i++) {
		auto colBox = transformedCollisionBox(i);

		sf::RectangleShape rectPreview(sf::Vector2f(colBox.width, colBox.height));

		rectPreview.setOutlineThickness(1.f);
		rectPreview.setFillColor(sf::Color::Transparent);
		rectPreview.setOutlineColor(sf::Color::Magenta);

		rectPreview.setPosition(colBox.left, colBox.top);

		target.draw(rectPreview, states);
	}
}

void objBasic::setPositionC(sf::Vector2f newPosition)
{
	// move object so its first collision box ends up in a certain place
	if (collisionBoxes.size() > 0)
		setPosition(newPosition - sf::Vector2f(collisionBoxes[0].left, collisionBoxes[0].top));
	else
		setPosition(newPosition);
}

void objBasic::setPositionC(float x, float y)
{
	setPositionC(sf::Vector2f(x, y));
}

void objBasic::generateCollisionBox(float offL, float offR, float offT, float offB) {
	// just generates a square box the size of the bounds.
	sf::FloatRect box(0, 0, sfSprite.getGlobalBounds().width, sfSprite.getGlobalBounds().height);

	box.left += offL;
	box.width -= offL;

	box.top += offT;
	box.height -= offT;

	box.width -= offR;

	box.height -= offB;

	collisionBoxes.push_back(box);
}

void objBasic::generateCollisionBoxWithPercentages(float offL, float offR, float offT, float offB)
{
	// just generates a square box the size of the bounds.
	sf::FloatRect box(0, 0, sfSprite.getGlobalBounds().width, sfSprite.getGlobalBounds().height);

	box.left += offL * sfSprite.getGlobalBounds().width;
	box.width -= offL * sfSprite.getGlobalBounds().width;

	box.top += offT * sfSprite.getGlobalBounds().height;
	box.height -= offT * sfSprite.getGlobalBounds().height;

	box.width -= offR * sfSprite.getGlobalBounds().width;
	box.height -= offB * sfSprite.getGlobalBounds().height;

	collisionBoxes.push_back(box);
}

sf::FloatRect objBasic::transformedCollisionBox(int id) const {
	return sf::FloatRect(getPosition().x + (getScale().x *collisionBoxes[id].left),
		getPosition().y + (getScale().y * collisionBoxes[id].top),
		getScale().x * collisionBoxes[id].width, 
		getScale().y * collisionBoxes[id].height);
}

objBasic::collision objBasic::resolveMovement(float byX, float byY) {
	return theGame()->resolveMovement(this, (int)byX, (int)byY);
}

float objBasic::getDrawBottom() {
	return getMainBounds().top + getMainBounds().height + drawBottomOffset;
}

void objBasic::playAudio(std::shared_ptr<sfSoundBufferResource> sound, bool replaceSame, bool generatePitch)
{
	// use the center of the screen as the "listen" point

	// calculate vector distance without normalisation or pythag
	sf::Vector2f lp(theGame()->worldCamera->bounds().left + (theGame()->worldCamera->bounds().top / 2.f), theGame()->worldCamera->bounds().top + (theGame()->worldCamera->bounds().height / 2.f)); // find center of camera focus
	sf::Vector2f dist = getPosition() - lp;

	// play sound
	theGame()->sound->play(sound, dist, replaceSame, generatePitch);
}

void objBasic::pullCamera(std::shared_ptr<view> toPull, float multiplier)
{
	pulledCamera = toPull;
	pulledCameraMultiplier = clamp(multiplier, 0.f, 1.f);
}

void objBasic::stopPullingCamera()
{
	pulledCamera.reset();
}

// BASIC UI (user interface) OBJECT
// ----------------------------------------------------------------------------
uiBasic::uiBasic(float x, float y, textureData sd)
	: basic(x,y, sd)
{
}

void uiBasic::worldDraw(sf::RenderTarget &target, sf::RenderStates states) const {
	onWorldDraw.notify(this);
}

void uiBasic::registerProperties()
{
	basic::registerProperties();
}

void uiBasic::drawBindPrompt(sf::RenderTarget & target, sf::RenderStates states, int yPosition, std::vector<std::string> keybindNames, std::string caption) const
{
	pDrawBindPrompt(target, states, yPosition, keybindNames, caption);
}

float uiBasic::drawInfoSimple(sf::RenderTarget & target, sf::RenderStates states, sf::Vector2f screenPos, std::string info) const
{
	return pDrawInfoSimple(target, states, screenPos, info);
}

float uiBasic::drawInfo(sf::RenderTarget & target, sf::RenderStates states, sf::Vector2f screenPos, std::string info, sf::Color col, bool center, unsigned int charSize, float rot, std::shared_ptr<sfFontResource> customFont) const
{
	return pDrawInfo(target, states, screenPos, info, col, center, charSize, rot, customFont);
}

void uiBasic::drawSelf(sf::RenderTarget & target, sf::RenderStates states, sf::Vector2f screenPos, unsigned int sprite, unsigned int frame, sf::Color col) const
{
	// sanity!
	if (sprites.size() <= sprite)
		return;

	if (sprites[sprite].animations.frameRects.size() <= frame)
		return;

	// drawing can be unsafe because of the sanity checks
	const sf::Texture* tex = getTexPointer();

	sf::Sprite fakeSprite(*tex, sprites[sprite].animations.frameRects[frame]);
	fakeSprite.setPosition(screenPos);
	fakeSprite.setColor(col);

	target.draw(fakeSprite, states);
}

void uiBasic::bringToFront()
{
	theGame()->state->bringToFront(theGame()->state->localIndexOf(this));
}

bool uiBasic::isMouseOver()
{
	sf::Vector2f mousePos(theGame()->mousePos.x, theGame()->mousePos.y);
	sf::FloatRect bounds = sfSprite.getGlobalBounds();

	return bounds.contains(mousePos);
}

bool uiBasic::isOnScreen() const
{
	return true; // let's just always assume this is visible.
	// There are never normally enough UI on the screen to lag the game anyway.
	// And there never will be. Right?
	// RIGHT?
	// Yeah I can see myself rewriting this function in the near future.
}

bool compareObjects(std::shared_ptr<objBasic> i, std::shared_ptr<objBasic> j) {
	return i->operator<(*j);
}

bool compareUi(std::shared_ptr<uiBasic> i, std::shared_ptr<uiBasic> j)
{
	return i->operator<(*j);
}

// SE
DeclareScriptingCustom(user_type<basic>(), "basic");
DeclareScriptingCustom(base_class<sf::Transformable, basic>());
DeclareScriptingCustom(base_class<sf::Drawable, basic>());
DeclareScriptingCustom(base_class<serializable, basic>());
DeclareScriptingCustom(base_class<shadable, basic>());
DeclareScriptingCustom(fun(&basic::clone), "clone");
DeclareScriptingCustom(fun(&basic::getCenter), "getCenter");
DeclareScriptingCustom(fun(&basic::getProperty), "getProperty");
DeclareScriptingCustom(fun(&basic::getPropertyList), "getPropertyList");
DeclareScriptingCustom(fun<basic::spriteData*, basic>(&basic::getCurrentSprite), "getCurrentSprite");
DeclareScriptingCustom(fun(&basic::replaceCurrentSprite), "replaceCurrentSprite");
DeclareScriptingCustom(fun(&basic::setCurrentSprite), "setCurrentSprite");
DeclareScriptingCustom(fun(&basic::sprites), "sprites");
DeclareScriptingCustom(fun(&basic::sfSprite), "sfSprite");
DeclareScriptingCustom(fun(&basic::getGroup), "getGroup");
DeclareScriptingCustom(fun(&basic::scheduleDestroy), "destroy");
DeclareScriptingCustom(fun(&basic::vars), "vars");
DeclareScriptingCopyOperator(basic);

DeclareScriptingCustom(user_type<basic::textureData>(), "textureData");
DeclareScriptingCustom(constructor<basic::textureData()>(), "textureData");
DeclareScriptingCustom(constructor<basic::textureData(std::shared_ptr<sfTextureResource>)>(), "textureData");
DeclareScriptingCustom(constructor<basic::textureData(std::shared_ptr<sfTextureResource>, unsigned int, unsigned int)>(), "textureData");
DeclareScriptingCustom(fun(&basic::textureData::defaultFrameSize), "defaultFrameSize");
DeclareScriptingCustom(fun(&basic::textureData::resource), "resource");
DeclareScriptingCustom(fun(&basic::textureData::frameSize), "frameSize");
DeclareScriptingCopyOperator(basic::textureData);

DeclareScriptingCustom(user_type<basic::spriteData>(), "spriteData");
DeclareScriptingCustom(fun(&basic::spriteData::animations), "animations");
DeclareScriptingCustom(fun(&basic::spriteData::texture), "texture");
DeclareScriptingCopyOperator(basic::spriteData);

DeclareScriptingCustom(user_type<objBasic::collision>(), "collision");
DeclareScriptingCustom(constructor<objBasic::collision()>(), "collision");
DeclareScriptingCustom(constructor<objBasic::collision(const objBasic::collision&)>(), "collision");
DeclareScriptingCustom(fun(&objBasic::collision::hitX), "hitX");
DeclareScriptingCustom(fun(&objBasic::collision::hitY), "hitY");
DeclareScriptingCustom(fun(&objBasic::collision::involved), "involved");
DeclareScriptingCustom(fun(&objBasic::collision::main), "main");
DeclareScriptingCopyOperator(objBasic::collision);

DeclareScriptingBasic(objBasic);
DeclareScriptingCustom(constructor<objBasic(float, float, basic::textureData)>(), "objBasic");
DeclareScriptingCustom(fun(&objBasic::collisionBoxes), "collisionBoxes");
DeclareScriptingCustom(fun(&objBasic::displayText), "displayText");
DeclareScriptingCustom(fun(&objBasic::drawShadows), "drawShadows");
DeclareScriptingCustom(fun(&objBasic::generateCollisionBox), "generateCollisionBox");
DeclareScriptingCustom(fun(&objBasic::generateCollisionBoxWithPercentages), "generateCollisionBoxWithPercentages");
DeclareScriptingCustom(fun(&objBasic::transformedCollisionBox), "transformedCollisionBox");
DeclareScriptingCustom(fun(&objBasic::drawBottomOffset), "drawBottomOffset");
DeclareScriptingCustom(fun(&objBasic::playAudio), "playAudio");
DeclareScriptingCustom(fun(&objBasic::getPrefabSource), "getPrefabSource");
DeclareScriptingCustom(fun(&objBasic::pullCamera), "pullCamera");
DeclareScriptingCustom(fun(&objBasic::stopPullingCamera), "pullCamera");
DeclareScriptingCustom(fun<void, objBasic, float, float>(&objBasic::setPositionC), "setPositionC");
DeclareScriptingCustom(fun<void, objBasic, sf::Vector2f>(&objBasic::setPositionC), "setPositionC");
DeclareScriptingCustom(fun(&objBasic::uiName), "uiName");

DeclareScriptingBasic(uiBasic);
DeclareScriptingCustom(constructor<uiBasic(float, float, basic::textureData)>(), "uiBasic");
DeclareScriptingCustom(fun(&uiBasic::drawBindPrompt), "drawBindPrompt");
DeclareScriptingCustom(fun(&uiBasic::drawInfoSimple), "drawInfoSimple");
DeclareScriptingCustom(fun(&uiBasic::drawInfo), "drawInfo");