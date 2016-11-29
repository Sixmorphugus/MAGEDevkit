#include "io.h"
#include "platform.h"

// MOUSE
// ---------------------------------------------------
mouseMngr::mouseMngr()
{
	leftState = 0;
	rightState = 0;
	middleState = 0;

	scroll = 0;

	justUsed = false;

	// Doesn't support your batshit 8 button mouse. YOU ONLY NEED THREE.
}

void mouseMngr::processEvent(sf::Event evt) {
	// the Game state passes mouse events our way. According to these, we change our state to reflect that of the mouse.
	if (evt.type == sf::Event::MouseMoved) {
		position.x = (float)evt.mouseMove.x;
		position.y = (float)evt.mouseMove.y;

		justUsed = true;
	}
	else if (evt.type == sf::Event::MouseButtonPressed) {
		lpPosition.x = (float)evt.mouseButton.x;
		lpPosition.y = (float)evt.mouseButton.y;

		position.x = (float)evt.mouseButton.x;
		position.y = (float)evt.mouseButton.y;

		if (evt.mouseButton.button == sf::Mouse::Button::Left) {
			leftState = 2;
			onLeftPressed.notify(this);
		}
		else if (evt.mouseButton.button == sf::Mouse::Button::Right) {
			rightState = 2;
			onRightPressed.notify(this);
		}
		else if (evt.mouseButton.button == sf::Mouse::Button::Middle) {
			middleState = 2;
			onMiddlePressed.notify(this);
		}

		justUsed = true;
	}
	else if (evt.type == sf::Event::MouseButtonReleased) {
		lrPosition.x = (float)evt.mouseButton.x;
		lrPosition.y = (float)evt.mouseButton.y;

		position.x = (float)evt.mouseButton.x;
		position.y = (float)evt.mouseButton.y;

		if (evt.mouseButton.button == sf::Mouse::Button::Left) {
			leftState = -1;
			onLeftReleased.notify(this);
		}
		else if (evt.mouseButton.button == sf::Mouse::Button::Right) {
			rightState = -1;
			onRightReleased.notify(this);
		}
		else if (evt.mouseButton.button == sf::Mouse::Button::Middle) {
			middleState = -1;
			onMiddleReleased.notify(this);
		}

		justUsed = true;
	}
	else if (evt.type == sf::Event::MouseWheelScrolled) {
		scroll = evt.mouseWheelScroll.delta > 0 ? -1 : 1;
		justUsed = true;
	}
}

void mouseMngr::update() {
	// normalize everything

	// LEFT BUTTON
	if (leftState == 2)
		leftState = 1;
	else if (leftState == -1)
		leftState = 0;

	// RIGHT BUTTON
	if (rightState == 2)
		rightState = 1;
	else if (rightState == -1)
		rightState = 0;

	// MIDDLE BUTTON
	if (middleState == 2)
		middleState = 1;
	else if (middleState == -1)
		middleState = 0;

	scroll = 0;
	justUsed = false;
}

void mouseMngr::scrollReset() {
	scroll = 0; // !?!
}

void mouseMngr::reset()
{
	leftState = 0;
	rightState = 0;
	middleState = 0;

	scroll = 0;

	justUsed = false;
}

bool mouseMngr::getLeftDown() {
	return (leftState > 0);
}

bool mouseMngr::getRightDown() {
	return (rightState > 0);
}

bool mouseMngr::getMiddleDown() {
	return (middleState > 0);
}

bool mouseMngr::getLeftPressed() {
	return (leftState == 2);
}

bool mouseMngr::getRightPressed() {
	return (rightState == 2);
}

bool mouseMngr::getMiddlePressed() {
	return (middleState == 2);
}

bool mouseMngr::getLeftReleased() {
	return (leftState == -1);
}

bool mouseMngr::getRightReleased() {
	return (rightState == -1);
}

bool mouseMngr::getMiddleReleased() {
	return (middleState == -1);
}

void mouseMngr::setDefaultButtonTextures()
{
	mouseLeftTexture = "ui_input_mouseLeft";
	mouseRightTexture = "ui_input_mouseRight";
	mouseMiddleTexture = "ui_input_mouseMiddle";
}

