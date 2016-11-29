#include "Game.h"

// SUBSYSTEMS
#include "view.h"
#include "gameState.h" // includes group, which includes basic
#include "io.h" // IO stuff
#include "resources.h" // resource stuff
#include "prefabs.h" // prefab stuff
#include "jukebox.h"
#include "mixer.h"
#include "scriptingEngine.h"
#include "schedules.h"

// OBJECTS
#include "tilemap.h"
#include "uiPrimitives.h"
#include "light.h"
#include "physicsObject.h"

// MISC
#include "helpers.h"
#include "constants.h"
#include "sfResources.h"
#include "mageResources.h"

// LE GAME ENGINE
// SIMPLES

Game* globalGame = nullptr;

// CONSTRUCTOR, DESTRUCTOR
// -------------------------------------------------------------------------------------

Game::Game(int argc, char* argv[], sf::RenderWindow* wind):
	window(wind)
{
	if (!globalGame)
		globalGame = this;
	else
		p::fatal("Two instances of the Game core class cannot exist in the same process. Game is a singleton.");

	if (!platform::init()) {
		p::fatal("Platform init failed!\nPlease check that you're running the latest version of the game.");
	}

	debug = false;

	p::log("\nMAGE GAME ENGINE r" + BUILDSTRING + " for " + PLATFORM_STRING + "\nCopyright Deadhand 2016.\n", true);

	p::log("SFML v" + std::to_string(SFML_VERSION_MAJOR) + "." + std::to_string(SFML_VERSION_MINOR) + "." + std::to_string(SFML_VERSION_PATCH), true);
	p::log("CHAISCRIPT v" + std::to_string(chaiscript::version_major) + "." + std::to_string(chaiscript::version_minor) + "." + std::to_string(chaiscript::version_patch) + "\n", true);

	p::log("INITIALIZING", true);
	p::log("------------------------------------------", true);

	// CREATE SUBSYSTEMS
	p::info("STARTING SUBSYSTEMS");

	p::log("\tInput...");
	keyboard = std::make_shared<keyboardMngr>();
	mouse = std::make_shared<mouseMngr>();

	p::log("\tAudio...");
	sound = std::make_shared<mixer>();
	music = std::make_shared<jukebox>();

	p::log("\tResources and Prefabs...");
	resources = std::make_shared<resourceMngr>();
	prefabs = std::make_shared<prefabMngr>();

	p::log("\tGroup Renderers...");
	uiCamera = std::make_shared<uiView>();
	worldCamera = std::make_shared<worldView>();

	p::log("\tScripting...");
	scripting = std::make_shared<scriptingEngine>();

	p::log("\tScheduler...");
	scheduler = std::make_shared<scheduleMngr>();

	// state system is technically a subsystem but SHOWMANSHIP!
	p::info("Creating game world.\n");
	states = std::make_shared<gameStateMngr>();

	// INIT COMPONENTS
	scripting->initChai();

	fps = 0;
	tps = 0;
	missedTime = sf::seconds(0.f);
	isInFocus = true;
	worldObjectsFrozen = false;
	conservativeMouse = true;
	inMainUpdateLoop = false;
	lightingAmb = sf::Color::Transparent;
	colBoxes = SHOWCOLLISIONBOXES;
	showFps = SHOWFPS;
	didRunInit = false;
	tickScale = 1.f;
	state = states->current; // this never changes at the moment so only needs to be set once

	mouseMode = 0; // 0 is no mouse. Other numbers represent various sprite settings for the cursor.

	p::info("Loading settings");

	loadAudioSettings();

	if (window) {
		windowInit(window);
		loadWindowSettings(sf::VideoMode::getDesktopMode(), "MAGE Game Engine (r" + BUILDSTRING + ")");
	}

	// game stoof
	setLoadingScreen(new loadingScreen()); // default basic loading screen drawable

	p::info("Loading keybinds");
	setKeybinds();

	//p::info("Creating prefabs");
	//createPrefabs();

	if (argc > 0 && !(argc % 2)) {
		p::info("Reading launch args");

		for (int i = 0; i < argc; i) {
			std::string argData = argv[i];
			std::string argPeram = argv[i+1];

			p::info("- " + argData + "/" + argPeram);
		}
	}
	else {
		p::info("No valid launch args");
	}

	p::log("\nDONE!", true);

	p::log("\nREADY TO START", true);
	p::log("------------------------------------------", true);
}

