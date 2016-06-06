#include <avr/sleep.h>
#include <GSM.h>

#define MEASURING_DEVICES 5 // How many batteries are connected
#define GSM_PIN "5"
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
int port = 8080;

// Used for GSM connection
boolean notConnected = true;

void setup()
{
  pinMode(wakePin, INPUT); // Pin for waking up
  attachInterrupt(0, wakeUpNow, CHANGE); // Wake up on voltage change
  Serial.begin(1200);
}

String getDeviceData(int index) {
  Serial.print(String(index));

  while (!Serial.available())
    delay(50);

  return Serial.readString();
}

void wakeUpNow()
{
  // Get data for each battery and send it to server.
  // It sends batteries one by one, because it is easier
  // than sending an array.
  for (int i = 0; i < MEASURING_DEVICES; i++)
    sendData(getDeviceData(i));

  notConnected = true;
}

void sendData(const String data) {
  while (notConnected)
  {
    // Login to GSM module
    if ((gsmAccess.begin(GSM_PIN) == GSM_READY) &
        (gprs.attachGPRS((char*)GPRS_APN, (char*)GPRS_LOGIN, (char*)GPRS_PASSWORD) == GPRS_READY))
      notConnected = false;
    else
      delay(1000);
  }

  if (client.connect(server, port))
  {
    // Make a HTTP POST request to server
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

// Go to sleep
void sleepNow()
{
  sleep_enable();
  attachInterrupt(0, wakeUpNow, CHANGE);
  sleep_mode();
  sleep_disable();
  detachInterrupt(0);

}

// Program comes here after wakeUpNow() and goes to sleep after a sligh delay
void loop()
{
  delay(100);
  sleepNow();
}
