#include <Wire.h>
#include <LPS.h>
#include <LIS3MDL.h>
#include <LSM6.h>
//enkoder
#define PinA 2
#define PinB 3



LPS ps;
LIS3MDL mag;
LSM6 imu;


#define X_MIN -7171
#define X_MAX -29
#define Y_MIN -4785
#define Y_MAX 1763
#define Z_MIN 5109
#define Z_MAX 12056

// Deklinacja magnetyczna
#define dek_stopnie 5
#define dek_minuty 16



char report[100];
float odchylenie;
//enkoder
unsigned long time = 0;
long ilosc_impulsow = 0;
long licznik = 0;


void setup()
{
  Serial.begin(250000);
  Wire.begin();
  mag.init();
  mag.enableDefault();

  //enkoder
  pinMode(PinA, INPUT);
  pinMode(PinB, INPUT);
  attachInterrupt(0, blinkA, LOW);
  attachInterrupt(1, blinkB, LOW);
  time = millis();

  //warunek czy barometr działą, i cos jeszcze

  if (!ps.init())
  {
    Serial.println("Failed to autodetect pressure sensor!");
    while (1);
  }

  ps.enableDefault();

  if (!imu.init())
  {
    Serial.println("Failed to detect and initialize IMU!");
    while (1);
  }
  imu.enableDefault();
}



