#ifndef CF_BASE_H
#define CF_BASE_H

#include "CF_Button.h"
#include "CF_Dialog.h"
#include "CF_Edit.h"
#include "CF_Image.h"
#include "CF_RadioButton.h"
#include "CF_ScrollBar.h"
#include "CF_Slider.h"
#include "CF_Sprite.h"
#include "CF_Static.h"
#include "CF_Tab.h"		//non-panel
#include "CF_Window.h"
#include "CF_KeyEdit.h"

#define FIND_CONTROL_BUTTON(name) dynamic_cast<vgui::CFButton*>(FindChildByName(name))
#define FIND_CONTROL_DIALOG(name) dynamic_cast<vgui::CFDialog*>(FindChildByName(name))
#define FIND_CONTROL_EDIT(name) dynamic_cast<vgui::CFEdit*>(FindChildByName(name))
#define FIND_CONTROL_IMAGE(name) dynamic_cast<vgui::CFImage*>(FindChildByName(name))
#define FIND_CONTROL_RADIOBUTTON(name) dynamic_cast<vgui::CFRadioButton*>(FindChildByName(name))
#define FIND_CONTROL_SCROLLBAR(name) dynamic_cast<vgui::CFScrollBar*>(FindChildByName(name))
#define FIND_CONTROL_SLIDER(name) dynamic_cast<vgui::CFSlider*>(FindChildByName(name))
#define FIND_CONTROL_SPRITE(name) dynamic_cast<vgui::CFSprite*>(FindChildByName(name))
#define FIND_CONTROL_STATIC(name) dynamic_cast<vgui::CFStatic*>(FindChildByName(name))
#define FIND_CONTROL_WINDOW(name) dynamic_cast<vgui::CFWindow*>(FindChildByName(name))
#define FIND_CONTROL_KEYEDIT(name) dynamic_cast<vgui::CFKeyEdit*>(FindChildByName(name))

#define PROPORT ( g_pVGuiSchemeManager->GetProportionalScaledValue )
#define DEPROPORT ( g_pVGuiSchemeManager->GetProportionalNormalizedValue )

#endif