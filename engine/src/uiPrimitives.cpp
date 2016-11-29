#include "uiPrimitives.h"

#include "Game.h"
#include "platform.h"
#include "helpers.h"
#include "mixer.h"
#include "resources.h"
#include "view.h"
#include "io.h"

#include "sfResources.h"

// universal draw functions
void pDrawBindPrompt(sf::RenderTarget & target, sf::RenderStates states, int yPosition, std::vector<std::string> keybindNames, std::string caption)
{
	// find the needed textures
	std::vector<std::shared_ptr<sfTextureResource>> textures;

	for (unsigned int i = 0; i < keybindNames.size(); i++) {
		std::vector<std::shared_ptr<sfTextureResource>> bindTextures = theGame()->getKeybindTextures(keybindNames[i]);

		for (unsigned int j = 0; j < bindTextures.size(); j++) {
			textures.push_back(bindTextures[j]);
		}
	}

	// calculate x position
	int xPosition = (int)(theGame()->uiCamera->getSize().x) - 64;

	// draw text
	pDrawInfoSimple(target, states, sf::Vector2f((float)xPosition, (float)yPosition), caption);

	// draw keybind sprites in reverse order
	for (int i = (int)textures.size() - 1; i >= 0; i--) {
		sf::Sprite kbSprite(*textures[i]->get());
		kbSprite.setPosition((float)xPosition - kbSprite.getGlobalBounds().width - 8.f, (float)yPosition - floor(kbSprite.getGlobalBounds().height / 2.f));

		target.draw(kbSprite, states);

		xPosition -= (int)floor(kbSprite.getGlobalBounds().width) - 4;
	}
}

float pDrawInfoSimple(sf::RenderTarget & target, sf::RenderStates states, sf::Vector2f screenPos, std::string info)
{
	return pDrawInfo(target, states, screenPos, info, sf::Color::White);
}

float pDrawInfo(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f screenPos, std::string info, sf::Color col, bool center, unsigned int charSize, float rot, std::shared_ptr<sfFontResource> customFont) {
	std::shared_ptr<sfFontResource> defaultFont = theGame()->resources->getAs<sfFontResource>("__fontDef");

	if (!defaultFont)
		p::fatal("Font does not exist");

	if (customFont) {
		defaultFont = customFont;
	}

	// secretly round screenPos
	screenPos.x = floor(screenPos.x);
	screenPos.y = floor(screenPos.y);

	// create a message in the default font
	sf::Text shadow;
	shadow.setString(info);
	shadow.setScale(.5f, .5f); // (hotfix)
	shadow.setFont(*defaultFont->get());
	shadow.setCharacterSize(charSize);
	shadow.setPosition((sf::Vector2f)screenPos);
	shadow.setFillColor(sf::Color(50, 50, 80, col.a));
	shadow.setRotation(rot);

	sf::Text main;
	main.setString(info);
	main.setScale(.5f, .5f);
	main.setFont(*defaultFont->get());
	main.setCharacterSize(charSize);
	main.setPosition((sf::Vector2f)screenPos - sf::Vector2f(1.f, 1.f));
	main.setFillColor(col);
	main.setRotation(rot);

	if (center) {
		sf::Vector2f halfSize(main.getGlobalBounds().width / 2, main.getGlobalBounds().height / 2);

		main.move((sf::Vector2f) - halfSize);
		shadow.move((sf::Vector2f) - halfSize);
	}

	target.draw(shadow, states);
	target.draw(main, states);

	return floor(main.getGlobalBounds().width);
}

// loading screen.
loadingScreen::loadingScreen()
{
}

void loadingScreen::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	if (!canDraw()) return;

	target.clear(sf::Color(0, 0, 10));
	auto screenCenter = theGame()->uiCamera->getSize() / 2.f;

	// info
	pDrawInfo(target, states, screenCenter - sf::Vector2f(0.f, -(screenCenter.y - 64.f)), loading, sf::Color(255, 255, 200), true, 16U, 0.f);

	// loading bar
	float barLength = theGame()->uiCamera->getSize().x - 128.f;
	float barHt = 2.f;

	// back
	sf::RectangleShape bar;
	bar.setFillColor(sf::Color(50, 50, 50));
	bar.setSize(sf::Vector2f(barLength, barHt));
	bar.setPosition(screenCenter - sf::Vector2f(barLength / 2.f, -(screenCenter.y - 40.f)));

	target.draw(bar, states);

	// front
	bar.setFillColor(sf::Color(255, 255, 255));
	bar.setSize(sf::Vector2f((barLength / (float)countMax) * (float)(count + 1), barHt));

	target.draw(bar, states);
}

bool loadingScreen::canDraw() const {
	auto fd = theGame()->resources->get("__fontDef", false);

	if (fd) {
		return fd->isLoaded();
	}

	return false;
}

// boxWindow.
boxWindow::boxWindow(float x, float y) :
	color(20, 20, 50),
	size(0.f, 0.f),
	boxSize(16.f, 16.f),
	prct(0),
	noShadow(false),
	uiBasic(x, y, textureData(nullptr))
{
	setup();
}

