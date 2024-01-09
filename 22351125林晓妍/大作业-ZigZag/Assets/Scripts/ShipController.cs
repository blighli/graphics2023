using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ShipController : MonoBehaviour
{
    private Vector3 targetPoint, startPoint;//The position of the ball and the ship

    public GameObject bullet;
    public Transform firePoint;

    public bool isShooting = false;

    private float cooldown=1f;
    private float cooldownTimer=0;

    // Start is called before the first frame update
    void Start()
    {
        startPoint = transform.position;
    }

    // Update is called once per frame
    void Update()
    {
        targetPoint = BallController.instance.transform.position;
        targetPoint.y = transform.position.y;
        cooldownTimer -= Time.deltaTime;

        if (cooldownTimer <= 0)
        {
            isShooting = true;
            cooldownTimer = cooldown;
        }

        if (Vector3.Distance(transform.position, targetPoint) < 10f && isShooting)
        {
            firePoint.LookAt(BallController.instance.transform.position + new Vector3(0f, 1.2f, 0f));
            Vector3 targetDir = BallController.instance.transform.position - transform.position;
            float angle = Vector3.SignedAngle(targetDir, transform.forward, Vector3.up);

            if (Mathf.Abs(angle) < 70f)
            {
                Instantiate(bullet, firePoint.position, firePoint.rotation);
                GetComponent<AudioSource>().Play();
                isShooting = false;
            }
            else if (Mathf.Abs(angle) > 70f)
            {
                cooldownTimer = cooldown;
            }
        }
        
    }

}