#include "EngineInterface.h"
#include "UserSystem.h"
#include "ShopItemManager.h"
#include <keyvalues.h>
#include "filesystem.h"

#define USER_DATA_FILE	"User.vdf"

CUserSystem g_User;

void CUserSystem::Initialize(void)
{
	if (!Load())
	{
		Create();
		Save();

		if (!Load())
		{
			Sys_Error("Failed to load UserData.");
		}
	}
}

void CUserSystem::Shutdown(void)
{
	Save();
}

void CUserSystem::Create(void)
{
	m_bFirstLogin = true;
	m_szCallName[0] = L'\0';
	m_szClanName[0] = L'\0';
	m_iMyEP = 0;
	m_iMyGP = 40000;
	m_iMyCP = 0;
	m_iTotalKill = 0;
	m_iTotalDead = 0;
	m_iTotalWin = 0;
	m_iTotalLose = 0;
	m_iTotalHeadshotKill = 0;
	m_iTotalGrenadeKill = 0;
	m_iTotalKnifeKill = 0;
	m_iTotalDropOut = 0;
	m_iTotalFriendKill = 0;

	// Clear up inven items
	m_InvenItem.RemoveAll();

	// The base weapon
	AddInvenItem("1310FA71-A1B3-4120-A345-8549E5FB6B4D", g_ShopItemManager.FindItem(ITEMKNIFEINDEX), 0, 0);
	AddInvenItem("B63E0793-7A1B-407E-A3BE-E4104FE5D907", g_ShopItemManager.FindItem(ITEMUSPINDEX), 0, 0);
	AddInvenItem("99DF97B0-2BDC-44A7-90C5-0DDAA2CBAC46", g_ShopItemManager.FindItem(ITEMM16INDEX), 0, 0);
	AddInvenItem("D34485AE-6452-4805-8715-20B16F8FD322", g_ShopItemManager.FindItem(ITEMGLOCK18INDEX), 0, 0);
	AddInvenItem("01748206-F9E0-4744-91C8-2269FAF406BE", g_ShopItemManager.FindItem(ITEMGRENADEINDEX), 0, 0);

	for (int iBag = 0; iBag < BAG_MAX_NUM; iBag++)
	{
		for (int iSlot = 0; iSlot < BAG_MAX_SLOT; iSlot++)
		{
			// Clear up the package
			m_BagItem[iBag].m_pItem[iSlot] = (iSlot == BAG_SLOT_KNIFE) ? FindInvenItem(ITEMKNIFEINDEX) : NULL;
		}
	}
}

