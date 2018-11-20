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
    // Additional information may be needed
    Vector3 Origin;

    List<GameObject> objectTrails;

    Vector3[] anchorPoints;
    public GameObject sceneRoot;
    public GameObject anchorSelector;
    public Transform anchorPointMarker;



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
