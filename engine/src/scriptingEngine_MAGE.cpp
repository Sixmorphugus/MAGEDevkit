#include "scriptingEngine.h"

#include "animationManager.h"
#include "mixer.h"
#include "jukebox.h"
#include "physicsObject.h"
#include "uiPrimitives.h"
#include "light.h"
#include "tilemap.h"
#include "zone.h"
#include "gameState.h"

#include "helpers.h"

#include "Game.h"

ModulePtr sePreBoundObjects;

void sePreBoundCreatedCheck() {
	if (sePreBoundObjects.get() == nullptr) {
		sePreBoundObjects = ModulePtr(new Module());
	}
}

#define BIND_CONVERSIONS(obj) chai->add(base_class<basic, obj>());\
chai->add(base_class<sf::Transformable, obj>());\
BIND_COPY_OPERATOR(obj)

#define PLACEHOLDER_BIND(obj, base, name) chai->add(user_type<obj>(), name);  chai->add(base_class<base, obj>()); BIND_CONVERSIONS(obj);
#define PLACEHOLDER_BIND_C(obj, base, name) PLACEHOLDER_BIND(obj, base, name); chai->add(constructor<obj(float, float, basic::textureData)>(), name);

// remote binding nightmare fuel
// (REMINDER: IN C LANGUAGES THIS ACTUALLY WORKS)
seScriptingEngineRegistration::seScriptingEngineRegistration(Type_Info ti, std::string name)
{
	sePreBoundCreatedCheck();
	sePreBoundObjects->add(ti, name);
}

seScriptingEngineRegistration::seScriptingEngineRegistration(Type_Conversion d)
{
	sePreBoundCreatedCheck();
	sePreBoundObjects->add(d);
}

seScriptingEngineRegistration::seScriptingEngineRegistration(Proxy_Function f, std::string name)
{
	sePreBoundCreatedCheck();
	sePreBoundObjects->add(f, name);
}

seScriptingEngineRegistration::seScriptingEngineRegistration(const Boxed_Value t_bv, std::string t_name)
{
	sePreBoundCreatedCheck();
	sePreBoundObjects->add_global_const(t_bv, t_name);
}

seScriptingEngineRegistration::seScriptingEngineRegistration(ModulePtr vt)
{
	sePreBoundCreatedCheck();
	sePreBoundObjects->add(vt);
}

