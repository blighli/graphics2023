using System.Collections;
using System.Collections.Generic;
using System;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.AI;

public class SceneController : Singleton<SceneController>, IEndGameObserver
{
    public GameObject playerPrefab;
    public SceneFader faderPrefab;
    public GameObject player;
    bool ifDeadFinished;
    float waitTime;
    NavMeshAgent playerAgent;

    protected override void Awake()
    {
        base.Awake();
        DontDestroyOnLoad(this);
    }

    public void TransitionToDestination(TransitionPoint transitionPoint)
    {
        switch (transitionPoint.transitionType)
        {
            case TransitionPoint.TransitionType.SameScene:
                StartCoroutine(Transition(SceneManager.GetActiveScene().name, transitionPoint.destinationTag));
                break;
            case TransitionPoint.TransitionType.DifferentScene:
                StartCoroutine(Transition(transitionPoint.sceneName, transitionPoint.destinationTag));
                break;
        }
    }

    IEnumerator Transition(string sceneName, DestinationTag destinationTag)
    {
        for (int i = 0; i < GameManager.Instance.playerStats.Length; i++)
        {
            if (SceneManager.GetActiveScene().name != sceneName)
            {
                yield return SceneManager.LoadSceneAsync(sceneName);
                yield return Instantiate(playerPrefab, GetDestination(destinationTag).transform.position, GetDestination(destinationTag).transform.rotation);
                yield break;
            }
            else
            {
                player = GameManager.Instance.playerStats[i].gameObject;
                playerAgent = player.GetComponent<NavMeshAgent>();
                playerAgent.enabled = false;
                player.transform.SetPositionAndRotation(GetDestination(destinationTag).transform.position, GetDestination(destinationTag).transform.rotation);
                playerAgent.enabled = true;
                yield return null;
            }
        }
    }

    private TransitionDestination GetDestination(DestinationTag destinationTag)
    {
        var entrances = FindObjectsOfType<TransitionDestination>();

        for (int i = 0; i < entrances.Length; i++)
        {
            if (entrances[i].destinationTag == destinationTag)
            {
                return entrances[i];
            }
        }

        return null;
    }

    public void TransitionToFirstLevel()
    {
        GameManager.Instance.destinationTags.Clear();
        StartCoroutine(LoadLevel("GameScene"));
    }

    public void TransitionToMain()
    {
        StartCoroutine(LoadMain());
    }

    IEnumerator LoadLevel(string scene)
    {
        SceneFader sceneFader = Instantiate(faderPrefab);
        if (scene != "")
        {
            yield return StartCoroutine(sceneFader.fadeOut(2.5f));
            yield return SceneManager.LoadSceneAsync(scene);
            //yield return player = Instantiate(playerPrefab, GameManager.Instance.GetEntrance().position, GameManager.Instance.GetEntrance().rotation);
            yield return StartCoroutine(sceneFader.fadeIn(2.5f));
            yield break;
        }
    }

    IEnumerator LoadMain()
    {
        SceneFader sceneFader = Instantiate(faderPrefab);
        yield return StartCoroutine(sceneFader.fadeOut(2.5f));
        yield return SceneManager.LoadSceneAsync("OpenScene");
        yield return StartCoroutine(sceneFader.fadeIn(2.5f));
        yield break;
    }

    // Start is called before the first frame update
    void Start()
    {
        GameManager.Instance.AddObserver(this);
        ifDeadFinished = true;
        waitTime = 0;
    }

    // Update is called once per frame
    void Update()
    {
        try
        {
            if (player == null && GameManager.Instance.playerStats.Length != 0)
            {
                player = GameManager.Instance.playerStats[0].gameObject;
            }
        }
        catch (Exception)
        {

        }
    }

    public void EndNotify()
    {
        if (ifDeadFinished)
        {
            if (waitTime >= 3f)
            {
                ifDeadFinished = false;
                waitTime = 0;
                StartCoroutine(LoadMain());
            }
            else
            {
                waitTime += Time.deltaTime;
            }
               
        }
        
    }
}
