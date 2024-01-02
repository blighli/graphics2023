using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CreateNoise : MonoBehaviour
{
    public int width;
    public int height;
    //public int Hheight;
    public int octave;
    public float lacunarity;
    public float presistance;
    public float maxHeight;
    public int seed;

    public MeshRenderer meshRenderer;
    public MeshFilter meshFilter;
    public MeshCollider meshCollider;
    public Renderer textureRender;
    public GameObject terrainGameObject;
    //public Terrain terrain;

    public float[,] noiseMap;

    public ColorStrcut[] regions;

    Texture2D texture;



    [System.Serializable]
    public struct ColorStrcut
    {
        public string name;
        public float height;
        public Color color;
    }

    public void CreateNoiseMap()
    {
        noiseMap = Noise.CreateNoiseMap(width, height, octave, lacunarity, presistance, seed);
        //Texture2D texture = CreateTexture.GetTextureFromNoiseMap(noiseMap);
        texture = CreateTexture.GetColorTextureFromNoiseMap(noiseMap, regions, maxHeight);
        textureRender.sharedMaterial.mainTexture = texture;
        textureRender.sharedMaterial.mainTexture.filterMode = FilterMode.Point;
        //textureRender.transform.localScale = new Vector3(width, 1, height);

    }

    public void CreateNoiseRealMap()
    {
        //noiseMap = Noise.CreateNoiseMap(width, height, octave, lacunarity, presistance);
        //Texture2D texture = CreateTexture.GetColorTextureFromNoiseMap(noiseMap, regions, maxHeight);

        MeshData meshData = CreateTexture.CreateMeshDataFromNoiseMap(noiseMap, maxHeight);
        Mesh mesh = meshData.CreateMesh();

        meshFilter.sharedMesh = mesh;
        meshRenderer.sharedMaterial.mainTexture = texture;
        meshRenderer.sharedMaterial.mainTexture.filterMode = FilterMode.Point;
        meshCollider.sharedMesh = mesh;


    }

    public void CreateTrees(int waterIndex, int maxIndex)
    {
        int width = noiseMap.GetLength(0);
        int height = noiseMap.GetLength(1);
        int verticeIndex = 0;

        System.Random rand = new System.Random();

        int p = rand.Next(0, 1000);
        int maxVerticeIndex = 0;
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                for (int i = 0; i < regions.Length; i++)
                {
                    if (noiseMap[x, y] < regions[i].height && i > waterIndex && p >= System.Convert.ToInt32(1000 * (i - waterIndex + 1) / (maxIndex - waterIndex)))
                    {
                        maxVerticeIndex = verticeIndex;
                        GameObject gameObject = new GameObject();
                        gameObject.transform.position = terrainGameObject.transform.TransformPoint(meshFilter.sharedMesh.vertices[verticeIndex]);
                        Debug.Log(gameObject.transform.position);
                        p = rand.Next(0, 10000);
                        break;
                    }
                }
                verticeIndex++;
            }
        }
        Debug.Log(maxVerticeIndex);
    }

    //public void CreateNoiseTerrainMap()
    //{
    //    int width = noiseMap.GetLength(0);
    //    int height = noiseMap.GetLength(1);

    //    float topLeftX = width / -2f;
    //    float topLeftY = height / 2f;

    //    int verticeIndex = 0;

    //    for (int y = 0; y < height; y++)
    //    {
    //        for (int x = 0; x < width; x++)
    //        {
    //            noiseMap[x, y] *= maxHeight;

    //            verticeIndex++;
    //        }
    //    }
    //    terrain.terrainData.SetHeights(0, 0, noiseMap);
    //}

    // Start is called before the first frame update
    void Start()
    {
        CreateNoiseMap();
        CreateNoiseRealMap();
        //CreateTrees(2, 8);
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
