#include "EditorState.h"

#include "Editor.h"
#include "EditorFrame.h"

#include "MAGE/tilemap.h"
#include "MAGE/sfResources.h"
#include "MAGE/view.h"
#include "MAGE/io.h"

EditorState::copiedObject::copiedObject(std::shared_ptr<basic> toCopy, sf::Vector2f off)
{
	copy = std::shared_ptr<basic>(toCopy->clone());
	offset = off;
}

EditorState::EditorState()
{
	ignoreIncorporation = true;
	grids = true;
}

void EditorState::drawWorldObjects(sf::RenderTarget & target, sf::RenderStates states)
{
	gameState::drawWorldObjects(target, states);

	// add an outline to selected world objects
	if (mode == OBJECTS) {
		for (unsigned int i = 0; i < selections.size(); i++) {
			sf::RectangleShape sfrs;

			sfrs.setFillColor(sf::Color::Transparent);
			sfrs.setOutlineColor(sf::Color::White);
			sfrs.setOutlineThickness(2.f);

			sfrs.setPosition(selections[i]->getPosition());
			sfrs.setSize(selections[i]->getSize());
			sfrs.setOrigin(selections[i]->getOrigin());
			sfrs.setRotation(selections[i]->getRotation());

			target.draw(sfrs);
		}
	}
	else if (uniqueSelection() && (mode == TILEMAP || mode == COLLISION)) {
		auto sTm = std::dynamic_pointer_cast<tilemap>(uniqueSelection());

		if (sTm) {
			if (grids) {
				// draw grid on the tilemap
				for (unsigned int x = 0; x < sTm->size().x; x++) {
					sf::RectangleShape r(sf::Vector2f(2.f, sTm->getSize().y));
					r.setPosition(sTm->getPosition().x + (x * sTm->getFrameSize().x), sTm->getPosition().y);
					r.setFillColor(sf::Color(255, 255, 255, 50));

					target.draw(r, states);
				}

				for (unsigned int y = 0; y < sTm->size().y; y++) {
					sf::RectangleShape r(sf::Vector2f(sTm->getSize().x, 2.f));
					r.setPosition(sTm->getPosition().x, sTm->getPosition().y + (y * sTm->getFrameSize().y));
					r.setFillColor(sf::Color(255, 255, 255, 50));

					target.draw(r, states);
				}
			}

			// draw placable tile/effect
			sf::Vector2u mouseTilePos((unsigned int)floor(theGame()->mouseWorldPos.x / (float)sTm->getFrameSize().x),
				(unsigned int)floor(theGame()->mouseWorldPos.y / (float)sTm->getFrameSize().y));

			sf::Vector2f tileWorldPos((float)mouseTilePos.x * (float)sTm->getFrameSize().x,
				(float)mouseTilePos.y * (float)sTm->getFrameSize().y);

			if (mode == TILEMAP) {
				for (unsigned int x = 0; x < tilemapPlacement.size(); x++) {
					for (unsigned int y = 0; y < tilemapPlacement[x].size(); y++) {
						if (tilemapPlacement[x][y] < 0)
							continue;

						sf::Sprite tmTileSprite(*sTm->getTexPointer(),
							sTm->getCurrentSprite()->animations.frameRects.at(tilemapPlacement[x][y]));

						tmTileSprite.setPosition(tileWorldPos.x + (sTm->getFrameSize().x * x),
							tileWorldPos.y + (sTm->getFrameSize().y * y));

						tmTileSprite.setColor(sf::Color(255, 255, 255, 160));
						target.draw(tmTileSprite);
					}
				}

				// draw tile selections
				std::vector<sf::Vector2i> alreadyDrawnBoxes;

				auto alreadyDrawn = [](sf::Vector2i sv, std::vector<sf::Vector2i>& alreadyDrawnBoxesList) {
					for (unsigned int i = 0; i < alreadyDrawnBoxesList.size(); i++) {
						if (alreadyDrawnBoxesList[i] == sv)
							return true;
					}

					return false;
				};

				for (unsigned int i = 0; i < tilemapSelections.size(); i++) {
					auto ts = tilemapSelections[i];
					auto xMod = (ts.width < 0 ? -1 : 1);
					auto yMod = (ts.height < 0 ? -1 : 1);

					for (unsigned int x = ts.left; x != ts.left + ts.width; x += xMod) {
						for (unsigned int y = ts.top; y != ts.top + ts.height; y += yMod) {
							// check alreadyDrawn list
							auto vec = sf::Vector2i(x, y);
							if (!alreadyDrawn(vec, alreadyDrawnBoxes)) {
								alreadyDrawnBoxes.push_back(vec);

								sf::RectangleShape box;
								box.setPosition(sTm->getFrameSize().x * vec.x, sTm->getFrameSize().y * vec.y);
								box.setSize(sf::Vector2f((float)sTm->getFrameSize().x, (float)sTm->getFrameSize().y));

								box.setFillColor(sf::Color(0, 255, 255, 150));

								target.draw(box, states);
							}
						}
					}
				}
			}
			else {
				// draw collision edges mapwide
				for (unsigned int x = 0; x < sTm->size().x; x++) {
					for (unsigned int y = 0; y < sTm->size().y; y++) {
						sf::Vector2f twp((float)x * sTm->getFrameSize().x, (float)y * sTm->getFrameSize().y);

						if (sTm->tiles[x][y].colNorth) {
							sf::RectangleShape northLine(sf::Vector2f(sTm->getFrameSize().x, 2.f));
							northLine.setPosition(twp - sf::Vector2f(0.f, 1.f));
							northLine.setFillColor(sf::Color::Red);

							target.draw(northLine, states);
						}

						if (sTm->tiles[x][y].colWest) {
							sf::RectangleShape westLine(sf::Vector2f(2.f, sTm->getFrameSize().y));
							westLine.setPosition(twp - sf::Vector2f(1.f, 0.f));
							westLine.setFillColor(sf::Color::Red);

							target.draw(westLine, states);
						}
					}
				}

				// draw collision edge under the mouse
				if (tilemapCollisionWall == 0) {
					sf::RectangleShape northLine(sf::Vector2f(sTm->getFrameSize().x, 2.f));
					northLine.setPosition(tileWorldPos - sf::Vector2f(0.f, 1.f));
					northLine.setFillColor(sf::Color::Magenta);

					target.draw(northLine, states);
				}
				else if (tilemapCollisionWall == 1) {
					sf::RectangleShape eastLine(sf::Vector2f(2.f, sTm->getFrameSize().y));
					eastLine.setPosition(tileWorldPos - sf::Vector2f(1.f - sTm->getFrameSize().x, 0.f));
					eastLine.setFillColor(sf::Color::Magenta);

					target.draw(eastLine, states);
				}
				else if (tilemapCollisionWall == 2) {
					sf::RectangleShape southLine(sf::Vector2f(sTm->getFrameSize().x, 2.f));
					southLine.setPosition(tileWorldPos - sf::Vector2f(0.f, 1.f - sTm->getFrameSize().y));
					southLine.setFillColor(sf::Color::Magenta);

					target.draw(southLine, states);
				}
				else if (tilemapCollisionWall == 3) {
					sf::RectangleShape westLine(sf::Vector2f(2.f, sTm->getFrameSize().y));
					westLine.setPosition(tileWorldPos - sf::Vector2f(1.f, 0.f));
					westLine.setFillColor(sf::Color::Magenta);

					target.draw(westLine, states);
				}
			}
		}
	}

	// add an outline to the world
	sf::RectangleShape sfrs;

	sfrs.setFillColor(sf::Color::Transparent);
	sfrs.setOutlineColor(sf::Color::Magenta);
	sfrs.setOutlineThickness(2.f);

	sfrs.setPosition(mapBounds.left, mapBounds.top);
	sfrs.setSize(sf::Vector2f(mapBounds.width, mapBounds.height));

	target.draw(sfrs);
}

