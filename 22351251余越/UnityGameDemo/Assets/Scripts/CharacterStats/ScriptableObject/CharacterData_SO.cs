using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(fileName ="New Data",menuName = "CharacterStats/Data")]
public class CharacterData_SO : ScriptableObject
{
    [Header("Stats Info")]
    public int maxHealth;

    public int currentHealth;

    public float baseHealthRecover;

    public float currentHealthRecover;

    public int maxMana;

    public int currentMana;

    public float baseManaRecover;

    public float currentManaRecover;

    public int baseDefence;

    public int currentDefence;

    public int baseMagicDefence;

    public int currentMagicDefence;

    public bool ifDizzy = true;

    [Header("Kill")]

    public int killPoint;

    [Header("Level")]

    public int currentLevel;

    public int maxLevel;

    public int baseExp;

    public int currentExp;

    public float levelBuff;

    [Header("Inventory")]

    public List<ItemPrefab> ItemEquipped = new List<ItemPrefab>();

    public bool ifLvUp;

    public float LevelMultiplier
    {
        get { return 1 + (currentLevel - 1) * levelBuff; }
    }

    public void UpdateExp(int point)
    {
        currentExp += point;
        if (currentExp >= baseExp)
        {
            LevelUp();
        }
    }

    private void LevelUp()
    {
        ifLvUp = true;

        //Ã·…˝ Ù–‘
        currentLevel = Mathf.Clamp(currentLevel + 1, 0, maxLevel);

        if (currentLevel <= 6)
        {
            baseExp += (int)(baseExp * LevelMultiplier);
            maxHealth = Mathf.Min((int)(maxHealth * LevelMultiplier), maxHealth + 75);
            maxMana = Mathf.Min((int)(maxMana * LevelMultiplier), maxMana + 50);
            baseDefence = Mathf.Min((int)(baseDefence * (1 + (currentLevel - 1) * levelBuff / 2)), baseDefence + 7);
            baseMagicDefence = Mathf.Min((int)(baseMagicDefence * (1 + (currentLevel - 1) * levelBuff / 2)), baseMagicDefence + 7);
        }
        else
        {
            baseExp += baseExp;
            maxHealth += 40;
            maxMana += 30;
            baseDefence += 5;
            baseMagicDefence += 5;
        }

        currentHealth = maxHealth;
        currentMana = maxMana;

    }
}