Game::~Game()
{
	onGameDone.notify();
	platform::deInit();
}

bool Game::renderWindowExists()
{
	return window;
}

void Game::windowInit(sf::RenderWindow * wind)
{
	window = wind;

	shadowTex.create(getRenderWindow().getSize().x, getRenderWindow().getSize().y);
}

// STARTUP FUNCTIONS (modular design and all that)
// -------------------------------------------------------------------------------------

void Game::setKeybinds() {
	// ADD KEYBINDS HERE
	keyboard->setBind("Screenshot", { sf::Keyboard::LControl, sf::Keyboard::P }, true);
	keyboard->setBind("Grid", { sf::Keyboard::LControl, sf::Keyboard::G }, true);
	keyboard->setBind("Save", { sf::Keyboard::LControl, sf::Keyboard::S }, true);

	keyboard->setBind("Up", sf::Keyboard::W, true);
	keyboard->setBind("Down", sf::Keyboard::S, true);
	keyboard->setBind("Left", sf::Keyboard::A, true);
	keyboard->setBind("Right", sf::Keyboard::D, true);

	keyboard->setBind("Quick Cast", sf::Keyboard::Q, true);
	keyboard->setBind("Interact", sf::Keyboard::E, true);
	keyboard->setBind("Select", sf::Keyboard::Space, true);

	keyboard->setBind("Devmode", sf::Keyboard::F1, true);

	keyboard->setDefaultBindTextures();
	mouse->setDefaultButtonTextures();
}

void Game::loadResources() {
	// registry
	p::info("Creating variable registry...");

	// sound effects used by engine objects
	resources->add("sfx_begin", std::make_shared<sfSoundBufferResource>("base/sfx/enterBattle.wav"), true);
	resources->add("sfx_explosion", std::make_shared<sfSoundBufferResource>("base/sfx/Explosion2.wav"), true);
	resources->add("sfx_dlgDefault", std::make_shared<sfSoundBufferResource>("base/sfx/talk_info.wav"), true);
	resources->add("sfx_damage", std::make_shared<sfSoundBufferResource>("base/sfx/damage.wav"), true);
	resources->add("sfx_menuopen", std::make_shared<sfSoundBufferResource>("base/sfx/menuopen.wav"), true);
	resources->add("sfx_menuclose", std::make_shared<sfSoundBufferResource>("base/sfx/menuclose.wav"), true);
	resources->add("sfx_menuselect", std::make_shared<sfSoundBufferResource>("base/sfx/menuselect.wav"), true);
	resources->add("sfx_menuchoose", std::make_shared<sfSoundBufferResource>("base/sfx/menuchoose.wav"), true);
	resources->add("sfx_menuback", std::make_shared<sfSoundBufferResource>("base/sfx/menuback.wav"), true);

	// undeferred
	resources->add("__fontDef", std::make_shared<sfFontResource>("base/ui/text/PIXEARG_.TTF"), true);
	resources->add("__splash", std::make_shared<sfTextureResource>("base/ui/splash.png"), true);

	// generic engine stuff
	resources->add("mouse", std::make_shared<sfTextureResource>("base/ui/mouse.png"));

	// ui required by legacy classes
	resources->add("ui_title", std::make_shared<sfTextureResource>("base/ui/title2.png"));
	resources->add("ui_title_small", std::make_shared<sfTextureResource>("base/ui/title.png"));
	resources->add("ui_info_image", std::make_shared<sfTextureResource>("base/ui/infoimage.png"));

	resources->add("ui_block_base", std::make_shared<sfTextureResource>("base/ui/block/base.png"));

	resources->add("ui_notfound", std::make_shared<sfTextureResource>("base/ui/nosprite.png"));

	// lazyload ui
	resources->addFolder<sfTextureResource>("ui_input_", "base/ui/bindimages");

	// shaders
	resources->add("interactiveObject", std::make_shared<sfShaderResource>("base/shaders/outline.glsl", sf::Shader::Fragment));
	resources->add("desaturation", std::make_shared<sfShaderResource>("base/shaders/grayscale.glsl", sf::Shader::Fragment));

	// MAIN SCRIPT
	resources->add("main", std::make_shared<mageScriptResource>("resources/main.chai"), true);

	resources->update();
}

// PROCESS FUNCTIONS
// -------------------------------------------------------------------------------------

