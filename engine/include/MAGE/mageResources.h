#pragma once
#include "resources.h"

#include "scriptingEngine.h" // sorry

template<class T> class group;
typedef group<basic> Group;

class gameState;

// Scripts are a resource for the following reasons:
// - execution as part of resource loading
// - scripts themselves load resources
// - scripts are loaded from external files
// - scripts are cool

// In the chai engine, use(script) calls the scripting engine's function to do the same thing
// doFile will still load a script anonymously without adding a resource

class MAGEDLL mageScriptResource : public filepathResource
{
public:
	mageScriptResource();
	mageScriptResource(std::string filepath);

	bool load();
	Boxed_Value run();
};

// group resource is simply a saved group object.
// a group is a load of objects.
class MAGEDLL mageGroupResource : public filepathResource
{
public:
	mageGroupResource();
	mageGroupResource(std::string filepath);

	bool load();
	void unload();

	bool isNeeded();

	std::shared_ptr<Group> get();

private:
	std::shared_ptr<Group> rc;
};

// a gameState is a group with extra attributes like the background color, the music etc.
class MAGEDLL mageGameStateResource : public filepathResource
{
public:
	mageGameStateResource();
	mageGameStateResource(std::string filepath);

	bool load();
	void unload();

	bool isNeeded();

	std::shared_ptr<gameState> get();

private:
	std::shared_ptr<gameState> rc;
};

// I would like to be able to save prefabs to files but you can't at the moment (they must be created in scripts)