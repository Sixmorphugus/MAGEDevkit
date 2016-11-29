#pragma once
#include "basic.h"

// UiPrimitives contains:
// - a bunch of draw functions used everywhere as a wrapper around SFML's
// - a bunch of uiBasics with a wide veriety of uses
// - magicText!

// Universal draw functions
MAGEDLL void pDrawBindPrompt(sf::RenderTarget &target, sf::RenderStates states, int yPosition, std::vector<std::string> keybindNames, std::string caption);
MAGEDLL float pDrawInfoSimple(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f screenPos, std::string info);
MAGEDLL float pDrawInfo(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f screenPos, std::string info, sf::Color col, bool center = false, unsigned int charSize = 16, float rot = 0.f, std::shared_ptr<sfFontResource> customFont = nullptr);

// Drawable only - not registerable as objects:

// for drawing loading screens in the ui.
// multipurpose and updateless
class MAGEDLL loadingScreen :
	public sf::Drawable
{
public:
	loadingScreen();

	virtual bool canDraw() const;
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

public:
	std::string loading;
	unsigned int count, countMax;
};

// derivable class for transitions between maps/levels
// default transition (transition) uses the same technique as boxWindow
// derive and pass to certain Game functions to change
// (by default there is no transition between game states)
/*
class MAGEDLL transition :
	public sf::Drawable
{
	transition(Game* gm);

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

	bool isBlockingGameView();
	void block(float multiplier = 1.f);
	void unblock(float multiplier = 1.f);
public:
	float percentComplete;
};
*/

// uiBasic types!

// boxWindow.
// just a bunch of boxes that ease in and out
class MAGEDLL boxWindow :
	public uiBasic
{
public:
	boxWindow(float x, float y);
	boxWindow(float x, float y, sf::Color col, sf::Vector2f siz, sf::Vector2f boxSiz = sf::Vector2f(16, 16));

	sf::Color color;
	sf::Vector2f size, boxSize;

	void setup();
	virtual void update(sf::Time elapsed);
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

	int contentOpacity();
	bool fullyVisible();

	float prct;
	bool noShadow;

	std::vector< std::vector<sf::RectangleShape> > rectangles;
	sf::RectangleShape shadow;
};

// menu.
// A selection menu with a selector character (@ by default).
class MAGEDLL menu : public uiBasic
{
public:
	// defs
	enum alignMode {
		LEFT, // align text to the left (default)
		CENTER, // center each line of text
		RIGHT // align text to the right
	};

	// stuff
	menu(float x, float y);
	menu(float x, float y, std::vector<std::string> opList);

	void update(sf::Time elapsed);
	void draw(sf::RenderTarget &target, sf::RenderStates states) const;

	void validateSelected();

	void init();

	bool isMouseOver() { return mouseOver; }
	bool wasDismissed() { return dismissed; }

	//void setSelectorPosition(int op);
	
	void reset();

public:
	int selected, superSelected;
	float width;
	bool selectionChanged;
	std::string selectorText;
	alignMode alignMode;
	std::vector<std::string> optionList;
	bool ready;
	bool disabled;
	int maxOptions;
	int optionStart;
	bool noKbd;

	hook<basic*, unsigned int> onSelectionChanged;
	hook<basic*> onDismissed;

protected:
	sf::Vector2f scrollDragOffset;
	bool dragMode;

	bool mouseOver;
	bool scrollBarGlow;
	bool dismissed; // was esc hit?
	bool skipDown;
	float selectorPos, selectorPos2;
};

// textEntry - place for entering text
class MAGEDLL textEntry : public uiBasic
{
public:
	// constructors
	textEntry(float x, float y);

	void update(sf::Time elapsed);
	void draw(sf::RenderTarget &target, sf::RenderStates states) const;

public:
	bool active; // if active, text entry box can take input
	std::string contains;
	float width;
	bool mouseSensitive;
	unsigned int displayFrom;

	bool safe; // allow non-alnum characters
	bool ready; // was enter hit?

	hook<basic*> onReady;
};

// magicText
// text formattable with BBC tags
class MAGEDLL magicText : public uiBasic
{
public:
	magicText(float x, float y, std::string text = "");

	enum animationMode {
		NONE,
		JITTER,
		SLEEPY,
		SCOOBY,
		BULGE
	};

	enum colorMode {
		SOLID,
		BLINK,
		RAINBOW
	};

	enum alignmentMode {
		LEFT,
		CENTER
	};

	std::string text;

	void update(sf::Time elapsed);
	void draw(sf::RenderTarget &target, sf::RenderStates states) const;

	void newLine(sf::RenderTarget &target, sf::RenderStates states, unsigned int startPosition, animationMode am, colorMode cm, unsigned int s, sf::Color c, std::shared_ptr<sfFontResource> f, int num = 1) const;

	static std::string stopAtScrollPoint(std::string text, unsigned int textPhase);
	static unsigned int highestScrollPoint(std::string text);

	float findSizeX() const;

	int lineLimit;
	sf::Uint8 opacity;

	animationMode defaultAnimationMode;
	colorMode defaultColorMode;
	unsigned int defaultSize;
	sf::Color defaultColor;
	std::shared_ptr<sfFontResource> defaultFont;

	alignmentMode alignment;

	float lineSpacing;
};

// boxMenu.
// A selection menu with @ in a box.
class MAGEDLL boxMenu : public boxWindow, public menu
{
public:
	// stuff
	boxMenu(float x, float y);
	boxMenu(float x, float y, std::vector<std::string> opList, sf::Color col, sf::Vector2f siz, sf::Vector2f boxSiz = sf::Vector2f(16, 16));

	void update(sf::Time elapsed);
	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};

// boxTextEntry.
// Text entry in a box.
class MAGEDLL boxTextEntry : public boxWindow, public textEntry
{
public:
	// stuff
	boxTextEntry(float x, float y);
	boxTextEntry(float x, float y, sf::Color col, sf::Vector2f siz, sf::Vector2f boxSiz = sf::Vector2f(16, 16));

	void update(sf::Time elapsed);
	void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};

// pillarMenu
class MAGEDLL pillarMenu : public menu
{
public:
	pillarMenu(float x, float y);
	pillarMenu(float x, float y, std::vector<std::string> opList);

	void update(sf::Time elapsed);
	void draw(sf::RenderTarget &target, sf::RenderStates states) const;

	void floatIn();
	void floatOut();

public:
	bool drawLines;
	float targetX;

private:
	bool fi, fo;
};