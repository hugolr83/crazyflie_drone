#include "sensors_unit.h"

sensorsData_t sensorsData;
point_t initialPos;

void updateSensorsData(){
    estimatorKalmanGetEstimatedPos(&sensorsData.position);

    sensorsData.voltageFiltered = updateBatteryVoltage();
    sensorsData.batteryLevel = getBatteryPercentage();

    sensorsData.range.front = rangeGet(rangeFront) / 1000.0f;
    sensorsData.range.back = rangeGet(rangeBack) / 1000.0f;
    sensorsData.range.left = rangeGet(rangeLeft) / 1000.0f;
    sensorsData.range.right = rangeGet(rangeRight) / 1000.0f;
    
    logVarId_t logid = logGetVarId("stabilizer", "yaw");
    float headingDeg = logGetFloat(logid);
    sensorsData.yaw = headingDeg * (float)M_PI / 180.0f;
}

void storeInitialPos() {
    initialPos.x = sensorsData.position.x;
    initialPos.y = sensorsData.position.y;
    initialPos.z = sensorsData.position.z;
    initialPos.timestamp = sensorsData.position.timestamp;
}