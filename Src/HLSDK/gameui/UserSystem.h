#ifndef USERDATASYSTEM_H
#define USERDATASYSTEM_H

#include <utlvector.h>
#include "CEValue.h"

class KeyValues;
class CShopItem;

class CInvenItem
{
public:
	CInvenItem()
	{
		m_szGuid[0] = '\0';
	}

	char			m_szGuid[37];
	CEValue<int>	m_iShopItemIndex;
	CEValue<int>	m_iBoughtTime;
	CEValue<int>	m_iPeriodTime;
	CEValue<int>	m_iDurability;	// 1~100

	CEValue<CShopItem *>	m_pShopItem;
};

enum BagNumber
{
	BAG_NUM_1 = 1,
	BAG_NUM_2,
	BAG_NUM_3,
};

enum BagSlot
{
	BAG_SLOT_PRIMARY,
	BAG_SLOT_SECOND,
	BAG_SLOT_KNIFE,
	BAG_SLOT_GRENADE1,
	BAG_SLOT_GRENADE2,
	BAG_SLOT_GRENADE3,
};

#define BAG_MAX_NUM		3
#define BAG_MAX_SLOT	6

class CBagItem
{
public:
	CEValue<CInvenItem *>	m_pItem[6];
};

class CUserSystem
{
public:
	void Initialize(void);
	void Shutdown(void);

public:
	// Interface

	bool				IsFirstLogin(void);
	void				SetName(const wchar_t *playerName);
	const wchar_t *		GetName(void);
	void				SetClan(const wchar_t *clanName);
	const wchar_t *		GetClan(void);
	void SetEP(int count);
	int					GetEP(void);
	int					GetCurLevel(void);
	int					GetNextLevel(void);
	int GetLevelEP(int level);
	void SetGP(int count);
	int					GetGP(void);
	void SetCP(int count);
	int					GetCP(void);
	void SetKill(int count);
	int					GetKill(void);
	void SetDeath(int count);
	int					GetDeath(void);
	void SetWin(int count);
	int					GetWin(void);
	void SetLose(int count);
	int					GetLose(void);

	CInvenItem		*FindInvenItem(const char *guid);
	CInvenItem		*FindInvenItem(int index);
	CInvenItem		*GetInvenItem(int nIndex);
	int				GetInvenItemCount(void);
	void			AddInvenItem(const char *szGuid, CShopItem *pShopItem, int iBoughtTime, int iPeriodTime);

	int				FindInvenItemInBag(CInvenItem *pItem);	// Return bag number {1~3}
	CInvenItem		*GetBagItem(int iBagNumber, int iSlot);
	void			RemoveBagItem(CInvenItem *pItem);
	void			SetBagItem(int iBagNumber, int iSlot, CInvenItem *pItem);

private:
	void	Create(void);
	bool	Load(void);
	void	Save(void);

	KeyValues	*m_pEntry;

	bool			m_bFirstLogin;

	wchar_t			m_szCallName[32];
	wchar_t			m_szClanName[32];

	CEValue<int>	m_iMyEP;
	CEValue<int>	m_iMyGP;
	CEValue<int>	m_iMyCP;
	CEValue<int>	m_iTotalKill;
	CEValue<int>	m_iTotalDead;
	CEValue<int>	m_iTotalWin;
	CEValue<int>	m_iTotalLose;
	CEValue<int>	m_iTotalHeadshotKill;
	CEValue<int>	m_iTotalGrenadeKill;
	CEValue<int>	m_iTotalKnifeKill;
	CEValue<int>	m_iTotalDropOut;
	CEValue<int>	m_iTotalFriendKill;

	CUtlVector<CInvenItem>	m_InvenItem;

	//背包应该加一组开关决定每个背包是否启用
	CBagItem	m_BagItem[3];	// 3 bags
};


extern CUserSystem g_User;

#endif