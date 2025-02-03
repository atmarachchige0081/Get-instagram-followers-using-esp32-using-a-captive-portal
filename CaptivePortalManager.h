#ifndef CAPTIVE_PORTAL_MANAGER_H
#define CAPTIVE_PORTAL_MANAGER_H

#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
class CaptivePortalManager
{
public:
    CaptivePortalManager(const char* apSSID = "ConfigPortal", 
                         const char* apPASS = "");

    void begin();
    void handle();

    int getFollowerCount() const { return _currentFollowerCount; }

private:
    void startCaptivePortal();
    void stopCaptivePortal();
    bool connectToWiFi(const String& ssid, const String& pass);
    void fetchInstagramFollowers(const String& userName);

    void handleRoot();
    void handleSubmit();
    void handleNotFound();

    DNSServer dnsServer;
    WebServer server;

    const char* apSSID;
    const char* apPASS;

    String wifiSSID;
    String wifiPASS;
    String instagramUser;
    bool   configReceived;

    unsigned long lastFetchTime;
    static const unsigned long FETCH_INTERVAL = 60000; // 60s

    static const char MAIN_page[];

    int _currentFollowerCount;
};

#endif
