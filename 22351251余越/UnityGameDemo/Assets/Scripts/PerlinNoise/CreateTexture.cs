//using System.Collections;
//using System.Collections.Generic;
using UnityEngine;

public static class CreateTexture
{
    public static Texture2D GetTextureFromNoiseMap(float[,] noiseMap)
    {
        int width = noiseMap.GetLength(0);
        int height = noiseMap.GetLength(1);

        Texture2D texture = new Texture2D(width,height);
        Color[] colors = new Color[width * height];

        for(int y = 0; y < height; y++)
        {
            for(int x = 0; x < width; x++)
            {
                colors[y * width + x] = Color.Lerp(Color.black, Color.white,noiseMap[x,y]);

            }
        }

        texture.SetPixels(colors);
        texture.Apply();

        return texture;

    }

    public static Texture2D GetColorTextureFromNoiseMap(float[,] noiseMap, CreateNoise.ColorStrcut[] regions, float maxHeight) 
    {
        int width = noiseMap.GetLength(0);
        int height = noiseMap.GetLength(1);

        Texture2D texture = new Texture2D(width, height);
        Color[] colors = new Color[width * height];

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                for(int i = 0; i < regions.Length; i++)
                {
                    if (noiseMap[x, y] < regions[i].height)
                    {
                        colors[y * width + x] = regions[i].color;
                        break;
                    }
                }

            }
        }
        //texture.filterMode = FilterMode.Point;
        texture.wrapMode = TextureWrapMode.Clamp;

        texture.SetPixels(colors);
        texture.Apply();
        return texture;

    }

    public static MeshData CreateMeshDataFromNoiseMap(float[,] noiseMap, float maxHeight)
    {
        int width = noiseMap.GetLength(0);
        int height = noiseMap.GetLength(1);

        float topLeftX = width / -2f;
        float topLeftY = height / 2f;

        MeshData meshData = new MeshData(width, height);

        int verticeIndex = 0;

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                meshData.vertices[verticeIndex] = new Vector3(topLeftX + x, noiseMap[x, y] * maxHeight, topLeftY - y);
                meshData.uvs[verticeIndex] = new Vector2(x / (float)width, y / (float)height);
                if (x < width - 1 && y < height - 1)
                {
                    meshData.AddTrangles(verticeIndex, verticeIndex + width + 1, verticeIndex + width);
                    meshData.AddTrangles(verticeIndex + width + 1, verticeIndex, verticeIndex + 1);
                }

                verticeIndex++;
            }
        }
        
        return meshData;

    }

    
}

public class MeshData
{
    public Vector3[] vertices;
    public Vector2[] uvs;
    public int[] trangles;

    public int tranglesIndex = 0;

    public MeshData(int width, int height)
    {
        vertices = new Vector3[width * height];
        uvs = new Vector2[width * height];
        trangles = new int[(width - 1) * (height - 1) * 6];
    }

    public void AddTrangles(int a,int b,int c)
    {
        trangles[tranglesIndex] = a;
        trangles[tranglesIndex + 1] = b;
        trangles[tranglesIndex + 2] = c;

        tranglesIndex += 3;

    }

    public Mesh CreateMesh()
    {
        Mesh mesh = new Mesh();
        mesh.name = "randomMap";
        mesh.vertices = vertices;
        mesh.uv = uvs;
        mesh.triangles = trangles;
        mesh.RecalculateNormals();
        mesh.RecalculateTangents();

        return mesh;

    }

}
