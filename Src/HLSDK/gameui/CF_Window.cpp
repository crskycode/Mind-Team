#include "CF_Window.h"

using namespace vgui;

// Force build it !
DECLARE_BUILD_FACTORY(CFWindow);

CFWindow::CFWindow(Panel *parent, const char *panelName) : EditablePanel(parent, panelName)
{
	m_iProportionalWidth = 800;
	m_iProportionalHeight = 600;
}

void CFWindow::SetProportionalBase(int width, int height)
{
	m_iProportionalWidth = width;
	m_iProportionalHeight = height;
}

void CFWindow::GetProportionalBase(int &width, int &height)
{
	width = m_iProportionalWidth;
	height = m_iProportionalHeight;
}

void CFWindow::OnSizeChanged(int wide, int tall)
{
	// Don't adjust child position when window size is change

	InvalidateLayout();

	for (int i = 0; i < GetChildCount(); i++)
	{
		GetChild(i)->InvalidateLayout();
	}

	Repaint();
}

void CFWindow::PaintBackground(void)
{
	// Don't draw background
}

void CFWindow::Paint(void)
{
	// Don't draw anything
}

void CFWindow::PaintBorder(void)
{
	// Don't draw border
}

void CFWindow::LoadControlSettings(const char *dialogResourceName, const char *pathID, KeyValues *pPreloadedKeyValues)
{
	BaseClass::LoadControlSettings(dialogResourceName, pathID, pPreloadedKeyValues);
	LayoutProportional();
}

void CFWindow::LayoutProportional(void)
{
	int screenWidth, screenHeight;

	surface()->GetScreenSize(screenWidth, screenHeight);

	float scale = screenHeight / (float)m_iProportionalHeight;

	for (int i = 0; i < GetChildCount(); i++)
	{
		int x, y, w, h;

		Panel *child = GetChild(i);

		child->GetBounds(x, y, w, h);
		child->SetBounds(x * scale, y * scale, w * scale, h * scale);
	}
}