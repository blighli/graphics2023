using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;


public class WaterS1Slot : SkillSlot
{
    public GameObject waterS1HitRangePrefab;

    public WaterBendingControll waterBendingControll;

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
            WaterS1Hit();
        }
    }

    public override void doSkill()
    {
        //执行动作
        skillManager.player.GetComponent<Animator>().SetTrigger("Magic");
        WaterS1PrefabCreate(targetPo);
        //转CD且扣蓝
        skillManager.playerBaseData.currentMana -= skillData.mana;
        remainingCD = skillData.CD;
    }

    public void WaterS1PrefabCreate(Vector3 target)
    {
        //Vector3 waterInstancePo = new Vector3(player.transform.position.x, player.transform.position.y * 2, player.transform.position.z);
        skillGO = Instantiate(skillPrefab, new Vector3(0,0,0), skillPrefab.transform.rotation);
        skillGO.transform.position += player.transform.position;
        waterBendingControll = skillGO.GetComponent<WaterBendingControll>();
        waterBendingControll.WaterBend(target);
    }
    
    public void WaterS1Hit()
    {
        if (waterBendingControll.ifDamage)
        {
            GameObject waterS1HitRange = Instantiate(waterS1HitRangePrefab, targetPo, Quaternion.identity);
            SphereCollider sphereSkillCollider = waterS1HitRange.GetComponent<SphereCollider>();
            Collider[] hitColliders = Physics.OverlapSphere(waterS1HitRange.transform.position + sphereSkillCollider.center, sphereSkillCollider.radius);
            for (int i = 0; i < hitColliders.Length; i++)
            {
                if (hitColliders[i].gameObject.CompareTag("Enemy"))
                {
                    var targetStats = hitColliders[i].gameObject.GetComponent<CharacterStats>();

                    targetStats.TakeSkillDamage(skillManager.playerBaseData, skillData, targetStats);
                }
            }

            waterBendingControll.ifDamage = false;
            Destroy(waterS1HitRange);
        }

    }
}
