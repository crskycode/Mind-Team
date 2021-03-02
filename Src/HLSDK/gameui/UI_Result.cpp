#include "UI_Result.h"
#include "CF_Base.h"

UIResult::UIResult(vgui::Panel *parent) : vgui::CFDialog(parent, "Result")
{
	SetProportional(true);

	LoadControlSettings("UI/Scripts/Result/Result_Base.txt");
	LoadControlSettings("UI/Scripts/Result/Result_MyResult_MVP.txt");
	LoadControlSettings("UI/Scripts/Result/Result_OK.txt");
	LoadControlSettings("UI/Scripts/Result/Result_TeamList.txt");
}