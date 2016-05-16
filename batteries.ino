#include <avr/sleep.h>
#include <GSM.h>
//TODO: Read from A0 the voltage of the battery(0 - 20.48V)

#define GSM_PIN 5
#define GPRS_APN ""
#define GPRS_LOGIN ""
#define GPRS_PASSWORD ""

GSMClient client;
GPRS gprs;
GSM gsmAccess;
 
int wakePin = 2;
int sleepStatus = 0;
const char server[] = "arduino.cc"; // Example
const char path[] = "/data";
int port = 3000;

void wakeUpNow()
{
  // Check each battery
  String data = "";

  // Send data
  sendData(data);
}

void sendData(const String data) {
  boolean notConnected = true;

  while(notConnected)
  {
    if((gsmAccess.begin("5") == GSM_READY) &
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY))
      notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("connecting...");

  if (client.connect(server, port))
  {
    Serial.println("connected");
    
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
  else
  {
    Serial.println("connection failed");
  }
}
 
void setup()
{
  pinMode(wakePin, INPUT);
  Serial.begin(9600);
  attachInterrupt(0, wakeUpNow, CHANGE);
}
 
void sleepNow()
{
    sleep_enable();
    attachInterrupt(0, wakeUpNow, CHANGE);
    analogReference(INTERNAL);
    sleep_mode();
    sleep_disable();
    detachInterrupt(0);
 
}
 
void loop()
{
  if (Serial.available()) {
    Serial.write(Serial.read());
  }
  
  delay(1000);
  Serial.println("Timer: Entering Sleep mode");
  delay(100);
  sleepNow();
}
