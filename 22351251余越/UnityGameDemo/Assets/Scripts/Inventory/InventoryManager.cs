using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using System;

public class InventoryManager : MonoBehaviour
{
    [Header("Bag")]
    public GameObject slotGrid;
    public GameObject emptySlot;//背包Slot模板
    public List<GameObject> slots = new List<GameObject>();//initBag函数初始化
    public Inventory bagData;//背包数据
    public Text itemInformation;//物品信息
    public Item ChosenItem;//当前选中物品
    public int ChosenItemID;//当前选中物品编号
    public InputField inputNumField;//输入物品丢弃数量
    private GameObject player;
    [Header("Equipped")]
    public CharacterData_SO playerBaseData;//人物基本信息
    public Transform weaponPo;//武器位置
    public GameObject weaponPrefabDefault;//默认武器
    public GameObject emptyEquipSlot;//装备Slot模板
    public GameObject EquipItemParent;//装备槽的主父对象
    public int EquipItemNum;//装备槽数量
    public List<GameObject> slotsEquipped = new List<GameObject>();//装备槽列表
    public List<Vector3> slotsEquippedPo = new List<Vector3>();//装备槽坐标列表
    public List<ItemType> itemsTypeNeed = new List<ItemType>();//装备类型列表


    private void Awake()
    {
        //Debug.Log(gameObject);
        player = gameObject.transform.parent.gameObject;
        
        itemInformation.text = "";
        ChosenItemID = -1;
    }



    #region Bag
    //新加新物品
    public void CreateNewItem(ItemPrefab itemPrefab)
    {
        for (int i = 0; i < bagData.ItemPrefabs.Count; i++)
        {
            if (bagData.ItemPrefabs[i].item == null)
            {
                bagData.ItemPrefabs[i] = itemPrefab;
                break;
            }
        }
        RefreshInventory();
    }

    //背包单个种类物品增加更新显示
    public void RefreshItem(Item item)
    {
        
        for (int i = 0; i < bagData.ItemPrefabs.Count; i++)
        {
            if (bagData.ItemPrefabs[i].item == item)
            {
                slotGrid.transform.GetChild(i).GetChild(0).GetChild(1).gameObject.GetComponent<Text>().text = bagData.ItemPrefabs[i].itemHeld.ToString("00");
            }
        }
    }

    //开局初始化刷新全部背包信息
    public void RefreshInventory()
    {
        slots.Clear();
        for (int i = 0; i < slotGrid.transform.childCount; i++)
        {
            Destroy(slotGrid.transform.GetChild(i).gameObject);
        }
        for (int i = 0; i < bagData.ItemPrefabs.Count; i++)
        {
            initBag(emptySlot, slotGrid, i);
        }
        //Debug.Log("Done");

    }

    public void initBag(GameObject emptySlot, GameObject slotGrid,int itemIndex)
    {
        slots.Add(Instantiate(emptySlot, slotGrid.transform.position, Quaternion.identity));
        //Debug.Log(i);
        slots[itemIndex].transform.SetParent(slotGrid.transform);
        slots[itemIndex].GetComponent<Slot>().slotID = itemIndex;
        if (bagData.ItemPrefabs[itemIndex].item)
        {
            slots[itemIndex].GetComponent<Slot>().SetupSlot(bagData.ItemPrefabs[itemIndex].item, bagData.ItemPrefabs[itemIndex].itemHeld);
        }
        slots[itemIndex].transform.localScale = new Vector3(1, 1, 1);
        slots[itemIndex].GetComponent<Slot>().inventoryManager = this;
        slots[itemIndex].GetComponent<Slot>().inventory = bagData;
        slots[itemIndex].transform.GetChild(0).GetComponent<ItemOnDrag>().myBagData = bagData;
        slots[itemIndex].transform.GetChild(0).gameObject.SetActive(true);
        if (slots[itemIndex].GetComponent<Slot>().slotItem == null)
        {
            //Debug.Log("T:" + i);
            slots[itemIndex].transform.GetChild(0).GetChild(0).gameObject.GetComponent<Image>().color = new Color(255, 255, 255, 0);
        }
        else
        {
            slots[itemIndex].transform.GetChild(0).GetChild(0).gameObject.GetComponent<Image>().color = new Color(255, 255, 255, 255);
        }
    }

