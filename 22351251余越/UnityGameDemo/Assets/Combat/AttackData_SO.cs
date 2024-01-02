using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(fileName = "New Attack", menuName = "Attack/Attack Data")]
public class AttackData_SO : ScriptableObject
{
    [Header("Player Base Data")]
    //基础数值
    public int baseMinDamage;

    public int baseMaxDamage;

    public int baseSkillPower;

    public float baseCriticalChance;

    public float baseCriticalMagicChance;

    [Header("Physic")]
    //基本攻击距离
    public float attackRange;
    //特殊攻击距离
    public float skillRange;
    //攻击冷却时间(1/攻速)
    public float coolDown;

    //当前数值
    //伤害浮动
    public int minDamage;

    public int maxDamage;
    //暴击伤害
    public float criticalMultiplier;
    //暴击率
    public float criticalChance;

    [Header("Magic")]
    //法强
    public int SkillPower;
    //法术暴击率
    public float criticalMagicChance;
}
