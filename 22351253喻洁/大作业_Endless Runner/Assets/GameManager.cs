using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameManager : MonoBehaviour
{
    public static GameManager instance = null;
    public AudioSource BGM;
    public AudioSource GameLose;

    public GameObject GameOverPanel;

    float timer;

    private void Awake()
    {
        instance = this;
    }

    // Start is called before the first frame update
    void Start()
    {
        GameOverPanel.SetActive(false);
    }

    // Update is called once per frame
    void Update()
    {
        /*if (GameLose.isPlaying)
        {
            timer += Time.deltaTime;
            if (timer >= GameLose.clip.length - 0.1f)
            {
                GameLose.Stop();
                GameLose.enabled = false;
            }
        }*/
    }

    public void StartGame()
    {
        BGM.Play();
        PlayerController.instance.StartGame();
        ScoreManager.instance.StartScore();
        //Debug.Log("Start Score");
    }

    public void GameOver()
    {
        ScoreManager.instance.StopScore();
        GameOverPanel.SetActive(true);
        GameLose.Play();
        Camera.main.GetComponent<CameraFollow>().gameOver = true;
    }
}
