using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class BossController : EnemyController
{
    [Header("UI")]
    public GameObject BossCanvas;
    public GameObject HealthBarHolder;
    public GameObject fireIcon;
    public GameObject DiaUI;

    [Header("技能释放")]
    public float lastSkillTime = 0;
    public float skillTime;
    public int skillID = 0;
    public bool ifSkilling = false;
    public float waitSkillTime = 0;

    [Header("烧伤Debuff")]
    public BuffItem fireDebuffItem;
    public SkillData fireDebuffSkill;
    public GameObject fireDebuffIconPrefab;
    public GameObject fireDebuffPrefab;

    [Header("核爆")]
    public GameObject explosionPrefab;
    public GameObject explosionGO;
    public SkillData explosionData;

    [Header("踩地板出石头")]
    public GameObject stonePrefab;
    public SkillData jumpData;

    [Header("喷火")]
    public GameObject firePrefab;
    public GameObject fireGO;
    public SkillData fireData;
    float fireHitTime;

    [Header("敲地板")]
    public GameObject stoneShockPrefab;
    public GameObject stoneShockGO;
    public SkillData stoneShockData;

    [Header("陨石")]
    public GameObject MeteorMarkerPrefab;
    public List<GameObject> MeteorMakerGOs = new List<GameObject>();
    public GameObject MeteorPrefab;
    public List<GameObject> MeteorGOs = new List<GameObject>();
    public GameObject MeteorAreaPrefab;
    public List<GameObject> MeteorAreaGOs = new List<GameObject>();
    public SkillData MeteorData;
    protected List<Vector3> MeteorPos = new List<Vector3>();

    [Header("激光")]
    public GameObject bulletPrefab;
    public GameObject bulletGO;

    #region 核爆
    public void useBomb()
    {
        anim.SetTrigger("Skill1");
    }
    public void ExplosionCreate()
    {
        explosionGO = Instantiate(explosionPrefab, transform.position + new Vector3(0, 0.9f, 0.7f), transform.rotation);
    }

    public void ExplosionHit()
    {
        Collider[] hitColliders = Physics.OverlapSphere(explosionGO.transform.position, 50f);
        for (int i = 0; i < hitColliders.Length; i++)
        {
            if (hitColliders[i].gameObject.CompareTag("Player"))
            {
                var targetStats = hitColliders[i].gameObject.GetComponent<CharacterStats>();

                targetStats.TakeSkillDamage(characterStats, explosionData, targetStats);
                targetStats.gameObject.GetComponent<Animator>().SetTrigger("Hit");
                if((float)characterStats.currentHealth / characterStats.maxHealth > 0.25)
                {
                    addFireDebuff(hitColliders[i].gameObject);
                }
                
            }
            else if (hitColliders[i].gameObject.CompareTag("Stone"))
            {
                hitColliders[i].gameObject.GetComponent<RedRock>().ifFire = true;
            }
        }
    }

    public void ExplosionEnd()
    {
        Destroy(explosionGO);
        ifSkilling = false;
    }
    #endregion
    #region 跳跃
    public void UseJump()
    {
        StartCoroutine(jump());
    }

    IEnumerator jump()
    {
        while(Vector3.Distance(transform.position, attackTarget.transform.position) >= 3f)
        {
            chasePlayer();
            yield return null;
        }
        isFollow = false;
        agent.isStopped = true;
        //transform.LookAt(attackTarget.transform);
        anim.SetTrigger("Jump");
    }

    public void JumpHit()
    {
        Collider[] hitColliders = Physics.OverlapSphere(transform.position, 3f);
        for (int i = 0; i < hitColliders.Length; i++)
        {
            if (hitColliders[i].gameObject.CompareTag("Player"))
            {
                var targetStats = hitColliders[i].gameObject.GetComponent<CharacterStats>();

                targetStats.TakeSkillDamage(characterStats, jumpData, targetStats);
                targetStats.gameObject.GetComponent<Animator>().SetTrigger("Dizzy");
                Instantiate(stonePrefab, hitColliders[i].gameObject.transform.position, transform.rotation);
            }
        }
        Instantiate(stonePrefab, transform.position + new Vector3(6, 0, 0), transform.rotation);
        Instantiate(stonePrefab, transform.position + new Vector3(-6, 0, 0), transform.rotation);
        Instantiate(stonePrefab, transform.position + new Vector3(0, 0, 6), transform.rotation);
        Instantiate(stonePrefab, transform.position + new Vector3(0, 0, -6), transform.rotation);
        ifSkilling = false;
    }
    #endregion
    #region 喷火
    public void UseFire()
    {
        StartCoroutine(Fire());
    }

    IEnumerator Fire()
    {
        while (Vector3.Distance(transform.position, attackTarget.transform.position) >= 4f)
        {
            chasePlayer();
            yield return null;
        }
        isFollow = false;
        agent.isStopped = true;
        transform.LookAt(attackTarget.transform);
        anim.SetTrigger("Skill2");
    }

    public void fireCreate()
    {
        fireGO=Instantiate(firePrefab, transform.position + new Vector3(0, 1.45f, 2.4f), transform.rotation);
        fireGO.transform.SetParent(transform);
        fireGO.transform.localPosition = new Vector3(0, 1.45f, 2.4f);
        fireHitTime = 0;
    }

    public void FireHit()
    {
        BoxCollider boxSkillCollider = fireGO.GetComponent<BoxCollider>();
        Collider[] hitColliders = Physics.OverlapBox(transform.position + boxSkillCollider.center, new Vector3(7, 1, 3), transform.rotation);
        for (int i = 0; i < hitColliders.Length; i++)
        {
            if (hitColliders[i].gameObject.CompareTag("Player") && transform.IsFacingTarget(hitColliders[i].gameObject.transform))
            {
                var targetStats = hitColliders[i].gameObject.GetComponent<CharacterStats>();

                targetStats.TakeSkillDamage(fireData, targetStats);
                if ((float)characterStats.currentHealth / characterStats.maxHealth > 0.25)
                {
                    addFireDebuff(hitColliders[i].gameObject);
                }
            }
            else if (hitColliders[i].gameObject.CompareTag("Stone") && transform.IsFacingTarget(hitColliders[i].gameObject.transform))
            {
                hitColliders[i].gameObject.GetComponent<RedRock>().ifFire = true;
            }
        }
    }

    public void FireEnd()
    {
        Destroy(fireGO);
        ifSkilling = false;
    }
    #endregion
    #region 石头冲击波
    public void useStoneAtk()
    {
        StartCoroutine(StoneAtk());
    }

    IEnumerator StoneAtk()
    {
        while (Vector3.Distance(transform.position, attackTarget.transform.position) >= 4f)
        {
            chasePlayer();
            yield return null;
        }
        isFollow = false;
        agent.isStopped = true;
        transform.LookAt(attackTarget.transform);
        anim.SetTrigger("Skill3");
    }
    
    public void StoneShockCreate()
    {
        stoneShockGO = Instantiate(stoneShockPrefab, transform.position, transform.rotation);
    }

    public void StoneShockHit()
    {
        BoxCollider boxSkillCollider = stoneShockGO.GetComponent<BoxCollider>();
        Vector3 rotation = transform.rotation.eulerAngles;
        Vector3 HitPo = new Vector3(0, 0, 5 * Mathf.Cos(rotation.y));

        Collider[] hitColliders = Physics.OverlapBox(transform.position + HitPo, new Vector3(3f, 5f, 9f), transform.rotation);
        for (int i = 0; i < hitColliders.Length; i++)
        {
            if (hitColliders[i].gameObject.CompareTag("Player"))
            {
                var targetStats = hitColliders[i].gameObject.GetComponent<CharacterStats>();

                targetStats.TakeSkillDamage(characterStats, stoneShockData, targetStats);

            }
            else if (hitColliders[i].gameObject.CompareTag("Stone"))
            {
                hitColliders[i].gameObject.GetComponent<RedRock>().ifFire = true;
            }
        }
    }

    public void StoneShockEnd()
    {
        Destroy(stoneShockGO);
        ifSkilling = false;
    }
    #endregion
    #region 陨石
    public void useMeteor()
    {
        anim.SetTrigger("Skill4");
        for (int i = 0; i < 2; i++)
        {
            MeteorPos.Add(transform.position + new Vector3(Random.Range(3, 10), 0, Random.Range(0, 5)));
            MeteorMakerGOs.Add(Instantiate(MeteorMarkerPrefab, MeteorPos[i], MeteorMarkerPrefab.transform.rotation));
            
        }
        MeteorPos.Add(attackTarget.transform.position);
        MeteorMakerGOs.Add(Instantiate(MeteorMarkerPrefab, MeteorPos[MeteorPos.Count - 1], MeteorMarkerPrefab.transform.rotation));
        
    }
    public void MeteorCreate()
    {
        for (int i = 0; i < 2; i++)
        {
            //MeteorPos.Add(transform.position + new Vector3(Random.Range(3, 10), 0, Random.Range(0, 5)));
            //MeteorMakerGOs.Add(Instantiate(MeteorMarkerPrefab, MeteorPos[i], transform.rotation));
            MeteorGOs.Add(Instantiate(MeteorPrefab, MeteorPos[i], transform.rotation));
        }
        //MeteorPos.Add(attackTarget.transform.position);
        MeteorGOs.Add(Instantiate(MeteorPrefab, MeteorPos[MeteorPos.Count - 1], transform.rotation));
        MeteorPos.Clear();
    }

    public void MeteorHit()
    {
        for (int j = 0; j < MeteorGOs.Count; j++)
        {
            Collider[] hitColliders = Physics.OverlapBox(MeteorGOs[j].transform.position, new Vector3(3f, 5f, 3f));
            for (int i = 0; i < hitColliders.Length; i++)
            {
                if (hitColliders[i].gameObject.CompareTag("Player"))
                {
                    var targetStats = hitColliders[i].gameObject.GetComponent<CharacterStats>();

                    targetStats.TakeSkillDamage(characterStats, MeteorData, targetStats);
                    addFireDebuff(hitColliders[i].gameObject);
                }
                else if (hitColliders[i].gameObject.CompareTag("Stone"))
                {
                    hitColliders[i].gameObject.GetComponent<RedRock>().ifFire = true;
                }
            }
            MeteorAreaGOs.Add(Instantiate(MeteorAreaPrefab, MeteorGOs[j].transform.position, transform.rotation));
            Destroy(MeteorMakerGOs[j]);
        }
        MeteorMakerGOs.Clear();
        MeteorAreaGOs.Clear();
    }

    public void MeteorEnd()
    {
        for (int j = 0; j < MeteorGOs.Count; j++)
        {
            Destroy(MeteorGOs[j]);
        }
        MeteorGOs.Clear();
        ifSkilling = false;
    }
    #endregion
    #region 激光
    public void useLaser()
    {
        anim.SetTrigger("Skill5");
    }

    public void BulletCreate()
    {
        bulletGO = Instantiate(bulletPrefab, transform.position + new Vector3(0, 0.9f, 0.7f), transform.rotation);
        bulletGO.GetComponent<LaserBullet>().destinationPo = attackTarget.transform.position + new Vector3(0, 2, 0);
        bulletGO.GetComponent<LaserBullet>().player = attackTarget;
    }

    public void BulletEnd()
    {
        ifSkilling = false;
    }

    #endregion

    void Start()
    {
        if (isGuard)
        {
            enemyStates = EnemyStates.GUARD;
        }
        else
        {
            enemyStates = EnemyStates.PATROL;
            GetNewWayPoint();
        }
        GameManager.Instance.AddObserver(this);
        fireDebuffItem.ID = 0;
        fireDebuffItem.PlusNum = fireDebuffSkill.baseDamage;
        fireDebuffItem.durTime = fireDebuffItem.remainingTime = fireDebuffSkill.contiTime;
        
    }
    private void Update()
    {
        UpdateUI();
        if (characterStats.currentHealth == 0)
        {
            DropItem();
            DiaUI.SetActive(true);
            isDead = true;
        }
        if (!PlayerDead)
        {
            SwitchStates();
            SwitchAnimation();
            if (lastAttackTime >= 0)
            {
                lastAttackTime -= Time.deltaTime;
            }
        }

        UpdateStats();
        anim.SetBool("Skilling", ifSkilling);

        if (fireGO != null)
        {
            fireHitTime += Time.deltaTime;
            if (fireHitTime >= 0.2)
            {
                FireHit();
                fireHitTime = 0;
            }
        }
        if (ifSkilling)
        {
            waitSkillTime += Time.deltaTime;
            if (waitSkillTime >= 1.5f)
            {
                ifSkilling = false;
            }
        }
        else
        {
            waitSkillTime = 0;
        }
        
    }

    public void addFireDebuff(GameObject targetGO)
    {
        if (!targetGO.GetComponent<FireDebuff>())
        {
            targetGO.AddComponent<FireDebuff>()
            .Setup(targetGO, fireDebuffItem)
            .buffPrefab = fireDebuffIconPrefab;
            targetGO.GetComponent<FireDebuff>().ReCalbuff();
            targetGO.GetComponent<FireDebuff>().launch();
            targetGO.GetComponent<FireDebuff>().fireDebuffData = fireDebuffSkill;
            targetGO.GetComponent<FireDebuff>().fireDebuffPrefab = fireDebuffPrefab;
        }
        else
        {
            targetGO.GetComponent<FireDebuff>().Setup(targetGO, fireDebuffItem);
        }
    }

    protected void SwitchStates()
    {
        if (isDead)
        {
            enemyStates = EnemyStates.DEAD;
        }
        //如果发现player 切换CHASE
        else if (FoundPlayer())
        {
            enemyStates = EnemyStates.CHASE;
            //Debug.Log("Find Player!");
        }

        switch (enemyStates)
        {
            case EnemyStates.GUARD:
                agent.isStopped = false;
                isChase = false;
                if (transform.position != guardPos)
                {
                    isWalk = true;
                    agent.isStopped = false;
                    agent.destination = guardPos;

                    if (Vector3.SqrMagnitude(guardPos - transform.position) <= agent.stoppingDistance)
                    {
                        isWalk = false;
                        transform.rotation = Quaternion.Lerp(transform.rotation, guadRotation, 0.03f);
                    }

                }

                break;
            case EnemyStates.PATROL:

                agent.isStopped = false;
                isChase = false;
                agent.speed = speed * 0.5f;
                //是否到了目标巡逻点
                if (Vector3.Distance(wayPoint, transform.position) <= agent.stoppingDistance)
                {
                    isWalk = false;
                    if (remainLookAtTime > 0)
                    {
                        remainLookAtTime -= Time.deltaTime;
                    }
                    else
                    {
                        remainLookAtTime = lookAtTime;
                        GetNewWayPoint();
                    }

                }
                else
                {
                    isWalk = true;
                    agent.destination = wayPoint;
                }
                break;
            case EnemyStates.CHASE:

                isWalk = false;
                isChase = true;

                agent.speed = speed;
                //未发现玩家
                if (!FoundPlayer())
                {
                    isFollow = false;
                    agent.isStopped = false;
                    if (remainLookAtTime > 0)
                    {

                        agent.destination = transform.position;
                        remainLookAtTime -= Time.deltaTime;
                    }
                    else if (isGuard)
                    {
                        //agent.destination = guardPos;
                        enemyStates = EnemyStates.GUARD;
                    }
                    else
                    {
                        enemyStates = EnemyStates.PATROL;
                    }
                    //agent.destination = transform.position;
                }
                //发现玩家
                else
                {
                    SwitchAttackType();
                }

                break;
            case EnemyStates.DEAD:
                coll.enabled = false;
                //agent.enabled = false;
                agent.radius = 0;
                Destroy(gameObject, 2f);
                break;
        }
    }

    void SwitchAttackType()
    {
        if (!ifSkilling)
        {
            //若不放技能
            if (lastSkillTime <= skillTime)
            {
                lastSkillTime += Time.deltaTime;
                chasePlayer();
                //在攻击范围内则攻击
                if (TargetInAttackRange())
                {
                    isFollow = false;
                    agent.isStopped = true;
                    if (lastAttackTime < 0)
                    {
                        lastAttackTime = characterStats.attackData.coolDown;

                        //暴击判断
                        characterStats.isCritical = UnityEngine.Random.value < characterStats.attackData.criticalChance;
                        //攻击
                        Attack();
                    }
                }
            }
            //放技能
            else
            {
                ifSkilling = true;
                isFollow = false;
                agent.isStopped = true;
                
                //Debug.Log(skillID);
                switch (skillID)
                {
                    case 0:
                        useBomb();
                        break;
                    case 1:
                        UseJump();
                        break;
                    case 2:
                        UseFire();
                        break;
                    case 3:
                        useStoneAtk();
                        break;
                    case 4:
                        useMeteor();
                        break;
                    case 5:
                        useLaser();
                        break;
                }
                float sliderPercent = (float)characterStats.currentHealth / characterStats.maxHealth;
                
                if (sliderPercent >= 0.75)
                {
                    skillID = (skillID + 1) % 3;
                }
                else if (sliderPercent >= 0.6)
                {
                    skillID = (skillID + 1) % 6;
                }
                else
                {
                    skillID = (int)(Random.Range(0, 6.9f));
                }
                lastSkillTime = 0;
                
                //agent.isStopped = false;
            }
            agent.stoppingDistance = stoppingDis;
        }

    }

    void UpdateUI()
    {
        if(isChase && characterStats.currentHealth > 0)
        {
            BossCanvas.SetActive(true);
            float sliderPercent = (float)characterStats.currentHealth / characterStats.maxHealth;
            HealthBarHolder.transform.GetChild(0).GetComponent<Image>().fillAmount = sliderPercent;
            if (sliderPercent <= 0.25)
            {
                fireIcon.GetComponent<RawImage>().color = new Color(0, 0, 0);
            }
        }
        else
        {
            BossCanvas.SetActive(false);
        }
    }

    void chasePlayer()
    {
        isFollow = true;
        agent.isStopped = false;
        agent.stoppingDistance = characterStats.attackData.attackRange;
        agent.destination = attackTarget.transform.position;
    }

    protected new void Attack()
    {
        transform.LookAt(attackTarget.transform);
        if (TargetInAttackRange())
        {
            if (characterStats.isCritical)
            {
                //近身暴击攻击
                anim.SetTrigger("Attack3");
            }
            else
            {
                bool ifLeft = UnityEngine.Random.value < 0.5;
                //近身攻击
                if (ifLeft)
                {
                    anim.SetTrigger("Attack1");
                }
                else
                {
                    anim.SetTrigger("Attack2");
                }
            }
        }
    }

    protected void BaseHit()
    {
        if (attackTarget != null && transform.IsFacingTarget(attackTarget.transform))
        {
            var targetStats = attackTarget.GetComponent<CharacterStats>();

            targetStats.TakeDamage(characterStats, targetStats);
            
        }

    }
    protected void CriHit()
    {
        if (attackTarget != null && transform.IsFacingTarget(attackTarget.transform))
        {
            var targetStats = attackTarget.GetComponent<CharacterStats>();

            targetStats.TakeDamage(characterStats, targetStats);

            if ((float)characterStats.currentHealth / characterStats.maxHealth > 0.5)
            {
                addFireDebuff(attackTarget);
            }
            
        }

    }


}
