#include <Arduino.h>
#include <Stepper.h>
#include "CaptivePortalManager.h"

CaptivePortalManager portalManager("ConfigPortal", "");

#define STEPS_PER_REV   2048
#define STEPS_PER_DIGIT 205  

Stepper myStepper(STEPS_PER_REV, 18, 19, 21, 23);

const int hallSensorPin = 15;

int current_position = 0;

void homeStepper() {
  Serial.println("Homing the stepper. Rotating until hall sensor is triggered...");
  
  while (digitalRead(hallSensorPin) == HIGH) {
    myStepper.step(-4); 
    delay(5); 
  }
}

void moveToDigit(int target_position) {
  if (target_position != current_position) {
    int steps_to_move;
    
    if (target_position > current_position) {
      steps_to_move = (target_position - current_position) * STEPS_PER_DIGIT;
    } else {
      steps_to_move = ((10 - current_position) + target_position) * STEPS_PER_DIGIT;
    }
    myStepper.step(-steps_to_move);

    Serial.print("Stepper moved from ");
    Serial.print(current_position);
    Serial.print(" -> ");
    Serial.println(target_position);

    current_position = target_position;
  } else {
    Serial.println("Already at target digit.");
  }
}
unsigned long lastPrintTime = 0;
const unsigned long PRINT_INTERVAL = 60000; 

static int lastKnownFollowers = -1;

void setup() {
  Serial.begin(115200);
  delay(1000);


  pinMode(hallSensorPin, INPUT); 

  myStepper.setSpeed(10);


  homeStepper();


  portalManager.begin();

  Serial.println("System ready: Homed stepper + Portal Manager started.");
}


void loop() {

  portalManager.handle();

  int followerCount = portalManager.getFollowerCount();
   
  if (followerCount != lastKnownFollowers && followerCount >= 0 && followerCount <= 9) {
    Serial.print("New follower digit: ");
    Serial.println(followerCount);
    moveToDigit(followerCount); 
    lastKnownFollowers = followerCount;
  }

  unsigned long now = millis();
  if (now - lastPrintTime >= PRINT_INTERVAL) {
    lastPrintTime = now;
    Serial.print("Latest Follower Count: ");
    Serial.println(followerCount);
  }

  delay(20);
}
