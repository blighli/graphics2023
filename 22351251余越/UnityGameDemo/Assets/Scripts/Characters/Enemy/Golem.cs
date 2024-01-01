using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

public class Golem : EnemyController
{
    [Header("Skill")]

    public float kickForce = 30;

    public GameObject rockPrefab;

    public Transform handPos;

    //Animation Event
    public void KickOff()
    {
        if (attackTarget != null && transform.IsFacingTarget(attackTarget.transform))
        {
            var targetStats = attackTarget.GetComponent<CharacterStats>();

            Vector3 direction = attackTarget.transform.position - transform.position;
            direction.y = 0;
            direction.Normalize();
            //Debug.Log("direction:" + direction);
            attackTarget.GetComponent<NavMeshAgent>().isStopped = true;
            attackTarget.GetComponent<NavMeshAgent>().velocity = direction * kickForce;
            attackTarget.GetComponent<Animator>().SetTrigger("Dizzy");
            targetStats.TakeDamage(characterStats, targetStats);
            //Debug.Log("µÐÈË»÷ÖÐ!");
        }

    }

    //Animation Event
    public void ThrowRock()
    {
        if (attackTarget != null)
        {
            var rock = Instantiate(rockPrefab, handPos.position, Quaternion.identity);
            rock.GetComponent<Rock>().target = attackTarget;
        }
    }



}
