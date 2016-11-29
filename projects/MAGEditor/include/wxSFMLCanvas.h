#pragma once

#include "wxAfx.h"

#include "SFML/Graphics.hpp"

class wxSFMLCanvas : public wxControl, public sf::RenderWindow
{
public:

	wxSFMLCanvas(wxWindow* Parent = NULL, wxWindowID Id = -1, const wxPoint& Position = wxDefaultPosition,
		const wxSize& Size = wxDefaultSize, long Style = 0);

	virtual ~wxSFMLCanvas() {};

	void setDrawMode(bool onOrOff);

private:

	DECLARE_EVENT_TABLE()

	virtual void OnUpdate() {};

	void OnIdle(wxIdleEvent&);

	void OnEraseBackground(wxEraseEvent&);

	wxPaintDC* drawLock;
};