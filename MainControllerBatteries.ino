#include <avr/sleep.h>
#include <GSM.h>
#include <AltSoftSerial.h>

#define MEASURING_DEVICES 5
#define END_SYMBOL '\0'
#define GSM_PIN 5
#define GPRS_APN ""
#define GPRS_LOGIN ""
#define GPRS_PASSWORD ""

GSMClient client;
GPRS gprs;
GSM gsmAccess;
 
// Pin for getting woken up by RTC
int wakePin = 2;
int sleepStatus = 0;
const char server[] = "arduino.cc"; // Not actual server
const char path[] = "/data";
int port = 3000;

// Used for GSM connection
boolean notConnected = true;

void setup()
{
  pinMode(wakePin, INPUT);
  Serial.begin(9600);
  attachInterrupt(0, wakeUpNow, CHANGE);
}

String getDeviceData(int index) {
  Serial.print(String(index));
  
  while(!Serial.available()) {delay(50);}
  return Serial.readString();
}

void wakeUpNow()
{
  for(int i = 0; i < MEASURING_DEVICES; i++)
    sendData(getDeviceData(i));

  notConnected = true;
}

void sendData(const String data) {
  while(notConnected)
  {
    if((gsmAccess.begin((char*)"5") == GSM_READY) &
        (gprs.attachGPRS((char*)GPRS_APN, (char*)GPRS_LOGIN, (char*)GPRS_PASSWORD) == GPRS_READY))
      notConnected = false;
    else
      delay(1000);
  }

  if (client.connect(server, port))
  {
    // Make a HTTP request:
    client.print("POST ");
    client.print(path);
    client.print(" HTTP/1.1\nContent-Type: application/json\nContent-Length: ");
    client.print(String(data.length()));
    client.print("\nHost: ");
    client.print(server);
    client.print("\n\n");
    client.println(data);
    client.println();
  }
}
 
void sleepNow()
{
    sleep_enable();
    attachInterrupt(0, wakeUpNow, CHANGE);
    sleep_mode();
    sleep_disable();
    detachInterrupt(0);
 
}
 
void loop()
{
  delay(100);
  sleepNow();
}
