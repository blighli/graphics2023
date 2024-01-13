using UnityEngine;

public class ColorManager : MonoBehaviour
{
    MeshRenderer msRenderer;

    [SerializeField] [Range(0f, 1f)] float lerpTime;

    [SerializeField] Color[] myColors;

    int colorIndex = 0;

    float t = 0f;

    int len;

    // Start is called before the first frame update
    void Start()
    {
        msRenderer = GetComponent<MeshRenderer>();
        len = myColors.Length;
    }

    // Update is called once per frame
    void Update()
    {
        if (ScoreManager.instance.Score > 10) {
            msRenderer.material.color = Color.Lerp(msRenderer.material.color, myColors[colorIndex], lerpTime * Time.deltaTime);

            t = Mathf.Lerp(t, 1f, lerpTime * Time.deltaTime);

            if (t > 0.9f)
            {
                t = 0f;
                colorIndex++;
                colorIndex = (colorIndex >= len) ? 0 : colorIndex;
            }
        }
        
    }
}
