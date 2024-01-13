using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerController : MonoBehaviour
{
    public static PlayerController instance = null;

    Rigidbody rb;

    Animator anim;

    public Swipe GestureSwipe;

    [SerializeField]
    float speed;//跑步的速度

    [SerializeField]
    float jumpForce;//跳跃的力度

    [SerializeField]
    float sideSpeed;//左右移动的速度

    private float lane;

    public int StarNum;
    public AudioSource Star;

    public int PlayerHealth=3;
    public AudioSource BoyHurt;

    public AudioSource Roll;
    public AudioSource Jump;

    public float blink;
    public float immuned;

    public Renderer ModelRender1;
    public Renderer ModelRender2;
    public Renderer ModelRender3;
    public Renderer ModelRender4;

    private float BlinkTime = 0.1f;
    private float ImmunedTime;

    public GameObject PotionBig;
    public GameObject PotionSmall;
    public AudioSource Potion;

    public GameObject SpeedUpText;
    public GameObject SpeedDownText;

    public GameObject StarEffect;

    private void Awake()
    {
        instance = this;
        rb = GetComponent<Rigidbody>();
        anim = GetComponent<Animator>();
    }

    // Start is called before the first frame update
    void Start()
    {
        anim.SetBool("StartRun", false);
        StarNum = 0;
        lane = 0f;
    }

    public void StartGame()
    {
        anim.SetBool("StartRun", true);
        rb.velocity = Vector3.forward * speed;
    }

    // Update is called once per frame
    void Update()
    {
        if (GestureSwipe.SwipeUp)
        {
            anim.SetTrigger("JumpTrigger");
            rb.AddForce(Vector3.up * jumpForce, ForceMode.Impulse);
            Jump.Play();
        }
        if (GestureSwipe.SwipeLeft)//让角色移动到左边的跑道
        {
            anim.SetTrigger("LeftTrigger");
            if(lane > -1)
            {
                lane -= 3.5f;
            }
        }

        if (GestureSwipe.SwipeRight)//让角色移动到右边的跑道
        {
            anim.SetTrigger("RightTrigger");
            if (lane < 1)
            {
                lane += 3.5f;
            }
        }

        if (GestureSwipe.SwipeDown)//让角色滚动
        {
            anim.SetTrigger("RollTrigger");
            Roll.Play();
        }

        Vector3 newPosition = transform.position;
        newPosition.x = Mathf.MoveTowards(newPosition.x,lane,sideSpeed*Time.deltaTime);
        transform.position = newPosition;

        if (ImmunedTime > 0)
        {
            ImmunedTime -= Time.deltaTime;
            BlinkTime -= Time.deltaTime;

            if (BlinkTime <= 0)//BlinkTime=0.1s
            {
                ModelRender1.enabled = !ModelRender1.enabled;
                ModelRender2.enabled = !ModelRender2.enabled;
                ModelRender3.enabled = !ModelRender3.enabled;
                ModelRender4.enabled = !ModelRender4.enabled;

                BlinkTime = blink;
            }
            if (ImmunedTime <= 0)//ImmunedTime=2s
            {
                ModelRender1.enabled = true;
                ModelRender2.enabled = true;
                ModelRender3.enabled = true;
                ModelRender4.enabled = true;
            }
        }
    }

    public void DamagePlayer()
    {
        if (ImmunedTime <= 0)
        {
            ImmunedTime = immuned;
            ModelRender1.enabled = false;
            ModelRender2.enabled = false;
            ModelRender3.enabled = false;
            ModelRender4.enabled = false;

            BlinkTime = blink;
        }
    }

    private void OnTriggerEnter(Collider other)
    {
        if (other.tag == "Star")
        {
            Star.Play();
            StarNum++;
            GameObject Sf = Instantiate(StarEffect, other.transform.position, Quaternion.identity);
            Destroy(other.gameObject);
        }

        if (other.tag == "PotionBig")
        {
            Potion.Play();
            speed++;
            GameObject Sp = Instantiate(SpeedUpText, transform.position, Quaternion.identity);
            Destroy(other.gameObject);
        }

        if (other.tag == "PotionSmall")
        {
            Potion.Play();
            speed--;
            GameObject Sd = Instantiate(SpeedDownText, transform.position, Quaternion.identity);
            Destroy(other.gameObject);
        }

        if (other.tag == "Obstacle")
        {
            BoyHurt.Play();
            DamagePlayer();
            Destroy(other.gameObject);
            PlayerHealth--;

            if (PlayerHealth == 0)
            {
                rb.velocity = Vector3.zero;
                rb.constraints = RigidbodyConstraints.FreezeAll;
                PlayerHealth = 0;              
                GameObject.Find("BlockSpawner").GetComponent<BlockSpawner>().enabled = false;
                anim.SetBool("GameOver", true);
                transform.position = this.transform.position;
                GameManager.instance.GameOver();
                UIManager.instance.GameOver();
                ScoreManager.instance.StopScore();
                Camera.main.GetComponent<CameraFollow>().gameOver = true;
            }
        }
    }

    private void OnTriggerExit(Collider other)
    {
        if (other.gameObject.tag == "Block")
        {
            Destroy(other.gameObject, 1f);
        }
    }
}
