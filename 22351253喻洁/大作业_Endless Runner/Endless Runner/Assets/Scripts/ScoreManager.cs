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
        PlayerPrefs.SetInt("score", Score);
    }

    // Update is called once per frame
    void Update()
    {

    }

    void IncrementScore()
    {
        Score += 1;
    }

    public void StartScore()
    {
        InvokeRepeating("IncrementScore", 0.1f, 0.5f);
        //Debug.Log(Score);
    }

    public void StopScore()
    {
        CancelInvoke("IncrementScore");
        PlayerPrefs.SetInt("score", Score);

        if (PlayerPrefs.HasKey("highScore"))
        {
            if (Score > PlayerPrefs.GetInt("highScore"))
            {
                PlayerPrefs.SetInt("highScore", Score);
            }
        }
        else
        {
            PlayerPrefs.SetInt("highScore", Score);
        }
        Debug.Log("HighScore: " + PlayerPrefs.GetInt("highScore").ToString());
    }
}