    public void UpdateItemInfo(Item slotItem, int slotID)
    {
        itemInformation.text = slotItem.itemInfo;
        ChosenItem = slotItem;
        ChosenItemID = slotID;

        if (ChosenItemID == -2)
        {
            int one = 1;
            inputNumField.text = one.ToString("00");
        }
        if (ChosenItemID >= 0)
        {
            inputNumField.text = bagData.ItemPrefabs[ChosenItemID].itemHeld.ToString("00");
        }
    }

    //丢弃物品
    public void ThrowItem()
    {   //防止点了空物品
        if(ChosenItemID!=-1 && ChosenItem.itemGOPrefab)
        {
            //实例化创造物品
            Vector3 deletePrefabGOPo = player.transform.position + new Vector3(0, player.GetComponent<Collider>().bounds.center.y - player.transform.position.y, 0);
            GameObject deletePrefabGO = Instantiate(ChosenItem.itemGOPrefab, deletePrefabGOPo, ChosenItem.itemGOPrefab.transform.rotation);

            deletePrefabGO.GetComponent<ItemOnWorld>().ItemHeld = int.Parse(inputNumField.text);//bagData.ItemPrefabs[ChosenItemID].itemHeld;
            //更改背包数据
            ItemPrefab deletePrefab;
            deletePrefab.itemHeld = bagData.ItemPrefabs[ChosenItemID].itemHeld - int.Parse(inputNumField.text);
            //若背包清空
            if (deletePrefab.itemHeld <= 0)
            {
                deletePrefab.item = null;
                //ChosenItem = null;
                inputNumField.text = "00";
                itemInformation.text = "";
            }
            //若背包未清空
            else
            {
                deletePrefab.item = bagData.ItemPrefabs[ChosenItemID].item;
            }

            //更改背包Data
            bagData.ItemPrefabs[ChosenItemID] = deletePrefab;

            if (deletePrefab.itemHeld <= 0)
            {
                ChosenItemID = -1;
            }

            //刷新显示
            RefreshInventory();
        }
    }

    #endregion