void EditorState::doCut()
{
	doCopy();
	doDelete();
}

void EditorState::doCopy()
{
	if (mode == OBJECTS) {
		editorClipboard.clear();

		for (unsigned int i = 0; i < selections.size(); i++) {
			sf::Vector2f offs(selections[i]->getPosition() - theGame()->mouseWorldPos);
			editorClipboard.push_back(copiedObject(selections[i], offs));
		}
	}
	else if (mode == TILEMAP) {
		auto tmS = std::dynamic_pointer_cast<tilemap>(uniqueSelection());

		if (tmS && tilemapSelections.size() > 0) {
			auto tl = initTilemapSelectionBoxUsage();

			// build a single 2D tile array from the tile locations selected
			tilemapPlacementClipboard.clear();

			for (unsigned int i = 0; i < tilemapSelections.size(); i++) {
				auto rec = tilemapSelections[i];

				for (unsigned int x = rec.left; x < rec.left + rec.width; x++) {
					for (unsigned int y = rec.top; y < rec.top + rec.height; y++) {
						int tileAtLoc = tmS->tiles[x][y].sheetId;

						sf::Vector2i ap = sf::Vector2i(x, y) - tl;

						if (tilemapPlacementClipboard.size() <= ap.x) {
							tilemapPlacementClipboard.resize(ap.x + 1);
						}

						if (tilemapPlacementClipboard[ap.x].size() <= ap.y) {
							tilemapPlacementClipboard[ap.x].resize(ap.y + 1);
						}

						tilemapPlacementClipboard[ap.x][ap.y] = tileAtLoc;
					}
				}
			}
		}
	}
}

