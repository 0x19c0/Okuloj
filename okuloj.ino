#include "DebugMacros.h"
#include "ESP8266WiFi.h"
#include "HTTPSRedirect.h"
extern "C" {
#include "user_interface.h"
}

#include "okuloj.h"

// Put target network channel here
const int kChannel = 0;
// Put your WiFi name and password here
const char* kSSID = "";
const char* kPassword = "";
// Put your Google Sheet Id here
const String kScriptID = "";
// Don't change host/port unless you really know what you're doing
const char* kHost = "script.google.com";
const int kPort = 443;
// Device name used to sign the request
const String kDeviceName = "";
// Base string used for building a GET request
const String kRequestBase = "/macros/s/" + kScriptID + "/exec?func=append&name=" + kDeviceName;

// Time between history uploads, in milliseconds
const unsigned long kUploadTimeThreshold = 30000;
// Maximum time for establishing internet connection, in milliseconds
const unsigned long kMaxConnectionTime = 20000;
// Packets below this size can't have macs and will be dropped
const int kMinPacketSize = 28;
// Number of devices reported in a single GET request
const int kRequestSize = 20;

// Object storing all mac addresses seen since the last flush
Logger logger;

// Time of the last logger flush
unsigned int flush_time;

// Connecting to internet
bool Connect() {
  wifi_set_opmode(STATION_MODE);
  WiFi.begin(kSSID, kPassword);
  DPRINT("Connecting.");

  unsigned long start_time = millis();
  while (WiFi.status() != WL_CONNECTED) {
    DPRINT(".");
    if (millis() - start_time > kMaxConnectionTime) {
      DPRINTLN("/nError: Failed to connect to network.");
      return false;
    }
    delay(500);
  }
  DPRINTLN();

  return true;
}

// Upload function flushes the devices history to google sheet
void Upload() {
  wifi_promiscuous_enable(0);

  WiFi.persistent(false);
  WiFi.disconnect(true);

  // Connecting to internet
  bool wifi_connected = Connect();
  if (!wifi_connected) {
    WiFi.disconnect();
    wifi_promiscuous_enable(1);
    return;
  }

  // Creating a new client and setting it up
  HTTPSRedirect client(kPort);
  client.setPrintResponseBody(true);
  client.setContentTypeHeader("application/json");

  int client_connected = client.connect(kHost, kPort);
  if (!client_connected) {
    DPRINTLN("Error: Failed to connect to host.");
    WiFi.disconnect();
    wifi_promiscuous_enable(1);
    return;
  }

  // Flushing devices history to GET requests
  while (!logger.empty()) {
    String payload;
    for (int i = 0; i < kRequestSize && !logger.empty(); ++i) {
      char temp[32];
      uint8_t* mac = logger.top();
      sprintf(temp, "&n%d=%02x:%02x:%02x:%02x:%02x:%02x", i,
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
      payload += temp;
      logger.pop();
    }
    String request = kRequestBase + payload;
    client.GET(request, kHost);
    ESP.wdtFeed();
  }

  WiFi.disconnect();
  wifi_promiscuous_enable(1);
}

// Callback function called every time a packet is sniffed
void Callback(uint8_t *buf, uint16_t len) {
  if (len < kMinPacketSize) return;

  uint8_t frame_type = buf[12];
  if (!FrameValid(frame_type)) return;

  uint8_t* mac_from = &buf[16];
  uint8_t* mac_to = &buf[22];
  if (!MacValid(mac_from) || !MacValid(mac_to)) return;
  if (MacMulticast(mac_from) || MacMulticast(mac_to)) return;

  logger.add(mac_from);
  logger.add(mac_to);
}

void setup() {
  Serial.begin(115200);

  DPRINTLN("Setup started...");

  flush_time = millis();

  wifi_set_opmode(STATION_MODE);
  wifi_promiscuous_enable(0);

  WiFi.disconnect();

  wifi_set_promiscuous_rx_cb(Callback);
  wifi_set_channel(kChannel);
  wifi_promiscuous_enable(1);

  DPRINTLN("Setup finished.");
}

void loop() {
  if (millis() - flush_time > kUploadTimeThreshold) {
    DPRINTLN("Upload threshold reached...");
    Upload();
    flush_time = millis();
    DPRINTLN("Upload threshold reset.");
  }
}
