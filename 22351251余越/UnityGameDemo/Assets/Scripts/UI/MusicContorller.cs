using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
public class MusicContorller : Singleton<MusicContorller>
{
    public AudioSource audioSource;
    public Slider slider;

    protected override void Awake()
    {
        base.Awake();
        DontDestroyOnLoad(this);
    }

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }
    public void audioChanged()
    {
        audioSource.volume = slider.value;
    }
}
