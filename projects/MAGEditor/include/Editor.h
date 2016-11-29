#pragma once

#include "wxAfx.h"
#include "MAGE/Game.h"

class EditorFrame;
class EditorState;

class Editor : public wxApp, public Game
{
public:
	Editor();

	bool OnInit();
	void onIdle(wxIdleEvent& evt);

	void setMageRender(bool on);

	void createObjectTree();
	void createObjectProperties();

	void update(sf::Time elapsed);

	void updateTreeSelections();

	sf::View getFixedDefaultView();

	void onAddPrefab(wxCommandEvent& event);
	void onAddTilemap(wxCommandEvent& event);

public:
	EditorFrame *frame;
	EditorState *eState;

	wxDataViewItem treeRoot;

private:
	bool mageRender;
};

Editor* theEditor();
EditorState* theEditorState();

DECLARE_APP(Editor);