// Chaiscript MAGE Binding
void scriptingEngine::bindMAGE()
{
	// Attach the game engine
	// the philosophy here is that we're attaching AS the game object - objects attached to the game become globals in the scripting engine

	// SFML objects
	chai->add_global(var(std::ref(theGame()->lightingAmb)), "lightingAmbientColor");
	chai->add_global(var(std::ref(theGame()->mouseMode)), "mouseMode");
	chai->add_global(var(theGame()->mouseTexture), "mouseTexture");
	chai->add_global(var(std::ref(theGame()->debug)), "debug");
	chai->add_global(var(std::ref(theGame()->isInFocus)), "gameIsInFocus");
	chai->add_global(var(std::ref(theGame()->worldObjectsFrozen)), "worldObjectsFrozen");
	chai->add_global(var(std::ref(theGame()->mousePos)), "mouseScreenPosition");
	chai->add_global(var(std::ref(theGame()->mouseWorldPos)), "mouseWorldPosition");
	chai->add_global(var(std::ref(theGame()->conservativeMouse)), "conservativeMouse");
	chai->add_global(var(std::ref(theGame()->shadowTex)), "lightingSurface");
	chai->add_global(var(std::ref(theGame()->colBoxes)), "colBoxes");
	chai->add_global(var(std::ref(theGame()->showFps)), "showFps");
	chai->add_global(var(std::ref(theGame()->tickScale)), "tickScale");

	// subsystems
	chai->add_global(var(theGame()->prefabs), "prefabs");
	chai->add_global(var(theGame()->mouse), "mouse");
	chai->add_global(var(theGame()->keyboard), "keyboard");
	chai->add_global(var(theGame()->resources), "resources");
	chai->add_global(var(theGame()->scheduler), "scheduler");
	chai->add_global(var(theGame()->music), "music");
	chai->add_global(var(theGame()->sound), "sound");
	chai->add_global(var(theGame()->states), "states");
	chai->add_global(var(theGame()->states->current), "state"); // don't get these two mixed up

	chai->add_global(var(theGame()->worldCamera), "worldCamera");
	chai->add_global(var(theGame()->uiCamera), "uiCamera");

	chai->add_global(var(theGame()), "theGame");

	// Object bindings registered before main() in the memory.
	chai->add(sePreBoundObjects);

	// player binding is in player.h now

	// TODO:
	PLACEHOLDER_BIND(light, objBasic, "light");
	PLACEHOLDER_BIND(zone, objBasic, "zone");

	// platform.h
	chai->add(fun(&platform::clearConsoleInput), "clearConsoleInput");
	chai->add(fun(&platform::fileDelete), "fileDelete");
	chai->add(fun(&platform::dirList), "dirList");
	chai->add(fun(&platform::realPath), "realPath");
	chai->add(fun(&platform::showMessageBox), "showMessageBox");
	chai->add(fun(&platform::writeToConsole), "writeToConsole");
	chai->add(fun(&platform::writeToLogfile), "writeToLogfile");

	// game functions (lambdas)
	chai->add(fun([&]() -> sf::RenderWindow& { return theGame()->getRenderWindow(); }), "getRenderWindow");
	chai->add(fun([&]() { return theGame()->renderWindowExists(); }), "renderWindowExists");
	chai->add(fun([&](sf::Vector2f p) { return theGame()->findObjectsAt(p); }), "findObjectsAt");
	chai->add(fun([&](sf::Vector2f p) { return theGame()->findUiAt(p); }), "findUiAt");
	chai->add(fun([&]() { theGame()->fixViews(); }), "fixViews");
	chai->add(fun([&](objBasic* o) { return theGame()->getCollidingBoxes(o); }), "getCollidingBoxes");
	chai->add(fun([&](objBasic* o) { return theGame()->getCollidingObjects(o); }), "getCollidingObjects");
	chai->add(fun([&]() { return theGame()->getDelta(); }), "getDelta");
	chai->add(fun([&]() { return theGame()->getFramerateLimit(); }), "getFramerateLimit");
	chai->add(fun([&]() { return theGame()->getWindowTitle(); }), "getWindowTitle");
	chai->add(fun([&]() { return theGame()->getScreenResolution(); }), "getScreenResolution");
	chai->add(fun([&](std::string s) { return theGame()->getKeybindTextures(s); }), "getKeybindTextures");
	chai->add(fun([&](objBasic* o) { return theGame()->hasCollisions(o); }), "hasCollisions");
	chai->add(fun([&]() { return theGame()->isFullscreen(); }), "isFullscreen");
	chai->add(fun([&]() { return theGame()->isVsyncEnabled(); }), "isVsyncEnabled");
	chai->add(fun([&](sf::Vector2f p, float f, float r, std::vector<objBasic*> exempt) { theGame()->physicsImpulse(p, f, r, exempt); }), "physicsImpulse");
	chai->add(fun([&](objBasic* o, int byX, int byY) { theGame()->resolveMovement(o, byX, byY); }), "resolveMovement");
	chai->add(fun([&](float master, float sfx, float music) { theGame()->setAudioSettings(master, sfx, music); }), "setAudioSettings");
	chai->add(fun([&](sf::VideoMode res, std::string title, bool fs, bool vsync, unsigned int mFps) { theGame()->setWindowSettings(res, title, fs, vsync, mFps); }), "setWindowSettings");
	chai->add(fun([&]() { return theGame()->getSimTime(); }), "getSimTime");
	chai->add(fun([&]() { theGame()->scripting->dump(); }), "dump");
	chai->add(fun([&](Boxed_Value& input) { theGame()->scripting->dump(&input); }), "dump");
	chai->add(fun([&](Boxed_Value& input) { theGame()->scripting->whatIs(&input); }), "whatIs");
	chai->add(fun([&]() { theGame()->getRenderWindow().close(); }), "quit");

	chai->add(fun([&](int fpsl) { theGame()->setFramerateLimit(fpsl); }), "setFramerateLimit");
	chai->add(fun([&](bool in) { theGame()->setFullscreen(in); }), "setFullscreen");
	chai->add(fun([&](bool in) { theGame()->setVsyncEnabled(in); }), "setVsyncEnabled");
	chai->add(fun([&](std::string in) { theGame()->setWindowTitle(in); }), "setWindowTitle");
	chai->add(fun([&](sf::VideoMode in) { theGame()->setScreenResolution(in); }), "setScreenResolution");

	chai->add(fun([&](std::string txt, int prog, int maxProg) { return theGame()->drawLoadingScreenFrame(txt, prog, maxProg); }), "drawLoadingScreenFrame");
}