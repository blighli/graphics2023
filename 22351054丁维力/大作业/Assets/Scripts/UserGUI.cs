using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class UserGUI : MonoBehaviour {

    private IUserAction action;
    private GUIStyle rule_style = new GUIStyle();
    private GUIStyle score_style = new GUIStyle();
    private GUIStyle text_style = new GUIStyle();
    private GUIStyle over_style = new GUIStyle();
    private  bool show_rule = false;                         //显示游戏规则
    void init(){
        rule_style.normal.textColor = new Color(1, 0, 0);
        rule_style.fontSize = 16;
        text_style.normal.textColor = new Color(0,0,0, 1);
        text_style.fontSize = 25;
        score_style.normal.textColor = new Color(1,0,1,1);
        score_style.fontSize = 25;
        over_style.normal.textColor = new Color(1, 0, 0);
        over_style.fontSize = 40;
    }
    void Start ()
    {
        action = SSDirector.GetInstance().CurrentScenceController as IUserAction;
        init();
    }

    void Update()
    {
        //获取方向键的偏移量
        float translationX = Input.GetAxis("Horizontal");
        float translationZ = Input.GetAxis("Vertical");
        //移动玩家
        action.MovePlayer(translationX, translationZ);
    }
    private void OnGUI()
    {
        GUI.Label(new Rect(10, 5, 200, 50), "分数:", text_style);
        GUI.Label(new Rect(75, 5, 200, 50), action.GetScore().ToString(), score_style);
        GUI.Label(new Rect(Screen.width/2 - 50, 5, 50, 50), "速度:", text_style);
        GUI.Label(new Rect(Screen.width/2 + 20, 5, 50, 50), action.GetSpeed().ToString(), score_style);
        GUI.Label(new Rect(Screen.width - 220, 5, 50, 50), "剩余水晶数:", text_style);
        GUI.Label(new Rect(Screen.width - 80, 5, 50, 50), action.GetCrystalNumber().ToString(), score_style);
        if(show_rule){
            ShowRule();
            if (GUI.Button(new Rect(Screen.width / 2 - 50, Screen.height / 2 + 100, 100, 50), "游戏开始"))
            {
                show_rule = false;
                action.Restart();
                return;
            }
        }
        else if(action.GetGameover() && action.GetScore() < 30)
        {
            GUI.Label(new Rect(Screen.width / 2 - 75, Screen.height / 2 - 150, 100, 100), "游戏结束", over_style);
            if (GUI.Button(new Rect(Screen.width / 2 - 50, Screen.height / 2 - 50 , 100, 50), "重新开始"))
            {
                action.Restart();
                return;
            }
            if (GUI.Button(new Rect(Screen.width / 2 - 50, Screen.height / 2 + 100, 100, 50), "游戏规则"))
            {
                show_rule = true;
                return;
            }
        }
        else if(action.GetGameover() && action.GetScore() >= 30)
        {
            GUI.Label(new Rect(Screen.width / 2 - 75, Screen.height / 2 - 150, 100, 100), "游戏胜利", over_style);
            if (GUI.Button(new Rect(Screen.width / 2 - 50, Screen.height / 2-50 , 100, 50), "重新开始"))
            {
                action.Restart();
                return;
            }
            if (GUI.Button(new Rect(Screen.width / 2 - 50, Screen.height / 2 + 100, 100, 50), "游戏规则"))
            {
                show_rule = true;
                return;
            }
        }
    }
    private void ShowRule(){
        GUI.Label(new Rect(Screen.width / 2 - 230, Screen.height / 2 - 90, 400, 100), "游戏规则：", rule_style);
        GUI.Label(new Rect(Screen.width / 2 - 230, Screen.height / 2 - 70, 400, 100), "1.使用上下键前后移动，使用左右键转向", rule_style);
        GUI.Label(new Rect(Screen.width / 2 - 230, Screen.height / 2 - 50, 400, 100), "2.玩家每次甩掉一个巡逻兵计一分，每收集一个水晶会随机触发效果", rule_style);
        GUI.Label(new Rect(Screen.width / 2 - 230, Screen.height / 2 - 30, 400, 100), "3.得分水晶随机加1~5分，变速水晶会使玩家加速或减速", rule_style);
        GUI.Label(new Rect(Screen.width / 2 - 230, Screen.height / 2 - 10, 400, 100), "4.玩家分数达到30则游戏胜利，与巡逻兵碰撞游戏结束", rule_style);
    }
}
