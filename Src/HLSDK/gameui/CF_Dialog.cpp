#include "CF_Dialog.h"
#include "EngineInterface.h"

using namespace vgui;

// Force build it !
DECLARE_BUILD_FACTORY(CFDialog);

CFDialog::CFDialog(Panel *parent, const char *panelName) : Frame(parent, panelName)
{
	// Hide the title bar
	SetTitleBarVisible(false);

	// Can't change size the dialog
	SetSizeable(false);

	// Can't move the dialog
	SetMoveable(false);
}

void CFDialog::MoveToCenterOfScreen(void)
{
	int screenWide, screenTall;

	// Get screen size
	surface()->GetScreenSize(screenWide, screenTall);

	// Move to center
	SetPos(screenWide / 2 - GetWide() / 2, screenTall / 2 - GetTall() / 2);
}

void CFDialog::PaintBackground(void)
{
	// Don't draw background
}

void CFDialog::Paint(void)
{
	// Don't draw panel
}

void CFDialog::PaintBorder(void)
{
	// Don't draw border
}

void CFDialog::ApplySchemeSettings(IScheme *pScheme)
{
	// Unset the border for panel
	SetBorder(NULL);
}