#ifndef ITEMSYSTEM_H
#define ITEMSYSTEM_H

#include <utlvector.h>
#include "CEValue.h"

enum ItemType
{
	ITEM_ALL = -1,
	ITEM_WEAPON,
	ITEM_CHAR,
	ITEM_ITEM,
};

enum ItemCharacterType
{
	ITEM_CHARACTER_ALL = -1,
	ITEM_CHARACTER_CHAR,
	ITEM_CHARACTER_HEAD,
	ITEM_CHARACTER_UPPER,
	ITEM_CHARACTER_LOWER,
	ITEM_CHARACTER_ACCES,
};

enum ItemWeaponType
{
	ITEM_WEAPON_ALL = -1,
	ITEM_WEAPON_PRIMARY,
	ITEM_WEAPON_SECOND,
	ITEM_WEAPON_KNIFE,
	ITEM_WEAPON_GREANDE,
	ITEM_WEAPON_SPECIAL,
};

enum ItemWeaponGrenadeType
{
	ITEM_WEAPON_GRENADE_ALL = -1,
	ITEM_WEAPON_GRENADE_BOMB,
	ITEM_WEAPON_GRENADE_FLASH,
	ITEM_WEAPON_GRENADE_SMOKE,
};

// Don't change it !
#define ITEMKNIFEINDEX				108
#define ITEMGRENADEINDEX			109
#define ITEMUSPINDEX				120
#define ITEMM16INDEX				127
#define ITEMBOMBPACKAGEINDEX		227
#define ITEMGLOCK18INDEX			144

class CShopItem
{
public:
	CShopItem()
	{
		memset(this, 0, sizeof(*this));
	}

public:
	CEValue<int>	m_nItemIndex;
	wchar_t			m_szName[32];
	CEValue<int>	m_bCashItem;
	CEValue<int>	m_iPrice;
	int				m_iItemType;
	int				m_iItemCategory;
	int				m_iItemCategory2;
	CEValue<int>	m_bRepairItem;
	CEValue<int>	m_bDeleteItem;
	wchar_t			m_szDescription[256];
	int				m_iWeaponPower;
	int				m_iWeaponAccuracy;
	int				m_iWeaponContinuity;
	int				m_iWeaponRecoil;
	int				m_iWeaponWeight;
	int				m_iWeaponLoadAmmo;
	int				m_iWeaponFullAmmo;
};

class CShopItemManager
{
public:
	void Initialize(void);
	void Shutdown(void);

	CShopItem *		FindItem(int iItemIndex);
	CShopItem *		GetItem(int iIndex);
	int				GetCount(void);

private:
	CUtlVector<CShopItem> m_Item;
};

extern CShopItemManager g_ShopItemManager;

#endif