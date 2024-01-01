using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SmallRock : Rock
{

    private void FixedUpdate()
    {
        if (rb.velocity.sqrMagnitude < 1f)
        {
            rockStates = Rock.RockStates.HitNothing;
            Instantiate(breakEffect, transform.position, Quaternion.identity);
            Destroy(gameObject);
        }
    }

    public void FlyToTarget()
    {
        if (target == null)
        {
            Destroy(gameObject);
            //target = FindObjectOfType<PlayerController>().gameObject;
        }
        direction = (target.transform.position - transform.position + Vector3.up * 3).normalized;
        rb.AddForce(direction * force, ForceMode.Impulse);
    }

    private void OnCollisionEnter(Collision other)
    {
        //Debug.Log(other);
        if (other.gameObject.CompareTag("Player"))
        {
            other.gameObject.GetComponent<CharacterStats>().TakeDamage(damage, other.gameObject.GetComponent<CharacterStats>());

            rockStates = RockStates.HitNothing;
            Instantiate(breakEffect, transform.position, Quaternion.identity);
            Destroy(gameObject);
        }
    }
}
