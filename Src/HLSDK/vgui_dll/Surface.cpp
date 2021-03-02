#include <vgui/VGUI.h>
#include <vgui/IHTML.h>
#include <vgui/ISurfaceV26.h>
#include <vgui/ISurface.h>

#include "vgui_internal.h"

#include <Color.h>
#include <tier1/utlrbtree.h>
#include <tier1/strtools.h>

#include <interface.h>

namespace vgui
{

class Texture
{
public:
	int id;
	char filename[MAX_PATH];
};

static bool TextureLessFunc(const Texture &lhs, const Texture &rhs)
{
	return lhs.id < rhs.id;
}

static CUtlRBTree<Texture, int> m_VGuiSurfaceTextures(0, 128, TextureLessFunc);

static Texture *AllocTextureForId(int id)
{
	Texture newTex = { id };
	int index = m_VGuiSurfaceTextures.Insert(newTex);
	return &m_VGuiSurfaceTextures[index];
}

static Texture *GetTextureById(int id)
{
	Texture findTex = { id };

	int index = m_VGuiSurfaceTextures.Find(findTex);

	if (m_VGuiSurfaceTextures.IsValidIndex(index))
	{
		return &m_VGuiSurfaceTextures[index];
	}

	return NULL;
}


class CSurface : public ISurface
{
public:
	CSurface();

