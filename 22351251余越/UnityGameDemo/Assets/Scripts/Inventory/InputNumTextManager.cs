using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class InputNumTextManager : MonoBehaviour
{
    public InputField inputField;
    public InventoryManager inventoryManager;
    public int maxNum = 0;

    public void limitNumInput()
    {
        int zero = 0;
        maxNum = 0;
        if (inventoryManager.ChosenItemID == -2)
        {
            maxNum = 1;
        }
        else if(inventoryManager.ChosenItemID >= 0 && inventoryManager.ChosenItem != null)
        {
            //Debug.Log(inventoryManager.ChosenItemID);
            maxNum = inventoryManager.bagData.ItemPrefabs[inventoryManager.ChosenItemID].itemHeld;
            
        }

        if (int.Parse(inputField.text) > maxNum)
        {
            inputField.text = maxNum.ToString("00");
        }
        else if (int.Parse(inputField.text) < 0)
        {
            inputField.text = zero.ToString("00");
        }
        else
        {
            inputField.text = int.Parse(inputField.text).ToString("00");
        }
    }


    // Start is called before the first frame update
    void Start()
    {
        int zero = 0;
        inputField.text = zero.ToString("00");
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
