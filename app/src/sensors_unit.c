#include "sensors_unit.h"

sensorsData_t sensorsData;

void updateSensorsData(){
    estimatorKalmanGetEstimatedPos(&sensorsData.position);

    sensorsData.batteryLevel = getBatteryPercentage();

    sensorsData.range.front = rangeGet(rangeFront) / 1000.0f;
    sensorsData.range.back = rangeGet(rangeBack) / 1000.0f;
    sensorsData.range.left = rangeGet(rangeLeft) / 1000.0f;
    sensorsData.range.right = rangeGet(rangeRight) / 1000.0f;


    // // get current height and heading
    // logid = logGetVarId("kalman", "stateZ");
    // height = logGetFloat(logid);
    // logid = (int)logGetVarId("stabilizer", "yaw");
    // float heading_deg = logGetFloat(logid);
    // heading_rad = heading_deg * (float)M_PI / 180.0f;


}