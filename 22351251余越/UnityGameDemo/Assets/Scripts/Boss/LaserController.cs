using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class LaserController : MonoBehaviour
{
    public GameObject LaserPrefab;

    private GameObject LaserGO;
    private Hovl_Laser LaserScript;

    // Start is called before the first frame update
    void Start()
    {
        CreateLaser(new Vector3(0, 10, 0), new Vector3(10, 10, 0));
    }

    // Update is called once per frame
    void Update()
    {
        
    }
    
    public void CreateLaser(Vector3 startPo,Vector3 destination)
    {
        LaserGO = Instantiate(LaserPrefab, startPo, transform.rotation);
        LaserGO.transform.LookAt(destination);

        LaserScript = LaserGO.GetComponent<Hovl_Laser>();
        LaserScript.MaxLength = Vector3.Distance(destination, startPo);
        //RotateToMouseDirection(StartPo, Destination);
    }

    public void StopLaser()
    {
        Destroy(LaserGO);
    }

}
