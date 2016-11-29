#include "Editor.h"

#include "wxSFMLCanvas.h"
#include "EditorFrame.h"
#include "EditorState.h"

#include "MAGE/jukebox.h"
#include "MAGE/helpers.h"
#include "MAGE/platform.h"

#include "MAGE/sfResources.h"
#include "MAGE/tilemap.h"

IMPLEMENT_APP_CONSOLE(Editor);

Editor* theEditor()
{
	auto e = dynamic_cast<Editor*>(wxTheApp);

	if (!e)
		p::fatal("wxTheApp was invalid");

	return e;
}

EditorState * theEditorState()
{
	return theEditor()->eState;
}

Editor::Editor()
	: Game(0, 0, nullptr)
{
	// init MAGE game here
	mageRender = false;
	worldObjectsFrozen = true;
	frame = nullptr;
	conservativeMouse = false;

	states->current = std::make_shared<EditorState>(); // normally deriving the gameState class is not allowed
	// we can get away with doing it this way though

	state = states->current;
	eState = dynamic_cast<EditorState*>(state.get());
	// now the base state has some extra functionality that will persist when others are "combined" with it

	music->volume = 0.f;
}

bool Editor::OnInit()
{
	// init wxWidgets here
	int defWidth = 1280;

	frame = new EditorFrame("MAGEdit", wxDefaultPosition, wxSize(defWidth, 720));
	frame->Show(true);

	// connect it to MAGE
	windowInit(frame->getSFMLViewport());
	setMageRender(true);

	state->clearObjects();

	treeRoot = frame->getObjectTree()->AppendContainer(wxDataViewItem(NULL), "GAMESTATE", -1, -1, new EditorItem(nullptr));

	return true;
}

void Editor::onIdle(wxIdleEvent & evt)
{
	frame->getSFMLViewport()->setMouseCursorVisible(true);

	if (mageRender) {
		fixViews();

		frame->getSFMLViewport()->setDrawMode(true);
		run(true);
		frame->getSFMLViewport()->setDrawMode(false);

		evt.RequestMore();

		// if the game closes so do we
		if (!getRenderWindow().isOpen()) {
			frame->Close(true);
		}
	}
}

void Editor::setMageRender(bool on)
{
	if (on && !mageRender)
	{
		Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(Editor::onIdle));
		mageRender = true;
	}
	else if (!on && mageRender)
	{
		Disconnect(wxEVT_IDLE, wxIdleEventHandler(Editor::onIdle));
		mageRender = false;
	}
}

void Editor::createObjectTree()
{
	// update the object tree
	auto tree = frame->getObjectTree();

	bool exp = tree->IsExpanded(treeRoot);
	tree->DeleteChildren(treeRoot);
	
	if(exp) tree->Expand(treeRoot);

	// add "State" item
	for (unsigned int i = 0; i < state->getNumObjects(); i++) {
		// in object mode, only add objects to the tree and avoid tilemaps
		if (eState->getMode() == EditorState::OBJECTS) {
			auto o = std::dynamic_pointer_cast<objBasic>(state->getObject(i));
			auto t = std::dynamic_pointer_cast<tilemap>(state->getObject(i));

			if (!o || t)
				continue;
		}
		// in ui mode, only add ui to the tree
		else if (eState->getMode() == EditorState::UI) {
			auto o = std::dynamic_pointer_cast<uiBasic>(state->getObject(i));

			if (!o)
				continue;
		}
		// in tilemap/collision mode, only add tilemaps to the tree
		else if (eState->getMode() == EditorState::TILEMAP || eState->getMode() == EditorState::COLLISION) {
			auto o = std::dynamic_pointer_cast<tilemap>(state->getObject(i));

			if (!o)
				continue;
		}

		auto item = tree->AppendItem(treeRoot, prefabs->nameOf(state->getObject(i)->prefabSource) + ": \"" + state->getObject(i)->uiName + "\"", -1, new EditorItem(state->getObject(i)));
	}

	updateTreeSelections();
}

