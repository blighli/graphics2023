using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Attack : MonoBehaviour
{
    public ParticleSystem KnifeLight;

    public void Show()
    {
        KnifeLight.Play();
    }

}
