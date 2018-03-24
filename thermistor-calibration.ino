#include <Adafruit_ADS1015.h>
#include <max6675.h>

// Pinouts
#define THERMODO 4
#define THERMOCS 5
#define THERMOCLK 6


#define NUMTHERM 2
//#define CHANNELFLIP
#define VCC      4.92     // We trust our instruments enough to tell use the Vo of the linear regulator output to the ADCs
#define ADCMUL   0.186

MAX6675 thermocouple(THERMOCLK, THERMOCS, THERMODO);

Adafruit_ADS1115 ads1015(0x48);
Adafruit_ADS1115 ads1015_2(0x49);
Adafruit_ADS1115 ads1015_3(0x4A);
Adafruit_ADS1115 ads1015_4(0x4B);

float multiplier = .186;
//long RTVals[] = {4,5};
//int ResistanceVals[] = {10030, 10040, 9980, 10050, // This is the list of resistances of R's complementing our thermocouples in a voltage divider circuit
//                          9990, 10010
//                         };
int ResistanceVals[] = {9990, 10030};
void setup(void)
{
  Serial.begin(115200);

  ads1015.begin();//ads setup
#if (NUMTHERM > 2)
  ads1015_2.begin();
#endif
#if (NUMTHERM > 4)
  ads1015_3.begin();
#endif
#if (NUMTHERM > 6)
  ads1015_4.begin();
#endif

}

void loop(void)
{
  //long RTVals[NUMTHERM];
  TakeTemp ();
  delay(200);
}


void GetResistance(float *RT, int n, int R1[], int adcmul) {
  
  long VT[NUMTHERM];
  adcmul = 0.186;
#ifdef CHANNELFLIP
  VT[0] = ads1015.readADC_Differential_0_1()   * -1;
#if (NUMTHERM > 1)
  VT[1] = ads1015.readADC_Differential_2_3()   * -1;
#endif
#if (NUMTHERM > 2)
  VT[2] = ads1015_2.readADC_Differential_2_3() * -1;
#endif
#if (NUMTHERM > 3)
  VT[3] = ads1015_2.readADC_Differential_0_1() * -1;
#endif
#if (NUMTHERM > 4)
  VT[4] = ads1015_3.readADC_Differential_2_3() * -1;
#endif
#if (NUMTHERM > 5)
  VT[5] = ads1015_3.readADC_Differential_0_1() * -1;
#endif
#if (NUMTHERM > 6)
  VT[6] = ads1015_4.readADC_Differential_0_1() * -1;
#endif
#if (NUMTHERM > 7)
  VT[6] = ads1015_4.readADC_Differential_2_3() * -1;
#endif

#else   // no CHANNELFLIP

  VT[0] = ads1015.readADC_Differential_0_1();
#if (NUMTHERM > 1)
  VT[1] = ads1015.readADC_Differential_2_3();
#endif
#if (NUMTHERM > 2)
  VT[2] = ads1015_2.readADC_Differential_2_3();
#endif
#if (NUMTHERM > 3) 
  VT[3] = ads1015_2.readADC_Differential_0_1();
#endif
#if (NUMTHERM > 4)
  VT[4] = ads1015_3.readADC_Differential_2_3();
#endif
#if (NUMTHERM > 5)
  VT[5] = ads1015_3.readADC_Differential_0_1();
#endif
#if (NUMTHERM > 6)
  VT[6] = ads1015_4.readADC_Differential_0_1();
#endif
#if (NUMTHERM > 7)
  VT[6] = ads1015_4.readADC_Differential_2_3();
#endif

#endif // CHANNELFLIP

// Josiah
// Voltage Divider
// Vo = Vcc + R2 / (R1 + R2)
// R2 = - (Vo * R1) / (Vo - Vcc)

  for (int x = 0; x < n; x++) {
    // From Josiah
    float VTf = ((float)VT[x] * ADCMUL) / 1000;
    RT[x] = ( - (VTf* R1[x]) / ( VTf - 4.92 )); // Where VT is the Voltage across the Thermistor
    //RT[x] = (R1[x] * ( (VCC/2) / ( ( (float)VT[x] * ADCMUL ) / 1000  ))); // to resistance
    //RT[x] = R1[x] * ((4.92 / 2) / ((((float)VT[x]) * .186) / 1000));
    //RT[x] = ((float)VT[x] * ADCMUL) / 1000 ;
  }
}

void printArr(float arr[], int n, long t) {
  Serial.print(t); Serial.print(F("\t"));
  for (int x = 0; x < n; x++) {
    Serial.print(arr[x]); 
    Serial.print(F("\t"));
  }
  Serial.print(thermocouple.readFahrenheit());
  Serial.println();
}

void TakeTemp() {
  float RTVals[NUMTHERM];
  GetResistance(RTVals, NUMTHERM, ResistanceVals, multiplier);
  printArr(RTVals, NUMTHERM, millis());
}
