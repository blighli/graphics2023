using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Ditzelgames;

[RequireComponent(typeof(Rigidbody))]
public class WaterFloat : MonoBehaviour
{
    public float AirDrag = 1;
    public float WaterDrag = 10;
    public Transform[] floatPoints;
    public bool affectDirection = true;
    public bool attachToSurface = false;
    //public float floatHeight = 1;
    public float floatForceFactor = 1;

    protected Rigidbody rigidbody;
    public Waves waves;

    protected float waterLine;
    protected Vector3[] waterLinePoints;

    protected Vector3 centerOffset;
    protected Vector3 smoothVectorRotation;
    protected Vector3 targetUp;

    public Vector3 Center { get { return transform.position + centerOffset; } }

    private void Awake()
    {
        rigidbody = GetComponent<Rigidbody>();
        rigidbody.useGravity = false;

        waterLinePoints = new Vector3[floatPoints.Length];
        for (int i = 0; i < floatPoints.Length; i++)
        {
            waterLinePoints[i] = floatPoints[i].position;
        }
        centerOffset = PhysicsHelper.GetCenter(waterLinePoints) - transform.position;
        waves = GameObject.Find("Waves").gameObject.GetComponent<Waves>();

    }


    // Start is called before the first frame update
    void Start()
    {
        
    }

    void FixedUpdate()
    {
        //default water surface
        var newWaterLine = 0f;// = floatHeight;
        var pointUnderWater = false;
        Vector3 floatForce = new Vector3(0, 0, 0);
        //set WaterLinePoints and WaterLine
        for (int i = 0; i < floatPoints.Length; i++)
        {
            //height
            waterLinePoints[i] = floatPoints[i].position;
            waterLinePoints[i].y = waves.GetHeight(floatPoints[i].position);
            newWaterLine += waterLinePoints[i].y / floatPoints.Length;
            if (waterLinePoints[i].y > floatPoints[i].position.y)
            {
                pointUnderWater = true;
                floatForce += new Vector3(0, waterLinePoints[i].y - floatPoints[i].position.y, 0) / floatPoints.Length;
            }
                
        }
        floatForce *= floatForceFactor;
        var waterLineDelta = newWaterLine - waterLine;
        waterLine = newWaterLine;

        //compute up vector
        targetUp = PhysicsHelper.GetNormal(waterLinePoints);

        //gravity
        var gravity = Physics.gravity;
        
        rigidbody.drag = AirDrag;
        if (waterLine > Center.y)
        {
            rigidbody.drag = WaterDrag;
            //under water
            if (attachToSurface)
            {
                //attach to water surface
                rigidbody.position = new Vector3(rigidbody.position.x, waterLine - centerOffset.y, rigidbody.position.z);
            }
            else
            {
                //go up
                gravity = affectDirection ? floatForce + targetUp * Physics.gravity.y : floatForce + Physics.gravity;
                transform.Translate(Vector3.up * waterLineDelta * 0.9f);
            }
        }
       rigidbody.AddForce(gravity * Mathf.Clamp(Mathf.Abs(waterLine - Center.y), 0, 1));

        //rotation
        if (pointUnderWater)
        {
            //attach to water surface
            targetUp = Vector3.SmoothDamp(transform.up, targetUp, ref smoothVectorRotation, 0.2f);
            rigidbody.rotation = Quaternion.FromToRotation(transform.up, targetUp) * rigidbody.rotation;
        }

    }

    //private void OnDrawGizmos()
    //{
    //    Gizmos.color = Color.green;
    //    if (floatPoints == null)
    //        return;

    //    for (int i = 0; i < floatPoints.Length; i++)
    //    {
    //        if (floatPoints[i] == null)
    //            continue;

    //        if (waves != null)
    //        {

    //            //draw cube
    //            Gizmos.color = Color.red;
    //            Gizmos.DrawCube(waterLinePoints[i], Vector3.one * 0.3f);
    //        }

    //        //draw sphere
    //        Gizmos.color = Color.green;
    //        Gizmos.DrawSphere(floatPoints[i].position, 0.1f);

    //    }

    //    //draw center
    //    if (Application.isPlaying)
    //    {
    //        Gizmos.color = Color.red;
    //        Gizmos.DrawCube(new Vector3(Center.x, waterLine, Center.z), Vector3.one * 1f);
    //        Gizmos.DrawRay(new Vector3(Center.x, waterLine, Center.z), targetUp * 1f);
    //    }
    //}
}