bool CUserSystem::Load(void)
{
	KeyValues *pUserData = new KeyValues("UserData");

	if (!pUserData)
	{
		Sys_Error("Out of memory.");
	}

	if (pUserData->LoadFromFile(g_pFullFileSystem, USER_DATA_FILE) == false)
	{
		return false;
	}

	m_bFirstLogin = pUserData->GetInt("FirstLogin");

	wcscpy(m_szCallName, pUserData->GetWString("CallName"));
	wcscpy(m_szClanName, pUserData->GetWString("ClanName"));

	m_iMyEP = pUserData->GetInt("EP");
	m_iMyGP = pUserData->GetInt("GP");
	m_iMyCP = pUserData->GetInt("CP");
	m_iTotalKill = pUserData->GetInt("TotalKill");
	m_iTotalDead = pUserData->GetInt("TotalDead");
	m_iTotalWin = pUserData->GetInt("TotalWin");
	m_iTotalLose = pUserData->GetInt("TotalLose");
	m_iTotalHeadshotKill = pUserData->GetInt("TotalHeadshotKill");
	m_iTotalGrenadeKill = pUserData->GetInt("TotalGrenadeKill");
	m_iTotalKnifeKill = pUserData->GetInt("TotalKnifeKill");
	m_iTotalDropOut = pUserData->GetInt("TotalDropOut");
	m_iTotalFriendKill = pUserData->GetInt("TotalFriendKill");

	m_InvenItem.RemoveAll();

	KeyValues *pItemData = pUserData->FindKey("InvenItem");

	if (!pItemData)
	{
		Sys_Error("Failed to load InvenItem.");
	}

	for (KeyValues *pItem = pItemData->GetFirstSubKey(); pItem; pItem = pItem->GetNextKey())
	{
		int iShopItemIndex = pItem->GetInt("ShopItemIndex");

		CShopItem *pShopItem = g_ShopItemManager.FindItem(iShopItemIndex);
		if (!pShopItem)
			continue;

		int i = m_InvenItem.AddToTail();

		strcpy(m_InvenItem[i].m_szGuid, pItem->GetString("Guid"));

		m_InvenItem[i].m_iShopItemIndex = pItem->GetInt("ShopItemIndex");
		m_InvenItem[i].m_iBoughtTime = pItem->GetInt("BoughtTime");
		m_InvenItem[i].m_iPeriodTime = pItem->GetInt("PeriodTime");
		m_InvenItem[i].m_iDurability = pItem->GetInt("Durability");

		// Save pointer, we can direct access the item :)
		m_InvenItem[i].m_pShopItem = pShopItem;
	}

	KeyValues *pBagItem = pUserData->FindKey("BagItem");

	if (!pBagItem)
	{
		Sys_Error("Failed to load BagItem.");
	}

	for (int nBag = 0; nBag < 3; nBag++)
	{
		char szBagNumber[2];
		sprintf(szBagNumber, "%d", nBag + 1);

		KeyValues *pBag = pBagItem->FindKey(szBagNumber);

		if (!pBag)
		{
			Sys_Error("Failed to load BagData.");
		}

		m_BagItem[nBag].m_pItem[0] = FindInvenItem(pBag->GetString("Primary"));
		m_BagItem[nBag].m_pItem[1] = FindInvenItem(pBag->GetString("Second"));
		m_BagItem[nBag].m_pItem[2] = FindInvenItem(pBag->GetString("Knife"));
		m_BagItem[nBag].m_pItem[3] = FindInvenItem(pBag->GetString("Grenade1"));
		m_BagItem[nBag].m_pItem[4] = FindInvenItem(pBag->GetString("Grenade2"));
		m_BagItem[nBag].m_pItem[5] = FindInvenItem(pBag->GetString("Grenade3"));
	}

	pUserData->deleteThis();

	return true;
}

