#include "tilemap.h"

#include "Game.h"

#include "constants.h"

#include "helpers.h"
#include "group.h"
#include "view.h"

// TILE
tilemap::tile::tile(tilemap* parent, sf::Vector2i position, int id) {
	mapPos = position;
	sheetId = id;
	prt = parent;
	tint = sf::Color::White;

	// use parent to infer position
	pos = sf::FloatRect(prt->getPosition().x + (prt->sfSprite.getGlobalBounds().width * mapPos.x), prt->getPosition().y + (prt->sfSprite.getGlobalBounds().height * mapPos.y), prt->sfSprite.getGlobalBounds().width, prt->sfSprite.getGlobalBounds().height);

	pos.left = floor(pos.left);
	pos.top = floor(pos.top);
	pos.width = floor(pos.width);
	pos.height = floor(pos.height);

	refreshVerts();

	colNorth = false;
	colWest = false;
}

tilemap::tile::tile() {
	// this should never be needed.
}

void tilemap::tile::refreshVerts() {
	prt->dirty = true;

	// use parent to infer position
	pos = sf::FloatRect(prt->getPosition().x + (prt->sfSprite.getGlobalBounds().width * mapPos.x), prt->getPosition().y + (prt->sfSprite.getGlobalBounds().height * mapPos.y), prt->sfSprite.getGlobalBounds().width, prt->sfSprite.getGlobalBounds().height);

	if (sheetId != -1) {
		// TRIANGLE 1
		// vert 1
		verts[0].position = sf::Vector2f(pos.left, pos.top); // pos
		verts[0].color = tint;
		verts[0].texCoords = sf::Vector2f((float)prt->getCurrentSprite()->animations.frameRects[sheetId].left, (float)prt->getCurrentSprite()->animations.frameRects[sheetId].top);

		// vert 2
		verts[1].position = sf::Vector2f(pos.left + pos.width, pos.top); // pos
		verts[1].color = tint;
		verts[1].texCoords = sf::Vector2f((float)prt->getCurrentSprite()->animations.frameRects[sheetId].left + pos.width, (float)prt->getCurrentSprite()->animations.frameRects[sheetId].top);

		// vert 3
		verts[2].position = sf::Vector2f(pos.left, pos.top + pos.height); // pos
		verts[2].color = tint;
		verts[2].texCoords = sf::Vector2f((float)prt->getCurrentSprite()->animations.frameRects[sheetId].left, (float)prt->getCurrentSprite()->animations.frameRects[sheetId].top + pos.height);

		// TRIANGLE 2
		// vert 1
		verts[3].position = sf::Vector2f(pos.left, pos.top + pos.height); // pos
		verts[3].color = tint;
		verts[3].texCoords = sf::Vector2f((float)prt->getCurrentSprite()->animations.frameRects[sheetId].left, (float)prt->getCurrentSprite()->animations.frameRects[sheetId].top + pos.height);

		// vert 2
		verts[4].position = sf::Vector2f(pos.left + pos.width, pos.top); // pos
		verts[4].color = tint;
		verts[4].texCoords = sf::Vector2f((float)prt->getCurrentSprite()->animations.frameRects[sheetId].left + pos.width, (float)prt->getCurrentSprite()->animations.frameRects[sheetId].top);

		// vert 3
		verts[5].position = sf::Vector2f(pos.left + pos.width, pos.top + pos.height); // pos
		verts[5].color = tint;
		verts[5].texCoords = sf::Vector2f((float)prt->getCurrentSprite()->animations.frameRects[sheetId].left + pos.width, (float)prt->getCurrentSprite()->animations.frameRects[sheetId].top + pos.height);
	}
	else {
		verts[0].position = sf::Vector2f(0, 0); // pos
		verts[0].color = tint;
		verts[0].texCoords = sf::Vector2f(0, 0);

		verts[1].position = sf::Vector2f(0, 0); // pos
		verts[1].color = tint;
		verts[1].texCoords = sf::Vector2f(0, 0);

		verts[2].position = sf::Vector2f(0, 0); // pos
		verts[2].color = tint;
		verts[2].texCoords = sf::Vector2f(0, 0);

		verts[3].position = sf::Vector2f(0, 0); // pos
		verts[3].color = tint;
		verts[3].texCoords = sf::Vector2f(0, 0);

		verts[4].position = sf::Vector2f(0, 0); // pos
		verts[4].color = tint;
		verts[4].texCoords = sf::Vector2f(0, 0);

		verts[5].position = sf::Vector2f(0, 0); // pos
		verts[5].color = tint;
		verts[5].texCoords = sf::Vector2f(0, 0);
	}
}

