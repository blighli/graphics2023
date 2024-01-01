using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FireArea : MonoBehaviour
{
    public float maxRemainTime = 4;
    public SkillData FireAreaData;
    float existTime = 0;
    float hitTime = 0;

    void Start()
    {
        
    }

    void Update()
    {
        existTime += Time.deltaTime;
        if(existTime>= maxRemainTime)
        {
            Destroy(gameObject);
        }
        
        hitTime += Time.deltaTime;
        if (hitTime >= 0.2)
        {
            Collider[] hitColliders = Physics.OverlapBox(transform.position, new Vector3(3, 5, 3));
            for (int i = 0; i < hitColliders.Length; i++)
            {
                if (hitColliders[i].gameObject.CompareTag("Player"))
                {
                    var targetStats = hitColliders[i].gameObject.GetComponent<CharacterStats>();
                    //Debug.Log("FireAreaHit");
                    targetStats.TakeSkillDamage(FireAreaData, targetStats);
                }
            }
            hitTime = 0;
        }

    }

}
