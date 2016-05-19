#include <avr/sleep.h>
#include <GSM.h>

#define MEASURING_DEVICES 5
#define END_SYMBOL '\0'
#define GSM_PIN 5
#define GPRS_APN ""
#define GPRS_LOGIN ""
#define GPRS_PASSWORD ""

SoftwareSerial dataSerial(2, 3);
GSMClient client;
GPRS gprs;
GSM gsmAccess;
 
int wakePin = 2;
int sleepStatus = 0;
const char server[] = "arduino.cc"; // Example
const char path[] = "/data";
int port = 3000;

void setup()
{
  pinMode(wakePin, INPUT);
  Serial.begin(9600);
  dataSerial.begin(4800);
  attachInterrupt(0, wakeUpNow, CHANGE);
}

String getDeviceData(int index) {
  String res = "";
  dataSerial.write(String(index));

  while(dataSerial.available()) {
    char readByte = (char)Serial.read();

    if(readByte == END_SYMBOL) {
      break;
    }
    
    res += readByte;
  }

  return res;
}

String getData() {
  String res = "";
  
  for(int i = 0; i < MEASURING_DEVICES; i++) {
    res += getDeviceData(i);
  }

  return res;
}

void wakeUpNow()
{
  sendData(getData());
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
  Serial.println("Timer: Entering Sleep mode");
  delay(100);
  sleepNow();
}
