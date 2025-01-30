#include <Arduino.h>
#include "CaptivePortalManager.h"
CaptivePortalManager portalManager("ConfigPortal", "");
void setup() {
  Serial.begin(115200);
  delay(1000);

  portalManager.begin();
}
void loop() {
  portalManager.handle();
}
