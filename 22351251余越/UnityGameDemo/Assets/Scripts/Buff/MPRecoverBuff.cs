using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class MPRecoverBuff : Buff
{
    void Update()
    {
        for (int i = 0; i < buffItems.Count; i++)
        {
            var thisbuffItem = buffItems[i];
            if (thisbuffItem.remainingTime > 0)
            {
                thisbuffItem.remainingTime -= Time.deltaTime;
                buffItems[i] = thisbuffItem;
            }
            else
            {
                buffItems.RemoveAt(i);
                ReCalbuff();
                launch();
            }
        }
        if (buffItems.Count == 0)
        {
            Destroy(buffGO);
            Destroy(this);
        }
    }

    public void launch()
    {
        gameObject.GetComponent<CharacterStats>().currentManaRecover = (int)(gameObject.GetComponent<CharacterStats>().baseManaRecover + PlusNumAll);
        targetBuffPanel = gameObject.GetComponent<PlayerController>().buffPanel;
        bool ifHave = false;
        for (int i = 0; i < targetBuffPanel.transform.childCount; i++)
        {
            buffGO = targetBuffPanel.transform.GetChild(i).gameObject;
            if (buffGO.GetComponent<BuffIcon>().buffType == BuffType.MPRECOVER)
            {
                buffGO.transform.GetChild(0).gameObject.GetComponent<Text>().text = PlusNumAll.ToString();
                ifHave = true;
                break;
            }
        }
        if (!ifHave)
        {
            buffGO = Instantiate(buffPrefab, new Vector3(0, 0, 0), Quaternion.identity);
            buffGO.transform.SetParent(targetBuffPanel.transform);
            buffGO.transform.GetChild(0).gameObject.GetComponent<Text>().text = PlusNumAll.ToString();
        }
    }

    public void ReCalbuff()
    {
        if (ifPercent)
        {
            PlusNumAll = 1.0f;
        }
        else
        {
            PlusNumAll = 0.0f;
        }

        for (int i = 0; i < buffItems.Count; i++)
        {
            if (ifPercent)
            {
                PlusNumAll *= (float)(buffItems[i].PlusNum) + 1;
            }
            else
            {
                PlusNumAll += (float)buffItems[i].PlusNum;
            }
        }
        if (ifPercent)
        {
            gameObject.GetComponent<CharacterStats>().currentManaRecover = (int)(gameObject.GetComponent<CharacterStats>().baseManaRecover * PlusNumAll);
        }
        else
        {
            gameObject.GetComponent<CharacterStats>().currentManaRecover = (int)(gameObject.GetComponent<CharacterStats>().baseManaRecover + PlusNumAll);
        }

    }
}
