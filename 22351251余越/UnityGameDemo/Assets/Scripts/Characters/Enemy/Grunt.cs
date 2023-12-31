using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

public class Grunt : EnemyController
{
    [Header("Skill")]
    public float kickForce = 10;

    public void KickOff()
    {
        if (attackTarget != null)
        {
            transform.LookAt(attackTarget.transform.position);

            Vector3 direction = attackTarget.transform.position - transform.position;
            direction.y = 0;
            direction.Normalize();
            //Debug.Log("direction:" + direction);
            attackTarget.GetComponent<NavMeshAgent>().isStopped = true;
            attackTarget.GetComponent<NavMeshAgent>().velocity = direction * kickForce;
            attackTarget.GetComponent<Animator>().SetTrigger("Dizzy");
            //Debug.Log("v:" + attackTarget.GetComponent<NavMeshAgent>().velocity);
        }
    }
}