void CUserSystem::Save(void)
{
	KeyValues *pUserData = new KeyValues("UserData");

	if (!pUserData)
	{
		Sys_Error("Out of memory.");
	}

	pUserData->SetInt("FirstLogin", m_bFirstLogin);
	pUserData->SetWString("CallName", m_szCallName);
	pUserData->SetWString("ClanName", m_szClanName);
	pUserData->SetInt("EP", m_iMyEP);
	pUserData->SetInt("GP", m_iMyGP);
	pUserData->SetInt("CP", m_iMyCP);
	pUserData->SetInt("TotalKill", m_iTotalKill);
	pUserData->SetInt("TotalDead", m_iTotalDead);
	pUserData->SetInt("TotalWin", m_iTotalWin);
	pUserData->SetInt("TotalLose", m_iTotalLose);
	pUserData->SetInt("TotalHeadshotKill", 0);
	pUserData->SetInt("TotalGrenadeKill", 0);
	pUserData->SetInt("TotalKnifeKill", 0);
	pUserData->SetInt("TotalDropOut", 0);
	pUserData->SetInt("TotalFriendKill", 0);

	KeyValues *pInvenItem = pUserData->CreateNewKey();

	if (!pInvenItem)
	{
		Sys_Error("Failed to save InvenItem.");
	}

	pInvenItem->SetName("InvenItem");

	for (int i = 0; i < m_InvenItem.Count(); i++)
	{
		KeyValues *pItem = pInvenItem->CreateNewKey();

		pItem->SetName("Item");
		pItem->SetString("Guid", m_InvenItem[i].m_szGuid);
		pItem->SetInt("ShopItemIndex", m_InvenItem[i].m_iShopItemIndex);
		pItem->SetInt("BoughtTime", m_InvenItem[i].m_iBoughtTime);
		pItem->SetInt("PeriodTime", m_InvenItem[i].m_iPeriodTime);
		pItem->SetInt("Durability", m_InvenItem[i].m_iDurability);
	}

	KeyValues *pBagItem = pUserData->CreateNewKey();

	if (!pBagItem)
	{
		Sys_Error("Failed to save BagItem.");
	}

	pBagItem->SetName("BagItem");

	for (int i = 0; i < BAG_MAX_NUM; i++)
	{
		KeyValues *pBag = pBagItem->CreateNewKey();

		if (m_BagItem[i].m_pItem[0] != NULL)
			pBag->SetString("Primary", m_BagItem[i].m_pItem[0]->m_szGuid);

		if (m_BagItem[i].m_pItem[1] != NULL)
			pBag->SetString("Second", m_BagItem[i].m_pItem[1]->m_szGuid);

		if (m_BagItem[i].m_pItem[2] != NULL)
			pBag->SetString("Knife", m_BagItem[i].m_pItem[2]->m_szGuid);

		if (m_BagItem[i].m_pItem[3] != NULL)
			pBag->SetString("Grenade1", m_BagItem[i].m_pItem[3]->m_szGuid);

		if (m_BagItem[i].m_pItem[4] != NULL)
			pBag->SetString("Grenade2", m_BagItem[i].m_pItem[4]->m_szGuid);

		if (m_BagItem[i].m_pItem[5] != NULL)
			pBag->SetString("Grenade3", m_BagItem[i].m_pItem[5]->m_szGuid);
	}

	if (pUserData->SaveToFile(g_pFullFileSystem, USER_DATA_FILE) == false)
	{
		Sys_Error("Failed to save UserData.");
	}

	pUserData->deleteThis();
}

bool CUserSystem::IsFirstLogin(void)
{
	return m_bFirstLogin;
}

void CUserSystem::SetName(const wchar_t *playerName)
{
	wcscpy(m_szCallName, playerName);
}

const wchar_t *CUserSystem::GetName(void)
{
	return m_szCallName;
}

void CUserSystem::SetClan(const wchar_t *clanName)
{
	wcscpy(m_szClanName, clanName);
}

const wchar_t *CUserSystem::GetClan(void)
{
	return m_szClanName;
}

void CUserSystem::SetEP(int count)
{
	m_iMyEP = count;
}

int CUserSystem::GetEP(void)
{
	return m_iMyEP;
}

