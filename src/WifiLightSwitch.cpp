#include "WifiNetworkData.h"
#include <user_config.h>
#include <SmingCore/SmingCore.h>

#define BUTTON_PIN 0
#define LED_PIN 3

const IPAddress gIpAddress(10, 18, 0, 66); //Classroom
//const IPAddress gIpAddress(10, 18, 0, 67); //Workroom
const unsigned gUdpPort = 42069; //42068 = workroom | 42069 classroom


bool gState = false;
bool gButtonToggled = false;
long gLastTimeToggled = 0l;
HttpClient gHttpClient;
Timer gProgramTimer;

void sendData();
void onDataSent(HttpClient& client, bool successful);
void getStatus();
void onStatusSent(HttpClient& client, bool successful);

void onUdpReceive(
  UdpConnection& connection,
  char *data,
  int size,
  IPAddress remoteIP,
  uint16_t remotePort);

UdpConnection gUdpConnection(onUdpReceive);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void setState(const String& response)
{
  if (response.indexOf("true") != -1)
  {
    gState = true;
  }
  else
  {
    gState = false;
  }
  digitalWrite(LED_PIN, !gState);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void onUdpReceive(
  UdpConnection& connection,
  char *data,
  int size,
  IPAddress remoteIP,
  uint16_t remotePort)
{
  String Data(data);
  if (remoteIP == IPAddress(10, 18, 0, 47))
  {
    if (Data == "1")
    {
      setState("true");
      return;
    }
    setState(data);
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void onDataSent(HttpClient& client, bool successful)
{
	if (successful)
  {
    setState(client.getResponseString());
  }
  else
  {
    sendData();
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void onStatusSent(HttpClient& client, bool successful)
{
	if (successful)
  {
    setState(client.getResponseString());
  }
  else
  {
    getStatus();
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void getStatus()
{
	while(gHttpClient.isProcessing())
  {
    return;
  }
	gUdpConnection.listen(gUdpPort);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void IRAM_ATTR toggleFlag()
{
  if (!gButtonToggled && millis() - gLastTimeToggled > 500)
  {
    gButtonToggled = true;
    gLastTimeToggled = millis();
    digitalWrite(LED_PIN, !gState);
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void sendData()
{
  if (gButtonToggled)
  {

    while(gHttpClient.isProcessing())
    {
      return;
    }

    gHttpClient.downloadString(
      "http://classroom-lights.west.sbhackerspace.com/toggle",
      onDataSent);
    gButtonToggled = false;
  }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void init()
{
  pinMode(LED_PIN, OUTPUT);

  WifiAccessPoint.enable(false);
  WifiStation.enable(true);
  WifiStation.config(ssid, password);
  WifiStation.setIP(
    gIpAddress,
    IPAddress(255, 255, 240, 0),
    IPAddress(10, 18, 0, 1));
  WifiStation.waitConnection(getStatus, 30, NULL);


	attachInterrupt(BUTTON_PIN, toggleFlag, RISING);
	gProgramTimer.initializeMs(100, sendData).start();
}