tilemap * tilemap::tile::getParent()
{
	return prt;
}

// TILEMAP
tilemap::tilemap(float x, float y, textureData sp)
	: objBasic(x, y, sp)
{
	sfSprite.setColor(sf::Color::Transparent);

	std::vector<tile> dummyVec;
	tiles.push_back(dummyVec);

	isForeground = false;

	dirty = (sprites.size() > 0);
}

void tilemap::resize(unsigned int w, unsigned int h, int newT) {
	sf::Vector2u prevSize = size();

	tiles.resize(w);

	for (unsigned int i = 0; i < tiles.size(); i++) {
		tiles[i].resize(h);

		for (unsigned int j = 0; j < tiles[i].size(); j++) {
			auto toPlace = newT == -1 ? tiles[i][j].sheetId : newT;

			if (tiles[i][j].getParent() == this)
				tiles[i][j].sheetId = toPlace;
			else {
				auto pTile = tiles[i][j];

				tiles[i][j] = tile(this, sf::Vector2i(i, j), toPlace);

				tiles[i][j].colNorth = pTile.colNorth;
				tiles[i][j].colWest = pTile.colWest;
			}
		}
	}

	scalarSize.x = w;
	scalarSize.y = h;

	dirty = true;
}

sf::Vector2u tilemap::size() const {
	if(tiles.size() > 0)
		return sf::Vector2u(tiles.size(), tiles[0].size());
	else {
		return sf::Vector2u(0, 0);
	}
}

void tilemap::generateTileCollisionBoxes()
{
	collisionBoxes.clear();

	unsigned int bxs = 0;

	for (unsigned int x = 0; x < size().x; x++) {
		for (unsigned int y = 0; y < size().y; y++) {
			if (tiles[x][y].colNorth) {
				sf::FloatRect box(sf::Vector2f(getPosition().x + (x * sfSprite.getGlobalBounds().width), getPosition().y + (y * sfSprite.getGlobalBounds().height)), sf::Vector2f(sfSprite.getGlobalBounds().width, sfSprite.getGlobalBounds().height));

				box.height = 6.f;
				box.top -= 3.f;

				collisionBoxes.push_back(box);

				bxs++;
			}
			if (tiles[x][y].colWest) {
				sf::FloatRect box(sf::Vector2f(x * sfSprite.getGlobalBounds().width, y * sfSprite.getGlobalBounds().height), sf::Vector2f(sfSprite.getGlobalBounds().width, sfSprite.getGlobalBounds().height));

				box.width = 6.f;
				box.left -= 3.f;

				collisionBoxes.push_back(box);

				bxs++;
			}
		}
	}
}

void tilemap::preUpdate(sf::Time elapsed)
{
	// messy position change tracking
	if (tilemapOffset != tilemapOffsetCheck) {
		tilemapOffsetCheck = tilemapOffset;
		setPosition(tilemapOffset.x * getCurrentSprite()->texture.frameSize.x, tilemapOffset.y * getCurrentSprite()->texture.frameSize.y);
		
		refresh();

		enforcePixelGrid = false;
	}
}

void tilemap::update(sf::Time elapsed) {
	if (dirty) {
		drawData.clear();

		for (unsigned int i = 0; i < size().x; i++) {
			for (unsigned int j = 0; j < size().y; j++) {
				for (unsigned int k = 0; k < 6; k++) {
					drawData.push_back(tiles[i][j].verts[k]);
				}
			}
		}

		generateTileCollisionBoxes();

		dirty = false;
	}

	if ((sf::Vector2u)scalarSize != size()) {
		resize(scalarSize.x, scalarSize.y);
	}
}