void EditorState::doPaste()
{
	if (mode == OBJECTS) {
		setSelectedObject(nullptr);

		for (unsigned int i = 0; i < editorClipboard.size(); i++) {
			auto o = editorClipboard[i].copy->clone();
			auto oi = attachNew(o);

			o->setPosition(theGame()->mouseWorldPos + editorClipboard[i].offset);

			addSelectedObject(getObject(oi));
		}

		theEditor()->createObjectTree();
		theEditor()->createObjectProperties();
	}
	else if (mode == TILEMAP) {
		auto tmS = std::dynamic_pointer_cast<tilemap>(uniqueSelection());

		if (tmS) {
			tilemapPlacement = tilemapPlacementClipboard;
		}

		tilemapSelections.clear(); // feels unnatural not to do this
	}
}

void EditorState::doDelete()
{
	if (mode == OBJECTS) {
		for (unsigned int i = 0; i < selections.size(); i++) {
			selections[i]->destroy();
		}

		doDeselect();
	}
	else if (mode == TILEMAP) {
		auto tmS = std::dynamic_pointer_cast<tilemap>(uniqueSelection());

		if (tmS) {
			auto tl = initTilemapSelectionBoxUsage();

			// remove every tilemap tile inside a box
			for (unsigned int i = 0; i < tilemapSelections.size(); i++) {
				auto rec = tilemapSelections[i];

				for (unsigned int x = rec.left; x < rec.left + rec.width; x++) {
					for (unsigned int y = rec.top; y < rec.top + rec.height; y++) {
						tmS->tiles[x][y].sheetId = -1;
						tmS->tiles[x][y].refreshVerts();
					}
				}
			}
		}

		tilemapSelections.clear();
	}
}

void EditorState::doSelectionDelete()
{
	if (mode != TILEMAP) {
		doDelete();
	}
	else {
		for (unsigned int i = 0; i < selections.size(); i++) {
			selections[i]->destroy();
		}

		doDeselect();
	}
}

void EditorState::doDeselect()
{
	setSelectedObject(nullptr);
	theEditor()->createObjectTree();
	theEditor()->createObjectProperties();
}

