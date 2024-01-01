using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

public class WindS2Slot : SkillSlot
{
    private float movedRange = 0;
    public float speed = 12;
    private Vector3 lastPosition;
    private Vector3 dirVec;
    private List<GameObject> EnemyHit = new List<GameObject>();

    public void OnClick()
    {
        //CD好了且蓝够了才能放技能
        if (remainingCD <= 0 && skillData.mana <= skillManager.playerBaseData.currentMana)
        {
            skillManager.gameObject.GetComponent<PlayerController>().ChooseSkillTarget(this);
        }
    }

    public override void doSkill()
    {
        dirVec= (targetPo - player.transform.position).normalized;
        //执行动作
        skillManager.player.GetComponent<Animator>().SetTrigger("Skill1");
        WindS2PrefabCreate();
        //转CD且扣蓝
        skillManager.playerBaseData.currentMana -= skillData.mana;
        remainingCD = skillData.CD;
    }

    // Start is called before the first frame update
    void Start()
    {
        player = skillManager.player;
        CDSlider = transform.GetChild(0).gameObject.GetComponent<Image>();
    }

    // Update is called once per frame
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
            WindS2Hit();
            WindS2Move();
        }
    }

    public void WindS2PrefabCreate()
    {
        float prefabHeight = player.GetComponent<Collider>().bounds.center.y - player.transform.position.y;
        movedRange = 0;

        skillGO = Instantiate(skillPrefab, player.transform.position + new Vector3(0, prefabHeight, 0), skillPrefab.transform.rotation);

        //skillGO.transform.SetParent(player.transform);
        lastPosition = skillGO.transform.position;
    }

    public void WindS2Hit()
    {
        BoxCollider boxSkillCollider = skillGO.GetComponent<BoxCollider>();
        //若飞过1个碰撞体单位距离
        if ((lastPosition-skillGO.transform.position).sqrMagnitude >= (boxSkillCollider.bounds.extents.x*2)* (boxSkillCollider.bounds.extents.x * 2))
        {
            Collider[] hitColliders = Physics.OverlapBox(skillGO.transform.position + boxSkillCollider.center, boxSkillCollider.bounds.extents);
            for (int i = 0; i < hitColliders.Length; i++)
            {
                if (hitColliders[i].gameObject.CompareTag("Enemy")&& !EnemyHit.Contains(hitColliders[i].gameObject))
                {
                    var targetStats = hitColliders[i].gameObject.GetComponent<CharacterStats>();
                    EnemyHit.Add(hitColliders[i].gameObject);

                    targetStats.TakeSkillDamage(skillManager.playerBaseData, skillData, targetStats);
                }
            }
        }

    }

    public void WindS2Move()
    {
        skillGO.transform.position += dirVec * speed * Time.deltaTime;
        movedRange += speed * Time.deltaTime;
        if (movedRange >= skillData.skillRange)
        {
            movedRange = 0;
            Destroy(skillGO);
            EnemyHit.Clear();
        }
    }

}
