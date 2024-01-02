using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using System;
public class FireDebuff : Buff
{
    public SkillData fireDebuffData;
    public GameObject fireDebuffPrefab;
    public GameObject fireDebuffGO;
    float lastHitTime = 0;
    void Update()
    {
        if (fireDebuffGO == null)
        {
            fireDebuffGO = Instantiate(fireDebuffPrefab, new Vector3(0, 0, 0), fireDebuffPrefab.transform.rotation);
            fireDebuffGO.transform.SetParent(gameObject.transform);
            fireDebuffGO.transform.localPosition = new Vector3(0, 1.6f, 0);
        }
        lastHitTime += Time.deltaTime;
        if (lastHitTime >= 1.0f)
        {
            gameObject.GetComponent<CharacterStats>().TakeSkillDamage(fireDebuffData, gameObject.GetComponent<CharacterStats>(), 1);
            
            lastHitTime = 0;
        }

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
            if (fireDebuffGO != null)
            {
                Destroy(fireDebuffGO);
            }
            Destroy(buffGO);
            Destroy(this);
        }

    }

    public void launch()
    {
        targetBuffPanel = gameObject.GetComponent<PlayerController>().buffPanel;
        bool ifHave = false;
        for (int i = 0; i < targetBuffPanel.transform.childCount; i++)
        {
            buffGO = targetBuffPanel.transform.GetChild(i).gameObject;
            if (buffGO.GetComponent<BuffIcon>().buffType == BuffType.FIRE)
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
        

    }
}