void loop()
{
  //megnetometr odczyt
  mag.read();
  float x = map(mag.m.x, X_MIN, X_MAX, -360, 360);
  float y = map(mag.m.y, Y_MIN, Y_MAX, -360, 360);
  float z = map(mag.m.z, Z_MIN, Z_MAX, -360, 360);

 // float rX = mag.m.x;
 // float rY = mag.m.y;
 // float rZ = mag.m.z;


  odchylenie = atan2(y, x);
  if (odchylenie < 0) odchylenie += 2 * PI;
  if (odchylenie > 2 * PI) odchylenie -= 2 * PI;
  // int kat = (int) (odchylenie * 180 / M_PI) + (dek_stopnie + (dek_minuty / 60.0)) / (180 / M_PI);
  float kat = (float) (odchylenie * 180 / M_PI) + (dek_stopnie + (dek_minuty / 60.0)) / (180 / M_PI);


  //zczytawanie z akclelerometr,
  imu.read();
  // snprintf(report, sizeof(report), "A: %8d %8d %8d    G: %8d %8d %8d",//6 na 8
  //        imu.a.x, imu.a.y, imu.a.z,
  //         imu.g.x, imu.g.y, imu.g.z);

  float aX = imu.a.x * 0.00061*9.174;
  float aY = imu.a.y * 0.00061*9.174;
  float aZ = imu.a.z * 0.00061*9.174;
  
  float rX = aX*PI/180;
 float rY = aY*PI/180;
  float rZ = aZ*PI/180;


  float aax = map(imu.a.x, -32767,  32767, -19.62, 19.62);
  float aay = map(imu.a.y, -32767,  32767, -19.62, 19.62);
  float aaz = map(imu.a.z, -32767,  32767, -19.62, 19.62);



  // zczyt z analogowego accelerometru


  //Analog read pins
const int xPin = 0;
const int yPin = 1;
const int zPin = 2;

//The minimum and maximum values that came from
//the accelerometer while standing still
//You very well may need to change these
int minVal = 400;
int maxVal = 620;

//to hold the caculated values
double analogx;
double analogy;
double analogz;

//read the analog values from the accelerometer
int xRead = analogRead(xPin);
int yRead = analogRead(yPin);
int zRead = analogRead(zPin);

//convert read values to degrees -90 to 90 – Needed for atan2
int xAng = map(xRead, minVal, maxVal, -90, 90);
int yAng = map(yRead, minVal, maxVal, -90, 90);
int zAng = map(zRead, minVal, maxVal, -90, 90);

//Caculate 360deg values like so: atan2(-yAng, -zAng)
//atan2 outputs the value of -π to π (radians)
//We are then converting the radians to degrees
analogx = (atan2(-yAng, -zAng) + PI);
analogy = (atan2(-xAng, -zAng) + PI);
analogz = (atan2(-yAng, -xAng) + PI);


//kompensacja wychylenia magneto przy pomocy accelerometru

//float tiltCompensate(Vector mag, Vector normAccel)
//{
  // Pitch & Roll 

  float roll;
  float pitch;
  float heading = 0;

  roll = asin(analogy);
  pitch = asin(analogx);

 // if (roll > 0.78 || roll < -0.78 || pitch > 0.78 || pitch < -0.78)
 //  {
 // heading = -1000;
// }

  // Some of these are used twice, so rather than computing them twice in the algorithem we precompute them before hand.
  float cosRoll = cos(analogy);
  float sinRoll = sin(analogy);  
  float cosPitch = cos(-analogx);
  float sinPitch = sin(-analogx);

  // Tilt compensation
  float Xh = x * cosPitch + z * sinPitch;
  float Yh = x * sinRoll * sinPitch + y * cosRoll - z * sinRoll * cosPitch;

  heading = atan2(Yh, Xh);

// odchylenie = atan2(y, x);
  if (heading < 0) heading += 2 * PI;
  if (heading > 2 * PI) heading -= 2 * PI;
  // int kat = (int) (odchylenie * 180 / M_PI) + (dek_stopnie + (dek_minuty / 60.0)) / (180 / M_PI);
  float katk = (float) (heading * 180 / M_PI) + (dek_stopnie + (dek_minuty / 60.0)) / (180 / M_PI);

 // return heading;
//}

  if (heading < 0) { heading += 2 * PI; }
  if (heading > 2 * PI) { heading -= 2 * PI; }



  //ciśnienie i barometr
  float pressure = ps.readPressureMillibars();
  float altitude = ps.pressureToAltitudeMeters(pressure);
  float temperature = ps.readTemperatureC();


//  while (licznik != ilosc_impulsow)
 // {
    licznik = ilosc_impulsow;
    //Serial.println(licznik);
    // Serial.print("p: ");
    // Serial.print(pressure);
    //  Serial.println(" mbar  a: ");
    // Serial.println(altitude);
    //  Serial.println(" m   t: ");
    //   Serial.print(temperature);
    //   Serial.print(" deg C");
    //  Serial.print("     azimute: ");
     Serial.println(kat);
      Serial.print("    ");
      Serial.println(katk);
      Serial.print("    ");
    //   Serial.print(z);
    //  Serial.print(" stopni");
    //  Serial.println(report);
 //  Serial.print (aX);
  // Serial.print (" ");
 //  Serial.print (rX);
 //  Serial.print (" ");
 //  Serial.print (imu.a.x);
   
     
 //    Serial.print ("              ");
 //     Serial.print (aY);
 //     Serial.print (" ");
 //  Serial.print (rY);
 //  Serial.print (" ");
  //   Serial.print (imu.a.y);
  //   Serial.print ("               ");
    // Serial.print (aZ);

    Serial.print (rX);
     Serial.print ("  ");
     Serial.print (rY);
     Serial.print ("  ");
     Serial.print (rZ);

     Serial.print ("       ");
     Serial.print (analogx);
     Serial.print ("  ");
     Serial.print (analogy);
     Serial.print ("  ");
     Serial.print (analogz);
      
  //   Serial.print (Xh);
  //   Serial.print (" ");
 //   Serial.print (Yh);
  //  Serial.print (" ");


    
   //  Serial.print (mag.m.x);
   // Serial.print (" ");
 //   Serial.print (heading*57.29578);
 //   Serial.print ("          ");
 //  Serial.print (x );
   // Serial.print (" " );
   //  Serial.print (y );
   //  Serial.print (" " );
   //  Serial.print (M_PI );
  //   Serial.print (" ");
  //   Serial.print (heading*57.2958);
   //  Serial.print (" ");
  //   Serial.print (roll*100);
   //  Serial.print (" ");
  //   Serial.print (pitch*100);
    Serial.println("");
    
    
 // }
}
// Serial.print(odchylenie);
void blinkA()
{
  if ((millis() - time) > 3)
    ilosc_impulsow--;
  time = millis();
}

void blinkB()
{
  if ((millis() - time) > 3)
    ilosc_impulsow++ ;
  time = millis();
}



//delay(100);

