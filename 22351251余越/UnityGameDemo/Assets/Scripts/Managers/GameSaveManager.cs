using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;

public class GameSaveManager : MonoBehaviour
{
    public InventoryManager myInventoryManager;
    public Inventory myInventoryData;

    public void SaveGame()
    {
        Debug.Log("�浵Ŀ¼" + Application.persistentDataPath + "/game_SaveData");
        if (!Directory.Exists(Application.persistentDataPath + "/game_SaveData")) 
        {
            Directory.CreateDirectory(Application.persistentDataPath + "/game_SaveData");
        }

        BinaryFormatter formatter = new BinaryFormatter();

        FileStream file = File.Create(Application.persistentDataPath + "/game_SaveData/myInventory.txt");

        var jsonData = JsonUtility.ToJson(myInventoryData);

        formatter.Serialize(file, jsonData);

        file.Close();
    }

    public void LoadGame()
    {
        BinaryFormatter formatter = new BinaryFormatter();

        if(File.Exists(Application.persistentDataPath + "/game_SaveData/myInventory.txt"))
        {
            FileStream file = File.Open(Application.persistentDataPath + "/game_SaveData/myInventory.txt", FileMode.Open);

            JsonUtility.FromJsonOverwrite((string)formatter.Deserialize(file), myInventoryData);

            file.Close();

            myInventoryManager.RefreshInventory();
        }



    }

    void Start()
    {
        
    }

    
    void Update()
    {
        
    }
}
