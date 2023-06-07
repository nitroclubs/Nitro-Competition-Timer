#include <Arduino.h>
// #define DEBUG_MODE 1 // DEBUG_MODE should be defined here!!!
#define NODEBUG_WEBSOCKETS //!-----
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <Hash.h>
#include <LittleFS.h>
#include <WiFiUDP.h>
#include <time.h>
#include <ArduinoJson.h>
#include <SimpleFTPServer.h>
extern "C"
{
#include "user_interface.h"
}
#include "DFRobot_VL53L0X.h"
// For ESPAsync_WiFiManager:
#include "PortalDefines.h"
#include "PortalCredentials.h"
#include "PortalDynamicParams.h"
// #define USE_DYNAMIC_PARAMETERS  false
//--- End for ESPAsync_WiFiManager
#include "SerialHelper.h"
#include "Globals.h"
#include "Sensor.h"
#include "Timer.h"
#include "Lights.h"
#include "Competition.h"
//=============================

ESP8266WiFiMulti WiFiMulti;

// -------------=========================
#pragma region AsyncWebServer
AsyncWebServer webserver(80);
#pragma endregion AsyncWebServer

// -------------=========================
#pragma region ESPAsync_WiFiManager_Lite

// https://github.com/khoih-prog/ESP_WiFiManager_Lite
ESPAsync_WiFiManager_Lite *ESPAsync_WiFiManager;

void heartBeatPrint()
{
  static int num = 1;

  if (WiFi.status() == WL_CONNECTED)
    Serial.print("H"); // H means connected to WiFi
  else
  {
    if (ESPAsync_WiFiManager->isConfigMode())
      Serial.print("C"); // C means in Config Mode
    else
      Serial.print("F"); // F means not connected to WiFi
  }

  if (num == 80)
  {
    Serial.println();
    num = 1;
  }
  else if (num++ % 10 == 0)
  {
    Serial.print(F(" "));
  }
}

void ESPWiFiManager_check_status()
{
  static unsigned long checkstatus_timeout = 0;

#define HEARTBEAT_INTERVAL 20000L
  // Print hearbeat every HEARTBEAT_INTERVAL (20) seconds.
  if ((millis() > checkstatus_timeout) || (checkstatus_timeout == 0))
  {
    heartBeatPrint();
    checkstatus_timeout = millis() + HEARTBEAT_INTERVAL;
  }
}

#if USING_CUSTOMS_STYLE
const char NewCustomsStyle[] PROGMEM =
    "<style>div,input{padding:5px;font-size:1em;}input{width:95%;}body{text-align: center;}"
    "button{background-color:blue;color:white;line-height:2.4rem;font-size:1.2rem;width:100%;}fieldset{border-radius:0.3rem;margin:0px;}</style>";
#endif
#pragma endregion ESPAsync_WiFiManager_Lite

// -------------=========================
#pragma region WebSocketsServer

WebSocketsServer webSocket = WebSocketsServer(81);
void webSocketEvent(byte, WStype_t, uint8_t *, size_t); // the parameters of this callback function are always the same -> num: id of the client who send the event, type: type of message, payload: actual data sent and length: length of payload
#pragma endregion WebSocketsServer

// -------------=========================
#pragma region SimpleFTPServer

FtpServer ftpSrv; // set #define FTP_DEBUG in ESP8266FtpServer.h to see ftp verbose on serial
void _callback(FtpOperation ftpOperation, unsigned int freeSpace, unsigned int totalSpace)
{
  switch (ftpOperation)
  {
  case FTP_CONNECT:
    Serial.println(F("FTP: Connected!"));
    break;
  case FTP_DISCONNECT:
    Serial.println(F("FTP: Disconnected!"));
    break;
  case FTP_FREE_SPACE_CHANGE:
    Serial.printf("FTP: Free space change, free %u of %u!\n", freeSpace, totalSpace);
    break;
  default:
    break;
  }
};
void _transferCallback(FtpTransferOperation ftpOperation, const char *name, unsigned int transferredSize)
{
  switch (ftpOperation)
  {
  case FTP_UPLOAD_START:
    Serial.println(F("FTP: Upload start!"));
    break;
  case FTP_UPLOAD:
    Serial.printf("FTP: Upload of file %s byte %u\n", name, transferredSize);
    break;
  case FTP_TRANSFER_STOP:
    Serial.println(F("FTP: Finish transfer!"));
    break;
  case FTP_TRANSFER_ERROR:
    Serial.println(F("FTP: Transfer error!"));
    break;
  default:
    break;
  }

  /* FTP_UPLOAD_START = 0,
   * FTP_UPLOAD = 1,
   *
   * FTP_DOWNLOAD_START = 2,
   * FTP_DOWNLOAD = 3,
   *
   * FTP_TRANSFER_STOP = 4,
   * FTP_DOWNLOAD_STOP = 4,
   * FTP_UPLOAD_STOP = 4,
   *
   * FTP_TRANSFER_ERROR = 5,
   * FTP_DOWNLOAD_ERROR = 5,
   * FTP_UPLOAD_ERROR = 5
   */
};
#pragma endregion SimpleFTPServer

