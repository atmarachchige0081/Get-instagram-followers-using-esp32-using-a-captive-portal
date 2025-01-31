#ifndef CAPTIVE_PORTAL_MANAGER_H
#define CAPTIVE_PORTAL_MANAGER_H

#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

/**
 * CaptivePortalManager
 * --------------------
 * Manages:
 *   1) Launching a Wi-Fi Access Point & DNS-based Captive Portal
 *   2) Collecting Wi-Fi + Instagram credentials
 *   3) Connecting to the provided Wi-Fi
 *   4) Fetching Instagram follower count periodically
 *   5) Storing the latest fetched follower count
 */
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
    static const unsigned long FETCH_INTERVAL = 60000; 

    static const char MAIN_page[];

    int _currentFollowerCount; 
};

#endif
