using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;
using System;

public enum EnemyStates { GUARD, PATROL, CHASE, DEAD }
[RequireComponent(typeof(NavMeshAgent))]
[RequireComponent(typeof(CharacterStats))]
public class EnemyController : MonoBehaviour,IEndGameObserver
{
    protected EnemyStates enemyStates;

    protected NavMeshAgent agent;

    protected Animator anim;

    protected Collider coll;

    protected CharacterStats characterStats;

    protected float stoppingDis;

    [Header("Basic Settings")]
    public float sightRadius;
    public bool isGuard;

    public float lookAtTime;

    protected float remainLookAtTime;

    protected float speed;

    protected Vector3 guardPos;

    protected GameObject attackTarget;

    protected float lastAttackTime;

    protected float lastOneSecond;

    [Header("Patrol State")]

    public float patrolRange;

    protected Vector3 wayPoint;

    protected Quaternion guadRotation;

    [Header("Drop")]

    public GameObject itemPrefabGO;
    public int itemNum = 1;

    //bool配合动画
    protected bool isWalk;
    protected bool isChase;
    protected bool isFollow;
    protected bool isDead;
    protected bool PlayerDead;

    private void Awake()
    {
        agent = GetComponent<NavMeshAgent>();
        anim = GetComponent<Animator>();
        characterStats = GetComponent<CharacterStats>();
        coll = GetComponent<Collider>();
        
        speed = agent.speed;
        guardPos = transform.position;
        guadRotation = transform.rotation;
        remainLookAtTime = lookAtTime;
        stoppingDis = agent.stoppingDistance;
        PlayerDead = false;
    }

    // Start is called before the first frame update
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
    }

    //void OnEnable()
    //{
    //    GameManager.Instance.AddObserver(this);
    //}

    void OnDisable()
    {
        if (!GameManager.isInitialized)
        {
            return;
        }
        GameManager.Instance.RemoveObserver(this);
    }


    void Update()
    {
        if (characterStats.currentHealth == 0)
        {
            DropItem();
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
    }

    protected void SwitchAnimation()
    {
        anim.SetBool("Walk", isWalk);
        anim.SetBool("Chase", isChase);
        anim.SetBool("Follow", isFollow);
        anim.SetBool("Critical", characterStats.isCritical);
        anim.SetBool("Death", isDead);
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
                    if (remainLookAtTime > 0)
                    {
                        
                        agent.destination = transform.position;
                        remainLookAtTime -= Time.deltaTime;
                    }
                    else if(isGuard)
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
                    agent.stoppingDistance = Mathf.Max(characterStats.attackData.attackRange, characterStats.attackData.skillRange);
                    isFollow = true;
                    agent.isStopped = false;
                    agent.destination = attackTarget.transform.position;
                }

                //在攻击范围内则攻击
                if (TargetInAttackRange() || TargetInSkillRange())
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
                agent.stoppingDistance = stoppingDis;

                break;
            case EnemyStates.DEAD:
                coll.enabled = false;
                //agent.enabled = false;
                agent.radius = 0;
                Destroy(gameObject, 2f);
                break;
        }
    }

    protected void Attack()
    {
        transform.LookAt(attackTarget.transform);
        if (TargetInAttackRange())
        {
            //近身攻击
            anim.SetTrigger("Attack");
        }
        if (TargetInSkillRange())
        {
            //特殊攻击
            anim.SetTrigger("Skill");
        }
    }

    protected bool FoundPlayer()
    {
        var colliders = Physics.OverlapSphere(transform.position, sightRadius);

        foreach (var target in colliders)
        {
            if (target.CompareTag("Player"))
            {
                attackTarget = target.gameObject;
                return true;
            }
        }

        return false;
    }

    protected bool TargetInAttackRange()
    {
        if (attackTarget != null)
        {
            return Vector3.Distance(attackTarget.transform.position, transform.position) <= characterStats.attackData.attackRange;
        }
        else
        {
            return false;
        }

    }

    protected bool TargetInSkillRange()
    {
        if (attackTarget != null)
        {
            return Vector3.Distance(attackTarget.transform.position, transform.position) <= characterStats.attackData.skillRange;
        }
        else
        {
            return false;
        }
    }


    protected void GetNewWayPoint()
    {
        float randomX = guardPos.x + UnityEngine.Random.Range(-patrolRange, patrolRange);
        float randomZ = guardPos.z + UnityEngine.Random.Range(-patrolRange, patrolRange);
        float randomY = transform.position.y;

        Ray ray = new Ray(new Vector3(randomX, 1000, randomZ), Vector3.down);

        RaycastHit hitInfo;
        if (Physics.Raycast(ray, out hitInfo))
        {
            //获得Y值
            switch (hitInfo.collider.gameObject.tag)
            {
                case "Ground":
                    randomY = hitInfo.point.y;
                    break;
                case "Enemy":
                    randomY = hitInfo.point.y - hitInfo.collider.transform.position.y;
                    break;
                case "Player":
                    randomY = hitInfo.point.y - hitInfo.collider.transform.position.y;
                    break;
                default:
                    randomY = transform.position.y;
                    break;
            }
        }

        Vector3 randomPoint = new Vector3(randomX, randomY, randomZ);

        NavMeshHit hit;
        wayPoint = NavMesh.SamplePosition(randomPoint, out hit, patrolRange, 1) ? hit.position : transform.position;

        //Debug.Log(wayPoint);
    }

    private void OnDrawGizmosSelected()
    {
        Gizmos.color = Color.blue;
        Gizmos.DrawWireSphere(transform.position, sightRadius);

    }

    //Animation Event
    protected void Hit()
    {
        if (attackTarget != null && transform.IsFacingTarget(attackTarget.transform))
        {
            var targetStats = attackTarget.GetComponent<CharacterStats>();

            targetStats.TakeDamage(characterStats, targetStats);
            //Debug.Log("敌人击中!");
        }

    }

    public void EndNotify()
    {
        //获胜动画
        //停止移动
        //停止agent
        PlayerDead = true;
        anim.SetBool("Win", true);
        isChase = false;
        isWalk = false;
        attackTarget = null;
    }

    protected void UpdateStats()
    {
        if (!isDead)
        {
            float dt = Time.time - lastOneSecond;
            if (dt >= 1.0f)
            {
                lastOneSecond = Time.time;
                characterStats.currentHealth += Convert.ToInt16(dt * characterStats.currentHealthRecover);
                characterStats.currentMana += Convert.ToInt16(dt * characterStats.currentMana);
            }
            if (characterStats.currentHealth > characterStats.maxHealth)
            {
                characterStats.currentHealth = characterStats.maxHealth;
            }
            if (characterStats.currentMana > characterStats.maxMana)
            {
                characterStats.currentMana = characterStats.maxMana;
            }
        }
    }

    protected void DropItem()
    {
        if (!isDead && itemPrefabGO != null)
        {
            Vector3 deletePrefabGOPo = transform.position + new Vector3(0, GetComponent<Collider>().bounds.center.y - transform.position.y, 0);
            GameObject itemGO= Instantiate(itemPrefabGO, deletePrefabGOPo, itemPrefabGO.transform.rotation);
            itemGO.GetComponent<ItemOnWorld>().ItemHeld = itemNum;
        }
        
    }

}