void Game::draw() {
	// GAME VIEW
	worldCamera->render(getRenderWindow(), state->bgCol);
	onGameWorldDraw.notify(getRenderWindow());

	// UI VIEW
	uiCamera->render(getRenderWindow());
	onGameUiDraw.notify(getRenderWindow());

	// draw in the fps
	if (showFps) {
		getRenderWindow().setView(*uiCamera.get());
		pDrawInfo(getRenderWindow(), sf::RenderStates(), sf::Vector2f(4.f, 4.f),
			"Multipurpose Arcade Game Engine r" + BUILDSTRING + "\n" +
			"FPS: " + std::to_string(fps) + "\n" +
			"TPS: " + std::to_string(tps) + "\n" +
			"Missed: " + std::to_string(missedTime.asMilliseconds()) + "ms\n" +
			"Objects: " + std::to_string(state->getNumObjects()),
			sf::Color::White, false, 8U);
	}

	// draw in the mouse
	if (mouseMode > 0 && mouseTexture) {
		// mouse textures are ALWAYS assumed to be 32x32 and in a row that is as long as it needs to be.
		// they're not rectangularized in the normal way.
		sf::IntRect mouseTexRect(32 * (mouseMode-1), 0, 32, 32);

		if (!mouseTexture->isLoaded()) {
			p::fatal("Somehow lost the mouse texture!\nDamn it. I know I left it here somewhere....");
		}
		else {
			sf::Sprite mouseS(*mouseTexture->get()); // I would use vertices but LAZYYY
			mouseS.setPosition(mousePos);
			mouseS.setTextureRect(mouseTexRect);
			mouseS.setOrigin(16, 16);

			getRenderWindow().draw(mouseS);
		}
	}

	onGameDraw.notify(getRenderWindow());

	getRenderWindow().display();
}

void Game::update(sf::Time elapsed) {
	p::update();

	music->update();
	sound->update();
	scripting->update();

	// check for console commands
	std::string in;
	if (p::getConsoleInput(in)) {
		if (in != "") {
			try {
				scripting->eval(in);
			}
			catch (chaiscript::exception::eval_error &e) {
				handleEvalError(e);
			}
			catch (std::runtime_error &e) {
				p::warn(e.what());
			}
			catch (...) {
				p::warn("Unknown error. Please never do that again.");
			}
		}
	}

	if (keyboard->getBind("Screenshot")->getPressed()) {
		sf::Texture windowTex;
		windowTex.update(*window);

		sf::Image img = windowTex.copyToImage();

		p::info("Saving as \"screenshots/shot" + std::to_string(time(0)) + ".png\"");
		if (!img.saveToFile("screenshots/shot" + std::to_string(time(0)) + ".png")) {
			p::info("Saving failed!");
		}
		else {
			p::info("Success!");
		}
	}

	// process game inputs too
	//if (keyboard.getBind("ESCAPE")->getPressed())
	//	window.close();

	// reorder the draw list
	state->sortWorldObjects();

	inMainUpdateLoop = true;

	// everything else is passed down to objects
	state->preUpdateObjects(elapsed);
	onGamePreUpdate.notify();

	state->updateObjects(elapsed);
	onGameUpdate.notify();

	inMainUpdateLoop = false;

	// mouse
	mousePos = uiCamera->relativePosition(mouse->position);
	mouseWorldPos = worldCamera->relativePosition(mouse->position);

	// io
	mouse->update();
	keyboard->update();

	// finish
	debug = true;

	mouseTexture = resources->getAs<sfTextureResource>("mouse");
}

void Game::drawLoadingScreenFrame(std::string text, int prog, int maxProg) {
	if (!window)
		return;

	getRenderWindow().setView(*uiCamera);
	uiCamera->setPosition(sf::Vector2f(0, 0));

	getRenderWindow().clear(state->bgCol);
	drawLoadingScreenFrameToTarget(getRenderWindow(), sf::RenderStates(), text, prog, maxProg);
	getRenderWindow().display();
}

void Game::drawLoadingScreenFrameToTarget(sf::RenderTarget &targ, sf::RenderStates states, std::string text, int prog, int maxProg) {
	if (loadScreen) {
		if (loadScreen->canDraw()) {
			loadScreen->count = prog;
			loadScreen->countMax = maxProg;
			loadScreen->loading = text;

			targ.draw(*loadScreen, states);
		}
	}
}

void Game::setLoadingScreen(loadingScreen* ls) {
	loadScreen = std::shared_ptr<loadingScreen>(ls);
}

