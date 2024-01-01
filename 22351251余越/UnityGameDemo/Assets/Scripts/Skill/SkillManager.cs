using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public enum PlayerType
{
    WIND,
    RAIN,
    WATER
}

public class SkillManager : MonoBehaviour
{
    public GameObject player;
    public CharacterStats playerBaseData;
    public List<GameObject> SkillList = new List<GameObject>();
    //public List<GameObject> readySkillList = new List<GameObject>();

    private void Awake()
    {
        player = gameObject;
        playerBaseData = gameObject.GetComponent<CharacterStats>();

    }

    public void WindS1Hit()
    {
        for (int i = 0; i < SkillList.Count; i++)
        {
            if (SkillList[i].GetComponent<WindS1Slot>())
            {
                SkillList[i].GetComponent<WindS1Slot>().WindS1Hit();
                break;
            }
        }
        
    }

    public void WindS1End()
    {
        for (int i = 0; i < SkillList.Count; i++)
        {
            if (SkillList[i].GetComponent<WindS1Slot>())
            {
                SkillList[i].GetComponent<WindS1Slot>().WindS1End();
                break;
            }
        }
    }


    public void updateSkillSlots(int level)
    {
        int unlockSkillNum;
        if (level <= 4)
        {
            unlockSkillNum = level * 2;
        }
        else
        {
            unlockSkillNum = 9;
        }
        for (int i = 0; i < unlockSkillNum; i++)
        {
            SkillList[i].SetActive(true);
        }
    }

    void Start()
    {
        updateSkillSlots(1);
    }

    void Update()
    {
        
    }
}