boxWindow::boxWindow(float x, float y, sf::Color col, sf::Vector2f siz, sf::Vector2f boxSiz) :
	color(col),
	size(siz),
	boxSize(boxSiz),
	prct(0),
	noShadow(false),
	uiBasic(x, y, textureData(nullptr))
{
	setup();
}

void boxWindow::setup() {
	rectangles.clear();

	if (size.x < 0 || size.y < 0) {
		platform::fatal("vertex error: boxWindow sizes cannot be negative");
	}

	// resize rectangle array
	unsigned int rectsX = (unsigned int)ceil(size.x);
	unsigned int rectsY = (unsigned int)ceil(size.y);

	rectangles.resize(rectsX);

	for (unsigned int x = 0; x < rectsX; x++) {
		for (unsigned int y = 0; y < rectsY; y++) {
			sf::RectangleShape shape(boxSize);
			shape.setFillColor(color);
			shape.setOrigin(boxSize.x / 2, boxSize.y / 2);
			shape.setPosition(getPosition() - getOrigin() + sf::Vector2f(x * boxSize.x, y * boxSize.y));

			rectangles[x].push_back(shape);
		}
	}

	shadow.setSize(sf::Vector2f(size.x * boxSize.x, size.y * boxSize.y));
	shadow.setPosition(getPosition() - getOrigin());
	shadow.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)floor(100 * prct)));
}

void boxWindow::update(sf::Time elapsed)
{
	// we don't NEED uiBasic's functionality here, so why update it?
	// updating would update:
	// - the sprite positioning
	// - the sprite animation manager
	// neither of which we're using or even want around.

	float percent = prct; // new percent

	if (percent > 1.0f)
		percent = 1.0f;
	if (percent < 0.0f)
		percent = 0.0f;

	prct = percent;

	percent *= 2;
	percent -= 1.f;

	for (unsigned int x = 0; x < rectangles.size(); x++) {
		for (unsigned int y = 0; y < rectangles[x].size(); y++) {
			// we want the top left rectangles to appear first, then for it to ease in as we get to the bottom left.
			float xProg = floor(rectangles.size() * percent) - x;
			float yProg = floor(rectangles.size() * percent) - y;

			if (xProg > 0)
				xProg = 0;

			if (yProg > 0)
				yProg = 0;

			if (xProg < -boxSize.x)
				xProg = -boxSize.x;

			if (yProg < -boxSize.y)
				yProg = -boxSize.y;
			
			float hProg;
			if (yProg > xProg)
				hProg = yProg;
			else
				hProg = xProg;

			rectangles[x][y].setPosition(getPosition() - getOrigin() + sf::Vector2f(x * boxSize.x, y * boxSize.y) - sf::Vector2f(hProg / 2, hProg / 2));
			rectangles[x][y].setSize(boxSize + sf::Vector2f(hProg, hProg));
		}
	}

	shadow.setPosition(getPosition() - getOrigin() - sf::Vector2f(2, 2));
	shadow.setFillColor(sf::Color(0, 0, 0, (sf::Uint8)floor(100 * prct)));
}

void boxWindow::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	// all calling uiBasic::draw would do is spend program time realizing we don't have a sprite and drawing nothing.
	// so we don't bother.

	//if(!noShadow)
		//target.draw(shadow, states);

	for (unsigned int x = 0; x < rectangles.size(); x++) {
		for (unsigned int y = 0; y < rectangles[x].size(); y++) {
			target.draw(rectangles[x][y], states);
		}
	}
}

bool boxWindow::fullyVisible() {
	if (prct >= 1.0f)
		return true;
	else
		return false;
}

int boxWindow::contentOpacity() {
	float opPercent = prct - 0.8f;

	if (opPercent < 0) {
		opPercent = 0;
	}

	opPercent *= 5;

	return (int)floor(255 * opPercent);
}

// menu.
menu::menu(float x, float y) :
	uiBasic(x, y, textureData(nullptr))
{
	init();
}

menu::menu(float x, float y, std::vector<std::string> opList) :
	uiBasic(x, y, textureData(nullptr)),
	optionList(opList)
{
	init();
}

