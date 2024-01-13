using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class UIManager : MonoBehaviour
{
    public static UIManager instance;
    public GameObject ZigzagPanel;
    public GameObject gameoverPanel;

    public GameObject TapTextToStart;
    public Text Score;
    public Text RealTimeScore;
    public Text HighScore1;
    public Text HighScore2;


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
        HighScore1.text = "High Score: "+PlayerPrefs.GetInt("highScore").ToString();//因为Score是int属性，但是输出需要String属性，所以要先进行类型转换
        
    }

    public void GameStart()//加上public才能使该函数被其他脚本所用
    {
        //TapTextToStart.SetActive(false);
        TapTextToStart.GetComponent<Animator>().Play("TextGoDown");
        ZigzagPanel.GetComponent<Animator>().Play("PanelGoingUp");
    }

    public void GameOver()
    {
        Score.text = ScoreManager.instance.Score.ToString();
        HighScore2.text = PlayerPrefs.GetInt("highScore").ToString();
        gameoverPanel.SetActive(true);
    }

    // Update is called once per frame
    void Update()
    {
        RealTimeScore.text = "Score:"+ScoreManager.instance.Score;
    }

    public void Reset()
    {
        SceneManager.LoadScene("Level1");
    }
}