	virtual void Shutdown(void);
	virtual void RunFrame(void);
	virtual VPANEL GetEmbeddedPanel(void);
	virtual void SetEmbeddedPanel(VPANEL pPanel);
	virtual void PushMakeCurrent(VPANEL panel, bool useInsets);
	virtual void PopMakeCurrent(VPANEL panel);
	virtual void DrawSetColor(int r, int g, int b, int a);
	virtual void DrawSetColor(Color col);
	virtual void DrawFilledRect(int x0, int y0, int x1, int y1);
	virtual void DrawFilledRectArray(IntRect *pRects, int numRects);
	virtual void DrawOutlinedRect(int x0, int y0, int x1, int y1);
	virtual void DrawLine(int x0, int y0, int x1, int y1);
	virtual void DrawPolyLine(int *px, int *py, int numPoints);
	virtual void DrawSetTextFont(HFont font);
	virtual void DrawSetTextColor(int r, int g, int b, int a);
	virtual void DrawSetTextColor(Color col);
	virtual void DrawSetTextPos(int x, int y);
	virtual void DrawGetTextPos(int& x,int& y);
	virtual void DrawPrintText(const wchar_t *text, int textLen, FontDrawType_t drawType = FONT_DRAW_DEFAULT);
	virtual void DrawUnicodeChar(wchar_t wch, FontDrawType_t drawType = FONT_DRAW_DEFAULT);
	virtual void DrawFlushText(void);
	virtual IHTML *CreateHTMLWindow(vgui::IHTMLEvents *events,VPANEL context);
	virtual void PaintHTMLWindow(vgui::IHTML *htmlwin);
	virtual void DeleteHTMLWindow(IHTML *htmlwin);
	virtual int	 DrawGetTextureId(char const *filename);
	virtual bool DrawGetTextureFile(int id, char *filename, int maxlen);
	virtual void DrawSetTextureFile(int id, const char *filename, int hardwareFilter, bool forceReload);
	virtual void DrawSetTextureRGBA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload);
	virtual void DrawSetTexture(int id);
	virtual void DrawGetTextureSize(int id, int &wide, int &tall);
	virtual void DrawTexturedRect(int x0, int y0, int x1, int y1);
	virtual bool IsTextureIDValid(int id);
	virtual int CreateNewTextureID(bool procedural = false);
#ifdef _X360
	virtual void DestroyTextureID(int id);
	virtual void UncacheUnusedMaterials(void);
#endif
	virtual void GetScreenSize(int &wide, int &tall);
	virtual void SetAsTopMost(VPANEL panel, bool state);
	virtual void BringToFront(VPANEL panel);
	virtual void SetForegroundWindow (VPANEL panel);
	virtual void SetPanelVisible(VPANEL panel, bool state);
	virtual void SetMinimized(VPANEL panel, bool state);
	virtual bool IsMinimized(VPANEL panel);
	virtual void FlashWindow(VPANEL panel, bool state);
	virtual void SetTitle(VPANEL panel, const wchar_t *title);
	virtual void SetAsToolBar(VPANEL panel, bool state);
	virtual void CreatePopup(VPANEL panel, bool minimised, bool showTaskbarIcon = true, bool disabled = false, bool mouseInput = true , bool kbInput = true);
	virtual void SwapBuffers(VPANEL panel);
	virtual void Invalidate(VPANEL panel);
	virtual void SetCursor(HCursor cursor);
	virtual bool IsCursorVisible(void);
	virtual void ApplyChanges(void);
	virtual bool IsWithin(int x, int y);
	virtual bool HasFocus(void);
	virtual bool SupportsFeature(SurfaceFeature_e feature);
	virtual void RestrictPaintToSinglePanel(VPANEL panel);
	virtual void SetModalPanel(VPANEL panel);
	virtual VPANEL GetModalPanel(void);
	virtual void UnlockCursor(void);
	virtual void LockCursor(void);
	virtual void SetTranslateExtendedKeys(bool state);
	virtual VPANEL GetTopmostPopup(void);
	virtual void SetTopLevelFocus(VPANEL panel);
	virtual HFont CreateFont(void);
	virtual bool SetFontGlyphSet(HFont font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags);
	virtual bool AddCustomFontFile(const char *fontFileName);
	virtual int GetFontTall(HFont font);
	virtual int GetFontAscent(HFont font, wchar_t wch);
	virtual bool IsFontAdditive(HFont font);
	virtual void GetCharABCwide(HFont font, int ch, int &a, int &b, int &c);
	virtual int GetCharacterWidth(HFont font, int ch);
	virtual void GetTextSize(HFont font, const wchar_t *text, int &wide, int &tall);
	virtual VPANEL GetNotifyPanel(void);
	virtual void SetNotifyIcon(VPANEL context, HTexture icon, VPANEL panelToReceiveMessages, const char *text);
	virtual void PlaySound(const char *fileName);
	virtual int GetPopupCount(void);
	virtual VPANEL GetPopup(int index);
	virtual bool ShouldPaintChildPanel(VPANEL childPanel);
	virtual bool RecreateContext(VPANEL panel);
	virtual void AddPanel(VPANEL panel);
	virtual void ReleasePanel(VPANEL panel);
	virtual void MovePopupToFront(VPANEL panel);
	virtual void MovePopupToBack(VPANEL panel);
	virtual void SolveTraverse(VPANEL panel, bool forceApplySchemeSettings = false);
	virtual void PaintTraverse(VPANEL panel);
	virtual void EnableMouseCapture(VPANEL panel, bool state);
	virtual void GetWorkspaceBounds(int &x, int &y, int &wide, int &tall);
	virtual void GetAbsoluteWindowBounds(int &x, int &y, int &wide, int &tall);
	virtual void GetProportionalBase(int &width, int &height);
	virtual void CalculateMouseVisible(void);
	virtual bool NeedKBInput(void);
	virtual bool HasCursorPosFunctions(void);
	virtual void SurfaceGetCursorPos(int &x, int &y);
	virtual void SurfaceSetCursorPos(int x, int y);
	virtual void DrawTexturedLine(const Vertex_t &a, const Vertex_t &b);
	virtual void DrawOutlinedCircle(int x, int y, int radius, int segments);
	virtual void DrawTexturedPolyLine(const Vertex_t *p,int n);
	virtual void DrawTexturedSubRect(int x0, int y0, int x1, int y1, float texs0, float text0, float texs1, float text1);
	virtual void DrawTexturedPolygon(int n, Vertex_t *pVertices);
	virtual const wchar_t *GetTitle(VPANEL panel);
	virtual bool IsCursorLocked(void) const;
	virtual void SetWorkspaceInsets(int left, int top, int right, int bottom);
	virtual bool DrawGetUnicodeCharRenderInfo(wchar_t ch, CharRenderInfo& info);
	virtual void DrawRenderCharFromInfo(const CharRenderInfo& info);
	virtual void DrawSetAlphaMultiplier(float alpha);
	virtual float DrawGetAlphaMultiplier(void);
	virtual void SetAllowHTMLJavaScript(bool state);
	virtual void OnScreenSizeChanged(int nOldWidth, int nOldHeight);
	virtual vgui::HCursor CreateCursorFromFile(char const *curOrAniFile, char const *pPathID = 0);
	virtual void *DrawGetTextureMatInfoFactory(int id);
	virtual void PaintTraverseEx(VPANEL panel, bool paintPopups = false);
	virtual float GetZPos(void) const;
	virtual void SetPanelForInput(VPANEL vpanel);
	virtual void DrawFilledRectFade(int x0, int y0, int x1, int y1, unsigned int alpha0, unsigned int alpha1, bool bHorizontal);
	virtual void DrawSetTextureRGBAEx(int id, const unsigned char *rgba, int wide, int tall, int imageFormat);
	virtual void DrawSetTextScale(float sx, float sy);
	virtual bool SetBitmapFontGlyphSet(HFont font, const char *windowsFontName, float scalex, float scaley, int flags);
	virtual bool AddBitmapFontFile(const char *fontFileName);
	virtual void SetBitmapFontName(const char *pName, const char *pFontFilename);
	virtual const char *GetBitmapFontName(const char *pName);
	virtual void ClearTemporaryFontCache(void);
	virtual IImage *GetIconImageForFullPath(char const *pFullPath);
	virtual void DrawUnicodeString(const wchar_t *pwString, FontDrawType_t drawType = FONT_DRAW_DEFAULT);
	virtual void PrecacheFontCharacters(HFont font, wchar_t *pCharacters);
	virtual const char *GetResolutionKey(void) const;
};

