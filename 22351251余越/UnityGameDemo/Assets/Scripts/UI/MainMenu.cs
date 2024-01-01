using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.Playables;
using System;
public class MainMenu : MonoBehaviour
{
    Button newGameBtn;

    Button quitBtn;

    PlayableDirector director;

    private void Start()
    {
        try
        {
            newGameBtn = transform.GetChild(1).GetComponent<Button>();
            quitBtn = transform.GetChild(2).GetComponent<Button>();

            newGameBtn.onClick.AddListener(playTimeLine);
            quitBtn.onClick.AddListener(QuitGame);

            director = FindObjectOfType<PlayableDirector>();
            
        }
        catch (Exception)
        {

        }
        director.stopped += NewGame;
    }

    void playTimeLine()
    {
        director.Play();
    }

    void NewGame(PlayableDirector director)
    {
        PlayerPrefs.DeleteAll();
        //转换场景
        SceneController.Instance.TransitionToFirstLevel();
    }

    void QuitGame()
    {
        Application.Quit();
        Debug.Log("退出游戏");
    }
}
