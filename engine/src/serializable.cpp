#include "serializable.h"
#include "platform.h"

std::string serializable::serialize()
{
	return "";
}

bool serializable::deserialize(std::string data)
{
	return true;
}

void serializable::saveToFile(std::string file)
{
	sf::Clock timer;
	std::ofstream saveFile(file);

	if (saveFile.bad()) {
		p::warn("Unable to save serializable object to file - Bad file location.");
		return;
	}

	saveFile << serialize();

	saveFile.close();

	p::info("Saved in " + std::to_string(timer.getElapsedTime().asMilliseconds()));
}

bool serializable::loadFromFile(std::string file)
{
	sf::Clock timer;

	std::ifstream saveFile(file);

	if (!saveFile.good()) {
		p::info("FAILED - bad file");
		return false;
	}

	std::string str;

	saveFile.seekg(0, std::ios::end);
	str.reserve(saveFile.tellg());
	saveFile.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(saveFile)),
		std::istreambuf_iterator<char>());

	bool result = deserialize(str);

	if(result)
		p::info("Loaded in " + std::to_string(timer.getElapsedTime().asMilliseconds()));

	return result;
}
