using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameManager : MonoBehaviour
{
    public static GameManager instance;
    public bool gameOver;
    public AudioSource Sea;
    public AudioSource BackGroundMusic;
    public AudioSource LoseGame;

    void Awake()
    {
        if (instance == null)
        {
            instance = this;
        }
    }

    float timer;

    // Start is called before the first frame update
    void Start()
    {
        gameOver = false;
        Sea.Play();
        BackGroundMusic.Play();
    }

    // Update is called once per frame
    void Update()
    {
        if (LoseGame.isPlaying)
        {
            timer += Time.deltaTime;
            if(timer >=LoseGame.clip.length - 1.5f)
            {
                LoseGame.Stop();
                LoseGame.enabled = false;
            }
        }
    }

    public void StartGame()
    {
        UIManager.instance.GameStart();
        ScoreManager.instance.StartScore();
        PlatformSpawner.instance.StartSpawningPlatform();
    }

    public void GameOver()
    {
        BackGroundMusic.Stop();
        LoseGame.Play();
        UIManager.instance.GameOver();
        ScoreManager.instance.StopScore();
        Debug.Log("Game Over");
        gameOver = true;
    }
}
