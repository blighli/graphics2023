using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TransitionPoint : MonoBehaviour
{

    public enum TransitionType
    {
        SameScene, DifferentScene
    }
    [Header("Transition Info")]
    public string sceneName;

    public TransitionType transitionType;

    public DestinationTag destinationTag;

    private bool canTrans;
    public bool ifcanTrans;

    private void OnTriggerStay(Collider other)
    {
        if (other.CompareTag("Player"))
        {
            canTrans = true;
        }
    }

    private void OnTriggerExit(Collider other)
    {
        if (other.CompareTag("Player"))
        {
            canTrans = false;
        }
    }

    // Start is called before the first frame update
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.E) && canTrans && ifcanTrans)
        {
            StartCoroutine(startTransition());
        }
    }
    public void readyStartTransition()
    {
        StartCoroutine(startTransition());
    }

    IEnumerator startTransition()
    {
        while (true)
        {
            
            yield return null;
            int rand = Random.Range(1, 6);
            //Debug.Log(rand);
            switch (rand)
            {
                case 1:
                    destinationTag = DestinationTag.EnterFlower;
                    break;
                case 2:
                    destinationTag = DestinationTag.EnterWind;
                    break;
                case 3:
                    destinationTag = DestinationTag.EnterMountain;
                    break;
                case 4:
                    destinationTag = DestinationTag.EnterWater;
                    break;
                case 5:
                    destinationTag = DestinationTag.EnterSnow;
                    break;
                default:
                    break;
            }
            if (GameManager.Instance.destinationTags.Count >= 3)
            {
                destinationTag = DestinationTag.BOSS;
            }
            //Debug.Log(destinationTag);
            if (GameManager.Instance.destinationTags.Count == 1)
            {
                if (Random.Range(0, 1) >= 0.5 && !GameManager.Instance.destinationTags.Contains(DestinationTag.EnterWater))
                {
                    destinationTag = DestinationTag.EnterWater;
                }
                else if(!GameManager.Instance.destinationTags.Contains(DestinationTag.EnterFlower))
                {
                    destinationTag = DestinationTag.EnterFlower;
                }
            }
            //Debug.Log(destinationTag);
            if (!GameManager.Instance.destinationTags.Contains(destinationTag))
            {
                GameManager.Instance.destinationTags.Add(destinationTag);
                break;
            }
            
        }

        //SceneController ´«ËÍ
        SceneController.Instance.TransitionToDestination(this);
    }
}
