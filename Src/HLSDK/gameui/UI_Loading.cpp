#include "UI_Loading.h"
#include "CF_Base.h"

UILoading::UILoading(vgui::Panel *parent) : vgui::CFDialog(parent, "Loading")
{
	SetProportional(true);

	LoadControlSettings("UI/Scripts/Loading.txt");

	m_cImageLoadingMapBg = FIND_CONTROL_IMAGE("ImageLoadingMapBg");
	m_cImageLoadingProgressBar = FIND_CONTROL_IMAGE("ImageLoadingProgressBar");
	m_cImageLoadingWaitIcon = FIND_CONTROL_IMAGE("ImageLoadingWaitIcon");

	m_iBarWidth = m_cImageLoadingProgressBar->GetWide();

	SetProgress( 1 );
}

void UILoading::SetMapBg(const char *fileName)
{
	m_cImageLoadingMapBg->SetImage(fileName);
}

void UILoading::SetProgress(int value)
{
	float percen = value / 100.0;

	m_cImageLoadingProgressBar->SetClip(0, 0, 728 * percen, 20);
	m_cImageLoadingProgressBar->SetWide(m_iBarWidth * percen);
}