void tilemap::refresh()
{
	for (unsigned int i = 0; i < tiles.size(); i++) {
		for (unsigned int j = 0; j < tiles.size(); j++) {
			tiles[i][j].refreshVerts();
		}
	}

	dirty = true;
}

float tilemap::getDrawBottom() {
	float btm = getMainBounds().top;

	if (isForeground)
		btm += getMainBounds().height;

	btm -= getGroup()->indexOf(this);

	return btm;
}

std::string tilemap::getTypeString()
{
	switch (isForeground) {
	case true:
		return "BACKGROUND";
	default:
		return "FOREGROUND";
	}
}

void tilemap::nextType()
{
	switch (isForeground) {
	case true:
		isForeground = false;
		break;
	default:
		isForeground = true;
		break;
	}
}

sf::Vector2f tilemap::getSize() const
{
	return sf::Vector2f(getCurrentSprite()->texture.frameSize.x * size().x, getCurrentSprite()->texture.frameSize.y * size().y);
}

std::vector<tilemap::tile>& tilemap::operator[](int x)
{
	return tiles[x];
}

void tilemap::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	states.texture = sfSprite.getTexture();

	if(drawData.size() > 0)
		target.draw(&drawData[0], drawData.size(), sf::Triangles, states);

	if (theGame()->colBoxes) {
		for (unsigned int i = 0; i < collisionBoxes.size(); i++) {
			sf::RectangleShape rectPreview(sf::Vector2f(collisionBoxes[i].width, collisionBoxes[i].height));

			rectPreview.setOutlineThickness(1.f);
			rectPreview.setFillColor(sf::Color::Transparent);
			rectPreview.setOutlineColor(sf::Color::Magenta);

			rectPreview.setPosition(getPosition().x + collisionBoxes[i].left, getPosition().y + collisionBoxes[i].top);

			target.draw(rectPreview, states);
		}
	}
}

void tilemap::drawShadows(sf::RenderTarget & target) const
{
    
}

void tilemap::drawRow(sf::RenderTarget & target, sf::RenderStates states, unsigned int row) const
{
	// WARNING: LOTS OF MATHS HERE
	// calculate where the camera is in relation to us
	sf::Vector2f camPos = theGame()->worldCamera->getCenter();
	sf::Vector2f camSize = theGame()->worldCamera->getSize();

	sf::Vector2f spriteSize(sfSprite.getGlobalBounds().width, sfSprite.getGlobalBounds().height);

	sf::FloatRect camRect(camPos - sf::Vector2f(camSize.x / 2, camSize.y / 2), camSize);

	// if the camera is near our top left move it there
	if (camRect.left < getPosition().x)
		camRect.left = getPosition().x;

	if (camRect.top < getPosition().y)
		camRect.top = getPosition().y;

	// convert the camera coords to tile coords
	unsigned int startX = (unsigned int) floor(camRect.left / spriteSize.x);
	unsigned int stopX = (unsigned int)floor((float)startX + (camRect.width / spriteSize.x) + 2.f);

	std::vector<sf::Vertex> drawData2;

	// create an array of the draw data for this row.
	for (unsigned int i = startX; i < size().x && i < stopX; i++) {
		for (unsigned int k = 0; k < 6; k++) {
			drawData2.push_back(tiles[i][row].verts[k]);
		}
	}

	// do the do
	sf::RenderStates texState;
	texState.texture = sfSprite.getTexture();

	if (drawData2.size() > 0)
		target.draw(&drawData2[0], drawData2.size(), sf::Triangles, sfSprite.getTexture());
}

