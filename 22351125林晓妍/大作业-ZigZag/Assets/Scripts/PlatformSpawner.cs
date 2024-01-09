using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlatformSpawner : MonoBehaviour
{
    public GameObject platform;
    Vector3 lastPos;
    float size;
    public bool gameOver;
    public static PlatformSpawner instance;
    public GameObject Diamonds;
    public GameObject Rocks;
    public GameObject PotionSmall;
    public GameObject PotionBig;
    public GameObject EnemyShip;

    void Awake()
    {
        if (instance == null)
        {
            instance = this;
        }
    }

    // Start is called before the first frame update
    void Start()
    {
        lastPos = platform.transform.position;
        size = platform.transform.localScale.x;

        for (int i = 0; i < 5; i++)
        {
            SpawnPlatform();
        }

    }

    public void StartSpawningPlatform()
    {
        InvokeRepeating("SpawnPlatform", 0.5f, 0.2f);
    }

    // Update is called once per frame
    void Update()
    {
        if (GameManager.instance.gameOver==true)
        {
            CancelInvoke("SpawnPlatform");//游戏如果结束，就停止循环生成新的平台
        }
        
    }

    void SpawnX()
    {
        Vector3 pos = new Vector3(lastPos.x+2, lastPos.y, lastPos.z);//调整生成物之间的间隔
        pos.x += size;
        lastPos = pos;

        Quaternion XRotation = Quaternion.Euler(0f, 90f, 0f);
        Instantiate(platform, pos, XRotation);

        int Rand = Random.Range(0, 20);
        if (Rand < 1)//概率非常小的事件
        {
            Instantiate(Diamonds, new Vector3(pos.x, pos.y+6, pos.z), Diamonds.transform.rotation);//这里不用Quaternion.identity是因为Diamond有自己的旋转角度
        }
        if (Rand >= 3 && Rand < 5)
        {
            Instantiate(Rocks, new Vector3(pos.x, pos.y+6, pos.z), Rocks.transform.rotation);//这里不用Quaternion.identity是因为Rocks有自己的旋转角度
        }
        if(Rand == 15)
        {
            Instantiate(EnemyShip, new Vector3(pos.x-3.5f, pos.y + 5, pos.z+5), Quaternion.identity);//这里不用Quaternion.identity是因为Rocks有自己的旋转角度
        }
        if (Rand > 18)
        {
            Instantiate(PotionBig, new Vector3(pos.x, pos.y + 6, pos.z), Quaternion.identity);
        }
        
    }

    void SpawnZ()
    {
        Vector3 pos = new Vector3(lastPos.x, lastPos.y, lastPos.z+2);//调整生成物之间的间隔
        pos.z += size;
        lastPos = pos;

        Instantiate(platform, pos, Quaternion.identity);
 
        int Rand = Random.Range(0, 20);
        if (Rand < 1)//概率非常小的事件
        {
            Instantiate(Diamonds, new Vector3(pos.x, pos.y+6, pos.z), Diamonds.transform.rotation);
        }
        if (Rand > 18)
        {
            Instantiate(PotionSmall, new Vector3(pos.x, pos.y + 6, pos.z), Quaternion.identity);
        }
    }

    void SpawnPlatform()
    {
        if (gameOver)
        {
            return;//停止生成轨道
        }

        int rand = Random.Range(0, 6);
        {
            if (rand < 2)
            {
                SpawnX();//在X方向生成轨道
            }
            else if (rand >= 2 && rand < 4)
            {
                SpawnZ();//在Z方向生成轨道
            }
           
        }

    }
}