void menu::update(sf::Time elapsed) {
	// mouse control (gotta do this regardless of disabledness)
	int considered = 0;
	mouseOver = false;

	selectionChanged = false;

	for (unsigned int i = optionStart; i < optionList.size(); i++) {
		// mouse collision box for this option
		sf::FloatRect mouseBox(sf::Vector2f(getPosition().x + 14, getPosition().y + (i * 12) - (optionStart * 12)), sf::Vector2f(width, 12.f));

		if (mouseBox.contains((sf::Vector2f)theGame()->mousePos) && optionList[i] != "") {
			mouseOver = true;

			if (!disabled) {
				selected = i;

				if (theGame()->mouse->getLeftPressed()) {
					ready = true;
					superSelected = selected;
					selectionChanged = true;
					onSelectionChanged.notify(this, superSelected);
				}
			}
		}

		considered++;

		if (considered >= maxOptions) {
			break;
		}
	}

	if (disabled || optionList.size() == 0)
		return;

	float selectorDist = selectorPos - (getPosition().y + (selected * 12) - 3.f - (optionStart * 12));
	selectorPos = selectorPos - (selectorDist / 2);

	if (!ready) {
		selectorDist = selectorPos2 - (getPosition().y + (selected * 12) - 3.f - (optionStart * 12));
		selectorPos2 = selectorPos - (selectorDist / 2);
	}
	else {
		selectorDist = selectorPos2 - (getPosition().y + (superSelected * 12) - 3.f - (optionStart * 12));
		selectorPos2 = selectorPos2 - (selectorDist / 2);
	}

	// don't bother updating uiBasic (see boxWindow)

	bool autoScroll = false;

	// keyboard control
	if (!noKbd) {
		if (theGame()->keyboard->getBind("Up")->getPressed()) {
			selected--;
			validateSelected();
			skipDown = false;
			autoScroll = true;
		}
		else if (theGame()->keyboard->getBind("Down")->getPressed()) {
			selected++;
			validateSelected();
			skipDown = true;
			autoScroll = true;
		}
		else if (theGame()->keyboard->getBind("Select")->getPressed()) {
			ready = true;
			superSelected = selected;
			selectionChanged = true;
			onSelectionChanged.notify(this, superSelected);
		}
		else if (theGame()->keyboard->getBind("ESCAPE")->getPressed()) {
			dismissed = true;
			onDismissed.notify(this);
		}
	}

	if (!mouseOver && noKbd && ready) {
		selected = superSelected;
	}

	// auto scroll if we go beyond the allowed range
	if (!dragMode) {
		if (selected >= optionStart + maxOptions) {
			optionStart += 1;
		}
		else if (selected < optionStart) {
			optionStart -= 1;
		}
	}

	auto highestScrollAllowed = (int)optionList.size() - maxOptions;

	if (mouseOver) {
		optionStart += theGame()->mouse->scroll;

		if (optionStart < 0 || (int)optionList.size() < maxOptions) {
			optionStart = 0;
		}
		else if (optionStart >= highestScrollAllowed) {
			optionStart = highestScrollAllowed;
		}
	}

	// skip gaps
	selected %= optionList.size(); // hotfix

	if (optionList[selected] == "") {
		if (skipDown) {
			selected++;
			selected %= optionList.size();
		}
		else {
			selected--;
			selected %= optionList.size();
		}
	}

	if (selectionChanged) {
		theGame()->sound->play(theGame()->resources->getAs<sfSoundBufferResource>("sfx_menuchoose"));
	}

	// scroll can be manipulated without the mouse now
	float height = 12.f * (maxOptions);
	float barHeight = (height / optionList.size()) *  (maxOptions);

	float offset = (height / optionList.size()) * optionStart;

	sf::FloatRect barBox(sf::Vector2f(getPosition().x + width + 16.f, getPosition().y - 2.f), sf::Vector2f(2.f, height));
	sf::FloatRect scrollerBox(sf::Vector2f(getPosition().x + width + 15.f, getPosition().y - 2.f + offset), sf::Vector2f(4.f, (barHeight)));

	bool culled = (((int)optionList.size()) - 1 > maxOptions);

	if (culled) { // moving dat scrollbar with dat mouse
		scrollBarGlow = (scrollerBox.contains(theGame()->mousePos));

		if (theGame()->mouse->getLeftPressed() && scrollBarGlow) {
			scrollDragOffset = theGame()->mousePos - sf::Vector2f(scrollerBox.left, scrollerBox.top);
			dragMode = true;
		}

		if (dragMode) {
			sf::Vector2f nsdo = theGame()->mousePos - (sf::Vector2f(scrollerBox.left, scrollerBox.top) + scrollDragOffset);
			float requiredDist = height / optionList.size();

			if (nsdo.y > requiredDist  && optionStart < highestScrollAllowed) {
				optionStart += 1;
			}
			else if (nsdo.y < -requiredDist && optionStart > 0) {
				optionStart -= 1;
			}
		}

		if (theGame()->mouse->getLeftReleased() && dragMode) {
			dragMode = false;
		}
	}
}

