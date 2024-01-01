using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

[CustomEditor(typeof(CreateNoise))]
public class CreateNoiseEditor : Editor
{
    public override void OnInspectorGUI()
    {
        CreateNoise temp = (CreateNoise)target;

        if (DrawDefaultInspector())
        {
            temp.CreateNoiseMap();
            temp.CreateNoiseRealMap();
            //temp.CreateNoiseTerrainMap();
        }
        if (GUILayout.Button("Create"))
        {
            temp.CreateNoiseMap();
            temp.CreateNoiseRealMap();
            //temp.CreateNoiseTerrainMap();
        }
    }

}
