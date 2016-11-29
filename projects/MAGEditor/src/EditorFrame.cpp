#include "EditorFrame.h"

#include "Editor.h"
#include "EditorState.h"

wxBEGIN_EVENT_TABLE(EditorFrame, wxFrame)
EVT_MENU(ID_Save, EditorFrame::OnSave)
EVT_MENU(ID_SaveAs, EditorFrame::OnSaveAs)
EVT_MENU(ID_Load, EditorFrame::OnLoad)
EVT_MENU(ID_New, EditorFrame::OnNew)
EVT_MENU(ID_Deselect, EditorFrame::OnDeselect)
EVT_MENU(ID_Delete, EditorFrame::OnDelete)
EVT_MENU(ID_DeleteS, EditorFrame::OnDeleteS)
EVT_MENU(ID_BringToFront, EditorFrame::OnBringToFront)
EVT_MENU(ID_SendToBack, EditorFrame::OnSendToBack)
EVT_MENU(ID_Cut, EditorFrame::OnCut)
EVT_MENU(ID_Copy, EditorFrame::OnCopy)
EVT_MENU(ID_Paste, EditorFrame::OnPaste)
EVT_MENU(wxID_EXIT, EditorFrame::OnExit)
EVT_MENU(wxID_ABOUT, EditorFrame::OnAbout)
EVT_MENU(ID_Grids, EditorFrame::OnGrids)
wxEND_EVENT_TABLE()

EditorFrame::EditorFrame(const wxString & title, const wxPoint & pos, const wxSize & size)
	: wxFrame(NULL, wxID_ANY, title, pos, size)
{
	// toolbar
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(ID_New, "&New\tCtrl-N",
		"Create a new map.");
	menuFile->Append(ID_Save, "&Save\tCtrl-S",
		"Save the map. You will be asked to specify a file if one wasn't already.");
	menuFile->Append(ID_SaveAs, "&Save As...\tCtrl-Alt-S",
		"Save the map to a specific file.");
	menuFile->Append(ID_Load, "&Open...\tCtrl-O",
		"Load a map into the editor from a specific file.");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);

	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT);

	wxMenu *menuEdit = new wxMenu;
	menuEdit->Append(ID_Undo, "&Undo\tCtrl-Z",
		"Undo the last thing you did.");
	menuEdit->Append(ID_Redo, "&Redo\tCtrl-Shift-Z",
		"Redo the last thing you undid.");
	menuEdit->AppendSeparator();
	menuEdit->Append(ID_Cut, "&Cut\tCtrl-X",
		"Cut selected objects.");
	menuEdit->Append(ID_Copy, "&Copy\tCtrl-C",
		"Copy selected objects.");
	menuEdit->Append(ID_Paste, "&Paste\tCtrl-V",
		"Paste objects from the internal clipboard.");

	menuEdit->Enable(ID_Undo, false);
	menuEdit->Enable(ID_Redo, false);

	wxMenu *menuSelection = new wxMenu;
	menuSelection->Append(ID_Deselect, "&Deselect\tCtrl-D",
		"Deselect selected objects.");
	menuSelection->AppendSeparator();
	menuSelection->Append(ID_Delete, "&Delete\tDel",
		"Delete whatever is selected.");
	menuSelection->Append(ID_DeleteS, "&Delete Objects\tCtrl-Del",
		"Delete selected objects. Note that in tilemap mode this deletes the selected tilemap, not selected tiles!");
	menuSelection->Append(ID_BringToFront, "&Bring to Front\tCtrl-Alt-Q",
		"Move object to the bottom of the object list. Note that this won't work for depth ordered object types.");
	menuSelection->Append(ID_Grids, "&Toggle Grids\tCtrl-G",
		"Allow or prevent the displaying of grids.");
	//menuSelection->Append(ID_SendToBack, "&Send to Back\tCtrl-Alt-E",
		//"Move object to the top of the object list. Note that this won't work for depth ordered object types.");

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuEdit, "&Edit");
	menuBar->Append(menuSelection, "&Selection");
	menuBar->Append(menuHelp, "&Help");

	SetMenuBar(menuBar);

	// status bar
	CreateStatusBar();
	SetStatusText("Ready.");

	// panel
	wxPanel *panel = new wxPanel(this, ID_Panel);

	// box sizer
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	panel->SetSizer(sizer);

	// sfml viewport
	wxSFMLCanvas* sfmlViewport = new wxSFMLCanvas(panel, ID_SFMLView);
	sizer->Add(sfmlViewport, 4, wxEXPAND);

	// 2nd panel
	wxPanel* panel2 = new wxPanel(panel, ID_Panel, wxDefaultPosition, wxSize(400, 0));
	sizer->Add(panel2, 1, wxEXPAND);

	// 2nd box sizer
	wxBoxSizer* sizer2 = new wxBoxSizer(wxVERTICAL);
	panel2->SetSizer(sizer2);

	// mode menu
	std::vector<wxString> choicesA = { "Object Mode", "Ui Mode", "Tilemap Mode", "Collision Mode" };
	wxArrayString choices;

	for (unsigned int i = 0; i < choicesA.size(); i++) {
		choices.Add(choicesA[i]);
	}

	wxChoice* lb = new wxChoice(panel2, ID_ListBox, wxDefaultPosition, wxDefaultSize, choices);
	sizer2->Add(lb, 0, wxEXPAND);

	Connect(wxID_ANY, wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler(EditorFrame::OnModeChanged), 0, this);

	// object tree
	wxDataViewTreeCtrl* objectTree = new wxDataViewTreeCtrl(panel2, ID_ObjectTree, wxDefaultPosition, wxDefaultSize);
	sizer2->Add(objectTree, 2, wxEXPAND);

	Connect(wxID_ANY, wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler(EditorFrame::OnSelectionChanged), 0, this);

	// property grid
	wxPropertyGrid* propGrid = new wxPropertyGrid(panel2, ID_PropertyGrid);
	sizer2->Add(propGrid, 1, wxEXPAND);

	Connect(wxID_ANY, wxEVT_PG_CHANGED, wxPropertyGridEventHandler(EditorFrame::OnPropertyChanged), 0, this);
}

