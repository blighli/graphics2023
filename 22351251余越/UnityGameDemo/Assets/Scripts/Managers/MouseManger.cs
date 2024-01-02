using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.AI;
using System;
using UnityEngine.EventSystems;

// [System.Serializable]
// public class EventVector3 : UnityEvent<Vector3> { }

public class MouseManger : Singleton<MouseManger>
{
    public GameObject player;
    private NavMeshAgent agent;

    public Texture2D point, doorway, attack, arrow, target;

    RaycastHit hitInfo;

    public event Action<Vector3> OnMouseClicked;
    public event Action<GameObject> OnEnemyClicked;
    public event Action<GameObject> OnItemClicked;

    protected override void Awake()
    {
        base.Awake();
        //DontDestroyOnLoad(this);
        if (player != null && player.GetComponent<NavMeshAgent>())
        {
            agent = player.GetComponent<NavMeshAgent>();
        }
    }

    private void Update()
    {
        try
        {
            if (agent == null && GameManager.Instance.playerStats.Length != 0)
            {
                player = GameManager.Instance.playerStats[0].gameObject;
                agent = player.GetComponent<NavMeshAgent>();
                //Debug.Log("Done");
                //player.GetComponent<PlayerController>().startFun();
            }
        }
        catch (Exception)
        {

        }
        SetCursorTexture();
        MouseControl();
    }

    void SetCursorTexture()
    {
        Ray ray = Camera.main.ScreenPointToRay(Input.mousePosition);

        if (Physics.Raycast(ray, out hitInfo))
        {
            // 切换鼠标贴图
            switch (hitInfo.collider.gameObject.tag)
            {
                case "Ground":
                    Cursor.SetCursor(arrow, new Vector2(16, 16), CursorMode.Auto);
                    break;
                case "Enemy":
                    Cursor.SetCursor(attack, new Vector2(16, 16), CursorMode.Auto);
                    break;
                case "Attackable":
                    Cursor.SetCursor(attack, new Vector2(16, 16), CursorMode.Auto);
                    break;
                case "Portal":
                    if (player.GetComponent<PlayerController>().attackType == AttackType.NEAR)
                    {
                        Cursor.SetCursor(doorway, new Vector2(16, 16), CursorMode.Auto);
                    }
                    else
                    {
                        Cursor.SetCursor(arrow, new Vector2(16, 16), CursorMode.Auto);
                    }
                    break;
                case "Item":
                    Cursor.SetCursor(point, new Vector2(16, 16), CursorMode.Auto);
                    break;
                case "Player":
                    Cursor.SetCursor(point, new Vector2(16, 16), CursorMode.Auto);
                    break;
                default:
                    Cursor.SetCursor(arrow, new Vector2(16, 16), CursorMode.Auto);
                    break;
            }
        }
    }

    void MouseControl()
    {
        if (agent != null && agent.enabled)
        {
            PointerEventData eventData = new PointerEventData(EventSystem.current);
            eventData.position = Input.mousePosition;
            List<RaycastResult> raycastResults = new List<RaycastResult>();
            //向点击位置发射一条射线，检测是否点击UI
            EventSystem.current.RaycastAll(eventData, raycastResults);
            if (raycastResults.Count > 0)
            {
                return;
            }

            if (Input.GetMouseButtonDown(1) && hitInfo.collider != null)
            {
                //Debug.Log(hitInfo.collider.gameObject);
                if (hitInfo.collider.gameObject.CompareTag("Ground"))
                {
                    OnMouseClicked?.Invoke(hitInfo.point);
                }
                if (hitInfo.collider.gameObject.CompareTag("Enemy"))
                {
                    OnEnemyClicked?.Invoke(hitInfo.collider.gameObject);
                }
                if (hitInfo.collider.gameObject.CompareTag("Attackable") && player.GetComponent<PlayerController>().attackType == AttackType.NEAR)
                {
                    OnEnemyClicked?.Invoke(hitInfo.collider.gameObject);
                }
                if (hitInfo.collider.gameObject.CompareTag("Portal"))
                {
                    OnMouseClicked?.Invoke(hitInfo.point);
                }
                if (hitInfo.collider.gameObject.CompareTag("Item"))
                {
                    OnItemClicked?.Invoke(hitInfo.collider.gameObject);
                }
            }
        }

    }
}
