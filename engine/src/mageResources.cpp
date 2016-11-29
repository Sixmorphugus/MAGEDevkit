#include "mageResources.h"
#include "Game.h"

#include "gameState.h"

// script
mageScriptResource::mageScriptResource()
{}

mageScriptResource::mageScriptResource(std::string filepath)
	: filepathResource(filepath)
{}

bool mageScriptResource::load()
{
	run();

	return resource::load();
}

Boxed_Value mageScriptResource::run()
{
	return theGame()->scripting->evalFile(filepath);
}

// magePrefabResource
mageGroupResource::mageGroupResource()
{}

mageGroupResource::mageGroupResource(std::string filepath)
	: filepathResource(filepath)
{}

bool mageGroupResource::load()
{
	rc = std::make_shared<Group>();
	
	if (!rc->loadFromFile(filepath)) return false;
	return filepathResource::load();
}

void mageGroupResource::unload()
{
	rc = nullptr;
	filepathResource::unload();
}

std::shared_ptr<Group> mageGroupResource::get()
{
	return rc;
}

bool mageGroupResource::isNeeded()
{
	// if this music track isn't referenced anywhere, unload it to save memory.
	// note that this SHOULD be safe - it's impossible to "get" an unloaded resource.
	return (numReferences() > 0);
}

mageGameStateResource::mageGameStateResource()
{}

mageGameStateResource::mageGameStateResource(std::string filepath)
	: filepathResource(filepath)
{}

bool mageGameStateResource::load()
{
	rc = std::make_shared<gameState>();

	if (!rc->loadFromFile(filepath)) return false;
	return filepathResource::load();
}

void mageGameStateResource::unload()
{
	rc = nullptr;
	filepathResource::unload();
}

std::shared_ptr<gameState> mageGameStateResource::get()
{
	return rc;
}

bool mageGameStateResource::isNeeded()
{
	return (numReferences() > 0);
}

// SE Binding

#line 30000

// mageScriptResource
DeclareScriptingFilepathResource(mageScriptResource);
DeclareScriptingFunction(&mageScriptResource::run, "run");

// magGroupResource
DeclareScriptingFilepathResource(mageGroupResource);
DeclareScriptingFunction(&mageGroupResource::get, "get");

// mageGameStateResource
DeclareScriptingFilepathResource(mageGameStateResource);
DeclareScriptingFunction(&mageGameStateResource::get, "get");