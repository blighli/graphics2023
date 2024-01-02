using System.Collections;
using System.Collections.Generic;
using UnityEngine;

//物品种类
public enum ItemType
{
    WEAPON,//武器
    HAT,//帽子
    RING,//戒指
    BOOK,//书本
    CONSUME,//药水等消耗品
    ALL//任意物品类型（仅用于物品栏物品要求）
}

[CreateAssetMenu(fileName ="New Item",menuName ="Inventory/New Item")]
public class Item : ScriptableObject
{
    public string itemName;
    public Sprite itemImage;
    //public List<Item> itemPlayerHeld = new List<Item>();
    [TextArea]
    public string itemInfo;
    //public int itemHeld = 1;
    public GameObject itemGOPrefab;
    public ItemType itemType;
    [Header("ItemData")]
    public int atkPlus;
    public int spPlus;
    public float criticalChancePlus;
    public float criticalMagicChancePlus;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
