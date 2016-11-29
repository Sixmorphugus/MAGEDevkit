#pragma once
#include "wxAfx.h"
#include "MAGE/gameState.h"

class Editor;
class wxVariant;

class EditorState : public gameState
{
public:
	enum Mode {
		OBJECTS,
		UI,
		TILEMAP,
		COLLISION
	};

private:
	struct copiedObject {
		copiedObject(std::shared_ptr<basic> toCopy, sf::Vector2f off = sf::Vector2f());

		std::shared_ptr<basic> copy;
		sf::Vector2f offset;
	};

public:
	EditorState();

	void drawWorldObjects(sf::RenderTarget &target, sf::RenderStates states);
	void drawUiObjects(sf::RenderTarget &target, sf::RenderStates state);

	void doCut();
	void doCopy();
	void doPaste();
	void doDelete();
	void doSelectionDelete();
	void doDeselect();

	void toggleGrids();

	void setMode(Mode m);
	Mode getMode();

	virtual void clearObjects();
	virtual void updateObjects(sf::Time elapsed);

	// manipulation of objects/ui
	void setSelectedObject(std::shared_ptr<basic> obj);
	void addSelectedObject(std::shared_ptr<basic> obj);
	bool isSelected(std::shared_ptr<basic> obj);

	void updateObjectProp(std::string prop, wxVariant dat);
	std::vector<std::vector<int>> getTilemapSelection();

	std::shared_ptr<basic> uniqueSelection();

	void bringSelectionToFront();
	void sendSelectionToBack();

private:
	sf::Vector2i initTilemapSelectionBoxUsage();

public:
	std::vector<copiedObject> editorClipboard;
	std::vector<std::vector<int>> tilemapPlacementClipboard;

	std::vector<std::vector<int>> tilemapPlacement;
	std::vector<sf::IntRect> tilemapSelections;
	unsigned int tilemapCurSlBox;
	unsigned short tilemapCollisionWall;
	std::vector<std::shared_ptr<basic>> selections;
	sf::Vector2f selectionOffset;
	std::string curLevelPath;
	bool editsUnsaved;
	bool grids;

private:
	Mode mode;
	sf::Time rightDownTime;
};
