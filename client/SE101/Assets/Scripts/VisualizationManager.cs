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
        tvpairs = new List<TVPair>();
        anchorPoints = new Vector3[4];
	}


	// Receive new position data from server and draw trails
	void Update () {
        if(!timeFrozen && tvpairs.Count>0){
            ChangeTrailDisplayRange(tvpairs.Count - 1);
        }
	}

    void GetPositionDataFromServer(){
        
    }

    // 4 Anchor points are needed 
    public void SetAnchorPoint(int index){
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
        // The origin is located at the middle of the anchor points
        // Find the average of all andhor point x, y, z
        float xcoord = 0f;
        float ycoord = 0f;
        float zcoord = 0f;

        Vector3 SWtoNW = anchorPoints[1] - anchorPoints[0];

        originForward = Quaternion.LookRotation(SWtoNW);

        for (int i = 0; i < 4; i++){
            xcoord += anchorPoints[i].x;
            ycoord += anchorPoints[i].y;
            zcoord += anchorPoints[i].z;
        }

        xcoord /= 4;
        ycoord /= 4;
        zcoord /= 4;

        ycoord += originHeight;

        origin = new Vector3(xcoord, ycoord, zcoord);
    }

    // Show a little glowing ball at the origin point
    void DisplayOrigin(){
        // TODO: Origin object should face towards north
        // Calculate this with the direciton of the vector3 between NW and SW
        CalculateOrigin();
        originDisplay = Instantiate(originPointMarker, origin, originForward, null);
    }

    public void ToggleDisplayOrigin(){
        if(originDisplay == null){
            DisplayOrigin();
        }
        else{
            Destroy(originDisplay.gameObject);
        }
    }


    // Cache new data from the server (do not display), and hold present timestamp
    // constant to allow for scrubbing through the timeline.
    void FreezeTime(){
        
    }

    // Append cached position data to position list, and reset present
    // timestamp
    void UnfreezeTime(){
        
    }

    // Clear stored object path data, and clear object trails
    public void ClearPathData(){
        foreach(GameObject trailItem in objectTrails){
            Destroy(trailItem);
        }

        tvpairs = new List<TVPair>();
    }

    public void ToggleTimeFreeze(){
        if(!timeFrozen){
            FreezeTime();
            timeFrozen = true;
            freezeTimeButton.gameObject.GetComponent<Image>().color = new Color(0.1475169f, 0.8018868f, 0.2082552f);

            // Enable Time Slider interaction
            timeSlider.interactable = true;
        }
        else{
            UnfreezeTime();
            timeFrozen = false;
            freezeTimeButton.gameObject.GetComponent<Image>().color = new Color(1.0f, 0.2431373f, 0.3294118f);

            // Disable Time Slider interaction and reset position
            timeSlider.interactable = false;
            timeSlider.value = 0f;
        }
    }

    void ShowObjectTrail(int index){
        objectTrails.Add(Instantiate(objectTrail, tvpairs[index].GetVector3() + origin, Quaternion.identity, null).gameObject);
    }

    public void ChangeTrailDisplayRangeSlider(){
        ChangeTrailDisplayRange(timeSlider.value);
    }

    // Display trail data items up to ${deltatime} from the present
    // ${deltatime} should be negative
    void ChangeTrailDisplayRange(float deltatime){
        double presentTime = tvpairs[tvpairs.Count - 1].GetTime();
        for (int i = tvpairs.Count - 1; i >= 0; i--){
            if (tvpairs[i].GetTime() < (presentTime - deltatime)){
                ChangeTrailDisplayRange(i);
                break;
            }
        }
    }

    // Display the first ${index} items from item trail data
    void ChangeTrailDisplayRange(int index){
        // If index is greater than current size+1, create all display objects behind it
        if(index >= objectTrails.Count){
            for (int i = objectTrails.Count; i <= index; i++){
                ShowObjectTrail(i);
            }
        }
        else if(index < objectTrails.Count){
            for (int i = objectTrails.Count - 1; i > index; i--){
                Destroy(objectTrails[i]);
            }
        }
        else{
            throw new System.Exception("THIS SHOULD NEVER HAPPEN!");
        }
    }

    public void LoadRandomTestData(){
        List<TVPair> testData = new List<TVPair>();
        testData.Add(new TVPair(0, new Vector3(0f, 0f, 0f)));
        testData.Add(new TVPair(0.5, new Vector3(0.1f, 0f, 0f)));
        testData.Add(new TVPair(1, new Vector3(0.2f, 0.1f, 0f)));
        testData.Add(new TVPair(2, new Vector3(0.3f, 0.1f, 0.1f)));
        testData.Add(new TVPair(2.3, new Vector3(0.4f, 0.1f, 0.2f)));
        testData.Add(new TVPair(2.33, new Vector3(0.65f, 0.1f, 0.222f)));
        testData.Add(new TVPair(2.333, new Vector3(0.7f, 0.1f, 0.1f)));
        testData.Add(new TVPair(3, new Vector3(0.9f, 0.1f, 0f)));
        testData.Add(new TVPair(4, new Vector3(1.2f, 0.2f, 0f)));

        tvpairs = testData;
    }

    // xWidth is the width between NE and NW pylons
    // zWidth is the width between NE and SE pylons
    // Places markers at specific places to test calibration
    public void LoadCalibrationTestData(){
        
    }
}