// -------------=========================
#pragma region Competition
//
// Instantiate sensor object with: 
// min readisngs distance = 35 mm, 
// max reading distance = 350 mm
// sensor reading interval 25 ms
// time with no readings while the robot passes in front of the sensor - 3 seconds (3000ms)
Sensor *sensor = new Sensor(35, 350, 25, 3000);
Timer *timer = new Timer(25, sensor);
Lights *lights = new Lights();
Competition *competition = new Competition(sensor, timer,lights);

#pragma endregion Competition

//!---------------------------------------------
void setup()
{
  serial::setupSerial(); // Skipped if DEBUG_MODE 1 is defined on top

// -===========================----------
#pragma region ESPAsync_WiFiManager setup
  //   delay(200);
  //   Serial.print(F("\n"));
  //   Serial.print(F("\n"));
  //   Serial.print(FS_Name);
  //   Serial.print(F(" on "));
  //   Serial.println(ARDUINO_BOARD);
  //   Serial.println(ESP_ASYNC_WIFI_MANAGER_LITE_VERSION);

#if USING_MRD
  // Serial.println(ESP_MULTI_RESET_DETECTOR_VERSION);
#else
  // Serial.println(ESP_DOUBLE_RESET_DETECTOR_VERSION);
#endif

  ESPAsync_WiFiManager = new ESPAsync_WiFiManager_Lite();
  String AP_SSID = "NitroCompetitionTimer";
  String AP_PWD = "nitro123timer";

  // Set customized AP SSID and PWD
  ESPAsync_WiFiManager->setConfigPortal(AP_SSID, AP_PWD);

  // Optional to change default AP IP(192.168.4.1) and channel(10)
  // ESPAsync_WiFiManager->setConfigPortalIP(IPAddress(192, 168, 120, 1));
  ESPAsync_WiFiManager->setConfigPortalChannel(0);

#if USING_CUSTOMS_STYLE
  ESPAsync_WiFiManager->setCustomsStyle(NewCustomsStyle);
#endif

#if USING_CUSTOMS_HEAD_ELEMENT
  ESPAsync_WiFiManager->setCustomsHeadElement(PSTR("<style>html{filter: invert(10%);}</style>"));
#endif

#if USING_CORS_FEATURE
  ESPAsync_WiFiManager->setCORSHeader(PSTR("Your Access-Control-Allow-Origin"));
#endif
  // Set customized DHCP HostName
  // ESPAsync_WiFiManager->begin(HOST_NAME);
  ESPAsync_WiFiManager->begin("NitroLapTimer");
  // Set customized DHCP HostName
  // Or use default Hostname "ESP_XXXXXX"
  // ESPAsync_WiFiManager->begin();
#pragma endregion ESPAsync_WiFiManager setup

// -===========================----------
#pragma region WebSocket setup
  webSocket.begin();
  webSocket.onEvent(webSocketEvent); // define a callback function 
#pragma endregion WebSocket setup

  // -===========================----------
#pragma region AsyncWebServer setup
  // Route for root / web page
  webserver.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
               {
  request->send(LittleFS, "/index.html", "text/html"); });
  // Route for root / web page
  webserver.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(LittleFS, "/index.html", "text/html"); });
  webserver.on("/img/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(LittleFS, "/img/favicon.ico", "image/x-icon"); });
  webserver.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(LittleFS, "/favicon.ico", "image/x-icon"); });
  webserver.on("/img/favicon.png", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(LittleFS, "/img/favicon.png", "image/png"); });
  webserver.on("/js/main.js", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(LittleFS, "/js/main.js", "application/javascript"); });
  webserver.on("/css/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(LittleFS, "/css/style.css", "text/css"); });

  webserver.begin();
#pragma endregion AsyncWebServer setup

  // --------======================================
#pragma region SimpleFTPServer setup

  if (LittleFS.begin())
  {
    ftpSrv.setCallback(_callback);
    ftpSrv.setTransferCallback(_transferCallback);
    Serial.println("LittleFS opened!");
    ftpSrv.begin("stefan", "st"); // username, password for ftp.   (default 21, 50009 for PASV)
  }
#pragma endregion SimpleFTPServer setup

  // -===========================----------
#pragma region Competition setup

  serial::setupSerial();
  timer->setCompetitionObjectPointer(competition);
  sensor->setLightsObjectPointer(lights);
  sensor->setCompetitionObjectPointer(competition);
  competition->setWebSocketsServersObjectPointer(&webSocket);
  sensor->setup();
  sensor->enableDetection();
  lights->setup();
  competition->setup();

#pragma endregion Competition setup
}

//!---------------------------------------------
void loop()
{
  // Competition related
  timer->loop();
  sensor->loop();
  // End Competition related
  webSocket.loop();
  ftpSrv.handleFTP(); // make sure in loop you call handleFTP()!!
}

//!---------------------------------------------

void webSocketEvent(byte num, WStype_t type, uint8_t *payload, size_t length)
{
  //  Serial.println("Main webSocketEvent will call Competition::wsEventCallback with the same params ");
  competition->wsEventCallback(num, type, payload, length);
}