void EditorState::updateObjects(sf::Time elapsed)
{
	float speed = theGame()->worldCamera->getZoomLevel() * 10;

	if (theGame()->keyboard->getBind("Up")->getDown()) {
		theGame()->worldCamera->move(0.f, -speed);
	}
	if (theGame()->keyboard->getBind("Down")->getDown()) {
		theGame()->worldCamera->move(0.f, speed);
	}
	if (theGame()->keyboard->getBind("Left")->getDown()) {
		theGame()->worldCamera->move(-speed, 0.f);
	}
	if (theGame()->keyboard->getBind("Right")->getDown()) {
		theGame()->worldCamera->move(speed, 0.f);
	}

	theGame()->worldCamera->setZoomLevel(theGame()->worldCamera->getZoomLevel() + ((float)theGame()->mouse->scroll / 4.f));
	theGame()->worldCamera->setZoomLevel(clamp(theGame()->worldCamera->getZoomLevel(), 0.25f, 4.f));

	theEditor()->frame->SetTitle("MAGEdit - " + curLevelPath + (editsUnsaved ? "*" : ""));
	theEditor()->frame->SetStatusText(p::getLastLogged());

	if (theGame()->mouse->getRightPressed()) {
		rightDownTime = theGame()->getSimTime();
	}

	sf::Time timeRightWasDown = theGame()->getSimTime() - rightDownTime;

	// OBJECT MODES
	if (mode == OBJECTS || mode == UI) {
		if (theGame()->mouse->getLeftDown()) {
			if (theGame()->mouse->getLeftPressed()) {
				std::vector<std::shared_ptr<basic>> ouc;

				if (mode == OBJECTS) {
					auto wouc = theGame()->findObjectsAt(theGame()->mouseWorldPos);

					for (unsigned int i = 0; i < wouc.size(); i++) {
						ouc.push_back(wouc[i]);
					}
				}
				else {
					auto uouc = theGame()->findUiAt(theGame()->mousePos);

					for (unsigned int i = 0; i < uouc.size(); i++) {
						ouc.push_back(uouc[i]);
					}
				}

				if (theGame()->keyboard->getBind("SHIFT")->getDown()) {
					if (ouc.size() > 0) {
						auto ourObj = ouc.at(ouc.size() - 1);

						addSelectedObject(ourObj);
						selectionOffset = ourObj->getPosition() - (mode == OBJECTS ? theGame()->mouseWorldPos : theGame()->mousePos);
					}
				}
				else {
					if (ouc.size() > 0) {
						auto ourObj = ouc.at(ouc.size() - 1);

						if (!isSelected(ourObj))
							setSelectedObject(ourObj);
						else {
							addSelectedObject(ourObj); // do as above for things that are already selected
						}
						selectionOffset = ourObj->getPosition() - (mode == OBJECTS ? theGame()->mouseWorldPos : theGame()->mousePos);
					}
					else {
						setSelectedObject(nullptr);
						theEditor()->createObjectProperties();
					}
				}
			}

			// move first selection
			if (selections.size() > 0 && !theEditor()->keyboard->getBind("SHIFT")->getDown()) {
				auto ourObj = selections.at(selections.size() - 1);

				auto oop = ourObj->getPosition();
				ourObj->setPosition((mode == OBJECTS ? theGame()->mouseWorldPos : theGame()->mousePos) + selectionOffset);
				ourObj->doPixelGridCorrection(false);
				auto oMovement = ourObj->getPosition() - oop;

				for (unsigned int i = 0; i < selections.size() - 1; i++) {
					selections[i]->move(oMovement);
					selections[i]->doPixelGridCorrection(ourObj->enforcePixelGrid);
				}

				editsUnsaved = true;

				theEditor()->createObjectProperties();
			}
		}
		else if (theGame()->mouse->getLeftReleased()) {
			theEditor()->createObjectTree(); // object order is changed when objects are moved around (depth rendering)
		}

		if (theGame()->mouse->getRightReleased()) {
			// open context menu
			wxMenu* addMenu = new wxMenu("Create Object");

			unsigned int id = 0;
			for (unsigned int i = 0; i < theGame()->prefabs->getCount(); i++) {
				auto p = theGame()->prefabs->getByIndex(i);

				if (p->castsTo<objBasic>() && !p->castsTo<tilemap>() && mode == OBJECTS) {
					addMenu->Append(id, theGame()->prefabs->nameOf(p));
				}
				else if (p->castsTo<uiBasic>() && mode == UI) {
					addMenu->Append(id, theGame()->prefabs->nameOf(p));
				}

				id++;
			}

			theEditor()->Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Editor::onAddPrefab));
			theEditor()->frame->getSFMLViewport()->PopupMenu(addMenu, wxPoint(theGame()->mouse->position.x, theGame()->mouse->position.y));
			theEditor()->Disconnect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Editor::onAddPrefab));
		}
	}

	// TILEMAP MODE
	else if (mode == TILEMAP) {
		if (theGame()->mouse->getRightReleased()) {
			if (uniqueSelection()) {
				if (tilemapSelections.size() == 0) {
					// open tile selection window
					tilemapPlacement = getTilemapSelection();
				}
			}
			else {
				// open context menu
				wxMenu* addMenu = new wxMenu("Create Tile Layer");

				unsigned int id = 0;
				for (unsigned int i = 0; i < theGame()->prefabs->getCount(); i++) {
					auto p = theGame()->prefabs->getByIndex(i);

					if (p->castsTo<tilemap>()) {
						addMenu->Append(id, theGame()->prefabs->nameOf(p));
					}

					id++;
				}

				theEditor()->Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Editor::onAddPrefab));
				theEditor()->frame->getSFMLViewport()->PopupMenu(addMenu, wxPoint(theGame()->mouse->position.x, theGame()->mouse->position.y));
				theEditor()->Disconnect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Editor::onAddPrefab));
			}
		}

		auto tmp = std::dynamic_pointer_cast<tilemap>(uniqueSelection());

		if (tmp) {
			sf::Vector2i mtp((unsigned int)floor(theGame()->mouseWorldPos.x / tmp->getFrameSize().x),
				(unsigned int)floor(theGame()->mouseWorldPos.y / tmp->getFrameSize().y));

			if (theGame()->mouse->getLeftDown()) {
				for (unsigned int x = 0; x < tilemapPlacement.size(); x++) {
					for (unsigned int y = 0; y < tilemapPlacement[x].size(); y++) {
						if (tmp->size().x <= mtp.x + x || tmp->size().y <= mtp.y + y || tilemapPlacement[x][y] == -1)
							continue;

						tmp->tiles[mtp.x + x][mtp.y + y].sheetId = tilemapPlacement[x][y];
						tmp->tiles[mtp.x + x][mtp.y + y].refreshVerts();
					}
				}
			}
			if (theGame()->mouse->getRightDown()) {
				if (theGame()->mouse->getRightPressed()) {
					if (!theGame()->keyboard->getBind("SHIFT")->getDown()) {
						tilemapSelections.clear();
					}

					selectionOffset = theGame()->mouseWorldPos;
					tilemapCurSlBox = tilemapSelections.size();
				}

				sf::Vector2i otp((unsigned int)floor(selectionOffset.x / tmp->getFrameSize().x),
					(unsigned int)floor(selectionOffset.y / tmp->getFrameSize().y));

				auto tm = std::dynamic_pointer_cast<tilemap>(uniqueSelection());

				if (distanceBetweenVectors(selectionOffset, theGame()->mouseWorldPos) > 4.f) {
					if (tilemapCurSlBox == tilemapSelections.size()) {
						tilemapSelections.push_back(sf::IntRect(otp, mtp - otp));
					}
					else {
						tilemapSelections[tilemapCurSlBox] = sf::IntRect(otp, mtp - otp);
					}
				}
			}
		}
	}
	else if (mode == COLLISION) {
		// COLLISION WALL PLACEMENT
		auto tmp = std::dynamic_pointer_cast<tilemap>(uniqueSelection());

		if (tmp) {
			sf::Vector2u mouseTilePos((unsigned int)floor(theGame()->mouseWorldPos.x / (float)tmp->getFrameSize().x),
				(unsigned int)floor(theGame()->mouseWorldPos.y / (float)tmp->getFrameSize().y));

			sf::Vector2f tileWorldPos((float)mouseTilePos.x * (float)tmp->getFrameSize().x,
				(float)mouseTilePos.y * (float)tmp->getFrameSize().y);

			if (theGame()->mouse->getLeftPressed() || !theGame()->mouse->getLeftDown()) {
				// which collision wall are we placing
				// 0 = north, 1 = east, 2, 3 = obvious

				// where in the tile is the mouse?
				auto mouseTileOffset = theGame()->mouseWorldPos - tileWorldPos;

				// now that we have that we need four points for the middle of each edge
				sf::Vector2f tileNorthEdge(tmp->getFrameSize().x / 2.f, 0.f);
				sf::Vector2f tileEastEdge(tmp->getFrameSize().x, tmp->getFrameSize().y / 2.f);
				sf::Vector2f tileSouthEdge(tmp->getFrameSize().x / 2.f, tmp->getFrameSize().y);
				sf::Vector2f tileWestEdge(0.f, tmp->getFrameSize().y / 2.f);

				// get each distance
				float nDist = distanceBetweenVectors(mouseTileOffset, tileNorthEdge);
				float eDist = distanceBetweenVectors(mouseTileOffset, tileEastEdge);
				float sDist = distanceBetweenVectors(mouseTileOffset, tileSouthEdge);
				float wDist = distanceBetweenVectors(mouseTileOffset, tileWestEdge);

				// shortest distance is the mode
				tilemapCollisionWall = lowestOf<float>({ nDist, eDist, sDist, wDist });
			}

			if (theGame()->mouse->getLeftDown()) {
				if (mouseTilePos.x >= tmp->size().x || mouseTilePos.y >= tmp->size().y)
					return; // not a real tile, could crash the game

				switch (tilemapCollisionWall) {
					// easy ones first
				default:
				case 0: // north (default)
					tmp->tiles[mouseTilePos.x][mouseTilePos.y].colNorth = true;
					break;
				case 3: // west
					tmp->tiles[mouseTilePos.x][mouseTilePos.y].colWest = true;
					break;

					// these others require us to change a property that possibly exists on an ADJACENT tile
				case 1: // east
					if (mouseTilePos.x + 1 < tmp->size().x) {
						tmp->tiles[mouseTilePos.x + 1][mouseTilePos.y].colWest = true;
					}
					break;
				case 2: // south
					if (mouseTilePos.y + 1 < tmp->size().y) {
						tmp->tiles[mouseTilePos.x][mouseTilePos.y + 1].colNorth = true;
					}
					break;
				}
			}

			if (theGame()->mouse->getRightDown()) {
				if (mouseTilePos.x >= tmp->size().x || mouseTilePos.y >= tmp->size().y)
					return; // not a real tile, could crash the game

							// just take all collision walls off this tile
				tmp->tiles[mouseTilePos.x][mouseTilePos.y].colNorth = false;
				tmp->tiles[mouseTilePos.x][mouseTilePos.y].colWest = false;
			}
		}
	}

	gameState::updateObjects(elapsed);
}

