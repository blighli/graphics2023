using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class InventoryMove : MonoBehaviour, IDragHandler
{
    RectTransform currentRect;

    public void OnDrag(PointerEventData eventData)
    {
        currentRect.anchoredPosition += eventData.delta;
    }

    private void Awake()
    {
        currentRect = GetComponent<RectTransform>();

    }

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