void menu::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	// don't bother drawing uiBasic (see boxWindow)

	if (!disabled && !noKbd) {
		if (theGame()->mouseMode != 0) {
			drawBindPrompt(target, states, (int)(theGame()->uiCamera->getSize().y) - 32 - (24 * 2), { "Up", "Down" }, "Select");
			drawBindPrompt(target, states, (int)(theGame()->uiCamera->getSize().y) - 32 - 24, { "Select" }, "Accept");
		}

		drawBindPrompt(target, states, (int)(theGame()->uiCamera->getSize().y) - 32, { "ESCAPE" }, "Back");
	}

	// draw a box
	if (!disabled && selected != superSelected && selectorPos > getPosition().y - 4.f && selectorPos < getPosition().y + (maxOptions * 12.f)) {
		sf::RectangleShape rectBox(sf::Vector2f(width, 12.f));
		rectBox.setPosition(getPosition().x + 14, selectorPos);
		rectBox.setFillColor(sf::Color(50, 50, 80, 100));

		target.draw(rectBox, states);
	
		// draw selector
		if(!ready)
			drawInfo(target, states, sf::Vector2f(getPosition().x, selectorPos + 2.f), selectorText, sf::Color(255, 255, 200));
	}

	if (ready && !disabled && selectorPos2 > getPosition().y - 4.f && selectorPos2 < getPosition().y + (maxOptions * 12.f)) {
		sf::RectangleShape rectBox(sf::Vector2f(width, 12.f));
		rectBox.setPosition(getPosition().x + 14, selectorPos2);
		rectBox.setFillColor(sf::Color(50, 50, 80, 200));

		target.draw(rectBox, states);

		drawInfo(target, states, sf::Vector2f(getPosition().x, selectorPos2 + 2.f), selectorText, sf::Color(255, 255, 200));
	}

	// draw our options (todo: actually bother with the alignment setting, it's kinda important)
	int rendered = 0;
	bool culled = (((int)optionList.size())-1 > maxOptions);

	for (unsigned int i = optionStart; i < optionList.size(); i++) {
		std::string opString = optionList[i];
		//drawInfo(target, states, sf::Vector2f(getPosition().x + 16.f, getPosition().y + (i * 12.f) - (optionStart * 12.f)), opString, sf::Color(255, 255, 150));
		// no longer will we draw info!
		// now we use magicText here.

		magicText temp(getPosition().x + 16.f, getPosition().y + (i * 12.f) - (optionStart * 12.f), "[c:ffff96]" + opString);
		temp.draw(target, states);

		rendered++;

		if (rendered >= maxOptions) {
			//culled = true;
			break;
		}
	}

	if (culled) {
		// draw scroll bar along the side of the list
		float height = 12.f * (maxOptions);
		float barHeight = (height / optionList.size()) *  (maxOptions);

		float offset = (height / optionList.size()) * optionStart;

		sf::RectangleShape rectBox(sf::Vector2f(2.f, height));
		rectBox.setPosition(getPosition().x + width + 16.f, getPosition().y - 2.f);
		rectBox.setFillColor(sf::Color(50, 50, 80, 100));

		sf::RectangleShape rectBox2(sf::Vector2f(4.f, (barHeight)));
		rectBox2.setPosition(getPosition().x + width + 15.f, getPosition().y - 2.f + offset);
		rectBox2.setFillColor(sf::Color(50, 50, scrollBarGlow ? 160 : 80, 255));

		target.draw(rectBox, states);
		target.draw(rectBox2, states);
	}
}

void menu::validateSelected() {
	if (selected < 0) {
		selected = 0;
	}
	else if (selected > ((int)optionList.size()) - 1) {
		selected = optionList.size() - 1;
	}
	else {
		theGame()->sound->play(theGame()->resources->getAs<sfSoundBufferResource>("sfx_menuselect"));
	}
}

void menu::init()
{
	selected = 0;
	superSelected = -1;
	disabled = false;
	noKbd = false;

	alignMode = menu::LEFT;
	skipDown = true;
	width = 128.f;

	ready = false;
	dismissed = false;

	selectorPos = getPosition().y + (selected * 12) - 2;
	selectorPos2 = 0;
	selectorText = "@";
	maxOptions = 8;

	optionStart = 0;

	scrollBarGlow = false;
	dragMode = false;
}

/*
void menu::setSelectorPosition(int op)
{
	selected = op;

	optionStart = op;
	auto highestScrollAllowed = (int)optionList.size() - maxOptions;

	if (optionStart > highestScrollAllowed) {
		optionStart = highestScrollAllowed;
	}
}
*/

void menu::reset()
{
	ready = false;
	superSelected = -1;
	optionStart = 0;
	selected = 0;
}

// textEntry
textEntry::textEntry(float x, float y)
: uiBasic(x, y, textureData(nullptr))
{
	active = true;
	ready = false;
	width = 128.f;
	mouseSensitive = true;
	safe = false;
}

void textEntry::update(sf::Time elapsed)
{
	displayFrom = 0;

	sf::Text benchmarkObject(contains, *theGame()->resources->getAs<sfFontResource>("__fontDef")->get(), 16U);
	float spaceUsed = benchmarkObject.getGlobalBounds().width;
	float charSpace = spaceUsed / (float)contains.size();

	if (spaceUsed > width) {
		float dif = spaceUsed - width;
		displayFrom = (unsigned int)floor(dif / charSpace);
	}

	if (active) {
		if (theGame()->keyboard->inputStream == "\b") {
			if(contains.size() > 0)
				contains.erase(contains.begin() + (contains.size() - 1));
		}
		else {
			if (safe) {
				if (isalnum(theGame()->keyboard->inputStream[0]) || theGame()->keyboard->inputStream[0] == ' ')
					contains += theGame()->keyboard->inputStream;
			}
			else {
				if (isprint(theGame()->keyboard->inputStream[0]) && theGame()->keyboard->inputStream[0] != '"')
					contains += theGame()->keyboard->inputStream;
			}
		}
	}

	if (mouseSensitive) {
		sf::FloatRect msRect(getPosition(), sf::Vector2f(width, 12.f));

		if (msRect.contains(theGame()->mousePos)) {
			if (theGame()->mouse->getLeftPressed()) {
				active = true;
			}
		}
		else {
			if (theGame()->mouse->getLeftPressed()) {
				active = false;
			}
		}
	}

	if (theGame()->keyboard->getBind("ENTER")->getPressed() && active) {
		ready = true;
		active = false;
	}
}