void EditorState::drawUiObjects(sf::RenderTarget &target, sf::RenderStates states) {
	gameState::drawUiObjects(target, states);

	if (uniqueSelection()) {
		sf::Vector2u editorTileSize(uniqueSelection()->getFrameSize().x / 2, uniqueSelection()->getFrameSize().y / 2);
		int maxTilesOnScreen = floor((theGame()->uiCamera->getSize().x - editorTileSize.x) / editorTileSize.x);
		int tilesOnScreen = 0;
		int tileRow = 0;

		bool col = true;
	}
}

void EditorState::setSelectedObject(std::shared_ptr<basic> obj)
{
	selections.clear();
	tilemapSelections.clear();

	if (obj)
		addSelectedObject(obj);
	else
		theEditor()->updateTreeSelections();
}

void EditorState::addSelectedObject(std::shared_ptr<basic> obj)
{
	for (unsigned int i = 0; i < selections.size(); i++) {
		if (selections[i] == obj) {
			selections.erase(selections.begin() + i);
		}
	}

	selections.push_back(obj);

	theEditor()->createObjectProperties();
	tilemapPlacement = { { 0U } };

	theEditor()->updateTreeSelections();
}

bool EditorState::isSelected(std::shared_ptr<basic> obj)
{
	for (unsigned int i = 0; i < selections.size(); i++) {
		if (selections[i] == obj) {
			return true;
		}
	}

	return false;
}

