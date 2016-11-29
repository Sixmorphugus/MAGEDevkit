// Chris's all-fulfilling Input script for SFML
// "Seems legit!"

#pragma once
#include "StdAfx.h"
#include "SfmlAfx.h"
#include "hook.h"

class MAGEDLL input {
public:
	virtual void processEvent(sf::Event evt) = 0;
	virtual void update() = 0;
};

// MOUSE
// ---------------------------------------------------
class MAGEDLL mouseMngr : public input
{
public:
	mouseMngr();

	void processEvent(sf::Event evt);
	void update();

	void scrollReset();
	void reset();

	bool getLeftDown();
	bool getRightDown();
	bool getMiddleDown();

	bool getLeftPressed();
	bool getRightPressed();
	bool getMiddlePressed();

	bool getLeftReleased();
	bool getRightReleased();
	bool getMiddleReleased();

	void setDefaultButtonTextures();

public:
	hook<mouseMngr*> onLeftPressed;
	hook<mouseMngr*> onRightPressed;
	hook<mouseMngr*> onMiddlePressed;
	hook<mouseMngr*> onLeftReleased;
	hook<mouseMngr*> onRightReleased;
	hook<mouseMngr*> onMiddleReleased;

	sf::Vector2f position;

	sf::Vector2f lpPosition;
	sf::Vector2f lrPosition;

	std::string mouseLeftTexture, mouseRightTexture, mouseMiddleTexture;

	int scroll;
	bool justUsed;

private:
	int leftState, rightState, middleState;
};

// KEYBOARD
// ---------------------------------------------------
class MAGEDLL keyboardMngr : public input
{
public:
	keyboardMngr();

	class MAGEDLL keybind {
	public:
		enum state {
			RELEASED = -1,
			IDLE = 0,
			DOWN = 1,
			PRESSED = 2
		};

	public:
		keybind();
		keybind(sf::Keyboard::Key singleKey, bool rebindable = false);
		keybind(std::initializer_list<sf::Keyboard::Key> keysIn, bool rebindable = false);

	public:
		hook<keybind*> onPressed;
		hook<keybind*> onReleased;

		std::vector<sf::Keyboard::Key> keys;
		std::vector<bool> keysIn;
		state s = IDLE;
		bool rebindable = false;

		inline bool getDown();
		inline bool getPressed();
		inline bool getReleased();
		inline bool getUp();
		inline bool getIdle();
		void consumeEvent();
	};

	void setBind(std::string bName, keybind& bind);
	void setBind(std::string bName, sf::Keyboard::Key singleKey, bool rebindable = false);
	void setBind(std::string bName, std::initializer_list<sf::Keyboard::Key> keysIn, bool rebindable = false);

	keybind* getBind(std::string bName);
	bool isBind(std::string bName);

	std::vector<std::string> getBindTextures(std::string bName);

	void processEvent(sf::Event evt);
	void update();

	keybind* operator[](std::string bName);

	void setDefaultBindTextures();

public:
	bool keyJustPressed;
	std::string inputStream;

	std::map<sf::Keyboard::Key, std::string> keyTextureCache;

private:
	std::map<std::string, keybind> bindings;
};

bool keyboardMngr::keybind::getDown() {
	return s > 0;
}

bool keyboardMngr::keybind::getPressed() {
	return s == 2;
}

bool keyboardMngr::keybind::getReleased() {
	return s == -1;
}

bool keyboardMngr::keybind::getUp() {
	return s <= 0;
}

bool keyboardMngr::keybind::getIdle() {
	return s == 0;
}

// CONTROLLA (COCO-COLA)
// ---------------------------------------------------
class controllerMngr : public input {
	controllerMngr(int listenToIndex);

	void processEvent(sf::Event evt);
	void update();
	// TODO
};