CSurface g_Surface;

EXPOSE_SINGLE_INTERFACE(CSurface, ISurface, VGUI_SURFACE_INTERFACE_VERSION);


CSurface::CSurface()
{
	g_pSurface = this;
}

void CSurface::Shutdown(void)
{
	g_pSurfaceV26->Shutdown();
}

void CSurface::RunFrame(void)
{
	g_pSurfaceV26->RunFrame();
}

VPANEL CSurface::GetEmbeddedPanel(void)
{
	return g_pSurfaceV26->GetEmbeddedPanel();
}

void CSurface::SetEmbeddedPanel(VPANEL pPanel)
{
	g_pSurfaceV26->SetEmbeddedPanel(pPanel);
}

void CSurface::PushMakeCurrent(VPANEL panel, bool useInsets)
{
	g_pSurfaceV26->PushMakeCurrent(panel, useInsets);
}

void CSurface::PopMakeCurrent(VPANEL panel)
{
	g_pSurfaceV26->PopMakeCurrent(panel);
}

void CSurface::DrawSetColor(int r, int g, int b, int a)
{
	g_pSurfaceV26->DrawSetColor(r, g, b, a);
}

void CSurface::DrawSetColor(Color col)
{
	g_pSurfaceV26->DrawSetColor(col);
}

void CSurface::DrawFilledRect(int x0, int y0, int x1, int y1)
{
	g_pSurfaceV26->DrawFilledRect(x0, y0, x1, y1);
}

void CSurface::DrawFilledRectArray(IntRect *pRects, int numRects)
{
}

void CSurface::DrawOutlinedRect(int x0, int y0, int x1, int y1)
{
	g_pSurfaceV26->DrawOutlinedRect(x0, y0, x1, y1);
}

void CSurface::DrawLine(int x0, int y0, int x1, int y1)
{
	g_pSurfaceV26->DrawLine(x0, y0, x1, y1);
}

