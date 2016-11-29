#pragma once
#include "basic.h"

class MAGEDLL tilemap :
	public objBasic
{
public:
	class MAGEDLL tile {
	public:
		tile(tilemap* parent, sf::Vector2i position, int id);
		tile();

		void refreshVerts();

		tilemap* getParent();

		sf::Vector2i mapPos;
		sf::FloatRect pos;
		int sheetId = -1;
		int tilesAboveThis;

		sf::Color tint;

		sf::Vertex verts[6];

		bool colNorth;
		bool colWest;
	private:
		tilemap* prt;
	};

public:
	tilemap(float x, float y, textureData sprite);

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
	virtual void drawShadows(sf::RenderTarget &target) const;

	virtual void drawRow(sf::RenderTarget &target, sf::RenderStates states, unsigned int row) const;

	void registerProperties();

	// tilemap needs a special saving/loading routine
	virtual std::string serialize();
	virtual bool deserialize(std::string data);

	CLONEABLE(tilemap)

	void resize(unsigned int w, unsigned int h, int newT = -1);
	sf::Vector2u size() const;

	void generateTileCollisionBoxes();
	void preUpdate(sf::Time elapsed);
	void update(sf::Time elapsed);

	void refresh();

	void clearTiles();

	float getDrawBottom();

	std::string getTypeString();
	void nextType();

	sf::Vector2f getSize() const;

public:
	std::vector< std::vector<tile> > tiles;

	sf::Vector2i tilemapOffset;
	sf::Vector2i tilemapOffsetCheck;

	// shortcut operator
	std::vector<tile> &operator[](int x);
	std::string name;

	bool isForeground;

	bool dirty;
private:
	sf::Vector2i scalarSize;
	std::vector<sf::Vertex> drawData, shadowData;
};