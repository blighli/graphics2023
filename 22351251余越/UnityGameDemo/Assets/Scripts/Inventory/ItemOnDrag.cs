using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class ItemOnDrag : MonoBehaviour,IBeginDragHandler,IDragHandler,IEndDragHandler
{
    //private Vector3 _vec3TargetScreenSpace;
    //private Vector3 _vec3TargetWorldSpace;
    //private Transform _trans;
    //private Vector3 _vec3MouseScreenSpace;
    //private Vector3 _vec3Offset;
    //public static GameObject itemOnDrag;

    public Transform originalParent;
    //public GameObject myBag;
    public Inventory myBagData;
    private int currentItemID;
    //public GameObject inventoryManagerGO;
    public InventoryManager inventoryManager;

    public void OnBeginDrag(PointerEventData eventData)
    {
        originalParent = transform.parent;
        currentItemID = originalParent.GetComponent<Slot>().slotID;
        transform.SetParent(transform.parent.parent.parent.parent.parent);
        transform.position = eventData.position;
        GetComponent<CanvasGroup>().blocksRaycasts = false;

        if (originalParent.GetComponent<Slot>().slotItem)
        {
            //Debug.Log("Done");
            inventoryManager.UpdateItemInfo(originalParent.GetComponent<Slot>().slotItem, originalParent.GetComponent<Slot>().slotID);
        }
    }

    public void OnDrag(PointerEventData eventData)
    {
        transform.position = eventData.position;
        //Debug.Log(eventData.pointerCurrentRaycast.gameObject);
    }

    public void OnEndDrag(PointerEventData eventData)
    {
        if (!eventData.pointerCurrentRaycast.gameObject || eventData.pointerCurrentRaycast.gameObject.name != "Item Image")
        {
            //Debug.Log("F");
            transform.SetParent(originalParent);
            transform.position = originalParent.position;
        }
        else
        {
            switch (originalParent.GetComponent<Slot>().slotType)
            {
                case SlotType.BAG:
                    if (eventData.pointerCurrentRaycast.gameObject.GetComponentInParent<Slot>().slotType == SlotType.BAG)
                    {
                        transform.SetParent(eventData.pointerCurrentRaycast.gameObject.transform.parent.parent);
                        transform.position = eventData.pointerCurrentRaycast.gameObject.transform.parent.parent.transform.position;

                        var temp = myBagData.ItemPrefabs[currentItemID];
                        myBagData.ItemPrefabs[currentItemID] = myBagData.ItemPrefabs[eventData.pointerCurrentRaycast.gameObject.GetComponentInParent<Slot>().slotID];
                        myBagData.ItemPrefabs[eventData.pointerCurrentRaycast.gameObject.GetComponentInParent<Slot>().slotID] = temp;

                        eventData.pointerCurrentRaycast.gameObject.transform.parent.position = originalParent.position;
                        eventData.pointerCurrentRaycast.gameObject.transform.parent.SetParent(originalParent);

                        inventoryManager.RefreshInventory();
                        //eventData.pointerCurrentRaycast.gameObject.transform.parent.GetComponent<Slot>().slotID = currentItemID;
                        //originalParent.GetComponent<Slot>().slotID = otherItemID;

                        GetComponent<CanvasGroup>().blocksRaycasts = true;
                        return;
                    }
                    else if (eventData.pointerCurrentRaycast.gameObject.GetComponentInParent<Slot>().slotType == SlotType.EQUIP)
                    {
                        transform.SetParent(originalParent);
                        transform.position = originalParent.position;
                        if (eventData.pointerCurrentRaycast.gameObject.GetComponentInParent<Slot>().itemTypeNeed == inventoryManager.ChosenItem.itemType)
                        {
                            inventoryManager.EquipBtnClick();
                        }
                    }
                    break;
                case SlotType.EQUIP:
                    transform.SetParent(originalParent);
                    transform.position = originalParent.position;
                    switch (eventData.pointerCurrentRaycast.gameObject.GetComponentInParent<Slot>().slotType)
                    {
                        case SlotType.BAG:
                            inventoryManager.EquipBtnClick();
                            inventoryManager.UpdateItemInfo(originalParent.GetComponentInParent<Slot>().slotItem,
                                originalParent.GetComponentInParent<Slot>().slotID);
                            break;
                        case SlotType.EQUIP:
                            
                            break;

                    }
                    break;
            }
            

            
        }
            
        GetComponent<CanvasGroup>().blocksRaycasts = true;
    }
    

    void Start()
    {
        inventoryManager = transform.parent.GetComponent<Slot>().inventoryManager;
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    //Ray ray;
    //RaycastHit hitInfo;

    //private void OnMouseDown()
    //{
    //    itemOnDrag = gameObject;
    //    GetComponent<BoxCollider>().enabled = false;
    //    myBag.GetComponent<BoxCollider>().enabled = false;
    //}

    //IEnumerator OnMouseOver()
    //{
    //    ray = Camera.main.ScreenPointToRay(Input.mousePosition);
    //    if (gameObject == itemOnDrag)
    //    {
    //        //Debug.Log("OnMouseOver");
    //        _vec3TargetScreenSpace = Camera.main.WorldToScreenPoint(_trans.position);
    //        _vec3MouseScreenSpace = new Vector3(Input.mousePosition.x, Input.mousePosition.y, _vec3TargetScreenSpace.z);
    //        _vec3Offset = _trans.position - Camera.main.ScreenToWorldPoint(_vec3MouseScreenSpace);
    //        while (Input.GetMouseButton(0))
    //        {
    //            //Debug.Log("getmousebutton0");
    //            _vec3MouseScreenSpace = new Vector3(Input.mousePosition.x, Input.mousePosition.y, _vec3TargetScreenSpace.z);
    //            _vec3TargetWorldSpace = Camera.main.ScreenToWorldPoint(_vec3MouseScreenSpace) + _vec3Offset;
    //            _trans.position = _vec3TargetWorldSpace;
    //            if (Physics.Raycast(ray, out hitInfo))
    //            {
    //                Debug.Log(hitInfo.collider.gameObject);
    //            }

    //            yield return new WaitForFixedUpdate();
    //        }
    //    }

    //}

    //private void OnMouseUp()
    //{
    //    itemOnDrag = null;
    //    GetComponent<BoxCollider>().enabled = true;
    //    myBag.GetComponent<BoxCollider>().enabled = true;
    //}
}
