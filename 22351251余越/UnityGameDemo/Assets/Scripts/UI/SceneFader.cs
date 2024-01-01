using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SceneFader : MonoBehaviour
{
    CanvasGroup canvasGroup;
    public float fadeInDuration;
    public float fadeOutDuration;

    private void Awake()
    {
        canvasGroup = GetComponent<CanvasGroup>();

        DontDestroyOnLoad(gameObject);
    }

    public IEnumerator fadeOutIn()
    {
        yield return fadeOut(fadeOutDuration);
        yield return fadeIn(fadeInDuration);
    }

    public IEnumerator fadeOut(float time)
    {
        while (canvasGroup.alpha<1)
        {
            canvasGroup.alpha += Time.deltaTime / time;
            yield return null;
        }
    }

    public IEnumerator fadeIn(float time)
    {
        while (canvasGroup.alpha > 0)
        {
            canvasGroup.alpha -= Time.deltaTime / time;
            yield return null;
        }
        Destroy(gameObject);
    }
}