// KEYBOARD
// ---------------------------------------------------
keyboardMngr::keyboardMngr() {
	// set some defeaults
	keyJustPressed = false;
	inputStream = "";

	// get your keybind collection started today!
	setBind("CTRL", sf::Keyboard::LControl);
	setBind("SHIFT", sf::Keyboard::LShift);

	setBind("CUT", { sf::Keyboard::LControl, sf::Keyboard::X });
	setBind("COPY", { sf::Keyboard::LControl, sf::Keyboard::C });
	setBind("PASTE", { sf::Keyboard::LControl, sf::Keyboard::V });

	setBind("DELETE", sf::Keyboard::Delete);
	setBind("ESCAPE", sf::Keyboard::Escape);
	setBind("ENTER", sf::Keyboard::Return);
	setBind("TILDE", sf::Keyboard::Tilde);
}

void keyboardMngr::processEvent(sf::Event evt)
{
	if (evt.type == sf::Event::KeyPressed) {
		keyJustPressed = true;

		for (auto i = bindings.begin(); i != bindings.end(); i++) {
			unsigned int keysDown = 0;
			for (unsigned int k = 0; k < i->second.keys.size(); k++) {
				// i'm too young for this
				if (evt.key.code == i->second.keys[k]) {
					i->second.keysIn[k] = true;
				}

				if (i->second.keysIn[k]) {
					keysDown++;
				}
			}

			// is this binding ready to activate?
			if (keysDown >= i->second.keys.size() && i->second.getUp()) {
				i->second.s = keybind::PRESSED;
				i->second.onPressed.notify(&i->second);
				//std::cout << i->first << std::endl;
			}
		}
	}
	else if (evt.type == sf::Event::KeyReleased)
	{
		for (auto i = bindings.begin(); i != bindings.end(); i++) {
			unsigned int keysDown = 0;
			for (unsigned int k = 0; k < i->second.keys.size(); k++) {
				// i'm too young for this
				if (evt.key.code == i->second.keys[k]) {
					i->second.keysIn[k] = false;
				}

				if (i->second.keysIn[k]) {
					keysDown++;
				}
			}

			// is this binding ready to deactivate?
			if (keysDown < i->second.keys.size() && i->second.getDown()) {
				i->second.s = keybind::RELEASED;
				i->second.onReleased.notify(&i->second);
			}
		}
	}
	else if (evt.type == sf::Event::TextEntered) {
		inputStream = (char)evt.text.unicode;
	}
}

void keyboardMngr::update() {
	keyJustPressed = false;
	inputStream = "";

	for (auto i = bindings.begin(); i != bindings.end(); i++) {
		i->second.consumeEvent();
	}
}

void keyboardMngr::setBind(std::string bName, keybind& bind)
{
	bindings[bName] = bind;
}

void keyboardMngr::setBind(std::string bName, sf::Keyboard::Key singleKey, bool rebindable) {
	keybind nkb(singleKey, rebindable);
	bindings[bName] = nkb;
}

void keyboardMngr::setBind(std::string bName, std::initializer_list<sf::Keyboard::Key> keysIn, bool rebindable) {
	keybind nkb(keysIn, rebindable);
	bindings[bName] = nkb;
}

keyboardMngr::keybind* keyboardMngr::getBind(std::string name) {
	if (isBind(name)) {
		return &bindings[name];
	}
	else {
		return nullptr;
	}
}

keyboardMngr::keybind* keyboardMngr::operator[](std::string name)
{
	if (isBind(name)) {
		return &bindings[name];
	}
	else {
		bindings[name] = keybind(); // create a new one and return that
		return &bindings[name];
	}
}

