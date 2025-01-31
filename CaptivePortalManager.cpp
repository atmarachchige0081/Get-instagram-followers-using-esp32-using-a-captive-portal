#include "CaptivePortalManager.h"

const char CaptivePortalManager::MAIN_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 Captive Portal</title>
  <style>
    body {
      margin: 0; padding: 0; font-family: 'Arial', sans-serif;
      background: linear-gradient(to right, #4facfe, #00f2fe);
      display: flex; align-items: center; justify-content: center;
      height: 100vh; animation: fadeBg 1.5s ease-in-out;
    }
    .container {
      background: #ffffff; width: 90%; max-width: 400px;
      border-radius: 10px; padding: 20px;
      box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);
      text-align: center; animation: fadeIn 0.8s ease-in-out;
    }
    h2 { color: #007bff; font-size: 24px; margin-bottom: 10px; }
    p { color: #333; margin-bottom: 20px; }
    input {
      width: 90%; padding: 12px; margin: 8px 0;
      border: 1px solid #ccc; border-radius: 5px; font-size: 16px;
    }
    .button {
      width: 90%; padding: 12px; border: none;
      border-radius: 5px; background-color: #007bff;
      color: #fff; font-size: 18px; cursor: pointer;
      transition: background-color 0.3s;
    }
    .button:hover { background-color: #0056b3; }
    @keyframes fadeIn {
      from { opacity: 0; transform: scale(0.95); }
      to { opacity: 1; transform: scale(1); }
    }
    @keyframes fadeBg {
      from { background-position: 0% 50%; }
      to { background-position: 100% 50%; }
    }
  </style>
</head>
<body>
  <div class="container">
    <h2>ESP32 Captive Portal</h2>
    <p>Enter Wi-Fi & Instagram details:</p>
    <form action="/submit" method="POST">
      <input type="text" name="wifi_ssid" placeholder="Wi-Fi SSID" required><br>
      <input type="password" name="wifi_password" placeholder="Wi-Fi Password" required><br>
      <input type="text" name="instagram_username" placeholder="Instagram Username" required><br>
      <button class="button" type="submit">Connect</button>
    </form>
  </div>
</body>
</html>
)rawliteral";

CaptivePortalManager::CaptivePortalManager(const char* apSSID, const char* apPASS)
  : server(80),
    apSSID(apSSID),
    apPASS(apPASS),
    configReceived(false),
    lastFetchTime(0),
    _currentFollowerCount(-1) 
{}

void CaptivePortalManager::begin()
{
    startCaptivePortal();
}

void CaptivePortalManager::handle()
{
    dnsServer.processNextRequest();
    server.handleClient();

    if (configReceived) {
        stopCaptivePortal();
        if (connectToWiFi(wifiSSID, wifiPASS)) {
            fetchInstagramFollowers(instagramUser);
            lastFetchTime = millis();
        }
        configReceived = false;
    }

    if (WiFi.status() == WL_CONNECTED) {
        unsigned long now = millis();
        if (now - lastFetchTime >= FETCH_INTERVAL) {
            fetchInstagramFollowers(instagramUser);
            lastFetchTime = now;
        }
    }
}

void CaptivePortalManager::startCaptivePortal()
{
    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSSID, apPASS);
    delay(1000);

    IPAddress AP_IP = WiFi.softAPIP();
    Serial.println("Captive Portal started.");
    Serial.print("AP IP address: ");
    Serial.println(AP_IP);

    dnsServer.start(53, "*", AP_IP);

    server.on("/", HTTP_GET, [this](){
        handleRoot();
    });
    server.on("/submit", HTTP_POST, [this](){
        handleSubmit();
    });
    server.onNotFound([this](){
        handleNotFound();
    });

    server.begin();
}

void CaptivePortalManager::stopCaptivePortal()
{
    dnsServer.stop();
    server.stop();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
    Serial.println("Captive portal stopped.");
}

bool CaptivePortalManager::connectToWiFi(const String& ssid, const String& pass)
{
    Serial.println("Connecting to Wi-Fi...");
    WiFi.begin(ssid.c_str(), pass.c_str());

    unsigned long startTime = millis();
    const unsigned long TIMEOUT = 30000; // 30s
    while (WiFi.status() != WL_CONNECTED && (millis() - startTime) < TIMEOUT) {
        delay(500);
        Serial.print(".");
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Wi-Fi connected!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        return true;
    } else {
        Serial.println("Wi-Fi connection failed!");
        return false;
    }
}

void CaptivePortalManager::fetchInstagramFollowers(const String& userName)
{
    const char* HOST = "i.instagram.com";
    const int   PORT = 443;

    WiFiClientSecure client;
    client.setInsecure(); 

    Serial.printf("Connecting to Instagram at %s:%d\n", HOST, PORT);
    if (!client.connect(HOST, PORT)) {
        Serial.println("Connection failed.");
        return;
    }

    String urlPath = "/api/v1/users/web_profile_info/?username=" + userName;

    client.printf("GET %s HTTP/1.1\r\n", urlPath.c_str());
    client.printf("Host: %s\r\n", HOST);
    client.println("User-Agent: Instagram 76.0.0.15.395");
    client.println("Accept: application/json");
    client.println("X-IG-App-ID: 936619743392459");
    client.println("Connection: close");
    client.println();

    String rawResponse;
    while (client.connected() || client.available()) {
        if (client.available()) {
            String line = client.readStringUntil('\n');
            rawResponse += line + "\n";
        }
    }
    client.stop();

    int jsonIndex = rawResponse.indexOf("\r\n\r\n");
    if (jsonIndex == -1) {
        jsonIndex = rawResponse.indexOf("\n\n");
    }
    if (jsonIndex == -1) {
        jsonIndex = 0; 
    } else {
        jsonIndex += 4;
    }
    String jsonBody = rawResponse.substring(jsonIndex);

    StaticJsonDocument<4096> doc;
    DeserializationError error = deserializeJson(doc, jsonBody);
    if (error) {
        Serial.print("JSON parse error: ");
        Serial.println(error.f_str());
        return;
    }

    int followerCount = doc["data"]["user"]["edge_followed_by"]["count"] | -1;
    if (followerCount < 0) followerCount = 0; 
    if (followerCount > 9) followerCount = 9; 

    _currentFollowerCount = followerCount;

    Serial.println("-----------------------");
    Serial.printf("Instagram @%s has %d followers (clamped 0-9).\n", userName.c_str(), followerCount);
    Serial.println("-----------------------");
}

void CaptivePortalManager::handleRoot()
{
    server.send_P(200, "text/html", MAIN_page);
}

void CaptivePortalManager::handleSubmit()
{
    if (server.hasArg("wifi_ssid") && 
        server.hasArg("wifi_password") &&
        server.hasArg("instagram_username")) 
    {
        wifiSSID = server.arg("wifi_ssid");
        wifiPASS = server.arg("wifi_password");
        instagramUser = server.arg("instagram_username");
        configReceived = true;

        String response = "<html><body><h2>Credentials Received!</h2>"
                          "<p>SSID: " + wifiSSID + "</p>"
                          "<p>Password: " + wifiPASS + "</p>"
                          "<p>Instagram: " + instagramUser + "</p>"
                          "<p>Connecting to Wi-Fi...</p></body></html>";
        server.send(200, "text/html", response);
    } else {
        server.send(400, "text/plain", "Missing form fields.");
    }
}

void CaptivePortalManager::handleNotFound()
{
    server.send_P(200, "text/html", MAIN_page);
}
