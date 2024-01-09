using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraFollow : MonoBehaviour
{
    public GameObject Ball;
    Vector3 offset;//代表相机和小球之间的固定距离
    public float lerpRate;
    public bool gameOver;

    // Start is called before the first frame update
    void Start()
    {
        offset = Ball.transform.position - transform.position;//因为这个是相机的脚本，所以transform.position可以直接代表相机的位置
        gameOver = false;
    }

    // Update is called once per frame
    void Update()
    {
        if (!gameOver)
        {
            follow();
        }
    }

    void follow()
    {
        Vector3 pos = transform.position;
        Vector3 targetPos = Ball.transform.position - offset;
        pos = Vector3.Lerp(pos,targetPos,lerpRate*Time.deltaTime);
        transform.position = pos;
    }
}
