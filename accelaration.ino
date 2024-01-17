// Load libraries
#include "BluetoothSerial.h"

// Check if Bluetooth configs are enabled
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Bluetooth Serial object
BluetoothSerial SerialBT;

const int xpin = A0;
const int ypin = A3;
const int zpin = A4;

float threshold = 80;
float xval[100] = {0};
float yval[100] = {0};
float zval[100] = {0};
float xavg, yavg, zavg;
float calories, distance;
int steps, flag = 0;

void setup()
{
  Serial.begin(9600);
  SerialBT.begin("ESP32");
  Serial.println("The device started, now you can pair it with bluetooth!");
  calibrate(); 
}

void loop()
{
  int acc = 0;
  float totvect[100] = {0};
  float totave[100] = {0};
  float xaccl[100] = {0};
  float yaccl[100] = {0};
  float zaccl[100] = {0};
  for (int a = 0; a < 100; a++)
  {
    xaccl[a] = float(analogRead(xpin) - 345);
    delay(1);
    yaccl[a] = float(analogRead(ypin) - 346);
    delay(1);
    zaccl[a] = float(analogRead(zpin) - 416);
    delay(1);
    totvect[a] = sqrt(((xaccl[a] - xavg) * (xaccl[a] - xavg)) + ((yaccl[a] - yavg) * (yaccl[a] - yavg)) + ((zval[a] - zavg) * (zval[a] - zavg)));
    totave[a] = (totvect[a] + totvect[a - 1]) / 2 ;
    Serial.print("x : ");
    Serial.println(analogRead(xpin));
    Serial.print("y : ");
    Serial.println(analogRead(ypin));
    Serial.print("z : ");
    Serial.println(analogRead(zpin));
    Serial.print("totave[a] : ");
    Serial.println(totave[a]);
    delay(100);
    if (totave[a] > threshold && flag == 0)
    {
      steps = steps + 1;
      flag = 1;
    }
    else if (totave[a] > threshold && flag == 1)
    {
      // Don't Count
    }
    if (totave[a] < threshold   && flag == 1)
    {
      flag = 0;
    }
    if (steps < 0) {
      steps = 0;
    }
       
    Serial.print("steps: ");
    Serial.println(steps);
    
    delay(100);

    calories = steps * 0.045;
    distance = steps * 0.6;

    // Send number of steps and burnt calories
    SerialBT.print("totave[a] : ");
    SerialBT.println(totave[a]); 
    SerialBT.print("Steps count: ");
    SerialBT.println(steps);
    SerialBT.print("Burnt Calories: ");
    SerialBT.print(calories);
    SerialBT.println(" cal.");
    SerialBT.print("Distance: ");
    SerialBT.print(distance);
    SerialBT.println(" m.");
    SerialBT.println();
    if (SerialBT.available()) {
      float weight = Serial.write(SerialBT.read());
    }
  
  }
 
  //delay(1000);
}
void calibrate()
{
  float sum = 0;
  float sum1 = 0;
  float sum2 = 0;
  for (int i = 0; i < 100; i++) {
    xval[i] = float(analogRead(xpin) - 345);
    sum = xval[i] + sum;
  }
  delay(100);
  xavg = sum / 100.0;
  Serial.print("xavg :");
  Serial.println(xavg);
  for (int j = 0; j < 100; j++)
  {
    yval[j] = float(analogRead(ypin) - 346);
    sum1 = yval[j] + sum1;
  }
  yavg = sum1 / 100.0;
  Serial.print("yavg :");
  Serial.println(yavg);
  delay(100);
  for (int q = 0; q < 100; q++)
  {
    zval[q] = float(analogRead(zpin) - 416);
    sum2 = zval[q] + sum2;
  }
  zavg = sum2 / 100.0;
  delay(100);
  Serial.print("zavg :");
  Serial.println(zavg);
}
