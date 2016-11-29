#pragma once

/*
 ▄▀▀▄ ▄▀▄  ▄▀▀█▄   ▄▀▀▀▀▄   ▄▀▀█▄▄▄▄ 
█  █ ▀  █ ▐ ▄▀ ▀▄ █        ▐  ▄▀   ▐ 
▐  █    █   █▄▄▄█ █    ▀▄▄   █▄▄▄▄▄  
  █    █   ▄▀   █ █     █ █  █    ▌  
▄▀   ▄▀   █   ▄▀  ▐▀▄▄▄▄▀ ▐ ▄▀▄▄▄▄   
█    █    ▐   ▐   ▐         █    ▐   
▐    ▐                      ▐        
 
 Multiplatform Arcade Game Engine
   Property of Chris Sixsmith.
       Do not distribute.
*/

#include "platform.h" // OS stuff
#include "registry.h"
#include "hook.h"
#include "basic.h"

class jukebox;
class mixer;
class mouseMngr;
class keyboardMngr;
class gameStateMngr;
class gameState;
class resourceMngr;
class scheduleMngr;
class prefabMngr;
class scriptingEngine;

class uiView;
class worldView;

class tilemap;
class player;

class loadingScreen;

class sfTextureResource;

// The game class is the core of our game.
// It holds all of the managers and the window.
// It has our main loop.
// It has all of the top-down functions that examine global listings.
// It currently has the camera & viewport built in.
// It times and measures game logic.
// It is not the game, however, and if the game only consisted of the Game class it would be a shit game,
//  missing many important subsystems.

// You can and should derive this for your own applications.
// The macro DefineGame(classname) will allow you to have your game instance run instead of main().

class MAGEDLL Game
{
public:
	Game(int argc, char* argv[], sf::RenderWindow* wind);
	~Game();

	bool renderWindowExists();
	void windowInit(sf::RenderWindow* window);

	virtual int run(bool tickByTick = false);

	virtual void draw();
	virtual void update(sf::Time elapsed);

	virtual void drawLoadingScreenFrame(std::string text, int prog, int maxProg);
	virtual void drawLoadingScreenFrameToTarget(sf::RenderTarget &targ, sf::RenderStates states, std::string text, int prog, int maxProg);
	void setLoadingScreen(loadingScreen* lsn);
	std::shared_ptr<loadingScreen> getLoadingScreen();

	void setWindowSettings(sf::VideoMode resolution, std::string title, bool fullscreen, bool vsync, int maxFps);
	void setAudioSettings(float masterVolume, float sfxVolume, float musicVolume);

	virtual void fixViews();

	virtual float getDelta(); // delta - progress through a frame from 0.0 to 1.0

	// things that we can only really do on the top level

	objBasic::collision resolveMovement(objBasic* obj, int byX, int byY);
	bool hasCollisions(objBasic* obj);
	std::vector<std::shared_ptr<objBasic>> getCollidingObjects(objBasic* obj);
	std::vector<sf::FloatRect> getCollidingBoxes(objBasic* obj);
	std::vector<std::shared_ptr<objBasic>> findObjectsAt(sf::Vector2f pos);
	std::vector<std::shared_ptr<uiBasic>> findUiAt(sf::Vector2f pos);
	std::vector<std::shared_ptr<sfTextureResource>> getKeybindTextures(std::string bName);
	void physicsImpulse(sf::Vector2f position, float force, float radius, std::vector<objBasic*> exempt);
	sf::Vector2f findTextSize(std::string text, unsigned int fontScale = 18);

	int version();

	// window settings
	bool isFullscreen();
	bool isVsyncEnabled();
	int getFramerateLimit();
	sf::VideoMode getScreenResolution();
	std::string getWindowTitle();

	void setScreenResolution(sf::VideoMode res);
	void setFullscreen(bool tf);
	void setVsyncEnabled(bool tf);
	void setFramerateLimit(int fpsl);
	void setWindowTitle(std::string t);

	sf::Time getSimTime();

	sf::RenderWindow& getRenderWindow();

public:
	// SUBSYSTEMS
	std::shared_ptr<mouseMngr> mouse;
	std::shared_ptr<keyboardMngr> keyboard;

	std::shared_ptr<jukebox> music;
	std::shared_ptr<mixer> sound;

	std::shared_ptr<resourceMngr> resources;
	std::shared_ptr<prefabMngr> prefabs;

	std::shared_ptr<scriptingEngine> scripting;
	std::shared_ptr<scheduleMngr> scheduler;

	std::shared_ptr<gameStateMngr> states;
	std::shared_ptr<gameState> state;

	std::shared_ptr<uiView> uiCamera;
	std::shared_ptr<worldView> worldCamera;

	hook<> onGameInitDone;
	hook<> onGamePreUpdate;
	hook<> onGameUpdate;
	hook<sf::RenderTarget&> onGameDraw;
	hook<sf::RenderTarget&> onGameUiDraw;
	hook<sf::RenderTarget&> onGameWorldDraw;
	hook<> onGameDone;

	unsigned int mouseMode;
	std::shared_ptr<sfTextureResource> mouseTexture;

	bool showFps;
	bool debug;
	bool isInFocus;

	float fps, tps;

	sf::Vector2f mousePos, mouseWorldPos;

	bool worldObjectsFrozen;
	bool conservativeMouse;

	sf::RenderTexture shadowTex;

	// lighting
	sf::Color lightingAmb;

	bool colBoxes;
	bool inMainUpdateLoop;

	float tickScale;

protected:
	void loadWindowSettings(sf::VideoMode resolution = sf::VideoMode::getDesktopMode(), std::string title = "MAGE Game Engine", bool fullscreen = false, bool vsync = false, int maxFps = 0);
	void loadAudioSettings(float masterVolume = 100.f, float sfxVolume = 100.f, float musicVolume = 100.f);

	virtual void setKeybinds();
	virtual void loadResources();

protected:
	registry windowSettings;
	registry audioSettings;

	sf::Clock tickClock;
	sf::Time missedTime;

	sf::Clock magicClock;

	sf::Clock updateTime;

	bool consoleMode;

	bool didRunInit;
	std::string windowTitle;

	std::shared_ptr<loadingScreen> loadScreen;
private:
	sf::RenderWindow* window;
};

MAGEDLL Game* theGame();

#define DeclareGame(typ) int main(int argc, char *argv[]) {\
sf::RenderWindow wind;\
\
typ gm(argc, argv, &wind);\
return  gm.run();\
}
