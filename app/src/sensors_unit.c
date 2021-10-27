#include "sensors_unit.h"

sensorsData_t sensorsData;
point_t initialPos;

void updateSensorsData(){
    estimatorKalmanGetEstimatedPos(&sensorsData.position);

    sensorsData.batteryLevel = getBatteryPercentage();

    sensorsData.range.front = rangeGet(rangeFront) / 1000.0f;
    sensorsData.range.back = rangeGet(rangeBack) / 1000.0f;
    sensorsData.range.left = rangeGet(rangeLeft) / 1000.0f;
    sensorsData.range.right = rangeGet(rangeRight) / 1000.0f;
    
    logVarId_t logid = logGetVarId("stabilizer", "yaw");
    float heading_deg = logGetFloat(logid);
    sensorsData.yaw = heading_deg * (float)M_PI / 180.0f;
}

void storeInitialPos() {
    initialPos.x = sensorsData.position.x;
    initialPos.y = sensorsData.position.x;
    initialPos.z = sensorsData.position.x;
    initialPos.timestamp = sensorsData.position.x;
}