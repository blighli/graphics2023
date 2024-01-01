using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class WoodS1Slot : SkillSlot
{
    public GameObject defenceBuffPrefab;
    public GameObject defenceMagicBuffPrefab;
    public float remainTime;

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
            //执行动作
            skillManager.player.GetComponent<Animator>().SetTrigger("Heal");
            WoodS1PrefabCreate();
            //转CD且扣蓝
            skillManager.playerBaseData.currentMana -= skillData.mana;
            remainingCD = skillData.CD;
        }

    }
    public void WoodS1PrefabCreate()
    {
        float prefabHeight = player.GetComponent<Collider>().bounds.center.y - player.transform.position.y;

        skillGO = Instantiate(skillPrefab, player.transform.position + new Vector3(0, prefabHeight, 0), Quaternion.identity);

        skillGO.transform.SetParent(player.transform);
        skillGO.transform.localScale = prefabHeight * 2 * new Vector3(1, 1, 1);
        remainTime = skillData.contiTime;
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
            WoodS1Buff();
            if (remainTime <= 0)
            {
                remainTime = 0;
                Destroy(skillGO);
            }
        }
    }

    public void WoodS1Buff()
    {
        BuffItem DefenceBuffItem;
        DefenceBuffItem.ID = 0;
        DefenceBuffItem.PlusNum = skillData.defencePlus;
        DefenceBuffItem.durTime = DefenceBuffItem.remainingTime = 1;

        BuffItem DefenceMagicBuffItem;
        DefenceMagicBuffItem.ID = 0;
        DefenceMagicBuffItem.PlusNum = skillData.magicDefencePlus;
        DefenceMagicBuffItem.durTime = DefenceMagicBuffItem.remainingTime = 1;

        if (!player.GetComponent<DefenceBuff>())
        {
            player.AddComponent<DefenceBuff>()
            .Setup(player, DefenceBuffItem)
            .buffPrefab = defenceBuffPrefab;
            player.GetComponent<DefenceBuff>().ReCalbuff();
            player.GetComponent<DefenceBuff>().launch();
        }
        else
        {
            player.GetComponent<DefenceBuff>().Setup(player, DefenceBuffItem);
        }

        if (!player.GetComponent<DefenceMagicBuff>())
        {
            player.AddComponent<DefenceMagicBuff>()
            .Setup(player, DefenceMagicBuffItem)
            .buffPrefab = defenceMagicBuffPrefab;
            player.GetComponent<DefenceMagicBuff>().ReCalbuff();
            player.GetComponent<DefenceMagicBuff>().launch();
        }
        else
        {
            player.GetComponent<DefenceMagicBuff>().Setup(player, DefenceMagicBuffItem);
        }
    }
}
