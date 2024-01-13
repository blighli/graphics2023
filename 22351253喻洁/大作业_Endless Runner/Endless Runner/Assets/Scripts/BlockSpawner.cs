using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BlockSpawner : MonoBehaviour
{
    public GameObject[] platform;//Set up an array to store the different platform
    public GameObject oneblockplatform;
    public Transform Player;

    float spawnPointer = 0f;
    int sizeofPlatform;
    Vector3 lastPos;

    // Start is called before the first frame update
    void Start()
    {
        oneblockplatform = platform[Random.Range(0, platform.Length)];
        lastPos = oneblockplatform.transform.position;
        sizeofPlatform = oneblockplatform.transform.childCount * 20;
        FirstSpawnPlatform();
    }

    // Update is called once per frame
    void Update()
    {
        oneblockplatform = platform[Random.Range(0, platform.Length)];
        float humanDistance = Player.position.z;
        //Debug.Log("HumanDistance = " + humanDistance);
        //Debug.Log("SpawnPointer = " + spawnPointer);

        if(humanDistance > spawnPointer)
        {
            Vector3 newPos = lastPos;
            newPos.z += sizeofPlatform;
            spawnPointer += sizeofPlatform;
            lastPos = newPos;
            Instantiate(oneblockplatform, newPos, Quaternion.identity);
            sizeofPlatform = oneblockplatform.transform.childCount * 20;
        }
    }

    void FirstSpawnPlatform()
    {
        Vector3 newPos = lastPos;
        newPos.z += sizeofPlatform;
        lastPos = newPos;
        Instantiate(oneblockplatform, newPos, Quaternion.identity);
    }
}