void textEntry::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	std::string drawText = strSub(contains, displayFrom, drawText.size());

	if (active) {
		drawText += "|";
	}

	drawInfo(target, states, getPosition() - sf::Vector2f(0, 4.f), drawText, sf::Color(255, 255, 100));
}

// boxMenu.
boxMenu::boxMenu(float x, float y)
	: boxWindow(x, y), menu(x, y)
{
	alignMode = boxMenu::LEFT;
}

boxMenu::boxMenu(float x, float y, std::vector<std::string> opList, sf::Color col, sf::Vector2f siz, sf::Vector2f boxSiz)
	: boxWindow(x, y, col, siz, boxSiz), menu(x, y, opList)
{
	alignMode = boxMenu::LEFT;

	width = siz.x * boxSiz.x - 32;
	maxOptions = (int)floor(siz.y);
}

void boxMenu::update(sf::Time elapsed) {
	prct += 0.05f;

	boxWindow::update(elapsed);
	menu::update(elapsed);
}

void boxMenu::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	// the beauty of polymorhpism.
	boxWindow::draw(target, states);
	menu::draw(target, states);
}

boxTextEntry::boxTextEntry(float x, float y)
	: boxWindow(x, y), textEntry(x, y)
{
}

boxTextEntry::boxTextEntry(float x, float y, sf::Color col, sf::Vector2f siz, sf::Vector2f boxSiz)
	: boxWindow(x, y, col, siz, boxSiz), textEntry(x, y)
{
}

void boxTextEntry::update(sf::Time elapsed)
{
	width = size.x * boxSize.x;
	prct += 0.05f;

	boxWindow::update(elapsed);
	textEntry::update(elapsed);
}

void boxTextEntry::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	boxWindow::draw(target, states);
	textEntry::draw(target, states);
}

// magicText.
magicText::magicText(float x, float y, std::string textIn)
	: uiBasic(x, y, textureData(nullptr))
{
	text = textIn;
	lineLimit = -1;
	opacity = 255;
	alignment = LEFT;
	lineSpacing = 18.f;

	defaultFont = theGame()->resources->getAs<sfFontResource>("__fontDef");
	defaultAnimationMode = NONE;
	defaultColorMode = SOLID;
	defaultColor = sf::Color::White;
}

void magicText::update(sf::Time elapsed)
{
	// ...
}

