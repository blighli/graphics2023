using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraFollow : MonoBehaviour
{
    [SerializeField]
    Transform player;

    Vector3 offset;

    [SerializeField]
    float smoothRate;

    public bool gameOver;
    // Start is called before the first frame update
    void Start()
    {
        offset = player.position - transform.position;
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
        Vector3 CameraCurrentPos = transform.position;
        Vector3 CameraNewPos = player.position - offset;
        CameraCurrentPos = Vector3.Lerp(CameraCurrentPos, CameraNewPos, smoothRate);
        transform.position = CameraCurrentPos;
    }
}
