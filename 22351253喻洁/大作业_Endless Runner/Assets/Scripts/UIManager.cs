using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class UIManager : MonoBehaviour
{
    public static UIManager instance;
    public Text RealTimeScore;
    public Text Score;
    public Text StarNumText;
    public Text HealthNumText;
    public Text HighScoreText;

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
        
    }
    
    public void GameOver()
    {
        Score.text = ScoreManager.instance.Score.ToString();
        HighScoreText.text = PlayerPrefs.GetInt("highScore").ToString();
    }

    // Update is called once per frame
    void Update()
    {
        RealTimeScore.text = "Meter: " + ScoreManager.instance.Score;
        StarNumText.text = "x " + PlayerController.instance.StarNum;
        HealthNumText.text = "x " + PlayerController.instance.PlayerHealth;

    }

    public void Reset()
    {
        SceneManager.LoadScene("Level1");
    }

    public void QuitGame()
    {
        Application.Quit();
    }
}
