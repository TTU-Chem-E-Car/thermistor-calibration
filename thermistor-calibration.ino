#include <Adafruit_ADS1015.h>
#include "max6675.h"
Adafruit_ADS1115 ads1015;
Adafruit_ADS1115 ads1015_2(0x49);
Adafruit_ADS1115 ads1015_3(0x4A);
float   multiplier;
int thermoDO = 4;
int thermoCS = 5;
int thermoCLK = 6;
MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);
int vccPin = 3;
int gndPin = 2;
void setup(void)
{
  Serial.begin(250000);
  ads1015.begin();//ads setup
  ads1015_2.begin();
  ads1015_3.begin();
  pinMode(vccPin, OUTPUT); digitalWrite(vccPin, HIGH);
  pinMode(gndPin, OUTPUT); digitalWrite(gndPin, LOW);
  multiplier = .186;//mV per bit
  delay(1000);
}
void loop(void)
{
  long resistance[6];
  GetResistance(resistance);
  float temps[6];
  GetTemps(temps, resistance);
  if (ShouldRun(temps)) {
    while (true) {
      ;
    }
  }
  delay(1000);
}
void GetTemps(float TempVals[], long Res[]) {
  float c1Arr[8] = { -1.377605523e-02, -0.01371274189, -1.332394385e-02, -1.326305518e-02, -1.350268021e-02, -1.301421426e-02};
  float c2Arr[8] = {2.532018389e-03, 2.521505811e-03, 2.459057419e-03, 2.447190536e-03, 2.488536479e-03, 2.407772424e-03};
  float c3Arr[8] = { -7.936009870e-06, -7.894612939e-06, -7.660449294e-06, -7.609495028e-06, -7.774309414e-06, -7.465155865e-06};
  for (int y = 0; y < 6; y++) {
    TempVals[y] = Res[y];
    TempVals[y] = log(TempVals[y]);
    TempVals[y] = (1.0 / (c1Arr[y] + c2Arr[y] * TempVals[y] + c3Arr[y] * TempVals[y] * TempVals[y] * TempVals[y]));
    TempVals[y] = TempVals[y] - 273.15;
    TempVals[y] = (TempVals[y] * 9.0) / 5.0 + 32.0;
  }
}
void GetResistance(long Vals[]) {
  Vals[0] = ads1015.readADC_Differential_2_3() * -1;//yayyy when we wired it we flipped the connections so the -1
  Vals[1] = ads1015.readADC_Differential_0_1() * -1;
  Vals[2] = ads1015_2.readADC_Differential_2_3() * -1;
  Vals[3] = ads1015_2.readADC_Differential_0_1() * -1;
  Vals[4] = ads1015_3.readADC_Differential_2_3() * -1;
  Vals[5] = ads1015_3.readADC_Differential_0_1() * -1;
  int ResistanceVals[] = {10030, 10040, 9980, 10050,
                          9990, 10010
                         };
  for (int x = 0; x < 6; x++) {
    Vals[x] = ResistanceVals[x] * ((4.92 / 2) / ((((float)Vals[x]) * multiplier) / 1000));// to resistance
  }
}
void printArr(long arr[], long t) {
  Serial.print(t); Serial.print(F(" "));
  for (int x = 0; x < 6; x++) {
    Serial.print(arr[x]); Serial.print(F(" "));
  }
  Serial.print(thermocouple.readFahrenheit());
  Serial.println();
}
void printArr(float arr[], long t) {
  Serial.print(t); Serial.print(F(" "));
  for (int x = 0; x < 6; x++) {
    Serial.print(arr[x], 4); Serial.print(F(" "));
  }
  Serial.print(thermocouple.readFahrenheit());
  Serial.println();
}
boolean ShouldRun(float arr[]) {//input temps and temp limit
  if (true) {//wait time till take measurements from turning on millis() < 120000
    printArr(arr, millis());
    return true;
  }
  return false;
}
void TakeTemp() {
  long t = millis();
  long resistance[6];
  GetResistance(resistance);
  float temps[6];
  GetTemps(temps, resistance);
  printArr(resistance, t);
}
