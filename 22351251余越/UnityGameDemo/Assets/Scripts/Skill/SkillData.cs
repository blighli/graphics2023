using System.Collections;
using System.Collections.Generic;
using UnityEngine;


//伤害类型
public enum SkillDamageType
{
    PHYSICAL,//物理
    MAGIC//法术
}

//技能释放对象类型
public enum SkillOBJECTType
{
    TEAMMATE,//队友
    ENEMY,//敌人
    NO,//无目标
    GROUND//地面
}

[CreateAssetMenu(fileName = "New Skill", menuName = "Skills/New Skill")]
public class SkillData : ScriptableObject
{
    //蓝耗
    public int mana;
    //射程
    public float skillRange;

    [Header("Damage")]
    //基础伤害
    public int baseDamage;

    //加成系数
    public float HPFactor;//血量
    public float atkFactor;//攻击
    public float SPFactor;//法强

    //伤害类型
    public SkillDamageType skillDamageType;

    public float CD;

    [TextArea]//技能说明
    public string skillInfo;

    [Header("Buff")]
    public int atkPlus;
    public int SPPlus;
    public float atkPlusPercent;
    public float SPPlusPercent;
    public float HPRecoverPlus;
    public float MPRecoverPlus;
    public int defencePlus;
    public int magicDefencePlus;
    public float contiTime;
}
