using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class FishSwim : MonoBehaviour
{
    private Transform son;
    public bool moveToward = true;
    private float swimspeed = 2;
    private float rotatespeed = 3;

    private void Start()
    {
        son = this.transform;
    }

    private void Update()
    {
        Move();
    }
    private void Move()
    {
        if (son.position.z >= 10 && moveToward)
        {
            moveToward = false;
            transform.rotation = Quaternion.Euler(0f, 180f, 0f);//改变鱼游动的角度
        }
        else if (son.position.z <= 0 && !moveToward)
        {
            moveToward = true;
            transform.rotation = Quaternion.Euler(0f, 0f, 0f);//改变鱼游动的角度
        }
        son.position += (moveToward ? Vector3.forward : Vector3.back) * Time.deltaTime * swimspeed;
    }
}
