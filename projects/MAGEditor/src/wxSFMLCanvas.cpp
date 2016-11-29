#include "wxSFMLCanvas.h"

// this is needed to use the p:: logging functions
#include "MAGE/platform.h"
#undef Status

// some extras needed for gtk
#ifdef __WXGTK__
	#include <gtk/gtk.h>
	#include <gdk/gdkx.h>
#endif

BEGIN_EVENT_TABLE(wxSFMLCanvas, wxControl)
EVT_IDLE(wxSFMLCanvas::OnIdle)
EVT_ERASE_BACKGROUND(wxSFMLCanvas::OnEraseBackground)
END_EVENT_TABLE()

wxSFMLCanvas::wxSFMLCanvas(wxWindow* Parent, wxWindowID Id, const wxPoint& Position, const wxSize& Size, long Style) :
	wxControl(Parent, Id, Position, Size, Style)
{
#ifdef __WXGTK__
	GtkWidget* widget = GetHandle();

	gtk_widget_realize(widget);
	gtk_widget_set_double_buffered(widget, false);

	GdkWindow* Win = gtk_widget_get_window( widget );

	XFlush(GDK_WINDOW_XDISPLAY(Win));

	sf::RenderWindow::create((sf::WindowHandle)GDK_WINDOW_XID(Win));
#else
	// Tested under Windows XP only (should work with X11
	// and other Windows versions - no idea about MacOS)
	sf::RenderWindow::create((sf::WindowHandle)GetHandle());
#endif

	drawLock = nullptr;

	p::info("this finished");
}

void wxSFMLCanvas::OnIdle(wxIdleEvent&)
{
	// Send a paint message when the control is idle, to ensure maximum framerate
	Refresh();
}

void wxSFMLCanvas::OnEraseBackground(wxEraseEvent&)
{
}

void wxSFMLCanvas::setDrawMode(bool onOrOff)
{
	if (onOrOff && !drawLock) {
		drawLock = new wxPaintDC(this);
	}
	else if (!onOrOff && drawLock) {
		delete drawLock;
		drawLock = nullptr;
	}
}