void CSurface::DrawPolyLine(int *px, int *py, int numPoints)
{
	g_pSurfaceV26->DrawPolyLine(px, py, numPoints);
}

void CSurface::DrawSetTextFont(HFont font)
{
	g_pSurfaceV26->DrawSetTextFont(font);
}

void CSurface::DrawSetTextColor(int r, int g, int b, int a)
{
	g_pSurfaceV26->DrawSetTextColor(r, g, b, a);
}

void CSurface::DrawSetTextColor(Color col)
{
	g_pSurfaceV26->DrawSetTextColor(col);
}

void CSurface::DrawSetTextPos(int x, int y)
{
	g_pSurfaceV26->DrawSetTextPos(x, y);
}

void CSurface::DrawGetTextPos(int& x,int& y)
{
	g_pSurfaceV26->DrawGetTextPos(x, y);
}

void CSurface::DrawPrintText(const wchar_t *text, int textLen, FontDrawType_t drawType)
{
	g_pSurfaceV26->DrawPrintText(text, textLen);
}

void CSurface::DrawUnicodeChar(wchar_t wch, FontDrawType_t drawType)
{
	g_pSurfaceV26->DrawUnicodeChar(wch);
}

void CSurface::DrawFlushText(void)
{
	g_pSurfaceV26->DrawFlushText();
}

IHTML *CSurface::CreateHTMLWindow(vgui::IHTMLEvents *events,VPANEL context)
{
	return g_pSurfaceV26->CreateHTMLWindow(events, context);
}

void CSurface::PaintHTMLWindow(vgui::IHTML *htmlwin)
{
	g_pSurfaceV26->PaintHTMLWindow(htmlwin);
}

void CSurface::DeleteHTMLWindow(IHTML *htmlwin)
{
	g_pSurfaceV26->DeleteHTMLWindow(htmlwin);
}

int CSurface::DrawGetTextureId(char const *filename)
{
	int i = m_VGuiSurfaceTextures.FirstInorder();

	while (i != m_VGuiSurfaceTextures.InvalidIndex())
	{
		Texture *texture = &m_VGuiSurfaceTextures[i];

		if (!Q_stricmp(filename, texture->filename))
			return texture->id;

		i = m_VGuiSurfaceTextures.NextInorder(i);
	}

	return -1;
}

bool CSurface::DrawGetTextureFile(int id, char *filename, int maxlen)
{
	Texture *texture = GetTextureById(id);

	if (!texture)
		return false;

	Q_strncpy(filename, texture->filename, maxlen);
	return true;
}

void CSurface::DrawSetTextureFile(int id, const char *filename, int hardwareFilter, bool forceReload)
{
	Texture *texture = GetTextureById(id);

	if (!texture)
	{
		texture = AllocTextureForId(id);
		Q_memset(texture, 0, sizeof(Texture));
		texture->id = id;
		Q_strncpy(texture->filename, filename, MAX_PATH - 1);
	}

	g_pSurfaceV26->DrawSetTextureFile(id, filename, hardwareFilter, forceReload);
}

void CSurface::DrawSetTextureRGBA(int id, const unsigned char *rgba, int wide, int tall, int hardwareFilter, bool forceReload)
{
	g_pSurfaceV26->DrawSetTextureRGBA(id, rgba, wide, tall, hardwareFilter, forceReload);
}

void CSurface::DrawSetTexture(int id)
{
	g_pSurfaceV26->DrawSetTexture(id);
}

void CSurface::DrawGetTextureSize(int id, int &wide, int &tall)
{
	g_pSurfaceV26->DrawGetTextureSize(id, wide, tall);
}

void CSurface::DrawTexturedRect(int x0, int y0, int x1, int y1)
{
	g_pSurfaceV26->DrawTexturedRect(x0, y0, x1, y1);
}

bool CSurface::IsTextureIDValid(int id)
{
	return g_pSurfaceV26->IsTextureIDValid(id);
}

int CSurface::CreateNewTextureID(bool procedural)
{
	return g_pSurfaceV26->CreateNewTextureID(procedural);
}

