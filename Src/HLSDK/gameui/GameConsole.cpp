#include "GameConsole.h"

static CGameConsole g_GameConsole;

CGameConsole &GameConsole()
{
	return g_GameConsole;
}

CGameConsole::CGameConsole()
{
	m_bInitialized = false;
}

CGameConsole::~CGameConsole()
{
	m_bInitialized = false;
}

void CGameConsole::Activate(void)
{
	if (!m_bInitialized)
		return;

	vgui::surface()->RestrictPaintToSinglePanel(NULL);
	m_pConsole->Activate();
}

void CGameConsole::Initialize(void)
{
	m_pConsole = vgui::SETUP_PANEL( new CGameConsoleDialog() );

	int swide, stall;
	vgui::surface()->GetScreenSize(swide, stall);
	int offset = vgui::scheme()->GetProportionalScaledValue(16);

	m_pConsole->SetBounds(
		swide / 2 - (offset * 4),
		offset,
		(swide / 2) + (offset * 3),
		stall - (offset * 8));

	m_pConsole->SetMinimumSize(368, 352);

	m_bInitialized = true;
}

void CGameConsole::Hide(void)
{
	if (!m_bInitialized)
		return;

	m_pConsole->Hide();
}

void CGameConsole::Clear(void)
{
	if (!m_bInitialized)
		return;

	m_pConsole->Clear();
}

bool CGameConsole::IsConsoleVisible(void)
{
	if (!m_bInitialized)
		return false;
	
	return m_pConsole->IsVisible();
}

void CGameConsole::Printf(const char *format, ...)
{
	if (!m_bInitialized)
		return;

	static char buffer[1024];

	va_list ap;
	va_start(ap, format);
	_vsnprintf(buffer, sizeof(buffer)-1, format, ap);
	va_end(ap);

	m_pConsole->Print(buffer);
}

void CGameConsole::DPrintf(const char *format, ...)
{
	if (!m_bInitialized)
		return;

	static char buffer[1024];

	va_list ap;
	va_start(ap, format);
	_vsnprintf(buffer, sizeof(buffer)-1, format, ap);
	va_end(ap);

	m_pConsole->DPrint(buffer);
}

void CGameConsole::SetParent(int parent)
{
	if (!m_bInitialized)
		return;

	m_pConsole->SetParent( (vgui::VPANEL)parent );
}

EXPOSE_SINGLE_INTERFACE(CGameConsole, CGameConsole, GAMECONSOLE_INTERFACE_VERSION);