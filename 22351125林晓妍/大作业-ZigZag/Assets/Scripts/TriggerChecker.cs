using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class TriggerChecker : MonoBehaviour
{
    
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {

    }

    void OnTriggerExit(Collider col)
    {
        if(col.gameObject.tag == "Ball")
        {
            Invoke("FallDown", 1f);//延迟唤起该函数，否则小球会和平台一起往下掉
        }
    }

    void FallDown()
    {
        GetComponentInParent<Rigidbody>().useGravity = true;//因为platform是triggerchecker的父级，所以需要用get component in parent来获取platform的属性
        GetComponentInParent<Rigidbody>().isKinematic = false;
        GetComponentInParent<AudioSource>().Play();
        Destroy(transform.parent.gameObject, 2f);//摧毁父级的物体，即平台
    }
}
