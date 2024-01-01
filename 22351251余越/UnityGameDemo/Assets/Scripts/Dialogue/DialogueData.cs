using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(fileName ="New Dialogue", menuName = "Dialogues/New Dialogue")]
public class DialogueData : ScriptableObject
{
    public bool ifShown = false;

    [TextArea]
    public List<string> dialogues = new List<string>();



}