void EditorState::updateObjectProp(std::string prop, wxVariant dat)
{
	wxAny value = dat;
	if (uniqueSelection()) {
		if (uniqueSelection()->getProperty(prop).getType() == basic::prop::BOOLPROP)
			uniqueSelection()->getProperty(prop).update(value.As<bool>());
		else {
			uniqueSelection()->getProperty(prop).update(value.As<wxString>().ToStdString());
		}

		if (auto tm = std::dynamic_pointer_cast<tilemap>(uniqueSelection())) {
			tm->dirty = true;
		}

		editsUnsaved = true;
	}
	else {
		// game property was edited
		if (prop == "Map Left") {
			mapBounds.left = value.As<float>();
		}
		else if (prop == "Map Top") {
			mapBounds.top = value.As<float>();
		}
		else if (prop == "Map Width") {
			mapBounds.width = value.As<float>();
		}
		else if (prop == "Map Height") {
			mapBounds.height = value.As<float>();
		}
		else if (prop == "BG Red") {
			bgCol.r = value.As<sf::Uint8>();
		}
		else if (prop == "BG Green") {
			bgCol.g = value.As<sf::Uint8>();
		}
		else if (prop == "BG Blue") {
			bgCol.b = value.As<sf::Uint8>();
		}
		else if (prop == "Amb Red") {
			lightingAmb.r = value.As<sf::Uint8>();
		}
		else if (prop == "Amb Green") {
			lightingAmb.g = value.As<sf::Uint8>();
		}
		else if (prop == "Amb Blue") {
			lightingAmb.b = value.As<sf::Uint8>();
		}
		else if (prop == "Amb Opacity") {
			lightingAmb.a = value.As<sf::Uint8>();
		}
	}

	theEditor()->createObjectTree();
}

