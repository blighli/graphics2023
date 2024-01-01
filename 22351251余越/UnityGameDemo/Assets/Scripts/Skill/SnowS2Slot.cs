using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class SnowS2Slot : SkillSlot
{
    float existTime;
    bool ifDamage;
    bool ifDamage1;

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
            if (existTime >= 0.5f && !ifDamage)
            {
                SnowS2Hit();
                ifDamage = true;
            }
            if (existTime >= 3.0f && !ifDamage1)
            {
                SnowS2Hit();
                ifDamage1 = true;
            }
            if (existTime >= 4.0f)
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
        SnowS2PrefabCreate();
        existTime = 0;
        //转CD且扣蓝
        skillManager.playerBaseData.currentMana -= skillData.mana;
        remainingCD = skillData.CD;
    }

    public void SnowS2PrefabCreate()
    {
        skillGO = Instantiate(skillPrefab, player.transform.position, player.transform.rotation);
        ifDamage = false;
        ifDamage1 = false;
        skillGO.GetComponent<ParticleSystem>().Play();
    }

    public void SnowS2Hit()
    {
        BoxCollider boxSkillCollider = skillGO.GetComponent<BoxCollider>();
        Vector3 rotation = player.transform.rotation.eulerAngles;
        Vector3 HitPo = new Vector3(0, 0, 5 * Mathf.Cos(rotation.y));
        Collider[] hitColliders = Physics.OverlapBox(player.transform.position + HitPo, boxSkillCollider.bounds.extents, player.transform.rotation);
        
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
