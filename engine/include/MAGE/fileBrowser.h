#pragma once
#include "basic.h"

class menu;
class boxMenu;
class boxTextEntry;

class MAGEDLL fileBrowser : public uiBasic
{
public:
	fileBrowser(float x, float y);

	void update(sf::Time elapsed);
	void draw(sf::RenderTarget& target, sf::RenderStates states);

	bool isReady() { return ready; }
	std::string getInput() { return input; }

	void reset() { ready = false; };

	std::string getPath() { return path; }
	std::string getRoutePath() { return input; }
public:
	std::vector<std::string> extFilter;
	std::string autoExt;

private:
	bool ready;
	std::string input;
	std::string path;

	boxMenu* fbMenu;
	menu* opMenu;
	boxTextEntry* fbTextEntry;
};