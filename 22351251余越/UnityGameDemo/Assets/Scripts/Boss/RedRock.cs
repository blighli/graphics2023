using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RedRock : MonoBehaviour
{
    public bool ifFire = false;
    public bool ifHaveEnergyGO = false;
    public GameObject energyPrefab;
    public GameObject energyGO;
    public GameObject bombPrefab;
    public GameObject bombGO;
    public SkillData rockBombSkillData;

    float existTime = 0;

    void Start()
    {
        
    }

    void Update()
    {
        if (ifFire)
        {
            existTime += Time.deltaTime;

            if (!ifHaveEnergyGO)
            {
                energyGO = Instantiate(energyPrefab, transform.position + new Vector3(0, 1, 0), energyPrefab.transform.rotation);

                ifHaveEnergyGO = true;
            }

            if (existTime >= 1 && bombGO==null)
            {
                Destroy(energyGO);
                for (int i = 0; i < transform.childCount; i++)
                {
                    Destroy(transform.GetChild(i).gameObject);
                }
                bombGO = Instantiate(bombPrefab, transform.position + new Vector3(0, 1, 0), bombPrefab.transform.rotation);
                Collider[] hitColliders = Physics.OverlapSphere(transform.position, 3f);
                for (int i = 0; i < hitColliders.Length; i++)
                {
                    if (hitColliders[i].gameObject.CompareTag("Player"))
                    {
                        var targetStats = hitColliders[i].gameObject.GetComponent<CharacterStats>();

                        targetStats.TakeSkillDamage(rockBombSkillData, targetStats, 1);
                        targetStats.gameObject.GetComponent<Animator>().SetTrigger("Hit");
                    }
                }
            }
            if (existTime >= 3)
            {
                Destroy(bombGO);
                Destroy(gameObject);
            }
        }
    }
}
