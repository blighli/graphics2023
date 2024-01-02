using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

[System.Serializable]
public struct ItemPrefab
{
    public Item item;
    public int itemHeld;
}

[CreateAssetMenu(fileName ="New Inventory",menuName = "Inventory/New Inventory")]
public class Inventory : ScriptableObject
{
    //[SerializeReference]
    //public Dictionary<Item, int> itemDic = new Dictionary<Item, int>();
    //public ItemPrefab[] ItemPrefabs;
    public List<ItemPrefab> ItemPrefabs = new List<ItemPrefab>();//记录该背包中的不同种类的物品
    
    private void Awake()
    {
        
    }

    void Start()
    {
        //ItemPrefabs = new ItemPrefab[100];
    }

    void Update()
    {
        
    }
}