    //装备物品
    public void EquipBtnClick()
    {   //防止点了空物品
        if (ChosenItemID >= 0 && ChosenItem.itemGOPrefab)
        {
            //Debug.Log("A");
            for (int i = 0; i < slotsEquipped.Count; i++)
            {
                //若符合物品种类要求才可装备
                if(slotsEquipped[i].transform.GetComponent<Slot>().itemTypeNeed == ChosenItem.itemType)
                {
                    ItemPrefab itemEquipPrefab;
                    itemEquipPrefab.item = ChosenItem;
                    itemEquipPrefab.itemHeld = 1;

                    //若目标装备栏有物品
                    if (slotsEquipped[i].transform.GetComponent<Slot>().slotItem != null)
                    {
                        ItemPrefab itemPrefab;
                        itemPrefab.item = slotsEquipped[i].transform.GetComponent<Slot>().slotItem;
                        itemPrefab.itemHeld = 1;
                        bool haveIf = false;
                        for (int j = 0; j < bagData.ItemPrefabs.Count; j++)
                        {
                            if (bagData.ItemPrefabs[j].item == itemPrefab.item)
                            {
                                itemPrefab.itemHeld = bagData.ItemPrefabs[j].itemHeld + 1;
                                bagData.ItemPrefabs[j] = itemPrefab;
                                RefreshItem(itemPrefab.item);
                                haveIf = true;
                                break;
                            }
                        }
                        if (!haveIf)
                        {
                            CreateNewItem(itemPrefab);
                        }
                    }

                    //装备栏部分
                    //更新显示
                    slotsEquipped[i].transform.GetComponent<Slot>().slotItem = itemEquipPrefab.item;
                    slotsEquipped[i].transform.GetComponent<Slot>().SetupSlot(itemEquipPrefab.item, itemEquipPrefab.itemHeld);
                    slotsEquipped[i].transform.GetChild(0).GetChild(1).gameObject.GetComponent<Image>().color = new Color(255, 255, 255, 255);
                    //更新data
                    playerBaseData.ItemEquipped[i] = itemEquipPrefab;

                    //背包部分
                    //更改背包数据，物品数-1
                    ItemPrefab deletePrefab;
                    deletePrefab.itemHeld = bagData.ItemPrefabs[ChosenItemID].itemHeld - 1;
                    //若背包清空
                    if (deletePrefab.itemHeld <= 0)
                    {
                        deletePrefab.item = null;
                        deletePrefab.itemHeld = 0;
                        //ChosenItem = null;
                        inputNumField.text = "00";
                        itemInformation.text = "";
                    }
                    //若背包未清空
                    else
                    {
                        deletePrefab.item = ChosenItem;
                    }
                    //Debug.Log(deletePrefab.itemHeld);
                    //更改背包Data
                    bagData.ItemPrefabs[ChosenItemID] = deletePrefab;

                    if (deletePrefab.itemHeld <= 0)
                    {
                        ChosenItemID = -1;
                    }

                    //刷新显示
                    RefreshInventory();
                }
            }
        }
        else if (ChosenItemID == -2 && ChosenItem.itemGOPrefab)
        {
            //Debug.Log("B");
            for (int i = 0; i < slotsEquipped.Count; i++)
            {
                //遍历寻找对应物品栏
                if (slotsEquipped[i].transform.GetComponent<Slot>().slotItem == ChosenItem)
                {

                    ItemPrefab emptyItemPrefab;
                    emptyItemPrefab.item=null;
                    emptyItemPrefab.itemHeld = 0;

                    //更新背包
                    ItemPrefab itemPrefab;
                    itemPrefab.item = slotsEquipped[i].transform.GetComponent<Slot>().slotItem;
                    itemPrefab.itemHeld = 1;
                    bool haveIf = false;
                    for (int j = 0; j < bagData.ItemPrefabs.Count; j++)
                    {
                        if (bagData.ItemPrefabs[j].item == itemPrefab.item)
                        {
                            itemPrefab.itemHeld = bagData.ItemPrefabs[j].itemHeld + 1;
                            bagData.ItemPrefabs[j] = itemPrefab;
                            RefreshItem(itemPrefab.item);
                            haveIf = true;
                            break;
                        }
                    }
                    if (!haveIf)
                    {
                        CreateNewItem(itemPrefab);
                    }
                    //装备栏部分
                    //更新data
                    playerBaseData.ItemEquipped[i] = emptyItemPrefab;

                    initAllEquipSlots();

                }
            }
        }
        updateWeapon();
    }

