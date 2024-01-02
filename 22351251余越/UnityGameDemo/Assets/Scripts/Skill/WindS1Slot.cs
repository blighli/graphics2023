using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class WindS1Slot : SkillSlot
{
    float maxRemainTime = 1;
    float existTime = 0;
    bool ifHit = false;
    public void OnClick()
    {
        //CD好了且蓝够了才能放技能
        if (remainingCD <= 0 && skillData.mana <= skillManager.playerBaseData.currentMana)
        {
            //执行动作
            skillManager.player.GetComponent<Animator>().SetTrigger("Skill1");
            WindS1PrefabCreate();
            //转CD且扣蓝
            skillManager.playerBaseData.currentMana -= skillData.mana;
            remainingCD = skillData.CD;
        }
        
    }

    void Start()
    {
        player = skillManager.player;
        CDSlider = transform.GetChild(0).gameObject.GetComponent<Image>();
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
            if (existTime >= 0.5f && !ifHit)
            {
                ifHit = true;
                WindS1Hit();
            }
            if (existTime > 1){
                WindS1End();
            }
        }
    }

    public void WindS1PrefabCreate()
    {
        float prefabHeight = player.GetComponent<Collider>().bounds.center.y - player.transform.position.y;

        skillGO = Instantiate(skillPrefab, player.transform.position + new Vector3(0, prefabHeight, 0), player.transform.rotation);

        skillGO.GetComponent<ParticleSystem>().Play();
        skillGO.transform.SetParent(player.transform);
        existTime = 0;
        ifHit = false;
    }

    public void WindS1Hit()
    {
        //SphereCollider sphereSkillCollider = skillGO.GetComponent<SphereCollider>();
        Collider[] hitColliders = Physics.OverlapSphere(player.transform.position, 2.2f);
        for (int i = 0; i < hitColliders.Length; i++)
        {
            //Debug.Log("S1Hit");
            if (hitColliders[i].gameObject.CompareTag("Enemy"))
            {
                var targetStats = hitColliders[i].gameObject.GetComponent<CharacterStats>();

                targetStats.TakeSkillDamage(skillManager.playerBaseData, skillData, targetStats);
            }
        }
    }

    public void WindS1End()
    {
        Destroy(skillGO);
    }

}