std::shared_ptr<loadingScreen> Game::getLoadingScreen()
{
	return loadScreen;
}

int Game::run(bool tickByTick) {
	if (!didRunInit) {
		p::info("Loading resources");
		loadResources();

		onGameInitDone.notify();

		tickClock.restart();

		didRunInit = true;

		updateTime.restart();
	}

	bool stopLooping = false;
	sf::Clock frameTimer;

	try {
		while (getRenderWindow().isOpen() && !stopLooping)
		{
			resources->update();
			frameTimer.restart();

			draw();

			missedTime += tickClock.restart();

			if (tickScale <= 0) {
				p::warn("NO.");
				tickScale = 1.f;
			}

			sf::Time magicMs = sf::milliseconds(TICKMS * tickScale);

			while (missedTime >= magicMs) {
				if (missedTime >= magicMs * 10.f) {
					int mt = missedTime / magicMs;

					//p::warn("Can't keep up! " + std::to_string(missedTime.asMilliseconds()) + "ms behind, skipping " + std::to_string(mt) + " ticks");
					missedTime = magicMs * 10.f;
				}

				tps = floor(1.f / updateTime.getElapsedTime().asSeconds());

				missedTime -= magicMs;
				update(updateTime.restart());

				sf::Event evt;
				while (getRenderWindow().pollEvent(evt)) {
					if (isInFocus) {
						mouse->processEvent(evt);
						keyboard->processEvent(evt);
					}

					if (evt.type == sf::Event::LostFocus) {
						isInFocus = false;
						getRenderWindow().setMouseCursorVisible(true);
					}

					if (evt.type == sf::Event::GainedFocus) {
						isInFocus = true;
						getRenderWindow().setMouseCursorVisible(false);
					}

					if (evt.type == sf::Event::Closed) {
						getRenderWindow().close();
					}
				}
			}

			fps = floor(1.f / frameTimer.getElapsedTime().asSeconds());

			if (tickByTick) {
				stopLooping = true;
			}
		}
	}
	catch (std::runtime_error &e) {
		p::fatal("Fatal runtime error: " + std::string(e.what()));
	}

	return 0;
}

void Game::loadWindowSettings(sf::VideoMode resolution, std::string title, bool fullscreen, bool vsync, int maxFps) {
	windowSettings.init("config/window.cfg");
	windowSettings.header = "Generally self-explanatory window settings file.";

	windowSettings.add(registry::prop("resolution", std::to_string(resolution.width) + "x" + std::to_string(resolution.height)));
	windowSettings.add(registry::prop("fullscreen", fullscreen));
	windowSettings.add(registry::prop("vsync", vsync));
	windowSettings.add(registry::prop("maxFps", maxFps));

	windowSettings.load();

	auto vm = splitString(windowSettings.get("resolution")->as<std::string>(), 'x');

	setWindowSettings(sf::VideoMode(atof(vm[0].c_str()), atof(vm[1].c_str())),
		title,
		windowSettings.get("fullscreen")->as<bool>(),
		windowSettings.get("vsync")->as<bool>(),
		windowSettings.get("maxFps")->as<float>());
}

void Game::setWindowSettings(sf::VideoMode resolution, std::string title, bool fullscreen, bool vsync, int maxFps) {
	if(fullscreen != isFullscreen() || sf::Vector2u(resolution.width, resolution.height) != getRenderWindow().getSize())
	getRenderWindow().create(resolution, title, fullscreen ? sf::Style::Fullscreen : sf::Style::Titlebar);

	if (vsync != isVsyncEnabled())
		getRenderWindow().setVerticalSyncEnabled(vsync);

	if (maxFps != getFramerateLimit())
		getRenderWindow().setFramerateLimit(maxFps);

	if (vsync)
		getRenderWindow().setFramerateLimit(0);

	getRenderWindow().setTitle(title);
	windowTitle = title;

	if (!getRenderWindow().isOpen()) {
		p::fatal("Failed to apply settings to the window.\n\nIf you now get this every time you start the game, delete window.cfg in the config folder.\nIf this is your first attempt starting the game, refer to online help.");
	}

	p::info("Window settings:");
	p::log("\tresolution: " + std::to_string(resolution.width) + "x" + std::to_string(resolution.height));
	p::log("\ttitle: " + title);
	p::log("\tfullscreen: " + std::string(fullscreen ? "YES" : "NO"));
	p::log("\tvsync: " + std::string(vsync ? "YES" : "NO"));
	p::log("\tfpslimit: " + std::to_string(maxFps) + "fps" + (maxFps <= 30 && maxFps != 0 ? " cinematic experience" : ""));

	getRenderWindow().setMouseCursorVisible(false);

	// fix views
	fixViews();

	// save settings
	p::info("Saving window config.");

	windowSettings.get("resolution")->update(std::to_string(getRenderWindow().getSize().x) + "x" + std::to_string(getRenderWindow().getSize().y));
	windowSettings.get("fullscreen")->update(fullscreen);
	windowSettings.get("vsync")->update(vsync);
	windowSettings.get("maxFps")->update(maxFps);

	shadowTex.create(getRenderWindow().getSize().x, getRenderWindow().getSize().y);
}

