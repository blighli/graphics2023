using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public enum SlotType
{
    EQUIP,//装备栏
    BAG,//背包物品栏
    CONSUME//药水等消耗品
}

public class Slot : MonoBehaviour
{
    public int slotID;
    public InventoryManager inventoryManager;
    public Inventory inventory;
    public Item slotItem;
    public Image slotImage;
    public Sprite emptyImage;
    public Text slotNum;
    public string slotInfo;

    public GameObject itemInSlot;
    public ItemType itemTypeNeed;
    public SlotType slotType;//该槽的类型

    void Start()
    {

    }

    public void ItemOnClicked()
    {
        if (slotItem)
        {
            if (slotType == SlotType.EQUIP)
            {
                slotID = -2;
            }
            inventoryManager.UpdateItemInfo(slotItem, slotID);
        }
    }

    //赋值该装备槽的信息并显示
    public void SetupSlot(Item item,int num)
    {
        slotItem = item;
        slotImage.sprite = item.itemImage;
        slotInfo = item.itemInfo;
        if (slotNum != null)
        {
            for (int i = 0; i < inventory.ItemPrefabs.Count; i++)
            {
                if (inventory.ItemPrefabs[i].item == item && inventory.ItemPrefabs[i].itemHeld != 0)
                {
                    slotNum.text = num.ToString("00");
                }
            }
        }

        gameObject.SetActive(true);
    }

    void Update()
    {
        
    }
}
