#include "gameState.h"

#include "sfResources.h"
#include "mageResources.h"
#include "Game.h"

// GAME STATE MANAGER IMPLEMENTATION
gameStateMngr::gameStateMngr()
{
	current = std::make_shared<gameState>();

	// cool default state stuff?
	// naaaaah
}

void gameStateMngr::add(std::string name, std::shared_ptr<gameState> input)
{
	if (exists(name)) {
		p::warn("State already exists with name \"" + name + "\", replacing");
	}

	stateCache[name] = input;
}

bool gameStateMngr::exists(std::string name)
{
	return stateCache.count(name) > 0;
}

std::string gameStateMngr::nameOf(std::shared_ptr<gameState> input)
{
	// iteration
	for (auto i = stateCache.begin(); i != stateCache.end(); i++) {
		if (i->second == input) {
			return i->first;
		}
	}

	return "";
}

void gameStateMngr::setCurrent(std::string name)
{
	setCurrent<>(name);
}

void gameStateMngr::combineCurrent(std::string name)
{
	combineCurrent<>(name);
}

std::shared_ptr<gameState> gameStateMngr::get(std::string name)
{
	if (exists(name))
		return stateCache[name];
	else
		return nullptr;
}

std::shared_ptr<gameState> gameStateMngr::getByIndex(unsigned int index)
{
	if (index >= stateCache.size())
		return nullptr;

	auto curState = stateCache.begin();

	for (unsigned int i = 0; i < index; i++) {
		curState++;
	}

	return curState->second;
}

unsigned int gameStateMngr::getCount()
{
	return stateCache.size();
}

std::vector<std::shared_ptr<gameState>> gameStateMngr::list(std::string tag)
{
	std::vector<std::shared_ptr<gameState>> results;

	// now we must iterate
	for (auto i = stateCache.begin(); i != stateCache.end(); i++) {
		if (i->second->hasTag(tag)) {
			results.push_back(i->second);
		}
	}

	return results;
}

// GAME STATE IMPLEMENTATION
gameState::gameState()
{
	setVarDefaults();
}

gameState::gameState(Group & gr)
{
	setVarDefaults();
	group::set<basic, basic>(&gr);
}

gameState::gameState(std::shared_ptr<mageGroupResource> gr)
{
	setVarDefaults();

	if (!gr)
		return;

	if (!gr->isLoaded()) {
		p::warn("Unable to create gameState from unloaded mageGroupResource");
	}
	else {
		group::set<basic, basic>(gr->get().get());
	}
}

gameState::gameState(std::shared_ptr<mageGameStateResource> gs)
{
	setVarDefaults();

	if (!gs)
		return;

	if (!gs->isLoaded()) {
		p::warn("Unable to create gameState from unloaded mageGameStateResource!");
	}
	else {
		set<>(gs->get().get());
	}
}

std::string gameState::serialize()
{
	std::string serializedGroup = Group::serialize();

	// generate attributes
	std::stringstream attribStream;

	attribStream << "BGCOL " << (int)bgCol.r << "," << (int)bgCol.g << "," << (int)bgCol.b << "\n";
	attribStream << "FOG " << (int)lightingAmb.r << "," << (int)lightingAmb.g << "," << (int)lightingAmb.b << "," << (int)lightingAmb.a << "\n";

	attribStream << "BOUNDS " << mapBounds.left << "," << mapBounds.top << "," << mapBounds.width << "," << mapBounds.height << "\n";

	std::string attribString = attribStream.str();

	// add attributes onto the start of the string
	serializedGroup = attribString + serializedGroup;

	return serializedGroup;
}