static int gEPList[100][2] = 
{
	// 士兵
	{      0,        456 },
	{     457,       912 },
	{     913,      1824 },
	{    1825,      3192 },
	{    3193,      5016 },
	{    5017,      7296 },
	{    7297,     10032 },
	{    10033,    13224 },
	{    13225,    17784 },
	// 士官
	{    17785,    23940 },
	{    23941,    33060 },
	{    33061,    43092 },
	{    43093,    54036 },
	{    54037,    65892 },
	{    65893,    78660 },
	{    78661,    92340 },
	{    92341,   106932 },
	{   106933,   122436 },
	{   122437,   138852 },
	{   138853,   156180 },
	{   156181,   174420 },
	{   174421,   193572 },
	{   193573,   213636 },
	{   213637,   234612 },
	{   234613,   256500 },
	{   256501,   279300 },
	{   279301,   326724 },
	// 尉官
	{   326725,   375972 },
	{   375973,   427044 },
	{   427045,   479940 },
	{   479941,   534660 },
	{   534661,   591204 },
	{   591205,   649572 },
	{   649573,   709764 },
	{   709765,   771780 },
	{   771781,   835620 },
	{   835621,   901284 },
	{   901285,   968772 },
	{   968773,  1038084 },
	{  1038085,  1109220 },
	{  1109221,  1182180 },
	{  1182181,  1256964 },
	{  1256965,  1333572 },
	{  1333573,  1412004 },
	{  1412005,  1492260 },
	{  1492261,  1574340 },
	{  1574341,  1658244 },
	{  1658245,  1743972 },
	{  1743973,  1831524 },
	{  1831525,  1920900 },
	{  1920901,  2057700 },
	// 校官
	{  2057701,  2197236 },
	{  2197237,  2339508 },
	{  2339509,  2484516 },
	{  2484517,  2632260 },
	{  2632261,  2782740 },
	{  2782741,  2935956 },
	{  2935957,  3091908 },
	{  3091909,  3277044 },
	{  3277045,  3465372 },
	{  3465373,  3673536 },
	{  3673537,  3885177 },
	{  3885178,  4100295 },
	{  4100296,  4318890 },
	{  4318891,  4540962 },
	{  4540963,  4766511 },
	{  4766512,  5028198 },
	{  5028199,  5319183 },
	{  5319184,  5614500 },
	{  5614501,  5914149 },
	{  5914150,  6218130 },
	{  6218131,  6526500 },
	{  6526501,  6839202 },
	{  6839203,  7156236 },
	{  7156237,  7578036 },
	{  7578037,  8026911 },
	{  8026912,  8481771 },
	{  8481772,  8964561 },
	{  8964562,  9475851 },
	{  9475852, 10016211 },
	{ 10016212, 10586211 },
	// 将帅
	{ 10586212, 11186421 },
	{ 11186422, 11817411 },
	{ 11817412, 12479751 },
	{ 12479752, 13174011 },
	{ 13174012, 13900761 },
	{ 13900762, 14660571 },
	{ 14660572, 15454011 },
	{ 15454012, 16281651 },
	{ 16281652, 17144061 },
	{ 17144062, 18041811 },
	{ 18041812, 18975471 },
	{ 18975472, 19945611 },
	{ 19945612, 20952801 },
	{ 20952802, 21997611 },
	{ 21997612, 23080611 },
	{ 23080612, 24202371 },
	{ 24202372, 25363461 },
	{ 25363462, 26564451 },
	{ 26564451, 99999999 },
};

int CUserSystem::GetCurLevel(void)
{
	int ep = GetEP();

	for (int i = 0; i < 100; i++)
	{
		if (ep >= gEPList[i][0] && ep <= gEPList[i][1])
		{
			return i + 1;
		}
	}

	return 1;
}

int CUserSystem::GetNextLevel(void)
{
	int c = GetCurLevel() + 1;

	if (c > 100) c = 100;

	return c;
}

int CUserSystem::GetLevelEP(int level)
{
	return gEPList[level - 1][0];
}

void CUserSystem::SetGP(int count)
{
	m_iMyGP = count;
}

int CUserSystem::GetGP(void)
{
	return m_iMyGP;
}

void CUserSystem::SetCP(int count)
{
	m_iMyCP = count;
}

int CUserSystem::GetCP(void)
{
	return m_iMyCP;
}

void CUserSystem::SetKill(int count)
{
	m_iTotalKill = count;
}

int CUserSystem::GetKill(void)
{
	return m_iTotalKill;
}

void CUserSystem::SetDeath(int count)
{
	m_iTotalDead = count;
}

int CUserSystem::GetDeath(void)
{
	return m_iTotalDead;
}

void CUserSystem::SetWin(int count)
{
	m_iTotalWin = count;
}

int CUserSystem::GetWin(void)
{
	return m_iTotalWin;
}

void CUserSystem::SetLose(int count)
{
	m_iTotalLose = count;
}

int CUserSystem::GetLose(void)
{
	return m_iTotalLose;
}

CInvenItem *CUserSystem::FindInvenItem(const char *guid)
{
	if (!*guid)
		return NULL;

	int iCount = m_InvenItem.Count();

	for (int nItem = 0; nItem < iCount; nItem++)
	{
		char *gitem = m_InvenItem[nItem].m_szGuid;

		if (!stricmp(guid, gitem))
		{
			return &m_InvenItem[nItem];
		}
	}

	return NULL;
}

