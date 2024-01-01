//using System.Collections;
//using System.Collections.Generic;
using UnityEngine;

public static class Noise
{
    
    public static float[,] CreateNoiseMap(int width, int height, int octave,float lacunarioty, float presistance, int seed)
    {
        float[,] noiseMap = new float[width, height];

        float minNoiseHeight = float.MaxValue;
        float maxNoiseHeight = float.MinValue;

        System.Random rand = new System.Random(seed);
        int offsetX = rand.Next(-10000, 10000);
        int offsetY = rand.Next(-10000, 10000);

        for (int y = 0; y < height; y++)
        {
            for(int x = 0; x < width; x++)
            {
                float frequency = 1;
                float amplitude = 1;
                float noiseHeight = 0;
                float perlinValue = 0;

                for (int i = 0; i < octave; i++)
                {
                    perlinValue = Mathf.PerlinNoise((x + offsetX) * frequency, (y + offsetY) * frequency) * 2 - 1;

                    noiseHeight += perlinValue * amplitude;
                    
                    frequency *= lacunarioty;
                    amplitude *= presistance;
                }

                if (noiseHeight >= maxNoiseHeight)
                {
                    maxNoiseHeight = noiseHeight;

                }

                if (noiseHeight <= minNoiseHeight)
                {
                    minNoiseHeight = noiseHeight;
                }

                noiseMap[x, y] = noiseHeight;
            }
        }

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                noiseMap[x, y] = Mathf.InverseLerp(minNoiseHeight, maxNoiseHeight, noiseMap[x, y]);
                
            }
        }

        return noiseMap;
    }

}
