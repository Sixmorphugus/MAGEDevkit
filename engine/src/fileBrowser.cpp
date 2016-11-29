#include "fileBrowser.h"

#include "uiPrimitives.h"
#include "helpers.h"

fileBrowser::fileBrowser(float x, float y)
	: uiBasic(x, y, textureData(nullptr))
{
	input = "";
	ready = false;

	fbMenu = new boxMenu(x, y, {}, sf::Color::White, sf::Vector2f(12.f, 8.f));
	fbMenu->noKbd = true;
	fbMenu->prct = 1.f;

	opMenu = new menu(x + (fbMenu->size.x * fbMenu->boxSize.x), y, { "Select", "Delete" });
	opMenu->noKbd = true;
	opMenu->disabled = true;
	opMenu->width = 32.f;

	fbTextEntry = new boxTextEntry(x, y + (fbMenu->size.y * fbMenu->boxSize.y) + 2.f, sf::Color::White, sf::Vector2f(12.f, 1.f));
	fbTextEntry->prct = 1.f;
}

void fileBrowser::update(sf::Time elapsed)
{
	fbMenu->menu::setPosition(getPosition());
	fbMenu->optionList = p::dirList("./" + input, true);

	if (extFilter.size() > 0) {
		for (int i = 0; i < (int)fbMenu->optionList.size(); i++) {
			std::string name = fbMenu->optionList[i];
			std::string fe = fileExtension(name);

			if (fe == name)
				continue; // folder, ignore

			if (name == ".." || name == ".")
				continue; // special

			bool allowed = false;
			for (unsigned int j = 0; j < extFilter.size(); j++) {
				if (extFilter[j] == fe) {
					allowed = true;
					break;
				}
			}

			if (!allowed) {
				fbMenu->optionList.erase(fbMenu->optionList.begin() + i);
				i -= 1;
			}
		}
	}

	fbMenu->update(elapsed);
	fbTextEntry->update(elapsed);
	opMenu->update(elapsed);

	if (fbMenu->menu::isMouseOver()) {
		fbMenu->disabled = false;
		opMenu->disabled = true;
	}
	else if (opMenu->isMouseOver()) {
		fbMenu->disabled = true;
		opMenu->disabled = false;
	}

	if (fbMenu->ready) {
		std::string sOption = fbMenu->optionList[fbMenu->superSelected];
		fbMenu->reset();

		if (fileExtension(sOption) == sOption || sOption == "..") {
			input += sOption + "/";
		}
		else {
			fbTextEntry->contains = sOption;
		}
	}

	if (opMenu->ready) {
		unsigned int d = opMenu->superSelected;

		opMenu->reset();

		if (d == 0) {
			if (fbTextEntry->contains.find(".") == std::string::npos) {
				fbTextEntry->contains += "." + autoExt;
			}

			ready = true; // let parent do whatever with the file
		}
		else if (d == 1) {
			p::fileDelete(path);
		}
	}

	path = p::realPath("./" + input) + fbTextEntry->contains;
}

void fileBrowser::draw(sf::RenderTarget & target, sf::RenderStates states)
{
	fbMenu->draw(target, states);
	fbTextEntry->draw(target, states);
	opMenu->draw(target, states);

	std::string textList = "";

	for (unsigned int i = 0; i < extFilter.size(); i++) {
		if (textList != "") {
			textList += ", ";
		}

		textList += extFilter[i];
	}

	if(extFilter.size() > 0)
		drawInfoSimple(target, states, getPosition() + sf::Vector2f(0.f, (fbMenu->boxSize.y * fbMenu->size.y) + 32.f), "Listing: " + textList);

	drawInfoSimple(target, states, getPosition() + sf::Vector2f(0.f, (fbMenu->boxSize.y * fbMenu->size.y) + 20.f), "Target: \"" + path + "\"");
}
