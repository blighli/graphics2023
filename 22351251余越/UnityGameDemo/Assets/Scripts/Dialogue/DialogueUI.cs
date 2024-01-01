using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class DialogueUI : MonoBehaviour
{
    //public List<DialogueData> dialogueDatas = new List<DialogueData>();
    //public GameObject dialogueGO;

    //DialogueData thisDialogueData;
    //private int stringIndex;
    //public Text textShow;

    //private void Awake()
    //{
    //    //textShow = transform.GetChild(0).gameObject.GetComponent<Text>();
    //    //dialogueDatas[0].ifShown = false;
    //    //dialogueDatas[1].ifShown = false;
    //}
    private void Update()
    {
        if (gameObject.activeSelf && Input.GetMouseButtonDown(0))
        {
            SceneController.Instance.TransitionToMain();
            GameManager.Instance.destinationTags.Clear();
        }
    }

    //public void startShowDialogue(int i)
    //{
    //    dialogueGO.SetActive(true);
    //    thisDialogueData = dialogueDatas[i];
    //    showOneText(0);
    //    stringIndex = 0;
    //    StartCoroutine("showTexts");

    //}

    //IEnumerator showTexts()
    //{
    //    while(stringIndex < thisDialogueData.dialogues.Count)
    //    {
    //        if (Input.GetKeyDown(KeyCode.O))
    //        {
    //            showOneText(stringIndex);
    //            stringIndex++;
    //        }

    //        yield return null;
    //    }
    //    dialogueGO.SetActive(false);
    //}

    //void showOneText(int i)
    //{
    //    textShow.text = thisDialogueData.dialogues[i];
    //}

}
