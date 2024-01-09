using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class player : MonoBehaviour
{
    public Animator anim;
    private float inputH;
    private float inputV;
    public Rigidbody rbody;
    private bool run;
    private bool jump;
    // Start is called before the first frame update
    void Start()
    {
        anim = GetComponent<Animator>();
        rbody = GetComponent<Rigidbody>();
        run = false;
        jump = false;
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown("1"))
        {
            anim.Play("WAIT01", -1, 0f);
         }
        if (Input.GetKeyDown("2"))
        {
            anim.Play("WAIT02", -1, 0f);
        }
        if (Input.GetKeyDown("3"))
        {
            anim.Play("WAIT03", -1, 0f);
        }
        if (Input.GetKeyDown("4"))
        {
            anim.Play("WAIT04", -1, 0f);
        }
        if (Input.GetMouseButtonDown(0))
        {
            int n = Random.Range(0, 2);
            if (n == 0)
            {
                anim.Play("DAMAGED00",-1,0f);
            }
            else
            {
                anim.Play("DAMAGED01", -1, 0f);
            }
        }
        inputH = Input.GetAxis("Horizontal");
        inputV = Input.GetAxis("Vertical");
        anim.SetFloat("inputH",inputH);
        anim.SetFloat("inputV", inputV);
        anim.SetBool("run",run );
        anim.SetBool("jump", jump);

        /*float moveX = inputH * 2000f * Time.deltaTime;  
        float moveZ = inputV * 5000f * Time.deltaTime;*/
        float moveSpeed = 10f; // 调整移动速度
        float rotateSpeed = 15f; // 调整旋转速度
        float moveX = inputH * moveSpeed * Time.deltaTime;
        float moveZ = inputV * moveSpeed * Time.deltaTime;
        if (moveZ <= 0f)
        {
            moveX = 0f;
        }
        else if(run)
        {
            moveX *= 3f;
            moveZ *= 2f;
        }
        Vector3 movement = new Vector3(moveX, 0f, moveZ);
        rbody.MovePosition(rbody.position + movement * moveSpeed * Time.deltaTime);
        //Vector3 movement = new Vector3(moveX, 0f, moveZ);
        //transform.Translate(movement, Space.World);
        //rbody.velocity = new Vector3(moveX,0f,moveZ);
        if (Input.GetKey(KeyCode.LeftShift))
        {
            run = true;
        }
        else
        {
            run = false;
        }
        if (Input.GetKey(KeyCode.Space))
        {
            jump = true;
        }
        else
        {
            jump = false;
        }

    }
}
