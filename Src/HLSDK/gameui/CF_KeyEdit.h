#ifndef CF_KEYEDIT_H
#define CF_KEYEDIT_H

#include <vgui_controls/Panel.h>

// ITextureAPI.h
typedef struct tex_s tex_t;

namespace vgui
{

class CFKeyEdit : public Panel
{
	DECLARE_CLASS_SIMPLE(CFKeyEdit, Panel);

public:
	CFKeyEdit(Panel *parent, const char *panelName);

	virtual void SetVisible(bool state);
	virtual void SetEnabled(bool state);

	virtual void SetCaptureKeyboardEnable(bool state);
	virtual void SetCaptureMouseEnable(bool state);

	virtual void SetKey(const char *name);
	virtual char *GetKey(void);

	virtual void SetUpImage(const char *fileName);
	virtual void SetFocusImage(const char *fileName);
	virtual void SetDownImage(const char *fileName);

protected:
	virtual void ApplySettings(KeyValues *inResourceData);
	virtual void OnCursorEntered(void);
	virtual void OnCursorExited(void);
	virtual void OnKeyCodePressed(KeyCode code);
	virtual void OnMousePressed(MouseCode code);
	virtual void OnMouseReleased(MouseCode code);
	virtual void OnMouseWheeled(int delta);

	virtual void Paint(void);

private:
	void StartCapture(void);
	void EndCapture(KeyCode keyCode, MouseCode mouseCode, int wheelCode);

	int			_actively;
	bool		_capturing;
	bool		_captureKeyboard;
	bool		_captureMouse;
	char		_keyName[32];
	tex_t		*_upImage;
	tex_t		*_focusImage;
	tex_t		*_downImage;
};

}

#endif