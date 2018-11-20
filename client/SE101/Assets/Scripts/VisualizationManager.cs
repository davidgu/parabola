using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class VisualizationManager : MonoBehaviour {

    // Time vector pair
    private class TVPair{
        double time;
        Vector3 vector;

        public TVPair(double time, float x, float y, float z){
            this.time = time;
            this.vector = new Vector3(x, y, z);
        }

        public TVPair(double time, Vector3 vector){
            this.time = time;
            this.vector = vector;
        }

        public double GetTime(){
            return time;
        }

        public Vector3 GetVector3(){
            return vector;
        }
    }

    bool timeFrozen = false;

    // The origin point the server is using
    // Positive Z is forward
    // Additional information may be needed
    Vector3 origin;
    Quaternion originForward;
    Transform originDisplay;

    // Height of the origin in meters, from the ground plane
    const float originHeight = 1.5f;

    // Positions of the ball in time vector pair form
    List<TVPair> tvpairs;
    List<GameObject> objectTrails;

    // Anchor:
    // 0 -> SW
    // 1 -> NW
    // 2 -> NE
    // 3 -> SE
    Vector3[] anchorPoints;

    public GameObject anchorSelector;
    public Transform anchorPointMarker;
    public Transform originPointMarker;
    public Transform objectTrail;

    public Slider timeSlider;
    public Button freezeTimeButton;

	void Start(){
        objectTrails = new List<GameObject>();
        anchorPoints = new Vector3[4];
	}


	// Receive new position data from server and draw trails
	void Update () {
		
	}

    // 4 Anchor points are needed 
    void SetAnchorPoint(int index){
        // Align crosshairs directly above cone, with phone precisely 1 meter above ground.
        // Drop pillar of light into sky
        float xcoord = anchorSelector.transform.position.x;
        float ycoord = anchorSelector.transform.position.y - 1;
        float zcoord = anchorSelector.transform.position.z;
        Vector3 anchor = new Vector3(xcoord, ycoord, zcoord);
        anchorPoints[index] = anchor;
        DisplayAnchorPoint(index);
    }

    void DisplayAnchorPoint(int index){
        float xcoord = anchorPoints[index].x;
        float ycoord = anchorPoints[index].y + 100;
        float zcoord = anchorPoints[index].z;
        Instantiate(anchorPointMarker, new Vector3(xcoord, ycoord, zcoord), Quaternion.identity, null);
    }

    void CalculateOrigin(){
        
    }


    // Cache new data from the server, and hold present timestamp
    // constant to allow for scrubbing through the timeline.
    void FreezeTime(){
        
    }

    // Append cached position data to position list, and reset present
    // timestamp
    void UnfreezeTime(){
        
    }

    // Clear stored object path data, and clear object trails
    void ClearPathData(){
        
    }

    void ToggleTimeFreeze(){
        if(!TimeFrozen){
            FreezeTime();
        }
        else{
            UnfreezeTime();
        }
    }
}