void Editor::createObjectProperties()
{
	auto props = frame->getPropertyGrid();

	props->Clear();

	auto selection = eState->uniqueSelection();

	if (selection) {
		props->Append(new wxPropertyCategory("Object Properties"));

		// show object props
		for (unsigned int i = 0; i < selection->getPropertyList().size(); i++) {
			auto pName = selection->getPropertyList().at(i);
			auto prop = selection->getProperty(pName);
			auto pType = prop.getType();

			if (prop.hidden)
				continue;

			if (pType == basic::prop::FLOATPROP) {
				props->Append(new wxFloatProperty(pName, wxPG_LABEL, atof(prop.read().c_str())));
			}
			else if (pType == basic::prop::INTPROP || pType == basic::prop::COLPROP) {
				props->Append(new wxIntProperty(pName, wxPG_LABEL, atoi(prop.read().c_str())));
			}
			else if (pType == basic::prop::BOOLPROP) {
				props->Append(new wxBoolProperty(pName, wxPG_LABEL, atoi(prop.read().c_str()) > 0));
			}
			else if (pType == basic::prop::STRINGPROP) {
				props->Append(new wxStringProperty(pName, wxPG_LABEL, prop.read()));
			}
		}
	}
	else {
		// show gamestate props
		props->Append(new wxPropertyCategory("Map Bounds"));
		props->Append(new wxFloatProperty("Map Left", wxPG_LABEL, state->mapBounds.left));
		props->Append(new wxFloatProperty("Map Top", wxPG_LABEL, state->mapBounds.top));
		props->Append(new wxFloatProperty("Map Width", wxPG_LABEL, state->mapBounds.width));
		props->Append(new wxFloatProperty("Map Height", wxPG_LABEL, state->mapBounds.height));

		props->Append(new wxPropertyCategory("Background Color"));
		props->Append(new wxIntProperty("BG Red", wxPG_LABEL, state->bgCol.r));
		props->Append(new wxIntProperty("BG Blue", wxPG_LABEL, state->bgCol.b));
		props->Append(new wxIntProperty("BG Green", wxPG_LABEL, state->bgCol.g));
		
		props->Append(new wxPropertyCategory("Fog Color"));
		props->Append(new wxIntProperty("Amb Red", wxPG_LABEL, state->lightingAmb.r));
		props->Append(new wxIntProperty("Amb Blue", wxPG_LABEL, state->lightingAmb.b));
		props->Append(new wxIntProperty("Amb Green", wxPG_LABEL, state->lightingAmb.g));
		props->Append(new wxIntProperty("Amb Opacity", wxPG_LABEL, state->lightingAmb.a));
	}
}

void Editor::update(sf::Time elapsed)
{
	Game::update(elapsed);
	mouseMode = 0;
}

void Editor::updateTreeSelections()
{
	wxDataViewItemArray selectedItemsInTree;

	auto t = frame->getObjectTree();

	t->UnselectAll();

	for (unsigned int i = 0; i < eState->selections.size(); i++) {
		auto itm = EditorItem(eState->selections[i]);
		
		// find item in tree
		for (unsigned int j = 0; j < t->GetChildCount(treeRoot); j++) {
			auto ch = t->GetNthChild(treeRoot, j);
			auto itm2 = dynamic_cast<EditorItem*>(t->GetItemData(ch));

			if (itm.get() == itm2->get()) {
				selectedItemsInTree.Add(ch);
			}
		}
	}

	t->SetSelections(selectedItemsInTree);
}

sf::View Editor::getFixedDefaultView()
{
	return sf::View(sf::FloatRect(0.f, 0.f, frame->getSFMLViewport()->GetSize().x, frame->getSFMLViewport()->GetSize().y));
}

void Editor::onAddPrefab(wxCommandEvent & event)
{
	int objToCreate = event.GetId();

	if (objToCreate >= 0) {
		auto o = prefabs->newInstance(prefabs->nameOf(prefabs->getByIndex(objToCreate)));

		auto tm = std::dynamic_pointer_cast<tilemap>(o);

		if(!tm) // don't move tilemaps to the mouse
			o->setPosition(mouseWorldPos);

		state->attach(o);
	}

	createObjectTree();
	eState->editsUnsaved = true;
}

void Editor::onAddTilemap(wxCommandEvent & event)
{
	int tToCreate = event.GetId();

	if (tToCreate >= 0) {
		auto tList = prefabs->listType<tilemap>();

		auto tm = std::dynamic_pointer_cast<tilemap>(prefabs->newInstance(prefabs->nameOf(tList[tToCreate])));
		tm->resize(state->mapBounds.width / tm->getFrameSize().x, state->mapBounds.height / tm->getFrameSize().y);
	}

	createObjectTree();
	eState->editsUnsaved = true;
}
