using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using UnityEngine.UI;

public class RainS1Slot : SkillSlot
{
    public GameObject PlayerGroupGO;
    public float remainTime;
    public List<GameObject> skillGOs = new List<GameObject>();

    void Start()
    {
        player = skillManager.player;
        CDSlider = transform.GetChild(0).gameObject.GetComponent<Image>();
    }

    public void OnClick()
    {
        //CD好了且蓝够了才能放技能
        if (remainingCD <= 0 && skillData.mana <= skillManager.playerBaseData.currentMana)
        {
            player.GetComponent<Animator>().SetTrigger("Heal");
            for (int i = 0; i < PlayerGroupGO.transform.childCount; i++)
            {
                if (PlayerGroupGO.transform.GetChild(i).gameObject.activeSelf)
                {
                    RainS1PrefabCreate(PlayerGroupGO.transform.GetChild(i).gameObject);
                    remainTime = 1;
                }
            }
            skillManager.playerBaseData.currentMana -= skillData.mana;
            remainingCD = skillData.CD;
        }
    }

    void Update()
    {
        if (remainingCD > 0)
        {
            remainingCD -= Time.deltaTime;
        }
        float sliderPercent = remainingCD / skillData.CD;
        CDSlider.fillAmount = sliderPercent;
        if (skillGO != null)
        {
            remainTime -= Time.deltaTime;
            if (remainTime <= 0)
            {
                remainTime = 0;
                for (int i = 0; i < skillGOs.Count; i++)
                {
                    Destroy(skillGOs[i]);
                }
                skillGOs.Clear();
            }
        }
    }

    public void RainS1PrefabCreate(GameObject target)
    {
        GameObject thisSkillTargetGO = Instantiate(skillPrefab, target.transform.position, Quaternion.identity);
        skillGOs.Add(thisSkillTargetGO);
        thisSkillTargetGO.GetComponent<ParticleSystem>().Play();
        int HealPoint = skillData.baseDamage//基础数值
            + Convert.ToInt16(UnityEngine.Random.Range(player.GetComponent<CharacterStats>().attackData.minDamage, player.GetComponent<CharacterStats>().attackData.maxDamage) * skillData.atkFactor//AD加成
            + player.GetComponent<CharacterStats>().attackData.SkillPower * skillData.SPFactor//AP加成
            + player.GetComponent<CharacterStats>().maxHealth * skillData.HPFactor);//HP加成
        target.GetComponent<CharacterStats>().currentHealth += HealPoint;

    }



}