#ifdef _X360
void CSurface::DestroyTextureID(int id)
{
}

void CSurface::UncacheUnusedMaterials(void)
{
}
#endif

void CSurface::GetScreenSize(int &wide, int &tall)
{
	g_pSurfaceV26->GetScreenSize(wide, tall);
}

void CSurface::SetAsTopMost(VPANEL panel, bool state)
{
	g_pSurfaceV26->SetAsTopMost(panel, state);
}

void CSurface::BringToFront(VPANEL panel)
{
	g_pSurfaceV26->BringToFront(panel);
}

void CSurface::SetForegroundWindow(VPANEL panel)
{
	g_pSurfaceV26->SetForegroundWindow(panel);
}

void CSurface::SetPanelVisible(VPANEL panel, bool state)
{
	g_pSurfaceV26->SetPanelVisible(panel, state);
}

void CSurface::SetMinimized(VPANEL panel, bool state)
{
	g_pSurfaceV26->SetMinimized(panel, state);
}

bool CSurface::IsMinimized(VPANEL panel)
{
	return g_pSurfaceV26->IsMinimized(panel);
}

void CSurface::FlashWindow(VPANEL panel, bool state)
{
	g_pSurfaceV26->FlashWindow(panel, state);
}

void CSurface::SetTitle(VPANEL panel, const wchar_t *title)
{
	g_pSurfaceV26->SetTitle(panel, title);
}

void CSurface::SetAsToolBar(VPANEL panel, bool state)
{
	g_pSurfaceV26->SetAsToolBar(panel, state);
}

void CSurface::CreatePopup(VPANEL panel, bool minimised, bool showTaskbarIcon, bool disabled, bool mouseInput, bool kbInput)
{
	g_pSurfaceV26->CreatePopup(panel, minimised, showTaskbarIcon, disabled, mouseInput, kbInput);
}

void CSurface::SwapBuffers(VPANEL panel)
{
	g_pSurfaceV26->SwapBuffers(panel);
}

void CSurface::Invalidate(VPANEL panel)
{
	g_pSurfaceV26->Invalidate(panel);
}

void CSurface::SetCursor(HCursor cursor)
{
	g_pSurfaceV26->SetCursor(cursor);
}

bool CSurface::IsCursorVisible(void)
{
	return g_pSurfaceV26->IsCursorVisible();
}

void CSurface::ApplyChanges(void)
{
	g_pSurfaceV26->ApplyChanges();
}

bool CSurface::IsWithin(int x, int y)
{
	return g_pSurfaceV26->IsWithin(x, y);
}

bool CSurface::HasFocus(void)
{
	return g_pSurfaceV26->HasFocus();
}

bool CSurface::SupportsFeature(SurfaceFeature_e feature)
{
	if (feature == ISurface::OUTLINE_FONTS)
		return true;
	return g_pSurfaceV26->SupportsFeature(feature);
}

void CSurface::RestrictPaintToSinglePanel(VPANEL panel)
{
	g_pSurfaceV26->RestrictPaintToSinglePanel(panel);
}

void CSurface::SetModalPanel(VPANEL panel)
{
	g_pSurfaceV26->SetModalPanel(panel);
}

VPANEL CSurface::GetModalPanel(void)
{
	return g_pSurfaceV26->GetModalPanel();
}

void CSurface::UnlockCursor(void)
{
	g_pSurfaceV26->UnlockCursor();
}

void CSurface::LockCursor(void)
{
	g_pSurfaceV26->LockCursor();
}

void CSurface::SetTranslateExtendedKeys(bool state)
{
	g_pSurfaceV26->SetTranslateExtendedKeys(state);
}

VPANEL CSurface::GetTopmostPopup(void)
{
	return g_pSurfaceV26->GetTopmostPopup();
}

void CSurface::SetTopLevelFocus(VPANEL panel)
{
	g_pSurfaceV26->SetTopLevelFocus(panel);
}

