using System.Collections;
using System.Collections.Generic;
using System;
using UnityEngine;
using Cinemachine;
using UnityEngine.SceneManagement;

public class GameManager : Singleton<GameManager>
{
    public CharacterStats[] playerStats;
    public GameObject player;
    public DialogueUI dialogueUI;
    //public GameObject musicPrefab;
    public GameObject transitionPoint;
    public List<DestinationTag> destinationTags = new List<DestinationTag>();

    private CinemachineFreeLook followCamera;
    

    List<IEndGameObserver> endGameObservers = new List<IEndGameObserver>();

    protected override void Awake()
    {
        base.Awake();
        DontDestroyOnLoad(this);
        //Instantiate(musicPrefab);
    }

    public void RigisterPlayer(CharacterStats player, int playerID)
    {
        playerStats[playerID] = player;

        followCamera = FindObjectOfType<CinemachineFreeLook>();

        if (followCamera != null)
        {
            followCamera.Follow = playerStats[playerID].transform.GetChild(2);
            followCamera.LookAt = playerStats[playerID].transform.GetChild(2);
        }
    }

    public void AddObserver(IEndGameObserver observer)
    {
        endGameObservers.Add(observer);
    }

    public void RemoveObserver(IEndGameObserver observer)
    {
        endGameObservers.Remove(observer);
    }

    public void NotifyObservers()
    {
        foreach (var observer in endGameObservers)
        {
            observer.EndNotify();
        }
    }

    public Transform GetEntrance()
    {
        foreach (var item in FindObjectsOfType<TransitionDestination>())
        {
            if (item.destinationTag == DestinationTag.EnterFlower)
                return item.transform;
        }
        return null;
    }

    // Start is called before the first frame update
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.Escape))
        {
            SceneController.Instance.TransitionToMain();
        }
        if (transitionPoint == null)
        {
            try
            {
                transitionPoint = GameObject.Find("FirstTrans").gameObject;
                transitionPoint.GetComponent<TransitionPoint>().readyStartTransition();
            }
            catch (Exception)
            {

            }
            
        }
        if (SceneManager.GetActiveScene().name=="GameScene" && playerStats.Length==0)
        {
            try
            {
                player = GameObject.Find("PlayerGirl").gameObject;
                RigisterPlayer(player.GetComponent<CharacterStats>(), 0);
            }
            catch (Exception)
            {

            }
        }
    }

}