void tilemap::registerProperties()
{
	objBasic::registerProperties();

	registerProperty("isForeground", isForeground);
	registerProperty("mapWidth", scalarSize.x);
	registerProperty("mapHeight", scalarSize.y);

	deleteProperty("_X");
	deleteProperty("_Y");
	deleteProperty("_Z");

	deleteProperty("originX");
	deleteProperty("originY");
	deleteProperty("scaleX");
	deleteProperty("scaleY");

	registerProperty("_offsetX", tilemapOffset.x);
	registerProperty("_offsetY", tilemapOffset.y);

	resize(size().x, size().y);
}

std::string tilemap::serialize()
{
	std::string propData = basic::serialize();

	std::stringstream data;
	data << propData;

	// add a space
	data << " ";

	// add the tilemap size
	data << size().x << "x" << size().y;
	
	// add a space
	data << " ";

	// there used to be something of interest here
	// unfortunately you can no longer find the sprite name of an object because
	// the engine doesn't store a full resource for textureData.
	// This means we have to have prefabs for every kind of tilemap, as with everything else.

	// add a space
	data << " ";

	for (unsigned int x = 0; x < size().x; x++) {
		for (unsigned int y = 0; y < size().y; y++) {
			auto t = tiles[x][y];
			data << t.sheetId << "." << t.colNorth << "." << t.colWest << ",";
		}
	}

	return data.str();
}

bool tilemap::deserialize(std::string data)
{
	// I am much more pleased with this version of the tilemap loading code than the last one.

	// seperate tile IDs from prefab properties with a space
	auto splitData = splitString(data, ' ', '"');

	// deserialize properties
	basic::deserialize(splitData[0]);

	if (splitData.size() >= 1) {
		// deserialize the tilemap size
		auto sizeStrings = splitString(splitData[1], 'x');
		resize(atoi(sizeStrings[0].c_str()), atoi(sizeStrings[1].c_str()));
	}

	if (splitData.size() >= 2) {
		// removed
		// legacy map files will use the default tileset and size!
	}

	if (splitData.size() >= 3) {
		// deserialize the tilemap
		auto tilesAsStrings = splitString(splitData[3], ',');

		for (unsigned int i = 0; i < tilesAsStrings.size(); i++) {
			sf::Vector2u relevantTile(i / size().x, i % size().x);

			auto tileData = splitString(tilesAsStrings[i], '.');

			tiles[relevantTile.x][relevantTile.y].sheetId = atoi(tileData[0].c_str());
			tiles[relevantTile.x][relevantTile.y].colNorth = atoi(tileData[1].c_str());
			tiles[relevantTile.x][relevantTile.y].colWest = atoi(tileData[2].c_str());

			tiles[relevantTile.x][relevantTile.y].refreshVerts();
		}

		//p::info(std::to_string(tilesAsStrings.size()));
	}

	dirty = true;

	return splitData.size() >= 3;
}

void tilemap::clearTiles() {
	resize(size().x, size().y);
}

#include "scriptingEngine.h"

// tile
DeclareScriptingTypeNamed(tilemap::tile, "tile");
DeclareScriptingFunction(&tilemap::tile::colNorth, "colNorth");
DeclareScriptingFunction(&tilemap::tile::colWest, "colWest");
DeclareScriptingFunction(&tilemap::tile::getParent, "getParent");
//DeclareScriptingFunction(&tilemap::tile::mapPos, "mapPos"); omitted on purpose - does fuckall
DeclareScriptingFunction(&tilemap::tile::pos, "pos");
DeclareScriptingFunction(&tilemap::tile::refreshVerts, "refreshVerts");
DeclareScriptingFunction(&tilemap::tile::sheetId, "sheetId");
DeclareScriptingFunction(&tilemap::tile::tint, "tint");

// tilemap
DeclareScriptingBasic(tilemap);
DeclareScriptingBaseClass(objBasic, tilemap);
DeclareScriptingConstructor(tilemap(float, float, basic::textureData), "tilemap");
DeclareScriptingFunction(&tilemap::clearTiles, "clearTiles");
DeclareScriptingFunction(&tilemap::generateTileCollisionBoxes, "generateTileCollisionBoxes");
DeclareScriptingFunction(&tilemap::isForeground, "isForeground");
DeclareScriptingFunction(&tilemap::size, "size");