void Game::loadAudioSettings(float masterVolume, float sfxVolume, float musicVolume)
{
	audioSettings = registry();
	audioSettings.init("config/audio.cfg");
	audioSettings.header = "Self-explanitory audio settings file.";

	audioSettings.add(registry::prop("masterVolume", masterVolume));
	audioSettings.add(registry::prop("sfxVolume", sfxVolume));
	audioSettings.add(registry::prop("musicVolume", musicVolume));

	audioSettings.load(); // loads existing settings over the ones we set above if there are any

	setAudioSettings(audioSettings.get("masterVolume")->as<float>(),
		audioSettings.get("sfxVolume")->as<float>(),
		audioSettings.get("musicVolume")->as<float>()
	);
}

void Game::setAudioSettings(float masterVolume, float sfxVolume, float musicVolume)
{
	music->volume = musicVolume;
	sound->volume = sfxVolume;
	sf::Listener::setGlobalVolume(masterVolume);

	p::info("Audio settings:");
	p::log("\tmaster volume: " + std::to_string(masterVolume));
	p::log("\tsfx volume: " + std::to_string(sfxVolume));
	p::log("\tmusic volume: " + std::to_string(musicVolume));

	// save settings
	audioSettings.get("masterVolume")->update(masterVolume);
	audioSettings.get("sfxVolume")->update(sfxVolume);
	audioSettings.get("musicVolume")->update(musicVolume);
}

void Game::fixViews()
{
	auto wSize = sf::Vector2f(getRenderWindow().getSize().x, getRenderWindow().getSize().y);

	//p::info(std::to_string(wSize.x));

	worldCamera->setBaseSize(wSize);
	uiCamera->setBaseSize(wSize);

	uiCamera->setPosition(sf::Vector2f(0, 0));

	worldCamera->group = state;
	uiCamera->group = state;
}

float Game::getDelta()
{
	return tickClock.getElapsedTime().asMilliseconds() / TICKMS;
}

objBasic::collision Game::resolveMovement(objBasic* obj, int byX, int byY) {
	objBasic::collision cDat; // c dat?

	cDat.main = obj;

	cDat.hitX = false;
	cDat.hitY = false;

	if (byX == 0 && byY == 0) {
		return cDat; // nothing will result from this
	}

	obj->move(sf::Vector2f((float)byX, (float)byY));

	std::vector<sf::FloatRect> colliders = getCollidingBoxes(obj);
	cDat.involved = getCollidingObjects(obj);

	obj->move((float)-byX, (float)-byY);

	bool doX = true;
	bool doY = true;

	bool xCollision = false;
	bool yCollision = false;

	sf::Vector2f prevPos;

	while(doX || doY) {
		if (doX) {
			prevPos = obj->getPosition();

			if (byX < 0) {
				byX += 1;
				obj->move(-1.f, 0.f);
			}
			else if (byX > 0) {
				byX -= 1;
				obj->move(1.f, 0.f);
			}

			std::vector<sf::FloatRect> colliders2 = getCollidingBoxes(obj);

			if (colliders2.size() > 0) {
				doX = false;
				obj->setPosition(prevPos);

				xCollision = true;
			}

			if (byX == 0) {
				doX = false;
			}
		}
		if (doY) {
			prevPos = obj->getPosition();

			if (byY < 0) {
				byY += 1;
				obj->move(0.f, -1.f);
			}
			else if (byY > 0) {
				byY -= 1;
				obj->move(0.f, 1.f);
			}

			std::vector<sf::FloatRect> colliders2 = getCollidingBoxes(obj);

			if (colliders2.size() > 0) {
				doY = false;
				obj->setPosition(prevPos);

				yCollision = true;
			}

			if (byY == 0) {
				doY = false;
			}
		}
	}

	cDat.hitX = xCollision;
	cDat.hitY = yCollision;

	return cDat;
}