CInvenItem *CUserSystem::FindInvenItem(int index)
{
	int iCount = m_InvenItem.Count();

	for (int nItem = 0; nItem < iCount; nItem++)
	{
		if (m_InvenItem[nItem].m_iShopItemIndex == index)
		{
			return &m_InvenItem[nItem];
		}
	}

	return NULL;
}

CInvenItem *CUserSystem::GetInvenItem(int nIndex)
{
	if (!m_InvenItem.IsValidIndex(nIndex))
		return NULL;

	return &m_InvenItem[nIndex];
}

int CUserSystem::GetInvenItemCount(void)
{
	return m_InvenItem.Count();
}

void CUserSystem::AddInvenItem(const char *szGuid, CShopItem *pShopItem, int iBoughtTime, int iPeriodTime)
{
	if (!pShopItem)
	{
		Sys_Error("Invalid ShopItem.");
	}

	int i = m_InvenItem.AddToTail();

	m_InvenItem[i].m_iShopItemIndex = pShopItem->m_nItemIndex;
	m_InvenItem[i].m_iBoughtTime = iBoughtTime;
	m_InvenItem[i].m_iPeriodTime = iPeriodTime;
	m_InvenItem[i].m_iDurability = 0;

	strcpy(m_InvenItem[i].m_szGuid, szGuid);
}

int CUserSystem::FindInvenItemInBag(CInvenItem *pItem)
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			if (m_BagItem[i].m_pItem[j] == pItem)
			{
				return i + 1;
			}
		}
	}

	return 0;
}

CInvenItem *CUserSystem::GetBagItem(int iBagNumber, int iSlot)
{
	if (iBagNumber < BAG_NUM_1 || iBagNumber > BAG_NUM_3 || iSlot < BAG_SLOT_PRIMARY || iSlot > BAG_SLOT_GRENADE3)
		return NULL;

	int iBag = iBagNumber - 1;

	return m_BagItem[iBag].m_pItem[iSlot];
}

void CUserSystem::RemoveBagItem(CInvenItem *pItem)
{
	int iBag, iSlot;

	for (iBag = 0; iBag < BAG_MAX_NUM; iBag++)
	{
		for (iSlot = 0; iSlot < BAG_MAX_SLOT; iSlot++)
		{
			if (m_BagItem[iBag].m_pItem[iSlot] == pItem)
			{
				goto found;
			}
		}
	}

	if (iBag == BAG_MAX_NUM || iSlot == BAG_MAX_SLOT)
		return;

found:
	if (iSlot < BAG_SLOT_GRENADE1)
	{
		// can't remove the base weapon (knife)
		m_BagItem[iBag].m_pItem[iSlot] = (iSlot == BAG_SLOT_KNIFE) ? FindInvenItem(ITEMKNIFEINDEX) : NULL;
	}
	else
	{
		int count = BAG_SLOT_GRENADE3 - iSlot;

		if (count > 0)
		{
			for (int i = 0; i < count; i++)
			{
				m_BagItem[iBag].m_pItem[iSlot + i + 0] = m_BagItem[iBag].m_pItem[iSlot + i + 1];
				m_BagItem[iBag].m_pItem[iSlot + i + 1] = NULL;
			}
		}
		else
		{
			m_BagItem[iBag].m_pItem[iSlot] = NULL;
		}
	}
}

void CUserSystem::SetBagItem(int iBagNumber, int iSlot, CInvenItem *pItem)
{
	if (iBagNumber < BAG_NUM_1 || iBagNumber > BAG_NUM_3 || iSlot < BAG_SLOT_PRIMARY || iSlot > BAG_SLOT_GRENADE3)
		return;

	int iBag = iBagNumber - 1;

	if (m_BagItem[iBag].m_pItem[iSlot] == pItem)
		return;

	RemoveBagItem(pItem);

	m_BagItem[iBag].m_pItem[iSlot] = pItem;
}