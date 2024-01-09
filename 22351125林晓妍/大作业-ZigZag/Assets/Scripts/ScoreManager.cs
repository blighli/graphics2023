using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ScoreManager : MonoBehaviour
{
    public static ScoreManager instance;
    public int Score;

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
        Score = 0;
        PlayerPrefs.SetInt("score", Score);//保存分数到score
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    void IncrementScore()
    {
        Score += 1;
    }

    public void DoubleIncrementScore()
    {
        Score += 2;
    }

    public void StartScore()
    {
        InvokeRepeating("IncrementScore", 0.1f, 0.5f);//将会在incrementScore函数执行后的0.1s内执行，并且每隔0.5s执行
    }

    public void StopScore()
    {
        CancelInvoke("IncrementScore");
        PlayerPrefs.SetInt("score", Score);//存储玩家这一次的游戏值

        if (PlayerPrefs.HasKey("highScore"))//如果PlayerPrefs中存储有玩家的分数数据
        {
            if (Score > PlayerPrefs.GetInt("highScore"))//如果本次的分数高于之前的最高分
            {
                PlayerPrefs.SetInt("highScore", Score);//则将当前的分数存储为最高分
            }
        }
        else
        {
            PlayerPrefs.SetInt("highScore", Score);
        }
    }

    
}