HFont CSurface::CreateFont(void)
{
	return g_pSurfaceV26->CreateFont();
}

bool CSurface::SetFontGlyphSet(HFont font, const char *windowsFontName, int tall, int weight, int blur, int scanlines, int flags)
{
	return g_pSurfaceV26->AddGlyphSetToFont(font, windowsFontName, tall, weight, blur, scanlines, flags, 0x0, 0xFFFF);
}

bool CSurface::AddCustomFontFile(const char *fontFileName)
{
	return g_pSurfaceV26->AddCustomFontFile(fontFileName);
}

int CSurface::GetFontTall(HFont font)
{
	return g_pSurfaceV26->GetFontTall(font);
}

int CSurface::GetFontAscent(HFont font, wchar_t wch)
{
	return g_pSurfaceV26->GetFontAscent(font, wch);
}

bool CSurface::IsFontAdditive(HFont font)
{
	return false;
}

void CSurface::GetCharABCwide(HFont font, int ch, int &a, int &b, int &c)
{
	g_pSurfaceV26->GetCharABCwide(font, ch, a, b, c);
}

int CSurface::GetCharacterWidth(HFont font, int ch)
{
	return g_pSurfaceV26->GetCharacterWidth(font, ch);
}

void CSurface::GetTextSize(HFont font, const wchar_t *text, int &wide, int &tall)
{
	g_pSurfaceV26->GetTextSize(font, text, wide, tall);
}

VPANEL CSurface::GetNotifyPanel(void)
{
	return g_pSurfaceV26->GetNotifyPanel();
}

void CSurface::SetNotifyIcon(VPANEL context, HTexture icon, VPANEL panelToReceiveMessages, const char *text)
{
	g_pSurfaceV26->SetNotifyIcon(context, icon, panelToReceiveMessages, text);
}

void CSurface::PlaySound(const char *fileName)
{
	g_pSurfaceV26->PlaySound(fileName);
}

int CSurface::GetPopupCount(void)
{
	return g_pSurfaceV26->GetPopupCount();
}

VPANEL CSurface::GetPopup(int index)
{
	return g_pSurfaceV26->GetPopup(index);
}

bool CSurface::ShouldPaintChildPanel(VPANEL childPanel)
{
	return g_pSurfaceV26->ShouldPaintChildPanel(childPanel);
}

bool CSurface::RecreateContext(VPANEL panel)
{
	return g_pSurfaceV26->RecreateContext(panel);
}

void CSurface::AddPanel(VPANEL panel)
{
	return g_pSurfaceV26->AddPanel(panel);
}

void CSurface::ReleasePanel(VPANEL panel)
{
	return g_pSurfaceV26->ReleasePanel(panel);
}

void CSurface::MovePopupToFront(VPANEL panel)
{
	g_pSurfaceV26->MovePopupToFront(panel);
}

void CSurface::MovePopupToBack(VPANEL panel)
{
	g_pSurfaceV26->MovePopupToBack(panel);
}

void CSurface::SolveTraverse(VPANEL panel, bool forceApplySchemeSettings)
{
	g_pSurfaceV26->SolveTraverse(panel, forceApplySchemeSettings);
}

void CSurface::PaintTraverse(VPANEL panel)
{
	g_pSurfaceV26->PaintTraverse(panel);
}

void CSurface::EnableMouseCapture(VPANEL panel, bool state)
{
	g_pSurfaceV26->EnableMouseCapture(panel, state);
}

void CSurface::GetWorkspaceBounds(int &x, int &y, int &wide, int &tall)
{
	//g_pSurfaceV26->GetWorkspaceBounds(x, y, wide, tall);
	x = y = 0;
	g_pSurfaceV26->GetScreenSize(wide, tall);
}

void CSurface::GetAbsoluteWindowBounds(int &x, int &y, int &wide, int &tall)
{
	g_pSurfaceV26->GetAbsoluteWindowBounds(x, y, wide, tall);
}

