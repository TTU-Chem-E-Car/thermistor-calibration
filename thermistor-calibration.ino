#include <Adafruit_ADS1015.h>
#include <max6675.h>

// Pinouts
#define THERMODO 4
#define THERMOCS 5
#define THERMOCLK 6


#define NUMTHERM 6
#define CHANNELFLIP
#define VCC      4.92     // We trust our instruments enough to tell use the Vo of the linear regulator output to the ADCs


MAX6675 thermocouple(THERMOCLK, THERMOCS, THERMODO);

Adafruit_ADS1115 ads1015;
Adafruit_ADS1115 ads1015_2(0x49);
Adafruit_ADS1115 ads1015_3(0x4A);

float multiplier = 0.186;
long resistance[NUMTHERM];
int ResistanceVals[] = {10030, 10040, 9980, 10050, // This is the list of resistances of R's complementing our thermocouples in a voltage divider circuit
                          9990, 10010
                         };
void setup(void)
{
  Serial.begin(115200);

  ads1015.begin();//ads setup
#if NUMTHERM > 2
  ads1015_2.begin();
#endif
#if NUMTHERM > 4
  ads1015_3.begin();
#endif

}

void loop(void)
{
  delay(1000);
  GetResistance(resistance,ResistanceVals,multiplier);    
  for(int x = 0; x < 10; x++){
     TakeTemp();
  }
}

/*
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
*/

void GetResistance(long RTVals[], int R1Vals[], int adcmul) {
#ifdef CHANNELFLIP

  RTVals[0] = ads1015.readADC_Differential_0_1();
#if NUMTHERM > 1
  RTVals[1] = ads1015.readADC_Differential_2_3();
#endif
#if NUMTHERM > 2
  RTVals[2] = ads1015_2.readADC_Differential_2_3();
#endif
#if NUMTHERM > 3  
  RTVals[3] = ads1015_2.readADC_Differential_0_1();
#endif
#if NUMTHERM > 4
  RTVals[4] = ads1015_3.readADC_Differential_2_3();
#endif
#if NUMTHERM > 5  
  RTVals[5] = ads1015_3.readADC_Differential_0_1();
#endif

#else   // CHANNELFLIP
  RTVals[0] = ads1015.readADC_Differential_0_1()   * -1;
#if NUMTHERM > 1
  RTVals[1] = ads1015.readADC_Differential_2_3()   * -1;
#endif
#if NUMTHERM > 2
  RTVals[2] = ads1015_2.readADC_Differential_2_3() * -1;
#endif
#if NUMTHERM > 3  
  RTVals[3] = ads1015_2.readADC_Differential_0_1() * -1;
#endif
#if NUMTHERM > 4
  RTVals[4] = ads1015_3.readADC_Differential_2_3() * -1;
#endif
#if NUMTHERM > 5  
  RTVals[5] = ads1015_3.readADC_Differential_0_1() * -1;
#endif
#endif // CHANNELFLIP

  for (int x = 0; x < NUMTHERM; x++) {
    RTVals[x] = R1Vals[x] * ( (VCC/2) / ( ( ((float)RTVals[x]) * adcmul) / 1000)); // to resistance
  }
}

void printArr(long arr[], long t) {
  Serial.print(t); Serial.print(F(" "));
  for (int x = 0; x < 2; x++) {
    Serial.print(arr[x]); Serial.print(F(" "));
  }
  Serial.print(thermocouple.readFahrenheit());
  Serial.println();
}

void printArr(float arr[], long t) {
  Serial.print(t); Serial.print(F(" "));
  for (int x = 0; x < 2; x++) {
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
  GetResistance(resistance,ResistanceVals,multiplier);
  float temps[NUMTHERM];
  printArr(resistance, t);
}