bool Game::hasCollisions(objBasic* obj) {
	// check each of this object's collision boxes...
	int detected = 0;

	for (unsigned int i = 0; i < obj->collisionBoxes.size(); i++) {
		// against every other object's collision boxes...
		for (unsigned int j = 0; j < state->getNumWorldObjects(); j++) {
			if (state->getWorldObject(j).get() == obj)
				continue;

			for (unsigned int k = 0; k < state->getWorldObject(j)->collisionBoxes.size(); k++) {
				if (obj->transformedCollisionBox(i).intersects(state->getWorldObject(j)->transformedCollisionBox(k)))
					detected++;
			}
		}
	}

	return (detected > 0);
}

std::vector<std::shared_ptr<objBasic>> Game::getCollidingObjects(objBasic* obj) {
	// check each of this object's collision boxes...
	std::vector<std::shared_ptr<objBasic>> cols;

	for (unsigned int i = 0; i < obj->collisionBoxes.size(); i++) {
		// ...against every other object's collision boxes.
		for (unsigned int j = 0; j < state->getNumWorldObjects(); j++) {
			if (state->getWorldObject(j).get() == obj)
				continue;

			for (unsigned int k = 0; k < state->getWorldObject(j)->collisionBoxes.size(); k++) {
				if (obj->transformedCollisionBox(i).intersects(state->getWorldObject(j)->transformedCollisionBox(k))) {
					cols.push_back(state->getWorldObject(j));
					break;
				}
			}
		}
	}

	return cols;
}

std::vector<sf::FloatRect> Game::getCollidingBoxes(objBasic * obj)
{
	// check each of this object's collision boxes...
	std::vector<sf::FloatRect> cols;

	for (unsigned int i = 0; i < obj->collisionBoxes.size(); i++) {
		// ...against every other object's collision boxes.
		for (unsigned int j = 0; j < state->getNumWorldObjects(); j++) {
			if (state->getWorldObject(j).get() == obj)
				continue;

			for (unsigned int k = 0; k < state->getWorldObject(j)->collisionBoxes.size(); k++) {
				if (obj->transformedCollisionBox(i).intersects(state->getWorldObject(j)->transformedCollisionBox(k))) {
					cols.push_back(state->getWorldObject(j)->transformedCollisionBox(k));
					break;
				}
			}
		}
	}

	// also test against the tile collision layer and world edges
	sf::FloatRect northEdge(sf::Vector2f(state->mapBounds.left, state->mapBounds.top),
		sf::Vector2f(state->mapBounds.width, 2.f));
	
	sf::FloatRect eastEdge(sf::Vector2f(state->mapBounds.left + state->mapBounds.width, state->mapBounds.top),
		sf::Vector2f(2.f, state->mapBounds.height));

	sf::FloatRect southEdge(sf::Vector2f(state->mapBounds.left, state->mapBounds.top + state->mapBounds.height),
		sf::Vector2f(state->mapBounds.width, 2.f));

	sf::FloatRect westEdge(sf::Vector2f(state->mapBounds.left, state->mapBounds.top),
		sf::Vector2f(2.f, state->mapBounds.height));

	for (unsigned int i = 0; i < obj->collisionBoxes.size(); i++) {
		if (obj->transformedCollisionBox(i).intersects(northEdge)) {
			cols.push_back(northEdge);
		}
		if (obj->transformedCollisionBox(i).intersects(eastEdge)) {
			cols.push_back(eastEdge);
		}
		if (obj->transformedCollisionBox(i).intersects(southEdge)) {
			cols.push_back(southEdge);
		}
		if (obj->transformedCollisionBox(i).intersects(westEdge)) {
			cols.push_back(westEdge);
		}
	}

	return cols;
}

std::vector<std::shared_ptr<objBasic>> Game::findObjectsAt(sf::Vector2f pos)
{
	// check the pos...
	std::vector<std::shared_ptr<objBasic>> cols;

	// ...against every object's sprite box
	for (unsigned int i = 0; i < state->getNumWorldObjects(); i++) {
		sf::FloatRect spriteBox(state->getWorldObject(i)->sfSprite.getGlobalBounds());

		if (spriteBox.contains(pos)) {
			cols.push_back(state->getWorldObject(i));
		}
	}

	return cols;
}

