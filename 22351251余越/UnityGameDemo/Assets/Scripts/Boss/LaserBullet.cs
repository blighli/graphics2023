using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LaserBullet : MonoBehaviour
{
    public GameObject player;
    public SkillData laserData;
    public Vector3 destinationPo;
    public float speed;
    public GameObject laserPrefab;
    public List<GameObject> lasers = new List<GameObject>();
    public float laserLen = 8;
    public Vector3 rotatePo1, rotatePo2;
    public float rotateTime = 8;
    public float maxRemainTime = 8;
    float existTime = 0;
    float hitTime = 0;

    void Start()
    {
        
    }

    void Update()
    {
        if (Vector3.Distance(transform.position, destinationPo) > 0.05f)
        {
            Vector3 dir = (destinationPo - transform.position).normalized;
            transform.position += dir * speed * Time.deltaTime;
        }
        else
        {
            if (lasers.Count == 0)
            {
                for (int i = 0; i < 2; i++)
                {
                    CreateLaser();
                    existTime = 0;
                }
            }
            else
            {
                existTime += Time.deltaTime;

                rotatePo1 = destinationPo + new Vector3(laserLen * Mathf.Sin(Mathf.PI * 2 * existTime / rotateTime), 0, laserLen * Mathf.Cos(Mathf.PI * 2 * existTime / rotateTime));
                lasers[0].transform.LookAt(rotatePo1);

                Hovl_Laser LaserScript;

                LaserScript = lasers[0].GetComponent<Hovl_Laser>();
                LaserScript.MaxLength = Vector3.Distance(transform.position, rotatePo1);

                rotatePo2 = destinationPo - new Vector3(laserLen * Mathf.Sin(Mathf.PI * 2 * existTime / rotateTime), 0, laserLen * Mathf.Cos(Mathf.PI * 2 * existTime / rotateTime));
                lasers[1].transform.LookAt(rotatePo2);

                LaserScript = lasers[1].GetComponent<Hovl_Laser>();
                LaserScript.MaxLength = Vector3.Distance(transform.position, rotatePo2);
            }

            hitTime += Time.deltaTime;
            if (hitTime >= 0.2)
            {
                Vector3 p1 = new Vector3(player.transform.position.x, 0, player.transform.position.z);
                Vector3 p2 = new Vector3(rotatePo1.x, 0, rotatePo1.z);
                Vector3 p3 = new Vector3(rotatePo2.x, 0, rotatePo2.z);
                //Debug.Log(DistanceFromPoint2Line(p1, p2, p3));
                if(DistanceFromPoint2Line(p1, p2, p3) <= 1.2f)
                {
                    //Debug.Log("laserHit");
                    var targetStats = player.GetComponent<CharacterStats>();
                    //Debug.Log("FireAreaHit");
                    targetStats.TakeSkillDamage(laserData, targetStats);
                }
                hitTime = 0;
            }
        }


        if (existTime>= maxRemainTime)
        {
            for (int i = 0; i < lasers.Count; i++)
            {
                Destroy(lasers[i]);
            }
            Destroy(gameObject);
        }
    }

    void CreateLaser()
    {
        lasers.Add(Instantiate(laserPrefab, transform.position, transform.rotation));
    }

    private float DistanceFromPoint2Line(Vector3 p, Vector3 p1, Vector3 p2)
    {
        // 求A2B的距离
        float p2pDistance = Vector3.Distance(p2, p);
        Vector3 p2p1 = p2 - p1;
        Vector3 p2p = p2 - p;
        // 求p2p1·p2p
        float dotResult = Vector3.Dot(p2p1, p2p);
        // 求θ
        float seitaRad = Mathf.Acos(dotResult / (p2p1.magnitude * p2pDistance));
        // 求p点到p1p2的距离
        float distance = p2pDistance * Mathf.Sin(seitaRad);
        return distance;
    }

}