void CSurface::GetProportionalBase(int &width, int &height)
{
	//g_pSurfaceV26->GetProportionalBase(width, height);
	width = 800;
	height = 600;
}

void CSurface::CalculateMouseVisible(void)
{
	g_pSurfaceV26->CalculateMouseVisible();
}

bool CSurface::NeedKBInput(void)
{
	return g_pSurfaceV26->NeedKBInput();
}

bool CSurface::HasCursorPosFunctions(void)
{
	return g_pSurfaceV26->HasCursorPosFunctions();
}

void CSurface::SurfaceGetCursorPos(int &x, int &y)
{
	g_pSurfaceV26->SurfaceGetCursorPos(x, y);
}

void CSurface::SurfaceSetCursorPos(int x, int y)
{
	g_pSurfaceV26->SurfaceSetCursorPos(x, y);
}

void CSurface::DrawTexturedLine(const Vertex_t &a, const Vertex_t &b)
{
}

void CSurface::DrawOutlinedCircle(int x, int y, int radius, int segments)
{
}

void CSurface::DrawTexturedPolyLine(const Vertex_t *p,int n)
{
}

void CSurface::DrawTexturedSubRect(int x0, int y0, int x1, int y1, float texs0, float text0, float texs1, float text1)
{
}

void CSurface::DrawTexturedPolygon(int n, Vertex_t *pVertices)
{
}

const wchar_t *CSurface::GetTitle(VPANEL panel)
{
	return L"";
}

bool CSurface::IsCursorLocked(void) const
{
	return false;
}

void CSurface::SetWorkspaceInsets(int left, int top, int right, int bottom)
{
}

bool CSurface::DrawGetUnicodeCharRenderInfo(wchar_t ch, CharRenderInfo& info)
{
	return false;
}

void CSurface::DrawRenderCharFromInfo(const CharRenderInfo& info)
{
}

void CSurface::DrawSetAlphaMultiplier(float alpha)
{
}

float CSurface::DrawGetAlphaMultiplier(void)
{
	return 1.0;
}

void CSurface::SetAllowHTMLJavaScript(bool state)
{
	g_pSurfaceV26->SetAllowHTMLJavaScript(state);
}

void CSurface::OnScreenSizeChanged(int nOldWidth, int nOldHeight)
{
}

vgui::HCursor CSurface::CreateCursorFromFile(char const *curOrAniFile, char const *pPathID)
{
	return 1;
}

void *CSurface::DrawGetTextureMatInfoFactory(int id)
{
	return NULL;
}

void CSurface::PaintTraverseEx(VPANEL panel, bool paintPopups)
{
}

float CSurface::GetZPos(void) const
{
	return 1;
}

void CSurface::SetPanelForInput(VPANEL vpanel)
{
}

void CSurface::DrawFilledRectFade(int x0, int y0, int x1, int y1, unsigned int alpha0, unsigned int alpha1, bool bHorizontal)
{
}

void CSurface::DrawSetTextureRGBAEx(int id, const unsigned char *rgba, int wide, int tall, int imageFormat)
{
}

void CSurface::DrawSetTextScale(float sx, float sy)
{
}

bool CSurface::SetBitmapFontGlyphSet(HFont font, const char *windowsFontName, float scalex, float scaley, int flags)
{
	return false;
}

bool CSurface::AddBitmapFontFile(const char *fontFileName)
{
	return false;
}

void CSurface::SetBitmapFontName(const char *pName, const char *pFontFilename)
{
}

const char *CSurface::GetBitmapFontName(const char *pName)
{
	return "";
}

void CSurface::ClearTemporaryFontCache(void)
{
}

IImage *CSurface::GetIconImageForFullPath(char const *pFullPath)
{
	return NULL;
}

void CSurface::DrawUnicodeString(const wchar_t *pwString, FontDrawType_t drawType)
{
}

void CSurface::PrecacheFontCharacters(HFont font, wchar_t *pCharacters)
{
}

const char *CSurface::GetResolutionKey(void) const
{
	return "";
}

}