void keyboardMngr::setDefaultBindTextures()
{
	keyTextureCache[sf::Keyboard::A] = "ui_input_keyA";
	keyTextureCache[sf::Keyboard::B] = "ui_input_keyB";
	keyTextureCache[sf::Keyboard::C] = "ui_input_keyC";
	keyTextureCache[sf::Keyboard::D] = "ui_input_keyD";
	keyTextureCache[sf::Keyboard::E] = "ui_input_keyE";
	keyTextureCache[sf::Keyboard::F] = "ui_input_keyF";
	keyTextureCache[sf::Keyboard::G] = "ui_input_keyG";
	keyTextureCache[sf::Keyboard::H] = "ui_input_keyH";
	keyTextureCache[sf::Keyboard::I] = "ui_input_keyI";
	keyTextureCache[sf::Keyboard::J] = "ui_input_keyJ";
	keyTextureCache[sf::Keyboard::K] = "ui_input_keyK";
	keyTextureCache[sf::Keyboard::L] = "ui_input_keyL";
	keyTextureCache[sf::Keyboard::M] = "ui_input_keyM";
	keyTextureCache[sf::Keyboard::N] = "ui_input_keyN";
	keyTextureCache[sf::Keyboard::O] = "ui_input_keyO";
	keyTextureCache[sf::Keyboard::P] = "ui_input_keyP";
	keyTextureCache[sf::Keyboard::Q] = "ui_input_keyQ";
	keyTextureCache[sf::Keyboard::R] = "ui_input_keyR";
	keyTextureCache[sf::Keyboard::S] = "ui_input_keyS";
	keyTextureCache[sf::Keyboard::T] = "ui_input_keyT";
	keyTextureCache[sf::Keyboard::U] = "ui_input_keyU";
	keyTextureCache[sf::Keyboard::V] = "ui_input_keyV";
	keyTextureCache[sf::Keyboard::W] = "ui_input_keyW";
	keyTextureCache[sf::Keyboard::X] = "ui_input_keyX";
	keyTextureCache[sf::Keyboard::Y] = "ui_input_keyY";
	keyTextureCache[sf::Keyboard::Z] = "ui_input_keyZ";
	keyTextureCache[sf::Keyboard::Num0] = "ui_input_keyNum0";
	keyTextureCache[sf::Keyboard::Num1] = "ui_input_keyNum1";
	keyTextureCache[sf::Keyboard::Num2] = "ui_input_keyNum2";
	keyTextureCache[sf::Keyboard::Num3] = "ui_input_keyNum3";
	keyTextureCache[sf::Keyboard::Num4] = "ui_input_keyNum4";
	keyTextureCache[sf::Keyboard::Num5] = "ui_input_keyNum5";
	keyTextureCache[sf::Keyboard::Num6] = "ui_input_keyNum6";
	keyTextureCache[sf::Keyboard::Num7] = "ui_input_keyNum7";
	keyTextureCache[sf::Keyboard::Num8] = "ui_input_keyNum8";
	keyTextureCache[sf::Keyboard::Num9] = "ui_input_keyNum9";
	keyTextureCache[sf::Keyboard::Numpad0] = "ui_input_keyNum0";
	keyTextureCache[sf::Keyboard::Numpad1] = "ui_input_keyNum1";
	keyTextureCache[sf::Keyboard::Numpad2] = "ui_input_keyNum2";
	keyTextureCache[sf::Keyboard::Numpad3] = "ui_input_keyNum3";
	keyTextureCache[sf::Keyboard::Numpad4] = "ui_input_keyNum4";
	keyTextureCache[sf::Keyboard::Numpad5] = "ui_input_keyNum5";
	keyTextureCache[sf::Keyboard::Numpad6] = "ui_input_keyNum6";
	keyTextureCache[sf::Keyboard::Numpad7] = "ui_input_keyNum7";
	keyTextureCache[sf::Keyboard::Numpad8] = "ui_input_keyNum8";
	keyTextureCache[sf::Keyboard::Numpad9] = "ui_input_keyNum9";
	keyTextureCache[sf::Keyboard::Escape] = "ui_input_keyEscape";
	keyTextureCache[sf::Keyboard::LControl] = "ui_input_keyLControl";
	keyTextureCache[sf::Keyboard::LShift] = "ui_input_keyLShift";
	keyTextureCache[sf::Keyboard::LAlt] = "ui_input_keyLAlt";
	keyTextureCache[sf::Keyboard::LBracket] = "ui_input_keyLBracket";
	keyTextureCache[sf::Keyboard::RBracket] = "ui_input_keyRBracket";
	keyTextureCache[sf::Keyboard::SemiColon] = "ui_input_keySemiColon";
	keyTextureCache[sf::Keyboard::Comma] = "ui_input_keyComma";
	keyTextureCache[sf::Keyboard::Period] = "ui_input_keyPeriod";
	keyTextureCache[sf::Keyboard::Quote] = "ui_input_keyQuote";
	keyTextureCache[sf::Keyboard::Slash] = "ui_input_keySlash";
	keyTextureCache[sf::Keyboard::BackSlash] = "ui_input_keyBackSlash";
	keyTextureCache[sf::Keyboard::Tilde] = "ui_input_keyTilde";
	keyTextureCache[sf::Keyboard::Equal] = "ui_input_keyEqual";
	keyTextureCache[sf::Keyboard::Dash] = "ui_input_keyDash";
	keyTextureCache[sf::Keyboard::Space] = "ui_input_keySpace";
	keyTextureCache[sf::Keyboard::Return] = "ui_input_keyReturn";
	keyTextureCache[sf::Keyboard::Tab] = "ui_input_keyTab";
	keyTextureCache[sf::Keyboard::PageUp] = "ui_input_keyPageUp";
	keyTextureCache[sf::Keyboard::PageDown] = "ui_input_keyPageDown";
	keyTextureCache[sf::Keyboard::End] = "ui_input_keyEnd";
	keyTextureCache[sf::Keyboard::Home] = "ui_input_keyHome";
	keyTextureCache[sf::Keyboard::Insert] = "ui_input_keyInsert";
	keyTextureCache[sf::Keyboard::Delete] = "ui_input_keyDelete";
	keyTextureCache[sf::Keyboard::Add] = "ui_input_keyAdd";
	keyTextureCache[sf::Keyboard::Multiply] = "ui_input_keyMultiply";
	keyTextureCache[sf::Keyboard::Left] = "ui_input_keyLeft";
	keyTextureCache[sf::Keyboard::Right] = "ui_input_keyRight";
	keyTextureCache[sf::Keyboard::Up] = "ui_input_keyUp";
	keyTextureCache[sf::Keyboard::Down] = "ui_input_keyDown";
	keyTextureCache[sf::Keyboard::F1] = "ui_input_keyF1";
	keyTextureCache[sf::Keyboard::F2] = "ui_input_keyF2";
	keyTextureCache[sf::Keyboard::F3] = "ui_input_keyF3";
	keyTextureCache[sf::Keyboard::F4] = "ui_input_keyF4";
	keyTextureCache[sf::Keyboard::F5] = "ui_input_keyF5";
	keyTextureCache[sf::Keyboard::F6] = "ui_input_keyF6";
	keyTextureCache[sf::Keyboard::F7] = "ui_input_keyF7";
	keyTextureCache[sf::Keyboard::F8] = "ui_input_keyF8";
	keyTextureCache[sf::Keyboard::F9] = "ui_input_keyF9";
	keyTextureCache[sf::Keyboard::F10] = "ui_input_keyF10";
	keyTextureCache[sf::Keyboard::F11] = "ui_input_keyF11";
	keyTextureCache[sf::Keyboard::F12] = "ui_input_keyF12";
	keyTextureCache[sf::Keyboard::F13] = "ui_input_keyF13";
	keyTextureCache[sf::Keyboard::F14] = "ui_input_keyF14";
	keyTextureCache[sf::Keyboard::F15] = "ui_input_keyF15";
	keyTextureCache[sf::Keyboard::Unknown] = "ui_input_keyUnknown";

	// I had an algorythm write this
	// Can you blame me?
}

