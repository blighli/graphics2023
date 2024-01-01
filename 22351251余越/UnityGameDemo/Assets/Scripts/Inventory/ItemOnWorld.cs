using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ItemOnWorld : MonoBehaviour
{
    public float rotateSpeed = 100.0f;
    public Item thisItem;
    private ItemPrefab itemPrefab;
    public int ItemHeld = 1;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        if (gameObject.CompareTag("Item"))
        {
            gameObject.transform.Rotate(Vector3.up * rotateSpeed * Time.deltaTime, Space.Self);
        }
        
    }

    public void AddNewItem(InventoryManager pickerInventoryManager, Inventory pickerBagData)
    {
        itemPrefab.item = thisItem;
        itemPrefab.itemHeld = ItemHeld;

        for (int i = 0; i < pickerBagData.ItemPrefabs.Count; i++)
        {
            if (pickerBagData.ItemPrefabs[i].item == itemPrefab.item)
            {
                itemPrefab.itemHeld = pickerBagData.ItemPrefabs[i].itemHeld + ItemHeld;
                pickerBagData.ItemPrefabs[i] = itemPrefab;
                pickerInventoryManager.RefreshItem(thisItem);
                Destroy(gameObject);
                return;
            }
        }

        //pickerBagData.ItemPrefabs.Add(itemPrefab);
        pickerInventoryManager.CreateNewItem(itemPrefab);

        Destroy(gameObject);
        
    }
}