std::vector<std::vector<int>> EditorState::getTilemapSelection()
{
	auto ret = tilemapPlacement;

	if (!uniqueSelection())
		return ret;

	sf::Vector2u frameSize = uniqueSelection()->getFrameSize();
	auto tp = uniqueSelection()->getTexPointer();
	sf::Sprite tps(*tp);

	sf::RenderWindow selectorWindow;
	selectorWindow.create(sf::VideoMode(tp->getSize().x, tp->getSize().y), "Tilemap Tile Selector", sf::Style::Titlebar | sf::Style::Close);

	auto frameList = uniqueSelection()->getCurrentSprite()->animations.frameRects;

	sf::IntRect selectorPosition;

	bool selectionHappening = false;

	while (selectorWindow.isOpen()) {
		selectorWindow.clear(sf::Color::Black);

		// draw checkers as a background
		int maxSquaresInWindow = (float)tp->getSize().x / uniqueSelection()->getFrameSize().x;
		int columnCounter = 0;
		int rowCounter = 0;
		bool boxCol = true;

		for (unsigned int f = 0; f < frameList.size(); f++) {
			sf::RectangleShape rectBox((sf::Vector2f)uniqueSelection()->getFrameSize());
			rectBox.setFillColor(boxCol ? sf::Color::White : sf::Color(150, 150, 160));
			rectBox.setPosition(columnCounter * uniqueSelection()->getFrameSize().x, rowCounter * uniqueSelection()->getFrameSize().y);

			selectorWindow.draw(rectBox);

			boxCol = !boxCol;

			columnCounter++;

			if (columnCounter >= maxSquaresInWindow) {
				columnCounter = 0;
				rowCounter++;

				boxCol = ((float)rowCounter / 2.f == floor((float)rowCounter / 2.f));
			}
		}

		// draw the tilemap
		selectorWindow.draw(tps);

		// draw the mouse's selection
		sf::RectangleShape rect(sf::Vector2f(selectorPosition.width * uniqueSelection()->getFrameSize().x, selectorPosition.height * uniqueSelection()->getFrameSize().y));

		rect.setPosition(selectorPosition.left * uniqueSelection()->getFrameSize().x, selectorPosition.top * uniqueSelection()->getFrameSize().y);
		rect.setFillColor(sf::Color(255, 255, 0, 100));

		selectorWindow.draw(rect);

		// display
		selectorWindow.display();

		sf::Event evt;
		while (selectorWindow.pollEvent(evt)) {
			if (evt.type == sf::Event::Closed) {
				selectorWindow.close();
			}
			else if (evt.type == sf::Event::MouseMoved) {
				sf::Vector2u mouseTilePos;
				mouseTilePos.x = (int)floor(evt.mouseMove.x / uniqueSelection()->getFrameSize().x);
				mouseTilePos.y = (int)floor(evt.mouseMove.y / uniqueSelection()->getFrameSize().y);

				if (selectionHappening) {
					// change rect's size
					selectorPosition.width = (mouseTilePos.x - selectorPosition.left) + 1;
					selectorPosition.height = (mouseTilePos.y - selectorPosition.top) + 1;

					if (selectorPosition.width <= 0) selectorPosition.width = 1;
					if (selectorPosition.height <= 0) selectorPosition.height = 1;
				}
				else {
					// change rect's position, keep size at 1, 1
					selectorPosition.left = mouseTilePos.x;
					selectorPosition.top = mouseTilePos.y;
					selectorPosition.width = 1;
					selectorPosition.height = 1;
				}
			}
			else if (evt.type == sf::Event::MouseButtonPressed) {
				selectionHappening = true;
			}
			else if (evt.type == sf::Event::MouseButtonReleased) {
				if (!selectionHappening)
					continue;

				// do "complex" maths to figure out which tiles were selected and then close the window
				auto tileAt = [&](sf::Vector2u pt, int width) { return pt.x + (pt.y * (float)width); };
				ret.clear();

				// scale tilemapPlacement
				ret.resize(selectorPosition.width);

				for (unsigned int i = 0; i < ret.size(); i++) {
					ret[i].resize(selectorPosition.height);
				}

				for (unsigned int x = 0; x < ret.size(); x++) {
					for (unsigned int y = 0; y < ret[x].size(); y++) {
						ret[x][y] = tileAt(sf::Vector2u(selectorPosition.left + x, selectorPosition.top + y), maxSquaresInWindow);
					}
				}

				selectorWindow.close();
			}
		}
	}

	return ret;
}