bool EditorFrame::doUnsavedCheck()
{
	if (theEditorState()->editsUnsaved)
	{
		if (wxMessageBox(_("Current content has not been saved! Proceed?"), _("Please confirm"),
			wxICON_QUESTION | wxYES_NO, this) == wxNO)
			return true;
		//else: proceed asking to the user the new file to open
	}

	return false;
}

void EditorFrame::OnSave(wxCommandEvent & event)
{
	if (theEditorState()->curLevelPath == "new") {
		OnSaveAs(event);
	}

	theEditorState()->saveToFile(theEditorState()->curLevelPath);
}

void EditorFrame::OnSaveAs(wxCommandEvent & event)
{
	wxFileDialog
		openFileDialog(this, _("Save ALCH file"), "", "",
			"MAGE Game State (*.alch)|*.alch", wxFD_SAVE);

	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;     // the user changed idea...

	theEditorState()->saveToFile(openFileDialog.GetPath().ToStdString());
}

void EditorFrame::OnLoad(wxCommandEvent & event)
{
	if (doUnsavedCheck())
		return;

	wxFileDialog
		openFileDialog(this, _("Open ALCH file"), "", "",
			"MAGE Game State (*.alch)|*.alch", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return;     // the user changed idea...

	theEditorState()->clearObjects();

	theEditorState()->ignoreIncorporation = false;
	theEditorState()->loadFromFile(openFileDialog.GetPath().ToStdString());
	theEditorState()->curLevelPath = openFileDialog.GetPath().ToStdString();
	theEditorState()->ignoreIncorporation = true;
}

void EditorFrame::OnExit(wxCommandEvent& event)
{
	if (doUnsavedCheck())
		return;

	Close(true);
}

void EditorFrame::OnAbout(wxCommandEvent& event)
{
	auto ver = theEditor()->version();

	wxMessageBox("This is MAGEdit r" + std::to_string(ver) + "\n\nPlease ensure the version of MAGEdit you are using is the one for the game you're editing, or things can mess up.\n\nCopyright Chris Sixsmith 2016.",
		"About MAGEdit", wxOK | wxICON_INFORMATION);
}

void EditorFrame::OnNew(wxCommandEvent & event)
{
	if (doUnsavedCheck())
		return;

	theEditorState()->clearObjects();
}

void EditorFrame::OnSelectionChanged(wxDataViewEvent& event)
{
	auto d = getObjectTree()->GetItemData(event.GetItem());
	auto editorItem = dynamic_cast<EditorItem*>(d);

	if (editorItem) {
		theEditorState()->setSelectedObject(editorItem->get());
	}
}

void EditorFrame::OnPropertyChanged(wxPropertyGridEvent & event)
{
	auto propName = event.GetPropertyName();
	auto propVal = event.GetPropertyValue();
	
	theEditorState()->updateObjectProp(propName.ToStdString(), propVal);
}

void EditorFrame::OnModeChanged(wxCommandEvent & event)
{
	theEditorState()->setMode((EditorState::Mode)event.GetSelection());
}

void EditorFrame::OnDeselect(wxCommandEvent & event)
{
	theEditorState()->doDeselect();
}

void EditorFrame::OnDeleteS(wxCommandEvent & event)
{
	theEditorState()->doSelectionDelete();
}

void EditorFrame::OnDelete(wxCommandEvent & event)
{
	theEditorState()->doDelete();
}

void EditorFrame::OnBringToFront(wxCommandEvent & event)
{
	theEditorState()->bringSelectionToFront();
}

void EditorFrame::OnSendToBack(wxCommandEvent & event)
{
	theEditorState()->sendSelectionToBack();
}

void EditorFrame::OnCut(wxCommandEvent & event)
{
	theEditorState()->doCut();
}

void EditorFrame::OnCopy(wxCommandEvent & event)
{
	theEditorState()->doCopy();
}

void EditorFrame::OnPaste(wxCommandEvent & event)
{
	theEditorState()->doPaste();
}

void EditorFrame::OnGrids(wxCommandEvent & event) {
	theEditorState()->toggleGrids();
}

wxSFMLCanvas * EditorFrame::getSFMLViewport()
{
	return dynamic_cast<wxSFMLCanvas*>(FindWindowById(ID_SFMLView));
}

wxDataViewTreeCtrl * EditorFrame::getObjectTree()
{
	return dynamic_cast<wxDataViewTreeCtrl*>(FindWindowById(ID_ObjectTree));
}

wxPropertyGrid * EditorFrame::getPropertyGrid()
{
	return dynamic_cast<wxPropertyGrid*>(FindWindowById(ID_PropertyGrid));
}

wxChoice * EditorFrame::getModeChoice()
{
	return dynamic_cast<wxChoice*>(FindWindowById(ID_ListBox));
}