void magicText::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	int cOffset = 0;
	int lOffset = 0;
	unsigned int size = 16;

	animationMode curAnimMode = defaultAnimationMode;
	colorMode curColorMode = defaultColorMode;
	alignmentMode curAlignment = alignment;
	sf::Color curColor = defaultColor;

	curColor.a = opacity;

	std::shared_ptr<sfFontResource> f = defaultFont;

	std::vector<std::shared_ptr<sfTextureResource>> imagesToDraw;

	for (unsigned int i = 0; i < text.size(); i++) {
		int xOffset = 0;
		int yOffset = 0;
		int sizeOffset = 0;
		float rot = 0.f;

		sf::Color colorOffset = sf::Color::Transparent;

		std::string charString = { text[i] };

		float halfTextLength = findSizeX() / 2.f;

		// control string?
		if (charString == "[") {
			std::string commandString;
			for (unsigned int j = i+1; j < text.size(); j++) {
				std::string charString2 = { text[j] };

				if (charString2 != "]")
					commandString += charString2;
				else {
					i = j;
					break;
				}
			}

			// read the string code
			std::vector<std::string> codeSplit = splitString(commandString, ':');

			if (codeSplit.size() == 2) {
				std::string op = codeSplit[0];
				std::string per = codeSplit[1];

				if (op == "c" || op == "color") { // hex color code/color mode
					if (per == "solid")
						curColorMode = SOLID;
					else if (per == "blink")
						curColorMode = BLINK;
					else if (per == "rainbow")
						curColorMode = RAINBOW;
					else if (per == "clear" || per == "cl") {
						curColorMode = SOLID;
						curColor = sf::Color::White;
					}
					else if (per.size() == 6) {
						// convert hex code to color
						unsigned int hexValue = std::stoul(per, nullptr, 16);

						sf::Color rgbColor;

						rgbColor.r = ((hexValue >> 16) & 0xFF);  // Extract the RR byte
						rgbColor.g = ((hexValue >> 8) & 0xFF);   // Extract the GG byte
						rgbColor.b = ((hexValue)& 0xFF);        // Extract the BB byte
						rgbColor.a = opacity;

						curColor = rgbColor;
					}
				}
				else if (op == "a" || op == "anim") { // animation name
					if (per == "none") {
						curAnimMode = NONE;
					}
					else if (per == "jitter") {
						curAnimMode = JITTER;
					}
					else if (per == "sleepy") {
						curAnimMode = SLEEPY;
					}
					else if (per == "scooby") {
						curAnimMode = SCOOBY;
					}
					else if (per == "bulge") {
						curAnimMode = BULGE;
					}
				}
				else if (op == "n" || op == "newline") {
					newLine(target, states, i + 1, curAnimMode, curColorMode, size, curColor, f, atoi(per.c_str()));

					break;
				}
				else if (op == "s" || op == "size")
					size = atoi(per.c_str());
				else if (op == "i" || op == "image") {
					imagesToDraw = { theGame()->resources->getAs<sfTextureResource>(per) };
				}
				else if (op == "b" || op == "bind") {
					imagesToDraw = theGame()->getKeybindTextures(per);
				}
				else if (op == "f" || op == "font") {
					if (theGame()->resources->exists(per)) {
						f = theGame()->resources->getAs<sfFontResource>(per);
					}
					else {
						f = theGame()->resources->getAs<sfFontResource>("__fontDef");
					}
				}
			}

			continue;
		}

		if (curAnimMode == JITTER) {
			yOffset = randomInt(-1, 1);
		}
		else if (curAnimMode == SLEEPY) {
			yOffset = (int)floor(sin(theGame()->getSimTime().asSeconds() + cOffset) * 2.f);
		}
		else if (curAnimMode == SCOOBY) {
			yOffset = (int)floor(sin((theGame()->getSimTime().asSeconds() * 10) + (cOffset / 10)) * 4);
		}
		else if (curAnimMode == BULGE) {
			sizeOffset = (int)(sin(theGame()->getSimTime().asSeconds() + cOffset) * 4);
		}

		if (curColorMode == BLINK) {
			colorOffset.a = (sf::Uint8)floor(254.f * abs(sin(theGame()->getSimTime().asSeconds() * 10.f)));
		}
		else if (curColorMode == RAINBOW) {
			colorOffset.r = (sf::Uint8)floor(254.f * abs(sin(theGame()->getSimTime().asSeconds() + cOffset)));
			colorOffset.g = (sf::Uint8)floor(254.f * abs(sin(theGame()->getSimTime().asSeconds() + cOffset + 32.f)));
			colorOffset.b = (sf::Uint8)floor(254.f * abs(sin(theGame()->getSimTime().asSeconds() + cOffset + 64.f)));
		}

		auto charOffsetX = (float)cOffset + (float)xOffset;
		auto charOffsetY = (float)yOffset + (float)lOffset;

		if (curAlignment == LEFT) {
			// standard image drawing
			for (unsigned int img = 0; img < imagesToDraw.size(); img++) {
				sf::Sprite image(*imagesToDraw[img]->get());
				image.setPosition(getPosition() + sf::Vector2f(charOffsetX, charOffsetY - (image.getGlobalBounds().height / 2.f)));

				target.draw(image);

				cOffset += image.getGlobalBounds().width;

				charOffsetX = (float)cOffset + (float)xOffset;
			}

			// standard character drawing
			cOffset += drawInfo(target, states, getPosition() + sf::Vector2f(charOffsetX, charOffsetY), charString, curColor - colorOffset, false, size + sizeOffset, rot, f);
			cOffset += 1;
		}
		else {
			float centeredX = -halfTextLength + charOffsetX;

			// centered images
			for (unsigned int img = 0; img < imagesToDraw.size(); img++) {
				sf::Sprite image(*imagesToDraw[img]->get());
				image.setPosition(getPosition() + sf::Vector2f(centeredX, charOffsetY - (image.getGlobalBounds().height / 2.f)));

				target.draw(image);

				cOffset += image.getGlobalBounds().width;

				charOffsetX = (float)cOffset + (float)xOffset;
				centeredX = -halfTextLength + charOffsetX;
			}

			// centered characters
			cOffset += drawInfo(target, states, getPosition() + sf::Vector2f(centeredX, charOffsetY), charString, curColor - colorOffset, false, size + sizeOffset, rot, f);
			cOffset += 1;
		}

		imagesToDraw.clear();

		if (cOffset > theGame()->uiCamera->getSize().x - 128 && charString == " ") {
			newLine(target, states, i + 1, curAnimMode, curColorMode, size, curColor, f);

			break;
		}

		if (lineLimit != -1) {
			if (cOffset > lineLimit && charString == " ") {
				newLine(target, states, i + 1, curAnimMode, curColorMode, size, curColor, f);

				break;
			}
		}
	}
}

void magicText::newLine(sf::RenderTarget &target, sf::RenderStates states, unsigned int startPosition, animationMode am, colorMode cm, unsigned int s, sf::Color c, std::shared_ptr<sfFontResource> f, int num) const
{
	// cheat here.
	// create MORE MAGICTEXT and draw it
	magicText mt(*this);
	mt.text = strSub(text, startPosition, text.size());
	mt.setPosition(getPosition() + sf::Vector2f(0.f, lineSpacing * num));

	mt.defaultAnimationMode = am;
	mt.defaultColorMode = cm;
	mt.defaultColor = c;
	mt.defaultFont = f;
	mt.defaultSize = s;

	target.draw(mt, states);
}

std::string magicText::stopAtScrollPoint(std::string text, unsigned int textPhase)
{
	// a scroll point = an individual character in the text, skipping codepoints
	std::string t;
	for (unsigned int i = 0; i < text.size() && i < textPhase; i++) {
		std::string charString = { text[i] };

		// skip any control string
		if (charString == "[") {
			std::string commandString;
			for (unsigned int j = i; j < text.size(); j++) {
				std::string charString2 = { text[j] };

				commandString += charString2;

				if (charString2 == "]") {
					textPhase += j - i; // add lost space
					i = j;
					break;
				}
			}

			t += commandString;
		}
		else {
			t += charString;
		}
	}

	return t;
}