std::vector<std::shared_ptr<uiBasic>> Game::findUiAt(sf::Vector2f pos)
{
	// check the pos...
	std::vector<std::shared_ptr<uiBasic>> cols;

	// ...against every ui's sprite box
	for (unsigned int i = 0; i < state->getNumUiObjects(); i++) {
		sf::FloatRect spriteBox(state->getUiObject(i)->getMainBounds());

		// (actual test here)
		if (spriteBox.contains(pos)) {
			cols.push_back(state->getUiObject(i));
		}
	}

	return cols;
}

std::vector<std::shared_ptr<sfTextureResource>> Game::getKeybindTextures(std::string bName)
{
	auto textures = keyboard->getBindTextures(bName);

	std::vector<std::shared_ptr<sfTextureResource>> list;

	for (unsigned int i = 0; i < textures.size(); i++) {
		list.push_back(resources->getAs<sfTextureResource>(textures[i]));
	}

	return list;
}

void Game::physicsImpulse(sf::Vector2f position, float force, float radius, std::vector<objBasic*> exempt)
{
	for (unsigned int i = 0; i < state->getNumWorldObjects(); i++) {
		objBasic* obj = state->getWorldObject(i).get();
		physicsObject* p = dynamic_cast<physicsObject*>(obj);

		bool isExempt = false;

		for (unsigned int j = 0; j < exempt.size(); j++) {
			if (exempt[j] == state->getWorldObject(i).get()) {
				isExempt = true;
			}
		}

		if (p && !isExempt) {
			p->impulse(position, force, radius);
		}
	}
}

sf::Vector2f Game::findTextSize(std::string text, unsigned int fontScale)
{
	// just create text and measure it up
	sf::Font &dFont = *resources->getAs<sfFontResource>("__fontDef")->get();
	sf::Text t(text, dFont, fontScale);
	t.setScale(.5f, .5f);

	return sf::Vector2f(t.getGlobalBounds().width, t.getGlobalBounds().height);
}

int Game::version()
{
	return BUILD;
}

// HELPER FUNCTIONS
// -------------------------------------------------------------------------------------

sf::VideoMode Game::getScreenResolution()
{
	// resolution (the hard one)
	// it's stored as a string consisting of 2 numbers with an x between
	auto resString = windowSettings.get("resolution")->read();
	auto resStringSplit = splitString(resString, 'x', ' ', 2);

	sf::VideoMode wSize(atoi(resStringSplit[0].c_str()), atoi(resStringSplit[1].c_str()));

	return wSize;
}

std::string Game::getWindowTitle()
{
	// for some reason sfml can't get this.
	// luckily we have it stored.
	return windowTitle;
}

bool Game::isFullscreen()
{
	return windowSettings.get("fullscreen")->as<bool>();
}

bool Game::isVsyncEnabled()
{
	return windowSettings.get("vsync")->as<bool>();
}

int Game::getFramerateLimit() {
	return windowSettings.get("maxFps")->as<int>();
}

void Game::setScreenResolution(sf::VideoMode res)
{
	setWindowSettings(res, getWindowTitle(), isFullscreen(), isVsyncEnabled(), getFramerateLimit());
}

void Game::setFullscreen(bool tf)
{
	setWindowSettings(getScreenResolution(), getWindowTitle(), tf, isVsyncEnabled(), getFramerateLimit());
}

void Game::setVsyncEnabled(bool tf)
{
	setWindowSettings(getScreenResolution(), getWindowTitle(), isFullscreen(), tf, getFramerateLimit());
}

void Game::setFramerateLimit(int fpsl) 
{
	setWindowSettings(getScreenResolution(), getWindowTitle(), isFullscreen(), isVsyncEnabled(), fpsl);
}

void Game::setWindowTitle(std::string t)
{
	setWindowSettings(getScreenResolution(), t, isFullscreen(), isVsyncEnabled(), getFramerateLimit());
}

sf::Time Game::getSimTime()
{
	return magicClock.getElapsedTime();
}

sf::RenderWindow& Game::getRenderWindow()
{
	if (window == nullptr) {
		p::fatal("A rendering window is required to continue execution.");
	}

	return *window;
}

Game * theGame()
{
	return globalGame;
}
