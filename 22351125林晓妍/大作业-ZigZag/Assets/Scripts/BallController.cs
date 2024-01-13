using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BallController : MonoBehaviour
{
    public static BallController instance;

    [SerializeField]
    public float runspeed;

    [SerializeField]
    private float jumpspeed;

    public AudioSource BallBounce;
    public AudioSource FallInWater;
    public AudioSource HitRock;
    public AudioSource PickDiamond;
    public AudioSource ChangeDirection;
    public AudioSource Potion;

    bool started;//使玩家点击屏幕后再开始游戏
    bool gameOver;//判断小球是否掉下平台
    public bool isGround;
    Rigidbody rb;
    public GameObject Particles;//小球吃掉钻石后的特效
    public GameObject WaterSpary;
    public GameObject AddScore;
    public GameObject SpeedUp;
    public GameObject SpeedDown;

    public void Awake()
    {
        if (instance == null)
        {
            instance = this;
        }
    }

    // Start is called before the first frame update
    void Start()
    {
        started = false;
        gameOver = false;
        rb = GetComponent<Rigidbody>();
    }

    // Update is called once per frame
    void Update()
    {
        if (!started)
        {
            if (Input.GetMouseButtonDown(0))//如果用户点击鼠标
            {
                rb.velocity = new Vector3(runspeed, 0, 0);
                started = true;
                GameManager.instance.StartGame();
            }
        }

        if (Input.GetKeyDown(KeyCode.Space) && isGround)
        {
            Vector3 jump = new Vector3(0f, 5.0f, 0f);
            rb.AddForce(jump * jumpspeed * Time.deltaTime);
            BallBounce.Play();
        }

        isGround = false;

        if (!Physics.Raycast(transform.position, Vector3.down, 6f))//Raycast相当于一条射线，这里的属性为向下且长度为6，当射线没有接触到物体的时候说明小球掉下平台了，于是游戏结束
        {
            gameOver = true;
            rb.velocity = new Vector3(0f, -25f,0f);

            Camera.main.GetComponent<CameraFollow>().gameOver = true;//关联到相机脚本中的gameOver参数，使其变为true
            PlatformSpawner.instance.gameOver = true;

            GameManager.instance.GameOver();
        }

        if (Input.GetMouseButtonDown(0) && !gameOver)
        {
            ChangeDirection.Play();
            SwitchDirection();
        }
    }

    private void OnCollisionStay()
    {
        isGround = true;
    }

    void SwitchDirection()//每次点击使得小球朝不同的方向移动
    {
        if (rb.velocity.z > 0)
        {
            rb.velocity = new Vector3(runspeed, 0, 0);
            
        }
        else if (rb.velocity.x > 0)
        {
            rb.velocity = new Vector3(0, 0, runspeed);
        }
    }

    void OnTriggerEnter(Collider other)
    {
        if (other.tag == "Diamond")
        {
            PickDiamond.Play();
            GameObject Part = Instantiate(Particles, other.transform.position, Quaternion.identity);//产生粒子效果
            GameObject Score = Instantiate(AddScore, other.transform.position, Quaternion.identity);
            ScoreManager.instance.DoubleIncrementScore();
            Destroy(other.gameObject);
            Destroy(Part, 1f);
        }

        if(other.tag == "Water")
        {
            FallInWater.Play();
            gameOver = true;
            this.enabled = false;
            Camera.main.GetComponent<CameraFollow>().gameOver = true;//关联到相机脚本中的gameOver参数，使其变为true
            GameObject Spary = Instantiate(WaterSpary, transform.position, Quaternion.identity);//产生水花效果
            PlatformSpawner.instance.gameOver = true;
            Debug.Log("Ball fall into water");
            GameManager.instance.GameOver();
        }

        if(other.tag == "PotionBig")
        {
            Potion.Play();
            runspeed++;
            GameObject Sp = Instantiate(SpeedUp, transform.position, Quaternion.identity);
            Destroy(other.gameObject);
        }

        if(other.tag == "PotionSmall")
        {
            Potion.Play();
            runspeed--;
            GameObject Sd = Instantiate(SpeedDown, transform.position, Quaternion.identity);
            Destroy(other.gameObject);
        }
    }

    void OnCollisionEnter(Collision collision)
    {
        if(collision.collider.tag == "Ground")
        {
            isGround = true;
        }

        if (collision.collider.tag == "Rock")
        {
            HitRock.Play();
            gameOver = true;
            this.enabled = false;
            Camera.main.GetComponent<CameraFollow>().gameOver = true;//关联到相机脚本中的gameOver参数，使其变为true
            PlatformSpawner.instance.gameOver = true;
            Debug.Log("Ball collide rocks");
            GameManager.instance.GameOver();
        }

        if (collision.collider.tag == "Bullet")
        {
            gameOver = true;
            this.enabled = false;
            Camera.main.GetComponent<CameraFollow>().gameOver = true;//关联到相机脚本中的gameOver参数，使其变为true
            PlatformSpawner.instance.gameOver = true;
            Debug.Log("Ball hit by bullets");
            GameManager.instance.GameOver();
        }
    }

    void OnCollisionExit(Collision collision)
    {
        if (collision.collider.tag == "Ground")
        {
            isGround = false;
        }
    }
}