unsigned int magicText::highestScrollPoint(std::string text)
{
	// a scroll point = an individual character in the text, skipping codepoints
	int c = 0;
	for (unsigned int i = 0; i < text.size(); i++) {
		std::string charString = { text[i] };

		// skip any control string
		if (charString == "[") {
			std::string commandString;
			for (unsigned int j = i; j < text.size(); j++) {
				std::string charString2 = { text[j] };

				commandString += charString2;

				if (charString2 == "]") {
					i = j;
					break;
				}
			}
		}

		c++;
	}

	return c;
}

float magicText::findSizeX() const
{
	int cOffset = 0;
	int lOffset = 0;
	float sizeX = 0.f;

	unsigned int size = 16;

	for (unsigned int i = 0; i < text.size(); i++) {
		int xOffset = 0;
		int yOffset = 0;
		int sizeOffset = 0;

		std::string charString = { text[i] };

		// control string?
		if (charString == "[") {
			std::string commandString;
			for (unsigned int j = i + 1; j < text.size(); j++) {
				std::string charString2 = { text[j] };

				if (charString2 != "]")
					commandString += charString2;
				else {
					i = j;
					break;
				}
			}

			// read the string code
			std::vector<std::string> codeSplit = splitString(commandString, ':');

			if (codeSplit.size() == 2) {
				std::string op = codeSplit[0];
				std::string per = codeSplit[1];

				if (op == "n" || op == "newline") {
					break;
				}
				else if (op == "s" || op == "size")
					size = atoi(per.c_str());
				else if (op == "i" || op == "image") {
					float imWidth = 0.f;

					if (theGame()->resources->exists(per)) {
						imWidth = theGame()->resources->getAs<sfTextureResource>(per)->get()->getSize().x;
					}

					cOffset += imWidth;
				}
				else if (op == "b" || op == "bind") {
					float imWidth = 0.f;
					auto list = theGame()->keyboard->getBindTextures(per);

					for (unsigned int k = 0; k < list.size(); k++) {
						std::string bnd = list[k];

						if (theGame()->resources->exists(bnd)) {
							imWidth += theGame()->resources->getAs<sfTextureResource>(bnd)->get()->getSize().x;
						}
					}

					cOffset += imWidth;
				}
			}

			continue;
		}

		// measure this character
		sf::Text charText(charString, *theGame()->resources->getAs<sfFontResource>("__fontDef")->get(), size + sizeOffset);
		charText.setScale(.5f, .5f);

		cOffset += (int)charText.getGlobalBounds().width;
		cOffset += 1;

		if (cOffset > theGame()->uiCamera->getSize().x - 128 && charString == " ") {
			cOffset = 0;
			lOffset += size;
		}

		if (lineLimit != -1) {
			if (cOffset > lineLimit && charString == " ") {
				cOffset = 0;
				lOffset += size;
			}
		}

		if (cOffset > sizeX) {
			sizeX = (float)cOffset;
		}
	}

	return sizeX;
}

pillarMenu::pillarMenu(float x, float y)
	: menu(x, y)
{
	fi = false;
	fo = false;
	drawLines = false;

	targetX = x;
}

pillarMenu::pillarMenu(float x, float y, std::vector<std::string> opList)
	: menu(x, y, opList)
{
	fi = false;
	fo = false;
	drawLines = false;

	targetX = x;
}

void pillarMenu::update(sf::Time elapsed)
{
	menu::update(elapsed);

	if (fi) {
		float dist = (targetX - getPosition().x);
		move(dist * 0.1f, 0.f);

		if (dist < 0.1f) {
			fi = false;
			setPosition(targetX, getPosition().y);
		}
	}

	if (fo) {
		float dist = (getPosition().x - (-width));
		move(dist * 0.1f, 0.f);

		if (getPosition().x > theGame()->getRenderWindow().getSize().x) {
			fo = false;
		}
	}
}

void pillarMenu::floatIn() {
	setPosition(-width, getPosition().y);
	fi = true;
	fo = false;

	disabled = false;
}

void pillarMenu::floatOut()
{
	//setPosition(targetX, getPosition().y);
	fo = true;
	fi = false;

	disabled = true;
}

void pillarMenu::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	// draw a pillar the size of the menu going from the top of the screen to the bottom.
	sf::RectangleShape rec;

	rec.setPosition(sf::Vector2f(getPosition().x - 4.f, 0.f));
	rec.setSize(sf::Vector2f(width + 22.f, (float)theGame()->getRenderWindow().getSize().y));
	rec.setFillColor(sf::Color(0, 0, 20, 150));

	target.draw(rec);

	rec.setSize(sf::Vector2f(rec.getSize().x + 2.f, rec.getSize().y));
	rec.move(-1.f, 0.f);

	target.draw(rec);

	// lines
	if (drawLines) {
		rec.setPosition(rec.getPosition().x + 8.f, getPosition().y - 8.f);
		rec.setSize(sf::Vector2f(rec.getSize().x - 16.f, 1.f));
		rec.setFillColor(sf::Color::White);

		target.draw(rec);

		if((int)optionList.size() < maxOptions)
			rec.setPosition(rec.getPosition().x, getPosition().y + (optionList.size() * 12.f) + 4.f);
		else
			rec.setPosition(rec.getPosition().x, getPosition().y + (maxOptions * 12.f) + 4.f);

		rec.setSize(sf::Vector2f(rec.getSize().x, 1.f));
		rec.setFillColor(sf::Color::White);

		target.draw(rec);
	}

	menu::draw(target, states);
}

