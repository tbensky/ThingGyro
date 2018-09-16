#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <SparkFunLSM9DS1.h>

#define SAMPLES 20000

#define LSM9DS1_M  0x1E // Would be 0x1C if SDO_M is LOW
#define LSM9DS1_AG  0x6B // Would be 0x6A if SDO_AG is LOW]]]]]]]]

WiFiServer server(80);
LSM9DS1 imu;


void setup() 
{
  Serial.begin(115200);
  imu.settings.device.commInterface = IMU_MODE_I2C;
  imu.settings.device.mAddress = LSM9DS1_M;
  imu.settings.device.agAddress = LSM9DS1_AG;
 
  if (!imu.begin())
  {
    Serial.println("Can't open 9dof.");
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAP("wifi-gyro", "12345678");
  server.begin();

}

void loop() 
{
  int i, j;
  unsigned long start;
  float gx, gy, gz;
  float ax, ay, az;
  const int N = 2;
  
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  start = millis();

   for(i=0;i<SAMPLES;i++)
  {
    gx = gy = gz = 0.0;
    ax = ay = az = 0.0;

    for(j=1;j<=N;j++)
      {
        imu.readGyro();
        imu.readAccel();
       
        gx += imu.calcGyro(imu.gx);
        gy += imu.calcGyro(imu.gy);
        gz += imu.calcGyro(imu.gz);

        ax += imu.calcAccel(imu.ax);
        ay += imu.calcAccel(imu.ay);
        az += imu.calcAccel(imu.az);
      }

     gx /= N;
     gy /= N;
     gz /= N;

     ax /= N;
     ay /= N;
     az /= N;

    
    client.print(String(i));
    client.print(", ");
    client.print(String(millis() - start));
    client.print(", ");
  
    client.print(gx, 2);
    client.print(", ");
    client.print(gy, 2);
    client.print(", ");
    client.print(gz, 2);
    client.print(", ");
  
    client.print(ax, 2);
    client.print(", ");
    client.print(ay, 2);
    client.print(", ");
    client.print(az, 2);
    client.println();
    client.flush();
  }
}