bool gameState::deserialize(std::string saveString)
{
	std::stringstream saveFile;
	saveFile << saveString;

	int obCount = 0;

	int line = 0;

	int ht = 0;

	bool foundMusic = false;

	std::string sLine;

	while (!saveFile.eof())
	{
		line++;

		getline(saveFile, sLine);
		removeNewline(sLine);

		// there is only one space in a save file line.
		// this defines why the line is there.
		auto spaceSplit = splitStringAtFirst(sLine);

		if (spaceSplit.size() < 2) {
			// skip this line
			p::info("Ignoring wrong space format/blank section at line " + std::to_string(line));
			continue;
		}

		std::string type = spaceSplit[0];
		std::string sData = spaceSplit[1];

		if (type == "BGCOL") {
			// format:
			// BGCOL r,g,b

			auto commaSp = splitString(sData, ',', ' ', 3);

			bgCol.r = atoi(commaSp[0].c_str());
			bgCol.g = atoi(commaSp[1].c_str());
			bgCol.b = atoi(commaSp[2].c_str());
		}
		else if (type == "FOG") {
			// format:
			// FOG r,g,b

			auto commaSp = splitString(sData, ',', 4);

			lightingAmb.r = atoi(commaSp[0].c_str());
			lightingAmb.g = atoi(commaSp[1].c_str());
			lightingAmb.b = atoi(commaSp[2].c_str());
			lightingAmb.a = atoi(commaSp[3].c_str());
		}
		else if (type == "BOUNDS") {
			// format:
			// BOUNDS left,top,width,height
			auto commaSp = splitString(sData, ',', 4);

			mapBounds.left = atoi(commaSp[0].c_str());
			mapBounds.top = atoi(commaSp[1].c_str());
			mapBounds.width = atoi(commaSp[2].c_str());
			mapBounds.height = atoi(commaSp[3].c_str());

			// bounds is always last so don't use any more cpu time looking for other gameState lines
			break;
		}
	}

	return Group::deserialize(saveString);
}

void gameState::preUpdateObjects(sf::Time elapsed)
{
	onPreUpdate.notify(this);

	Group::preUpdateObjects(elapsed);
}

void gameState::updateObjects(sf::Time elapsed)
{
	onUpdate.notify(this);

	Group::updateObjects(elapsed);
}

void gameState::clearObjects()
{
	Group::clearObjects();
}

sf::Time gameState::getStateUseTime()
{
	return stateClock.getElapsedTime();
}

void gameState::setVarDefaults()
{
	bgCol = sf::Color::Black; // default state screen color is black
	mapBounds = sf::FloatRect(0, 0, 800, 800); // don't ask
	lightingAmb = sf::Color::Transparent;
}

// SEBind for gameState
DeclareScriptingType(gameState);
DeclareScriptingBaseClass(Group, gameState);
DeclareScriptingBaseClass(taggable, gameState);
DeclareScriptingBaseClass(groupBase, gameState);
DeclareScriptingBaseClass(serializable, gameState);
DeclareScriptingBaseClass(shadable, gameState);
DeclareScriptingConstructor(gameState(), "gameState");
DeclareScriptingConstructor(gameState(gameState&), "gameState");
DeclareScriptingConstructor(gameState(Group&), "gameState");
DeclareScriptingConstructor(gameState(std::shared_ptr<mageGroupResource>), "gameState");
DeclareScriptingConstructor(gameState(std::shared_ptr<mageGameStateResource>), "gameState");
DeclareScriptingFunction(&gameState::bgCol, "bgCol");
DeclareScriptingFunction(&gameState::getStateUseTime, "getStateUseTime");
DeclareScriptingFunction(&gameState::lightingAmb, "lightingAmb");
DeclareScriptingFunction(&gameState::mapBounds, "mapBounds");

// for gameStateMngr
DeclareScriptingType(gameStateMngr);
DeclareScriptingFunction(&gameStateMngr::add, "add");
DeclareScriptingFunction(&gameStateMngr::combineCurrent<>, "combineCurrent");
DeclareScriptingFunction(&gameStateMngr::combineCurrent<uiBasic>, "combineCurrentUi");
DeclareScriptingFunction(&gameStateMngr::combineCurrent<objBasic>, "combineCurrentWorld");
DeclareScriptingFunction(&gameStateMngr::current, "current");
DeclareScriptingFunction(&gameStateMngr::exists, "exists");
DeclareScriptingFunction(&gameStateMngr::get, "get");
DeclareScriptingFunction(&gameStateMngr::getByIndex, "getByIndex");
DeclareScriptingFunction(&gameStateMngr::getCount, "getCount");
DeclareScriptingFunction(&gameStateMngr::list, "list");
DeclareScriptingFunction(&gameStateMngr::nameOf, "nameOf");
DeclareScriptingFunction(&gameStateMngr::setCurrent<>, "setCurrent");
DeclareScriptingFunction(&gameStateMngr::setCurrent<uiBasic>, "setCurrentUi");
DeclareScriptingFunction(&gameStateMngr::setCurrent<objBasic>, "setCurrentWorld");