bool keyboardMngr::isBind(std::string name) {
	return (bindings.count(name) != 0);
}

std::vector<std::string> keyboardMngr::getBindTextures(std::string bName)
{
	std::vector<std::string> textureList;
	keybind* kb = getBind(bName);

	if (kb != nullptr) {
		std::vector<sf::Keyboard::Key> keys = kb->keys;

		for (unsigned int i = 0; i < keys.size(); i++) {
			sf::Keyboard::Key key = keys[i];

			if (keyTextureCache.count(key) != 0) { // don't include keys with no bound texture
				textureList.push_back(keyTextureCache[key]);
			}
			else {
				p::warn("No texture found for key " + std::to_string((int)key) + "! UI will display unknown.");
				textureList.push_back(keyTextureCache[sf::Keyboard::Unknown]);
			}
		}
	}

	return textureList;
}

keyboardMngr::keybind::keybind() {
	// ...
}

keyboardMngr::keybind::keybind(sf::Keyboard::Key singleKey, bool r) {
	keys.push_back(singleKey);
	keysIn.push_back(false);
	s = keybind::IDLE;
	rebindable = r;
}

keyboardMngr::keybind::keybind(std::initializer_list<sf::Keyboard::Key> keysInA, bool r) {
	keys = keysInA;

	for (unsigned int i = 0; i < keys.size(); i++) {
		keysIn.push_back(false);
	}

	s = keybind::IDLE;
	rebindable = r;
}

void keyboardMngr::keybind::consumeEvent() {
	if (s == keybind::PRESSED)
		s = keybind::DOWN;

	if (s == keybind::RELEASED)
		s = keybind::IDLE;
}


// CONTROLLA (COCO-COLA)
// ---------------------------------------------------
// TODO

