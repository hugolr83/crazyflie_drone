#include "battery.h"

const float voltageCapacities[] = {
    3.27F, // 0   %
    3.61F, // 5   %
    3.69F, // 10  %
    3.71F, // 15  %
    3.73F, // 20  %
    3.75F, // 25  %
    3.77F, // 30  %
    3.79F, // 35  %
    3.80F, // 40  %
    3.82F, // 45  %
    3.84F, // 50  %
    3.85F, // 55  %
    3.87F, // 60  %
    3.91F, // 65  %
    3.95F, // 70  %
    3.98F, // 75  %
    4.02F, // 80  %
    4.08F, // 85  %
    4.11F, // 90  %
    4.15F, // 95  %
    4.20F, // 100 %
};


static int fromVoltageToPercentage(double voltage)
{
  int charge = 0;
  const int n = sizeof(voltageCapacities) / sizeof(float);

  if (voltage < (double)voltageCapacities[0])
  {
    return 0;
  }
  if (voltage > (double)voltageCapacities[n-1])
  {
    return 100;
  }
  while (voltage >  (double)voltageCapacities[charge])
  {
    charge ++;
  }

  const int batteryLevelIncrement = 5;
  return charge * batteryLevelIncrement;
}


int getBatteryPercentage(){
  double voltage = pmGetBatteryVoltage();
  return fromVoltageToPercentage(voltage);
}