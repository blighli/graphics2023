using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class WaterS2Slot : SkillSlot
{
    public GameObject defenceBuffPrefab;
    public GameObject defenceMagicBuffPrefab;
    public GameObject MPRecoverBuffPrefab;
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
            WaterS2PrefabCreate();
            //转CD且扣蓝
            skillManager.playerBaseData.currentMana -= skillData.mana;
            remainingCD = skillData.CD;
        }

    }

    public void WaterS2PrefabCreate()
    {

        skillGO = Instantiate(skillPrefab, player.transform.position, Quaternion.identity);
        //skillGO.transform.SetParent(player.transform);
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
            WaterS2Buff();
            if (remainTime <= 0)
            {
                remainTime = 0;
                Destroy(skillGO);
            }
        }
    }

    public void WaterS2Buff()
    {
        SphereCollider sphereSkillCollider = skillGO.GetComponent<SphereCollider>();
        Collider[] hitColliders = Physics.OverlapSphere(skillGO.transform.position + sphereSkillCollider.center, sphereSkillCollider.radius * sphereSkillCollider.transform.localScale.x);
        //Debug.Log(hitColliders.Length);
        for (int i = 0; i < hitColliders.Length; i++)
        {
            if (hitColliders[i].gameObject.CompareTag("Player"))
            {
                //Debug.Log(hitColliders[i].gameObject);
                var targetGO = hitColliders[i].gameObject;

                BuffItem DefenceBuffItem;
                DefenceBuffItem.ID = 0;
                DefenceBuffItem.PlusNum = skillData.defencePlus;
                DefenceBuffItem.durTime = DefenceBuffItem.remainingTime = 2;

                BuffItem DefenceMagicBuffItem;
                DefenceMagicBuffItem.ID = 0;
                DefenceMagicBuffItem.PlusNum = skillData.magicDefencePlus;
                DefenceMagicBuffItem.durTime = DefenceMagicBuffItem.remainingTime = 2;

                BuffItem MPRecoerBuffItem;
                MPRecoerBuffItem.ID = 0;
                MPRecoerBuffItem.PlusNum = (int)skillData.MPRecoverPlus;
                MPRecoerBuffItem.durTime = MPRecoerBuffItem.remainingTime = 2;

                if (!targetGO.GetComponent<DefenceBuff>())
                {
                    targetGO.AddComponent<DefenceBuff>()
                    .Setup(targetGO, DefenceBuffItem)
                    .buffPrefab = defenceBuffPrefab;
                    targetGO.GetComponent<DefenceBuff>().ReCalbuff();
                    targetGO.GetComponent<DefenceBuff>().launch();
                }
                else
                {
                    targetGO.GetComponent<DefenceBuff>().Setup(targetGO, DefenceBuffItem);
                }

                if (!targetGO.GetComponent<DefenceMagicBuff>())
                {
                    targetGO.AddComponent<DefenceMagicBuff>()
                    .Setup(targetGO, DefenceMagicBuffItem)
                    .buffPrefab = defenceMagicBuffPrefab;
                    targetGO.GetComponent<DefenceMagicBuff>().ReCalbuff();
                    targetGO.GetComponent<DefenceMagicBuff>().launch();
                }
                else
                {
                    targetGO.GetComponent<DefenceMagicBuff>().Setup(targetGO, DefenceMagicBuffItem);
                }

                if (!targetGO.GetComponent<MPRecoverBuff>())
                {
                    targetGO.AddComponent<MPRecoverBuff>()
                    .Setup(targetGO, MPRecoerBuffItem)
                    .buffPrefab = MPRecoverBuffPrefab;
                    targetGO.GetComponent<MPRecoverBuff>().ReCalbuff();
                    targetGO.GetComponent<MPRecoverBuff>().launch();
                }
                else
                {
                    targetGO.GetComponent<MPRecoverBuff>().Setup(targetGO, MPRecoerBuffItem);
                }
            }
        }
    }

}
