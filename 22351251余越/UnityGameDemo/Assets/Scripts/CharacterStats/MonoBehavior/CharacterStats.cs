using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CharacterStats : MonoBehaviour
{
    public event Action<int, int> UpdateHealthBarOnAttack;

    public CharacterData_SO templateData;

    public CharacterData_SO characterData;

    public AttackData_SO attackData;

    [HideInInspector]
    public bool isCritical;

    private void Awake()
    {
        if (templateData != null)
        {
            characterData = Instantiate(templateData);
        }
        characterData.currentHealthRecover = characterData.baseHealthRecover;
        characterData.currentManaRecover = characterData.baseManaRecover;
        
    }
    //血量 蓝量 血量回复 蓝量回复 物抗 魔抗 
    #region Read from Data_SO
    public int maxHealth
    {
        get
        {
            if (characterData != null)
            {
                return characterData.maxHealth;
            }
            else
            {
                return 0;
            }
        }
        set
        {
            characterData.maxHealth = value;
        }
    }

    public int currentHealth
    {
        get
        {
            if (characterData != null)
            {
                return characterData.currentHealth;
            }
            else
            {
                return 0;
            }
        }
        set
        {
            characterData.currentHealth = value;
        }
    }

    public int maxMana
    {
        get
        {
            if (characterData != null)
            {
                return characterData.maxMana;
            }
            else
            {
                return 0;
            }
        }
        set
        {
            characterData.maxMana = value;
        }
    }

    public int currentMana
    {
        get
        {
            if (characterData != null)
            {
                return characterData.currentMana;
            }
            else
            {
                return 0;
            }
        }
        set
        {
            characterData.currentMana = value;
        }
    }

    public int baseDefence
    {
        get
        {
            if (characterData != null)
            {
                return characterData.baseDefence;
            }
            else
            {
                return 0;
            }
        }
        set
        {
            characterData.baseDefence = value;
        }
    }

    public int currentDefence
    {
        get
        {
            if (characterData != null)
            {
                return characterData.currentDefence;
            }
            else
            {
                return 0;
            }
        }
        set
        {
            characterData.currentDefence = value;
        }
    }

    public int baseMagicDefence
    {
        get
        {
            if (characterData != null)
            {
                return characterData.baseMagicDefence;
            }
            else
            {
                return 0;
            }
        }
        set
        {
            characterData.baseMagicDefence = value;
        }
    }

    public int currentMagicDefence
    {
        get
        {
            if (characterData != null)
            {
                return characterData.currentMagicDefence;
            }
            else
            {
                return 0;
            }
        }
        set
        {
            characterData.currentMagicDefence = value;
        }
    }

    public float baseHealthRecover
    {
        get
        {
            if (characterData != null)
            {
                return characterData.baseHealthRecover;
            }
            else
            {
                return 0;
            }
        }
        set
        {
            characterData.baseHealthRecover = value;
        }
    }

    public float currentHealthRecover
    {
        get
        {
            if (characterData != null)
            {
                return characterData.currentHealthRecover;
            }
            else
            {
                return 0;
            }
        }
        set
        {
            characterData.currentHealthRecover = value;
        }
    }

    public float baseManaRecover
    {
        get
        {
            if (characterData != null)
            {
                return characterData.baseManaRecover;
            }
            else
            {
                return 0;
            }
        }
        set
        {
            characterData.baseManaRecover = value;
        }
    }

    public float currentManaRecover
    {
        get
        {
            if (characterData != null)
            {
                return characterData.currentManaRecover;
            }
            else
            {
                return 0;
            }
        }
        set
        {
            characterData.currentManaRecover = value;
        }
    }

    public bool ifDizzy
    {
        get
        {
            if (characterData != null)
            {
                return characterData.ifDizzy;
            }
            else
            {
                return true;
            }
        }
        set
        {
            characterData.ifDizzy = value;
        }
    }

    #endregion

    //攻击和技能伤害计算
    #region Character Combat

    public void TakeDamage(CharacterStats attacker, CharacterStats defener)
    {
        int damage = Mathf.Max(Convert.ToInt16(attacker.CurrentDamage() * getDefencePercent(defener)), 0);
        defener.currentHealth = Mathf.Max(currentHealth - damage, 0);

        if (attacker.isCritical && defener.ifDizzy)
        {
            defener.GetComponent<Animator>().SetTrigger("Hit");
        }

        try
        {
            UpdateHealthBarOnAttack?.Invoke(currentHealth, maxHealth);
        }
        catch (Exception e)
        {
            Debug.Log(e);
        }

        //经验
        if (currentHealth <= 0)
        {
            for (int i = 0; i < GameManager.Instance.playerStats.Length; i++)
            {
                GameManager.Instance.playerStats[i].characterData.UpdateExp(characterData.killPoint);

            }
        }
    }

    public void TakeDamage(int damage, CharacterStats defener)
    {
        int currentDamage = Mathf.Max(Convert.ToInt16(damage * getDefencePercent(defener)), 0);
        defener.currentHealth = Mathf.Max(currentHealth - currentDamage, 0);
        try
        {
            UpdateHealthBarOnAttack?.Invoke(currentHealth, maxHealth);
        }
        catch(Exception e)
        {
            Debug.Log(e);

        }

        if (currentHealth <= 0)
        {
            for (int i = 0; i < GameManager.Instance.playerStats.Length; i++)
            {
                GameManager.Instance.playerStats[i].characterData.UpdateExp(characterData.killPoint);
            }
            
        }
    }

    private int CurrentDamage()
    {
        float coreDamage = UnityEngine.Random.Range(attackData.minDamage, attackData.maxDamage);

        if (isCritical)
        {
            coreDamage *= attackData.criticalMultiplier;
            //Debug.Log("暴击" + coreDamage);
        }

        return (int)coreDamage;
    }

    public void TakeSkillDamage(CharacterStats attacker, SkillData skillData, CharacterStats defener)
    {
        int damage = skillData.baseDamage//基础伤害
            + Convert.ToInt16(UnityEngine.Random.Range(attacker.attackData.minDamage, attacker.attackData.maxDamage) * skillData.atkFactor//AD加成
            + attacker.attackData.SkillPower * skillData.SPFactor//AP加成
            + attacker.maxHealth * skillData.HPFactor);//HP加成

        if(UnityEngine.Random.value < attacker.attackData.criticalMagicChance)
        {
            damage = Convert.ToInt16(damage * attacker.attackData.criticalMultiplier);
            if (defener.ifDizzy)
            {
                defener.GetComponent<Animator>().SetTrigger("Hit");
            }
        }

        switch (skillData.skillDamageType)
        {
            case SkillDamageType.PHYSICAL:
                damage = Mathf.Max(Convert.ToInt16(damage * getDefencePercent(defener)), 0);
                break;
            case SkillDamageType.MAGIC:
                damage = Mathf.Max(Convert.ToInt16(damage * getMagicDefencePercent(defener)), 0);
                break;
        }

        defener.currentHealth = Mathf.Max(currentHealth - damage, 0);

        UpdateHealthBarOnAttack?.Invoke(currentHealth, maxHealth);
        //经验
        if (currentHealth <= 0)
        {
            for (int i = 0; i < GameManager.Instance.playerStats.Length; i++)
            {
                GameManager.Instance.playerStats[i].characterData.UpdateExp(characterData.killPoint);

            }
        }
    }

    public void TakeSkillDamage(SkillData skillData, CharacterStats defener,float hitTime = 0.2f)
    {
        int damage = (int)(skillData.baseDamage * hitTime);
        //Debug.Log(damage);
        switch (skillData.skillDamageType)
        {
            case SkillDamageType.PHYSICAL:
                damage = Mathf.Max(Convert.ToInt16(damage * getDefencePercent(defener)), 0);
                break;
            case SkillDamageType.MAGIC:
                damage = Mathf.Max(Convert.ToInt16(damage * getMagicDefencePercent(defener)), 0);
                break;
        }

        defener.currentHealth = Mathf.Max(currentHealth - damage, 0);

        UpdateHealthBarOnAttack?.Invoke(currentHealth, maxHealth);
        //经验
        if (currentHealth <= 0)
        {
            for (int i = 0; i < GameManager.Instance.playerStats.Length; i++)
            {
                GameManager.Instance.playerStats[i].characterData.UpdateExp(characterData.killPoint);

            }
        }
    }

    float getDefencePercent(CharacterStats defener)
    {
        return 1 - defener.currentDefence / (defener.currentDefence + 100);
    }

    public float getMagicDefencePercent(CharacterStats defener)
    {
        return 1 - defener.currentMagicDefence / (defener.currentMagicDefence + 100);
    }

    #endregion

    public void updateAttackData()
    {
        int tempminDamage = attackData.baseMinDamage;
        int tempmaxDamage = attackData.baseMaxDamage;
        int tempSkillPower = attackData.baseSkillPower;
        float tempcriticalChance = attackData.baseCriticalChance;
        float tempcriticalMagicChance = attackData.baseCriticalMagicChance;
        for (int i = 0; i < characterData.currentLevel; i++)
        {
            tempminDamage += 3;
            tempmaxDamage += 3;
            tempSkillPower += 5;
        }
        for (int i = 0; i < characterData.ItemEquipped.Count; i++)
        {
            if (characterData.ItemEquipped[i].item)
            {
                tempminDamage = tempminDamage + characterData.ItemEquipped[i].item.atkPlus;
                tempmaxDamage = tempmaxDamage + characterData.ItemEquipped[i].item.atkPlus;
                tempSkillPower = tempSkillPower + characterData.ItemEquipped[i].item.spPlus;
                tempcriticalChance = tempcriticalChance + characterData.ItemEquipped[i].item.criticalChancePlus;
                tempcriticalMagicChance = tempcriticalMagicChance + characterData.ItemEquipped[i].item.criticalMagicChancePlus;
            }
        }
        //Debug.Log(tempmaxDamage);
        attackData.minDamage = tempminDamage;
        attackData.maxDamage = tempmaxDamage;
        attackData.SkillPower = tempSkillPower;
        attackData.criticalChance = tempcriticalChance;
        attackData.criticalMagicChance = tempcriticalMagicChance;
    }

    // Start is called before the first frame update
    void Start()
    {
        characterData.ifLvUp = false;
    }

    // Update is called once per frame
    void Update()
    {
        if (characterData.ifLvUp)
        {
            gameObject.GetComponent<SkillManager>().updateSkillSlots(characterData.currentLevel);
            characterData.ifLvUp = false;
        }
    }

    

}
