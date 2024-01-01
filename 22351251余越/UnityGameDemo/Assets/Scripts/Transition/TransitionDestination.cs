using System.Collections;
using System.Collections.Generic;
using UnityEngine;
public enum DestinationTag
{
    BOSS, QUIT, EnterFlower, EnterWind, EnterMountain, EnterWater, EnterSnow, None
}

public class TransitionDestination : MonoBehaviour
{
    public DestinationTag destinationTag;

    // Start is called before the first frame update
    void Start()
    {

    }

    // Update is called once per frame
    void Update()
    {

    }
}
