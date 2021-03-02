#ifndef CF_BUTTON_H
#define CF_BUTTON_H

#include <vgui_controls/Button.h>

// ITextureAPI.h
typedef struct tex_s tex_t;

namespace vgui
{

class CFButton : public Button
{
	DECLARE_CLASS_SIMPLE(CFButton, Button);

public:
	CFButton(Panel *parent, const char *panelName);

	void SetUpImage(const char *fileName);
	void SetFocusImage(const char *fileName);
	void SetDownImage(const char *fileName);
	void SetDisableImage(const char *fileName);

	//void SetCommand(const char *command, ...);

protected:
	virtual void OnMouseDoublePressed(MouseCode code);
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