std::shared_ptr<basic> EditorState::uniqueSelection()
{
	if (selections.size() == 1) {
		return selections[0];
	}

	return nullptr;
}

void EditorState::bringSelectionToFront()
{
	auto us = uniqueSelection();

	if (us)
		bringToFront(indexOf(us.get()));

	theEditor()->createObjectTree();
}

void EditorState::sendSelectionToBack()
{
	auto us = uniqueSelection();

	if (us)
		sendToBack(indexOf(us.get()));
}

sf::Vector2i EditorState::initTilemapSelectionBoxUsage()
{
	auto tmO = std::dynamic_pointer_cast<tilemap>(uniqueSelection());
	sf::Vector2i tl;

	if (tmO) {
		// make an array with all selection rectangles "corrected" to be entirely unsigned
		std::vector<sf::IntRect> correctedRectangles;
		tl = (sf::Vector2i)tmO->size();

		for (unsigned int i = 0; i < tilemapSelections.size(); i++) {
			auto tmS = tilemapSelections[i];

			if (tmS.width < 0) {
				tmS.left += tmS.width + 1;
				tmS.width = -tmS.width;
			}

			if (tmS.height < 0) {
				tmS.top += tmS.height + 1;
				tmS.height = -tmS.height;
			}

			if (tmS.left < tl.x) {
				tl.x = tmS.left;
			}

			if (tmS.top < tl.y) {
				tl.y = tmS.top;
			}

			if (tmS.left + tmS.width >= tmO->size().x) {
				tmS.width -= ((tmS.left + tmS.width) - tmO->size().x);
			}

			if (tmS.top + tmS.height >= tmO->size().y) {
				tmS.height -= ((tmS.top + tmS.height) - tmO->size().y);
			}

			correctedRectangles.push_back(tmS);
		}

		tilemapSelections = correctedRectangles;
	}

	return tl;
}

void EditorState::setMode(Mode m)
{
	mode = m;
	theEditor()->createObjectTree();

	theEditor()->frame->getModeChoice()->SetSelection(mode);

	setSelectedObject(nullptr);
}

EditorState::Mode EditorState::getMode()
{
	return mode;
}

void EditorState::clearObjects()
{
	gameState::clearObjects();

	curLevelPath = "new";
	editsUnsaved = false;

	setSelectedObject(nullptr);
	setMode(OBJECTS);

	mapBounds.width = 1200;
	mapBounds.height = 1200;
}

void EditorState::toggleGrids() {
	grids = !grids;
}