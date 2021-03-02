#include "EngineInterface.h"
#include "ShopItemManager.h"
#include <keyvalues.h>
#include "filesystem.h"

CShopItemManager g_ShopItemManager;

void CShopItemManager::Initialize(void)
{
	KeyValues *pItemData = new KeyValues("Item");

	if (!pItemData)
	{
		Sys_Error("Out of memory.");
	}

	if (pItemData->LoadFromFile(g_pFullFileSystem, "configs/item.vdf") != true)
	{
		Sys_Error("Failed to load Item.");
	}

	for (KeyValues *pData = pItemData; pData; pData = pData->GetNextKey())
	{
		// Item index must be begin from 0
		if ( pData->GetInt("ItemIndex") < 0 )
			continue;

		int i = m_Item.AddToTail();

		// Get item values
		m_Item[i].m_nItemIndex			= pData->GetInt("ItemIndex");
		m_Item[i].m_bCashItem			= pData->GetInt("CashItem");
		m_Item[i].m_iPrice				= pData->GetInt("Price");
		m_Item[i].m_iItemType			= pData->GetInt("ItemType");
		m_Item[i].m_iItemCategory		= pData->GetInt("ItemCategory");
		m_Item[i].m_iItemCategory2		= pData->GetInt("ItemCategory2");
		m_Item[i].m_bRepairItem			= pData->GetInt("RepairItem");
		m_Item[i].m_bDeleteItem			= pData->GetInt("DeleteItem");

		// Get item name
		wcscpy( m_Item[i].m_szName, pData->GetWString("ItemName") );
	}

	pItemData->deleteThis();
}

void CShopItemManager::Shutdown(void)
{
	m_Item.RemoveAll();
}

CShopItem *CShopItemManager::FindItem(int iItemIndex)
{
	int iCount = m_Item.Count();

	for (int i = 0; i < iCount; i++)
	{
		if (m_Item[i].m_nItemIndex == iItemIndex)
		{
			return &m_Item[i];
		}
	}

	return NULL;
}

CShopItem * CShopItemManager::GetItem(int iIndex)
{
	if (!m_Item.IsValidIndex(iIndex))
		return NULL;

	return &m_Item[iIndex];
}

int CShopItemManager::GetCount(void)
{
	return m_Item.Count();
}