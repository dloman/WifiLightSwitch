#include "WifiNetworkData.h"
#include <user_config.h>
#include <SmingCore/SmingCore.h>

#define BUTTON_PIN 0
#define LED_PIN 3

HttpClient gHttpClient;

bool gState = false;
String gUrl = "http://workroom-lights.west.sbhackerspace.com/";

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
  if (remoteIP == IPAddress(10, 18, 0, 53))
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
void sendData()
{
	while(gHttpClient.isProcessing())
  {
    return;
  }
  digitalWrite(LED_PIN, !gState);
	gHttpClient.downloadString(
    gUrl + "toggle",
    onDataSent);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void getStatus()
{
	while(gHttpClient.isProcessing())
  {
    return;
  }
	gUdpConnection.listen(42068);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void init()
{
	pinMode(LED_PIN, OUTPUT);

	WifiStation.enable(true);
	WifiStation.config(ssid, password);
  WifiAccessPoint.enable(false);
	WifiStation.setIP(IPAddress(10, 18, 0, 101));

	WifiStation.waitConnection(getStatus, 30, NULL);
	attachInterrupt(BUTTON_PIN, sendData, RISING);
}