    public void initAllEquipSlots()
    {
        slotsEquipped.Clear();
        for (int i = 0; i < EquipItemParent.transform.childCount; i++)
        {
            Destroy(EquipItemParent.transform.GetChild(i).gameObject);
        }
        for (int i = 0; i < EquipItemNum; i++)
        {
            slotsEquipped.Add(Instantiate(emptyEquipSlot, EquipItemParent.transform.position, Quaternion.identity));
            slotsEquipped[i].transform.SetParent(EquipItemParent.transform);
            slotsEquipped[i].gameObject.GetComponent<RectTransform>().anchoredPosition = slotsEquippedPo[i];
            slotsEquipped[i].GetComponent<Slot>().slotID = -2;
            slotsEquipped[i].GetComponent<Slot>().itemTypeNeed = itemsTypeNeed[i];
            slotsEquipped[i].GetComponent<Slot>().slotType = SlotType.EQUIP;
            slotsEquipped[i].GetComponent<Slot>().inventory = bagData;
            slotsEquipped[i].transform.GetChild(0).GetComponent<ItemOnDrag>().myBagData = bagData;
            switch (slotsEquipped[i].GetComponent<Slot>().itemTypeNeed)
            {
                case ItemType.WEAPON:
                    slotsEquipped[i].transform.GetChild(1).gameObject.GetComponent<Text>().text = "武器";
                    break;
                case ItemType.RING:
                    slotsEquipped[i].transform.GetChild(1).gameObject.GetComponent<Text>().text = "戒指";
                    break;
                case ItemType.HAT:
                    slotsEquipped[i].transform.GetChild(1).gameObject.GetComponent<Text>().text = "帽子";
                    break;
                case ItemType.BOOK:
                    slotsEquipped[i].transform.GetChild(1).gameObject.GetComponent<Text>().text = "书籍";
                    break;
            }
            if (playerBaseData.ItemEquipped[i].item)
            {
                slotsEquipped[i].GetComponent<Slot>().SetupSlot(playerBaseData.ItemEquipped[i].item, playerBaseData.ItemEquipped[i].itemHeld);
            }
            slotsEquipped[i].transform.localScale = new Vector3(1, 1, 1);
            slotsEquipped[i].GetComponent<Slot>().inventoryManager = this;
            slotsEquipped[i].transform.GetChild(0).gameObject.SetActive(true);
            if (slotsEquipped[i].GetComponent<Slot>().slotItem == null)
            {
                //Debug.Log("T:" + i);
                slotsEquipped[i].transform.GetChild(0).GetChild(1).gameObject.GetComponent<Image>().color = new Color(255, 255, 255, 0);
            }
            else
            {
                slotsEquipped[i].transform.GetChild(0).GetChild(1).gameObject.GetComponent<Image>().color = new Color(255, 255, 255, 255);
            }
        }
    }

    public void updateWeapon()
    {
        bool ifEquipped = false;
        int i = 0;
        for (; i < slotsEquipped.Count; i++)
        {
            if (slotsEquipped[i].GetComponent<Slot>().itemTypeNeed == ItemType.WEAPON)
            {
                if (playerBaseData.ItemEquipped[i].item)
                {
                    ifEquipped = true;
                }
                break;
            }
        }
        
        Destroy(weaponPo.GetChild(3).gameObject);
        if (ifEquipped)
        {
            GameObject weaponGO=Instantiate(slotsEquipped[i].GetComponent<Slot>().slotItem.itemGOPrefab, weaponPo.position, Quaternion.identity);
            weaponGO.tag = "Untagged";
            weaponGO.layer = 6;
            weaponGO.transform.SetParent(weaponPo);
            weaponGO.transform.localPosition = new Vector3(-0.03f, 0, 0);
            //weaponGO.transform.Rotate(-Vector3.right * 90, Space.Self);
            weaponGO.transform.localRotation = Quaternion.Euler(90, 0, 0);
            weaponGO.transform.localScale = new Vector3(0.25f, 0.25f, 0.25f);
        }
        else
        {
            GameObject weaponGO = Instantiate(weaponPrefabDefault, weaponPo.position, Quaternion.identity);
            weaponGO.transform.SetParent(weaponPo);
            weaponGO.layer = 6;
            weaponGO.transform.localPosition = new Vector3(-0.043f, 0, 0);
            //weaponGO.transform.Rotate(-Vector3.right * 90, Space.Self);
            weaponGO.transform.localRotation = Quaternion.Euler(0, 0, 0);
            weaponGO.transform.localScale = new Vector3(0.66f, 0.66f, 0.66f);
        }
        player.GetComponent<CharacterStats>().updateAttackData();
    }

    void Start()
    {
        for (int i = 0; i < bagData.ItemPrefabs.Count; i++)
        {
            ItemPrefab itemPrefabEmpty;
            itemPrefabEmpty.item = null;
            itemPrefabEmpty.itemHeld = 0;
            bagData.ItemPrefabs[i] = itemPrefabEmpty;
        }
        playerBaseData = player.GetComponent<CharacterStats>().characterData;
        RefreshInventory();
        initAllEquipSlots();
        updateWeapon();
        
    }

    void Update()
    {
        
    }
}
