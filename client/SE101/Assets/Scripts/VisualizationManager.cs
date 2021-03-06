﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using System;
using WebSocketSharp;
using SimpleJSON;

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

    // Debug Panel Textboxes
    public Text VisualizedTimeText;
    public Text TVPHTimeText;
    public Text TVPairsSizeText;
    public Text ObjTrailSizeText;
    public Text FrozenHeadTimeText;
    public Text PredictedPos;

    bool timeFrozen = false;

    public GameObject debugPoint;
    public Vector3 debugPointLocation;

    public GameObject deltaCalibrationSphere;

    public Transform predictedLandingMarkerPrefab;
    GameObject predictedLandingMarker;
    Vector3 predictedLandingMarkerLocation;

    public GameObject WorldCoordnateSystem;

    // The origin point the server is using
    // Positive Z is forward
    // Additional information may be needed
    public Vector3 origin;
    Quaternion originForward;
    Transform originDisplay;

    public Vector3 delta;
    public Text deltaDisplayPanel;

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
    GameObject[] anchorPointMarkers;

    public GameObject anchorSelector;
    public Transform anchorPointMarker;
    public Transform originPointMarker;
    public Transform objectTrail;

    public InputField websocketAddress;
    public Text websocketServerStatus;

    public Slider timeSlider;
    public Button freezeTimeButton;

    double visualizedTime = 0;
    double frozenHeadTime = 0;

    const string wsurl = "ws://localhost:8080";

    WebSocket wsconn;


	void Start(){
        objectTrails = new List<GameObject>();
        tvpairs = new List<TVPair>();
        anchorPoints = new Vector3[4];
        anchorPointMarkers = new GameObject[4];

        // Set server default address
        websocketAddress.text = wsurl;

	}


	// Receive new position data from server and draw trails
	void Update () {
        PlaceDebugMarker();
        PlacePredictedLandingMarker();
        UpdateDebugPanel();
        ManageTVPairsSize();
        if(wsconn !=null && wsconn.IsConnected){
            websocketServerStatus.text = "Connection Status: Connected";
            try{
                if(!timeFrozen){
                    GetDataFromServer();
                }
            }
            catch(Exception e){
                
            }
        }
        else{
            websocketServerStatus.text = "Connection Status: Disconnected";
        }
        if (!timeFrozen && tvpairs.Count > 0)
        {
            // Display new data and update current time index
            visualizedTime = tvpairs[tvpairs.Count - 1].GetTime();
            ChangeTrailDisplayRange(tvpairs.Count - 1);
        }
	}

	private void OnDestroy()
	{
        wsconn.Close();
	}

    void ManageTVPairsSize(){
        if(tvpairs.Count > 100){
            int remRange = tvpairs.Count - 100;
            for (int i = 0; i < remRange; i++ ){
                Destroy(objectTrails[i].gameObject);
            }
            objectTrails.RemoveRange(0, remRange);
            tvpairs.RemoveRange(0, remRange);
        }
    }

	public void BuildWebSocketsConnection(){
        // Connect and set up websocket
        wsconn = new WebSocket(websocketAddress.text);
        wsconn.OnOpen += (sender, e) => {
            Debug.Log("WebSocket connection opened!");
        };

        wsconn.OnMessage += (sender, e) => {
            if (e.IsText)
            {
                string text = e.Data;
                string retcode = text.Split('#')[0];

                switch (retcode)
                {
                    case "curpos":
                        var json = JSON.Parse(text.Split('#')[1]);
                        //Debug.Log(text.Split('#')[1]);
                        double time = json["time"];
                        Vector3 pos = new Vector3(json["pos"]["x"].AsFloat,
                                                  json["pos"]["y"].AsFloat,
                                                  json["pos"]["z"].AsFloat);
                        debugPointLocation = pos;
                        //Debug.Log("Set debug point location to "+debugPointLocation.ToString());
                        tvpairs.Add(new TVPair(time, pos));
                        break;
                    case "all":
                        Debug.Log("Received all!");
                        break;
                    case "predictland":
                        var json2 = JSON.Parse(text.Split('#')[1]);
                        Debug.Log(text.Split('#')[1]);
                        Vector3 pos2 = new Vector3(json2["x"].AsFloat,
                                                  json2["y"].AsFloat,
                                                  json2["z"].AsFloat);
                        predictedLandingMarkerLocation = pos2;
                        break;
                    case "predictpath":
                        Debug.Log("Received predictpath!");
                        break;
                    case "error":
                        Debug.Log("Received error!");
                        break;
                    default:
                        Debug.Log("Error: Invalid retcode \""+retcode+"\"from server!");
                        break;
                }
            }
            else
            {
                Debug.Log("Error: Invalid data received from server!");
            }
        };
        wsconn.Connect();
    }

    void GetDataFromServer(){
        wsconn.Send("curpos");

        // Poll for predicted landing position
        wsconn.Send("predictland");
    }

    private void PlaceDebugMarker(){
        Debug.Log("Placed debug marker");
        debugPoint.transform.localPosition = debugPointLocation - delta;
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
        if(anchorPointMarkers[index]!=null){
            Destroy(anchorPointMarkers[index]);
        }
        anchorPointMarkers[index] = Instantiate(anchorPointMarker, new Vector3(xcoord, ycoord, zcoord), Quaternion.identity, null).gameObject;
    }

    public void CalibrateDelta(){
        // The delta calculation sphere is in the same real world position as the cone
        // serverPos - delta = calibpos
        // serverPos - calibPos = delta
        Vector3 calibPos = new Vector3(deltaCalibrationSphere.transform.position.x,
                                       deltaCalibrationSphere.transform.position.y - 0.5f,
                                       deltaCalibrationSphere.transform.position.z);
        Vector3 serverPos = debugPointLocation;
        delta = serverPos - calibPos;
        delta = WorldCoordnateSystem.transform.rotation * delta;
        deltaDisplayPanel.text = "Delta: " + delta.ToString();
    }

    public void CalibrateForwards(){
        
    }

    public void PlacePredictedLandingMarker(){
        if(predictedLandingMarker != null){
            Destroy(predictedLandingMarker);
        }
        predictedLandingMarker = Instantiate(predictedLandingMarkerPrefab, 
                                             predictedLandingMarkerLocation-delta, 
                                             Quaternion.identity, 
                                             WorldCoordnateSystem.transform).gameObject;
        predictedLandingMarker.transform.localPosition = predictedLandingMarkerLocation - delta;
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

        // Rotate world coordinates
        WorldCoordnateSystem.transform.rotation = originForward;
    }

    // Show a little glowing ball at the origin point
    void DisplayOrigin(){
        // TODO: Origin object should face towards north
        // Calculate this with the direciton of the vector3 between NW and SW
        CalculateOrigin();
        originDisplay = Instantiate(originPointMarker, origin, originForward, null);
        for (int i = 0; i < anchorPointMarkers.Length; i++){
            anchorPointMarkers[i].SetActive(true);
        }
    }

    public void ToggleDisplayOrigin(){
        if(originDisplay == null){
            DisplayOrigin();
        }
        else{
            Destroy(originDisplay.gameObject);
            for (int i = 0; i < anchorPointMarkers.Length; i++)
            {
                anchorPointMarkers[i].SetActive(false);
            }
        }
    }


    // Cache new data from the server (do not display), and hold present timestamp
    // constant to allow for scrubbing through the timeline.
    void FreezeTime(){
        timeFrozen = true;

        // Set frozen head time to current head time
        frozenHeadTime = tvpairs[tvpairs.Count - 1].GetTime();
    }

    // Append cached position data to position list, and reset present
    // timestamp
    void UnfreezeTime(){
        timeFrozen = false;
    }

    // Clear stored object path data, and clear object trails
    public void ClearPathData(){
        for (int i = 0; i < objectTrails.Count; i++){
            Destroy(objectTrails[i]);
        }
        objectTrails.Clear();
        tvpairs.Clear();
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
        Transform toAdd = Instantiate(objectTrail, tvpairs[index].GetVector3() -delta, Quaternion.identity, WorldCoordnateSystem.transform);
        toAdd.localPosition = tvpairs[index].GetVector3() - delta;
        objectTrails.Add(toAdd.gameObject);
    }

    public void ChangeTrailDisplayRangeSlider(){
        visualizedTime = frozenHeadTime + timeSlider.value;
        ChangeTrailDisplayRange(visualizedTime);
    }

    // Display trail data items up to ${time}
    // ${deltatime} should be negative
    void ChangeTrailDisplayRange(double time){
        for (int i = tvpairs.Count - 1; i >= 0; i--){
            if (tvpairs[i].GetTime() < (time)){
                Debug.LogError("Displaying up to index: " + i);
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
                objectTrails.RemoveAt(i);
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

    void UpdateDebugPanel(){
        VisualizedTimeText.text = "Visualized Time: " + visualizedTime;
        if(tvpairs.Count>0){
            TVPHTimeText.text = "TVPHTime: " + tvpairs[tvpairs.Count - 1].GetTime();
        }
        else{
            TVPHTimeText.text = "TVPHTime: N/A";
        }
        TVPairsSizeText.text = "TVPairs Size: " + tvpairs.Count;
        ObjTrailSizeText.text = "ObjTrail Size: " + objectTrails.Count;
        if(timeFrozen){
            FrozenHeadTimeText.text = "FrozenHead Time: " + frozenHeadTime;
        }
        else{
            FrozenHeadTimeText.text = "FrozenHead Time: N/A";
        }
        PredictedPos.text = "Predicted Pos: " + predictedLandingMarkerLocation.ToString();
    }
}
