using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;

public class MagicBullet : MonoBehaviour
{
    public GameObject Target;
    public Vector3 targetPo;
    public float speed;
    public float existTime;
    public CharacterStats attackerCharacterStats;
    public CharacterStats definerCharacterStats;

    void Start()
    {
        existTime = 0;
    }

    void Update()
    {
        existTime += Time.deltaTime;
        if (Target)
        {
            targetPo = Target.transform.position;
        }
        else if (existTime>=20.0f)
        {
            Destroy(gameObject);
        }
        Vector3 dirVec = (targetPo - gameObject.transform.position).normalized;
        gameObject.transform.position += dirVec * speed * Time.deltaTime;
        //gameObject.GetComponent<Rigidbody>().AddForce(dirVec * speed * Time.deltaTime, ForceMode.Impulse);
        //Debug.Log((Vector3.Distance(targetPo, gameObject.transform.position)));
        if (Target)
        {
            if (Vector3.Distance(targetPo,gameObject.transform.position)<= 1.5*Mathf.Max(Target.GetComponent<Collider>().bounds.extents.x, Mathf.Max(Target.GetComponent<Collider>().bounds.extents.y, Target.GetComponent<Collider>().bounds.extents.z)))
            {
            
                definerCharacterStats = Target.GetComponent<CharacterStats>();
                definerCharacterStats.TakeDamage(attackerCharacterStats, definerCharacterStats);
                Destroy(gameObject);
            }
        }
    }

    private void OnCollisionEnter(Collision other)
    {
        if (!Target)
        {
            if (other.gameObject.CompareTag("Ground"))
            {
                Destroy(gameObject);
            }
        }
    }


}
