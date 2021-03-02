#ifndef CF_RADIOBUTTON_H
#define CF_RADIOBUTTON_H

#include <vgui_controls/RadioButton.h>

// ITextureAPI.h
typedef struct tex_s tex_t;

namespace vgui
{

class CFRadioButton : public RadioButton
{
	DECLARE_CLASS_SIMPLE(CFRadioButton, RadioButton);

public:
	CFRadioButton(Panel *parent, const char *panelName);

	void SetRadioGroup(int group);
	int GetRadioGroup(void);

	void SetUpImage(const char *fileName);
	void SetFocusImage(const char *fileName);
	void SetDownImage(const char *fileName);
	void SetDisableImage(const char *fileName);

protected:
	virtual void PaintBackground(void);
	virtual void Paint(void);
	virtual void PaintBorder(void);
	virtual void ApplySettings(KeyValues *inResourceData);
	virtual void ApplySchemeSettings(IScheme *pScheme);

private:
	tex_t *		_upImage;
	tex_t *		_focusImage;
	tex_t *		_downImage;
	tex_t *		_disableImage;
};

}

#endif