// SE
DeclareScriptingCustom(user_type<boxWindow>(), "boxWindow");
DeclareScriptingCustom(base_class<uiBasic, boxWindow>());
DeclareScriptingBaseClass(basic, boxWindow);
DeclareScriptingBaseClass(sf::Transformable, boxWindow);
DeclareScriptingCustom(constructor<boxWindow(float, float, sf::Color, sf::Vector2f, sf::Vector2f)>(), "boxWindow");
DeclareScriptingCustom(fun(&boxWindow::boxSize), "boxSize");
DeclareScriptingCustom(fun(&boxWindow::color), "color");
DeclareScriptingCustom(fun(&boxWindow::contentOpacity), "contentOpacity");
DeclareScriptingCustom(fun(&boxWindow::fullyVisible), "fullyVisible");
DeclareScriptingCustom(fun(&boxWindow::prct), "percentVisible");

DeclareScriptingCustom(user_type<menu>(), "menu");
DeclareScriptingCustom(base_class<uiBasic, menu>());
DeclareScriptingBaseClass(basic, menu);
DeclareScriptingBaseClass(sf::Transformable, menu);
DeclareScriptingCustom(constructor<menu(float, float)>(), "menu");
DeclareScriptingCustom(constructor<menu(float, float, std::vector<std::string>)>(), "menu");
DeclareScriptingCustom(fun(&menu::disabled), "disabled");
DeclareScriptingCustom(fun(&menu::wasDismissed), "wasDismissed");
DeclareScriptingCustom(fun(&menu::maxOptions), "maxOptions");
DeclareScriptingCustom(fun(&menu::isMouseOver), "isMouseOverMenu");
DeclareScriptingCustom(fun(&menu::noKbd), "noKbd");
DeclareScriptingCustom(fun(&menu::optionList), "optionList");
DeclareScriptingCustom(fun(&menu::optionStart), "optionStart");

DeclareScriptingCustom(user_type<textEntry>(), "textEntry");
DeclareScriptingCustom(base_class<uiBasic, textEntry>());
DeclareScriptingBaseClass(basic, textEntry);
DeclareScriptingBaseClass(sf::Transformable, textEntry);
DeclareScriptingCustom(constructor<textEntry(float, float)>(), "textEntry");
DeclareScriptingCustom(fun(&textEntry::active), "active");
DeclareScriptingCustom(fun(&textEntry::contains), "contains");
DeclareScriptingCustom(fun(&textEntry::displayFrom), "displayFrom");
DeclareScriptingCustom(fun(&textEntry::ready), "ready");
DeclareScriptingCustom(fun(&textEntry::mouseSensitive), "mouseSensitive");
DeclareScriptingCustom(fun(&textEntry::safe), "safe");
DeclareScriptingCustom(fun(&textEntry::width), "width");

DeclareScriptingCustom(user_type<magicText>(), "magicText");
DeclareScriptingCustom(base_class<uiBasic, magicText>());
DeclareScriptingBaseClass(basic, magicText);
DeclareScriptingBaseClass(sf::Transformable, magicText);
DeclareScriptingCustom(constructor<magicText(float, float, std::string)>(), "magicText");
DeclareScriptingCustom(fun<float, magicText>(&magicText::findSizeX), "findSizeX");
DeclareScriptingCustom(fun(&magicText::highestScrollPoint), "highestScrollPoint");
DeclareScriptingCustom(fun(&magicText::lineLimit), "lineLimit");
DeclareScriptingCustom(fun(&magicText::stopAtScrollPoint), "stopAtScrollPoint");
DeclareScriptingCustom(fun(&magicText::text), "text");

DeclareScriptingCustom(user_type<pillarMenu>(), "pillarMenu");
DeclareScriptingCustom(base_class<menu, pillarMenu>());
DeclareScriptingCustom(type_conversion<pillarMenu*, uiBasic*>());
DeclareScriptingBaseClass(basic, pillarMenu);
DeclareScriptingBaseClass(sf::Transformable, pillarMenu);
DeclareScriptingCustom(type_conversion<pillarMenu*, uiBasic*>());
DeclareScriptingCustom(constructor<pillarMenu(float, float)>(), "pillarMenu");
DeclareScriptingCustom(constructor<pillarMenu(float, float, std::vector<std::string>)>(), "pillarMenu");
DeclareScriptingCustom(fun(&pillarMenu::floatIn), "floatIn");
DeclareScriptingCustom(fun(&pillarMenu::floatIn), "floatOut");
DeclareScriptingCustom(fun(&pillarMenu::drawLines), "drawLines");
DeclareScriptingCustom(fun(&pillarMenu::targetX), "targetX");