// SCRIPTING ENGINE BINDING
// ---------------------------------------------------
#include "scriptingEngine.h"

DeclareScriptingType(input);
DeclareScriptingFunction(&input::processEvent, "processEvent");
DeclareScriptingFunction(&input::update, "update");

DeclareScriptingType(mouseMngr);
DeclareScriptingBaseClass(input, mouseMngr);
DeclareScriptingConstructor(mouseMngr(), "mouseMngr");
DeclareScriptingFunction(&mouseMngr::getLeftDown, "getLeftDown");
DeclareScriptingFunction(&mouseMngr::getLeftPressed, "getLeftPressed");
DeclareScriptingFunction(&mouseMngr::getLeftReleased, "getLeftReleased");
DeclareScriptingFunction(&mouseMngr::getMiddleDown, "getMiddleDown");
DeclareScriptingFunction(&mouseMngr::getMiddlePressed, "getMiddlePressed");
DeclareScriptingFunction(&mouseMngr::getMiddleReleased, "getMiddleReleased");
DeclareScriptingFunction(&mouseMngr::justUsed, "justUsed");
DeclareScriptingFunction(&mouseMngr::lpPosition, "lpPosition");
DeclareScriptingFunction(&mouseMngr::mouseLeftTexture, "mouseLeftTexture");
DeclareScriptingFunction(&mouseMngr::mouseMiddleTexture, "mouseMiddleTexture");
DeclareScriptingFunction(&mouseMngr::mouseRightTexture, "mouseRightTexture");
DeclareScriptingFunction(&mouseMngr::reset, "reset");
DeclareScriptingFunction(&mouseMngr::scroll, "scroll");
DeclareScriptingFunction(&mouseMngr::scrollReset, "scrollReset");
DeclareScriptingFunction(&mouseMngr::setDefaultButtonTextures, "setDefaultButtonTextures");

DeclareScriptingEnum(keyboardMngr::keybind::state, DOWN);
DeclareScriptingEnum(keyboardMngr::keybind::state, IDLE);
DeclareScriptingEnum(keyboardMngr::keybind::state, PRESSED);
DeclareScriptingEnum(keyboardMngr::keybind::state, RELEASED);

DeclareScriptingTypeNamed(keyboardMngr::keybind, "keybind");
DeclareScriptingConstructor(keyboardMngr::keybind(), "keybind");
DeclareScriptingConstructor(keyboardMngr::keybind(std::initializer_list<sf::Keyboard::Key>, bool), "keybind");
DeclareScriptingFunction(&keyboardMngr::keybind::getDown, "getDown");
DeclareScriptingFunction(&keyboardMngr::keybind::getIdle, "getIdle");
DeclareScriptingFunction(&keyboardMngr::keybind::getPressed, "getPressed");
DeclareScriptingFunction(&keyboardMngr::keybind::getReleased, "getReleased");
DeclareScriptingFunction(&keyboardMngr::keybind::keys, "keys");
DeclareScriptingFunction(&keyboardMngr::keybind::keysIn, "keysIn");
DeclareScriptingFunction(&keyboardMngr::keybind::rebindable, "rebindable");
DeclareScriptingFunction(&keyboardMngr::keybind::s, "state");

DeclareScriptingType(keyboardMngr);
DeclareScriptingConstructor(keyboardMngr(), "keyboardMngr");
DeclareScriptingBaseClass(input, keyboardMngr);
DeclareScriptingFunction(&keyboardMngr::getBind, "getBind");
DeclareScriptingFunction(&keyboardMngr::getBindTextures, "getBindTextures");
DeclareScriptingFunction(&keyboardMngr::inputStream, "inputStream");
DeclareScriptingFunction(&keyboardMngr::isBind, "isBind");
DeclareScriptingFunction(&keyboardMngr::keyJustPressed, "keyJustPressed");
DeclareScriptingCustom(fun<void, keyboardMngr, std::string, std::initializer_list<sf::Keyboard::Key>, bool>(&keyboardMngr::setBind), "setBind");
DeclareScriptingCustom(fun<void, keyboardMngr, std::string, sf::Keyboard::Key, bool>(&keyboardMngr::setBind), "setBind");
DeclareScriptingCustom(fun<void, keyboardMngr, std::string, keyboardMngr::keybind&>(&keyboardMngr::setBind), "setBind");
DeclareScriptingFunction(&keyboardMngr::setDefaultBindTextures, "setDefaultBindTextures");