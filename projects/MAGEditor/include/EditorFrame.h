#pragma once

#include <memory>
#include "wxAfx.h"
#include "wxSFMLCanvas.h"

class Editor;
class basic;

class EditorItem : public wxClientData {
public:
	EditorItem(std::shared_ptr<basic> ptr) { shP = ptr; }
	std::shared_ptr<basic> get() { return shP; }

private:
	std::shared_ptr<basic> shP;
};

class EditorFrame : public wxFrame
{
public:
	EditorFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

	bool doUnsavedCheck();
private:
	void OnSave(wxCommandEvent& event);
	void OnSaveAs(wxCommandEvent& event);
	void OnLoad(wxCommandEvent& event);
	void OnExit(wxCommandEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnNew(wxCommandEvent& event);
	void OnSelectionChanged(wxDataViewEvent& event);
	void OnPropertyChanged(wxPropertyGridEvent& event);
	void OnModeChanged(wxCommandEvent& event);
	void OnDeselect(wxCommandEvent& event);
	void OnDelete(wxCommandEvent& event);
	void OnDeleteS(wxCommandEvent& event);
	void OnBringToFront(wxCommandEvent& event);
	void OnSendToBack(wxCommandEvent& event);
	void OnCut(wxCommandEvent& event);
	void OnCopy(wxCommandEvent& event);
	void OnPaste(wxCommandEvent& event);
	void OnGrids(wxCommandEvent& event);

	wxDECLARE_EVENT_TABLE();

public:
	wxSFMLCanvas* getSFMLViewport();
	wxDataViewTreeCtrl* getObjectTree();
	wxPropertyGrid* getPropertyGrid();
	wxChoice* getModeChoice();
};

// window object IDs
enum
{
	ID_SFMLView = 1,
	ID_ObjectTree = 2,
	ID_Panel = 3,
	ID_PropertyGrid = 4,
	ID_ListBox = 5
};

// list item IDs
enum
{
	ID_Save = 10001,
	ID_SaveAs = 10002,
	ID_Load = 10003,
	ID_New = 10004,
	ID_Deselect = 10005,
	ID_DeleteS = 10006,
	ID_Delete = 10009,
	ID_BringToFront = 10007,
	ID_SendToBack = 10008,
	ID_Cut = 10010,
	ID_Copy = 10011,
	ID_Paste = 10012,
	ID_Undo = 10013,
	ID_Redo = 10014,
	ID_Grids = 10015
};