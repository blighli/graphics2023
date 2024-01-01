using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Hurler : EnemyController
{
    [Header("Skill")]

    public GameObject rockPrefab;

    public Transform handPos;


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
