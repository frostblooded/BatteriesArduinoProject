#include <avr/sleep.h>
#include <GSM.h>

#define MEASURING_DEVICES 5
#define END_SYMBOL '\0'
#define GSM_PIN 5
#define GPRS_APN ""
#define GPRS_LOGIN ""
#define GPRS_PASSWORD ""

// Software serial for GSM communication
SoftwareSerial dataSerial(3, 4);
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
  dataSerial.begin(9600);
  attachInterrupt(0, wakeUpNow, CHANGE);
}

String getDeviceData(int index) {
  dataSerial.write(String(index));
  
  while(!dataSerial.available()) {sleep(50)};
  return dataSerial.readString();
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
    Serial.println("connection failed");
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
  Serial.println("Entering Sleep mode");
  delay(100);
  sleepNow();
}
