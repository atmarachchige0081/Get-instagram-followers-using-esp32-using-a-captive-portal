#include <Arduino.h>
#include <Stepper.h>
#include "CaptivePortalManager.h"

CaptivePortalManager portalManager("ConfigPortal", "");

#define STEPS_PER_REV  2048
#define STEPS_PER_DIGIT 205  

Stepper stepper(STEPS_PER_REV, 1, 2, 3, 4);

int current_position = 0; 

void moveToDigit(int target_position) {
  if (target_position != current_position) {
    int steps_to_move;
    
    if (target_position > current_position) {
      steps_to_move = (target_position - current_position) * STEPS_PER_DIGIT;
    } else {
      steps_to_move = ((10 - current_position) + target_position) * STEPS_PER_DIGIT;
    }

    stepper.step(-steps_to_move);

    Serial.print("Stepper moved from ");
    Serial.print(current_position);
    Serial.print(" to ");
    Serial.println(target_position);

    current_position = target_position;
  } else {
    Serial.println("Already at that digit.");
  }
}


unsigned long lastPrintTime = 0;
const unsigned long PRINT_INTERVAL = 60000; 


void setup() {
  Serial.begin(115200);
  delay(1000);

  stepper.setSpeed(10); 

  portalManager.begin();
  
  Serial.println("System ready.");    
}


void loop() {
  portalManager.handle();

  int followerCount = portalManager.getFollowerCount();

  static int lastKnownFollowers = -1; 
  if (followerCount != lastKnownFollowers && followerCount >= 0 && followerCount <= 9) {
    Serial.print("New follower digit: ");
    Serial.println(followerCount);
    moveToDigit(followerCount); 
    lastKnownFollowers = followerCount;
  }

  unsigned long now = millis();
  if (now - lastPrintTime >= PRINT_INTERVAL) {
    lastPrintTime = now;
    followerCount= portalManager.getFollowerCount();
    Serial.print("Latest Follower Count: ");
    Serial.println(followerCount);
  }

  delay(50);
}
