using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public enum BuffType
{
    FIRE,
    HPRECOVER,
    MPRECOVER,
    MAGICDDEFENCE,
    DEFENCE
}

[System.Serializable]
public struct BuffItem
{
    public int ID;
    public float durTime;
    public float remainingTime;
    public int PlusNum;
}

public class Buff : MonoBehaviour
{
    //public event FinishState OnStateFinished;
    public List<BuffItem> buffItems = new List<BuffItem>();
    public GameObject target { get; set; }
    //public List<float> remainingTime = new List<float>();

    //public BuffType buffType;
    public float PlusNumAll = 0;
    protected GameObject targetBuffPanel;
    public GameObject buffPrefab;
    protected GameObject buffGO;
    public bool ifPercent;

    public Buff Setup(GameObject target, BuffItem buffItem)
    {
        this.target = target;
        bool ifExist = false;
        for (int i = 0; i < buffItems.Count; i++)
        {
            if (buffItems[i].ID == buffItem.ID)
            {
                ifExist = true;
                BuffItem thisbuffItem = buffItems[i];
                thisbuffItem.remainingTime = buffItem.durTime;
                break;
            }
        }
        if (!ifExist)
        {
            buffItems.Add(buffItem);
        }
        return this;
    }

}
