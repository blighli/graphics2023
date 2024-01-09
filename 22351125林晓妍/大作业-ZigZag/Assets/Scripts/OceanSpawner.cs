using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class OceanSpawner : MonoBehaviour
{
    public GameObject Ocean;
    public GameObject Fish;
    public GameObject Shark;

    Vector3 lastPos;
    float size;
    public bool gameOver;
    public static OceanSpawner instance;

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
        lastPos = Ocean.transform.position;
        for (int i = 0; i < 20; i++)
        {
            SpawnOcean();
        }
        InvokeRepeating("SpawnOcean", 1f, 0.3f);
    }

    // Update is called once per frame
    void Update()
    {
        if (GameManager.instance.gameOver == true)
        {
            CancelInvoke("SpawnPlatform");//游戏如果结束，就停止循环生成新的平台
        }
    }

    void SpawnX()
    {
        Vector3 pos = new Vector3(lastPos.x+30, lastPos.y, lastPos.z);//调整生成物之间的间隔
        pos.x += size;
        lastPos = pos;
        Quaternion XRotation = Quaternion.Euler(0f, 90f, 0f);
        Instantiate(Ocean, pos, Quaternion.identity);

        int Rand = Random.Range(0, 4);
        if (Rand < 1)//概率非常小的事件
        {
            Instantiate(Shark, new Vector3(pos.x, pos.y, pos.z), Shark.transform.rotation);//这里不用Quaternion.identity是因为Diamond有自己的旋转角度
        }
        if (Rand > 3)
        {
            Instantiate(Fish, new Vector3(pos.x, pos.y, pos.z), Fish.transform.rotation);//这里不用Quaternion.identity是因为Rocks有自己的旋转角度
        }
    }

    void SpawnZ()
    {
        Vector3 pos = new Vector3(lastPos.x, lastPos.y, lastPos.z+210);//调整生成物之间的间隔
        pos.z += size;
        lastPos = pos;
        Instantiate(Ocean, pos, Quaternion.identity);

        int Rand = Random.Range(0, 4);
        if (Rand < 1)//概率非常小的事件
        {
            Instantiate(Shark, new Vector3(pos.x, pos.y + 1, pos.z), Shark.transform.rotation);
        }
        if (Rand > 3)
        {
            Instantiate(Fish, new Vector3(pos.x, pos.y + 1, pos.z), Fish.transform.rotation);//这里不用Quaternion.identity是因为Rocks有自己的旋转角度
        }
    }

    void SpawnOcean()
    {
        if (gameOver)
        {
            return;//停止生成轨道
        }

        int rand = Random.Range(0, 4);
        {
            if (rand < 2)
            {
                SpawnX();//在X方向生成轨道
            }
            else if (rand >= 2)
            {
                SpawnZ();//在Z方向生成轨道
            }
        }

    }

}
