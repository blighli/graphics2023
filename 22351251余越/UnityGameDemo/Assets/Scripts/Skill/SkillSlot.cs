using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class SkillSlot : MonoBehaviour
{
    public SkillData skillData;

    public SkillManager skillManager;
    public GameObject player;
    public GameObject skillPrefab;
    public GameObject skillGO;
    public Vector3 targetPo;

    public float remainingCD = 0;
    public Image CDSlider;
    public virtual void doSkill() { }
}
