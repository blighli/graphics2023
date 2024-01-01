using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Ditzelgames;

[RequireComponent(typeof(WaterFloat))]
public class WaterBoat : MonoBehaviour
{
    public Transform motor;
    public float steerPower = 500f;
    public float power = 5f;
    public float maxSpeed = 10f;
    public float drag = 0.1f;

    //used Components
    protected Rigidbody rigidbody;
    protected Quaternion startRotation;
    protected ParticleSystem particleSystem;
    //protected Camera camera;

    //internal Properties
    protected Vector3 camVel;


    // Start is called before the first frame update
    void Start()
    {
        particleSystem = GetComponentInChildren<ParticleSystem>();
        rigidbody = GetComponent<Rigidbody>();
        startRotation = motor.localRotation;
        //camera = Camera.main;
    }

    private void FixedUpdate()
    {
        //default direction
        var forceDirection = transform.forward;
        var steer = 0;

        //steer direction [-1,0,1]
        if (Input.GetKey(KeyCode.A))
            steer = 1;
        if (Input.GetKey(KeyCode.D))
            steer = -1;


        //Rotational Force
        rigidbody.AddForceAtPosition(steer * transform.right * steerPower / 100f, motor.position);

        //compute vectors
        var forward = Vector3.Scale(new Vector3(1, 0, 1), transform.forward);
        var targetVel = Vector3.zero;

        //forward/backward poewr
        if (Input.GetKey(KeyCode.W))
            PhysicsHelper.ApplyForceToReachVelocity(rigidbody, forward * maxSpeed, power);
        if (Input.GetKey(KeyCode.S))
            PhysicsHelper.ApplyForceToReachVelocity(rigidbody, forward * -maxSpeed, power);

        //Motor Animation // Particle system
        motor.SetPositionAndRotation(motor.position, transform.rotation * startRotation * Quaternion.Euler(0, 30f * steer, 0));
        if (particleSystem != null)
        {
            if (Input.GetKey(KeyCode.W) || Input.GetKey(KeyCode.S))
                particleSystem.Play();
            else
                particleSystem.Pause();
        }

        //moving forward
        var movingForward = Vector3.Cross(transform.forward, rigidbody.velocity).y < 0;

        //move in direction
        rigidbody.velocity = Quaternion.AngleAxis(Vector3.SignedAngle(rigidbody.velocity, (movingForward ? 1f : 0f) * transform.forward, Vector3.up) * drag, Vector3.up) * rigidbody.velocity;

        //camera position
        //Camera.transform.LookAt(transform.position + transform.forward * 6f + transform.up * 2f);
        //Camera.transform.position = Vector3.SmoothDamp(Camera.transform.position, transform.position + transform.forward * -8f + transform.up * 2f, ref CamVel, 0.05f);
    }
}
