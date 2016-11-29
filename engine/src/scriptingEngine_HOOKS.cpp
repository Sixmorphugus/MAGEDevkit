#include "scriptingEngine.h"

#include "Game.h"
#include "physicsObject.h"
#include "uiPrimitives.h"

#include "io.h"

using namespace chaiscript;

#define HOOK(name, ...) \
chai->add(user_type< hook<__VA_ARGS__> >(), name); \
chai->add(constructor<hook<__VA_ARGS__>()>(), name); \
chai->add(fun(&hook<__VA_ARGS__>::registerObserver), "registerObserver"); \
chai->add(fun(&hook<__VA_ARGS__>::forgetObserver), "forgetObserver"); \
chai->add(fun(&hook<__VA_ARGS__>::clearObservers), "clearObservers"); \
chai->add(fun(&hook<__VA_ARGS__>::notify), "notify");

// game scripting hooks
void scriptingEngine::bindHooks()
{
	// Global hooks
	HOOK("hook");

	chai->add_global(var(std::ref(theGame()->onGameDone)), "onGameDone");
	chai->add_global(var(std::ref(theGame()->onGameInitDone)), "onGameInitDone");
	chai->add_global(var(std::ref(theGame()->onGamePreUpdate)), "onGamePreUpdate");
	chai->add_global(var(std::ref(theGame()->onGameUpdate)), "onGameUpdate");
	chai->add_global(var(std::ref(theGame()->onGameUiDraw)), "onGameUiDraw");
	chai->add_global(var(std::ref(theGame()->onGameDraw)), "onGameDraw");

	// mouse hooks
	HOOK("mouseHook", mouseMngr*);

	chai->add(fun(&mouseMngr::onLeftPressed), "onLeftPressed");
	chai->add(fun(&mouseMngr::onLeftReleased), "onLeftReleased");
	chai->add(fun(&mouseMngr::onMiddlePressed), "onMiddlePressed");
	chai->add(fun(&mouseMngr::onMiddleReleased), "onMiddleReleased");
	chai->add(fun(&mouseMngr::onRightPressed), "onRightPressed");
	chai->add(fun(&mouseMngr::onRightReleased), "onRightReleased");

	// keybind hooks
	HOOK("keyboardHook", keyboardMngr*);

	chai->add(fun(&keyboardMngr::keybind::onPressed), "onPressed");
	chai->add(fun(&keyboardMngr::keybind::onReleased), "onReleased");

	// basic object hooks
	HOOK("objectHook", basic*);
	HOOK("objectDrawHook", const basic*, sf::RenderTarget*);
	HOOK("objectInteractionHook", basic*, basic*);
	HOOK("objectSelectionHook", basic*, unsigned int);

	chai->add(fun(&basic::onDrawn), "onDrawn");
	chai->add(fun(&basic::onPreUpdate), "onPreUpdate");
	chai->add(fun(&basic::onUpdate), "onUpdate");
	chai->add(fun(&basic::onCreated), "onCreated");
	chai->add(fun(&basic::onDestroyed), "onDestroyed");
	chai->add(fun(&basic::onFirstUpdate), "onFirstUpdate");

	chai->add(fun(&uiBasic::onWorldDraw), "onWorldDraw");

	chai->add(fun(&physicsObject::onImpact), "onImpact");
	chai->add(fun(&physicsObject::onImpulse), "onImpulse");

	chai->add(fun(&menu::onDismissed), "onDismissed");
	chai->add(fun(&menu::onSelectionChanged), "onSelectionChanged");

	chai->add(fun(&textEntry::onReady), "onReady");
}
