using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class PlayerHeathUI : MonoBehaviour
{
    public int playerID;
    Text levelText;
    Image healthSlider;
    Image manaSlider;
    Image expSlider;

    private void Awake()
    {
        //DontDestroyOnLoad(gameObject);
        levelText = transform.GetChild(3).GetComponent<Text>();
        healthSlider = transform.GetChild(0).GetChild(0).GetComponent<Image>();
        manaSlider = transform.GetChild(1).GetChild(0).GetComponent<Image>();
        expSlider = transform.GetChild(2).GetChild(0).GetComponent<Image>();

    }

    private void Update()
    {
        levelText.text = "Level" + GameManager.Instance.playerStats[playerID].characterData.currentLevel.ToString("00");
        UpdateHealth();
        UpdateMana();
        UpdateExp();

    }

    void UpdateHealth()
    {
        float sliderPercent = (float)GameManager.Instance.playerStats[playerID].currentHealth / GameManager.Instance.playerStats[playerID].maxHealth;
        healthSlider.fillAmount = sliderPercent;
    }

    void UpdateExp()
    {
        float sliderPercent = (float)GameManager.Instance.playerStats[playerID].characterData.currentExp / GameManager.Instance.playerStats[playerID].characterData.baseExp;
        expSlider.fillAmount = sliderPercent;
    }

    void UpdateMana()
    {
        float sliderPercent = (float)GameManager.Instance.playerStats[playerID].characterData.currentMana / GameManager.Instance.playerStats[playerID].characterData.maxMana;
        manaSlider.fillAmount = sliderPercent;
    }

}
