using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class SnowS1Slot : SkillSlot
{
    float existTime;
    float lastHitTime;

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
            lastHitTime += Time.deltaTime;
            if (lastHitTime >= 1.0f)
            {
                SnowS1Hit();
                lastHitTime = 0;
            }
            if (existTime >= skillData.contiTime)
            {
                existTime = 0;
                Destroy(skillGO);
            }
        }
    }

    public override void doSkill()
    {
        //执行动作
        skillManager.player.GetComponent<Animator>().SetTrigger("Magic");
        SnowS1PrefabCreate();
        existTime = 0;
        lastHitTime = 0;
        //转CD且扣蓝
        skillManager.playerBaseData.currentMana -= skillData.mana;
        remainingCD = skillData.CD;
    }

    public void SnowS1PrefabCreate()
    {
        skillGO = Instantiate(skillPrefab, targetPo, player.transform.rotation);

        skillGO.GetComponent<ParticleSystem>().Play();
    }

    public void SnowS1Hit()
    {
        SphereCollider sphereSkillCollider = skillGO.GetComponent<SphereCollider>();
        Collider[] hitColliders = Physics.OverlapSphere(skillGO.transform.position + sphereSkillCollider.center, sphereSkillCollider.radius);
        for (int i = 0; i < hitColliders.Length; i++)
        {
            if (hitColliders[i].gameObject.CompareTag("Enemy"))
            {
                var targetStats = hitColliders[i].gameObject.GetComponent<CharacterStats>();

                targetStats.TakeSkillDamage(skillManager.playerBaseData, skillData, targetStats);
            }
        }
    }
}
