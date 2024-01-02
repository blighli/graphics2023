using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

public class WoodS2Slot : SkillSlot
{
    float existTime;
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
            skillManager.gameObject.GetComponent<PlayerController>().ChooseSkillTarget(this);
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
            existTime += Time.deltaTime;
            if (existTime >= 0.6f)
            {
                WoodS2Hit();
            }
            if(existTime >= 1.0f)
            {
                existTime = 0;
                Destroy(skillGO);
            }
        }
    }

    public override void doSkill()
    {
        //执行动作
        //skillManager.player.GetComponent<Animator>().SetTrigger("Skill3");
        WoodS2PrefabCreate();
        existTime = 0;
        //转CD且扣蓝
        skillManager.playerBaseData.currentMana -= skillData.mana;
        remainingCD = skillData.CD;
    }

    public void WoodS2PrefabCreate()
    {
        skillGO = Instantiate(skillPrefab, targetPo, player.transform.rotation);

        skillGO.GetComponent<ParticleSystem>().Play();
    }

    public void WoodS2Hit()
    {
        BoxCollider boxSkillCollider = skillGO.GetComponent<BoxCollider>();

        Collider[] hitColliders = Physics.OverlapBox(skillGO.transform.position + boxSkillCollider.center, boxSkillCollider.bounds.extents);
        for (int i = 0; i < hitColliders.Length; i++)
        {
            if (hitColliders[i].gameObject.CompareTag("Enemy"))
            {
                var targetStats = hitColliders[i].gameObject.GetComponent<CharacterStats>();

                targetStats.TakeSkillDamage(skillManager.playerBaseData, skillData, targetStats);

                if (targetStats.ifDizzy)
                {
                    hitColliders[i].gameObject.GetComponent<Animator>().SetTrigger("Hit");
                }
            }
        }
    }

}
