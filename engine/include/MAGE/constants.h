#pragma once
#include "build.h"

// modifiable
#define TICKRATE 60 // how many times the game will (try to) update a second
#define SHOWCOLLISIONBOXES false // whether to show collision boxes or not.
#define SHOWFPS false // whether to show the FPS, TPS and other stuff.
#define OPENREGISTRY true // whether to label the main registry file so you can see what's saved.
#define PLATFORM_LOGINFOALLOWED true
#define CRASHWHENEVALFAILS false
#define EDITORALLOWED true
#define ZOOMDEFAULT 0.5f

// dependent (do not edit)
#define BUILDSTRING std::to_string(BUILD)
#define TICKMS (1000 / TICKRATE)
