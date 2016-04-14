#include "Arduino.h"
#include "Streaming.h"
#include "Array.h"
#include "TimerOne.h"
#include "EventController.h"


const int BAUDRATE = 9600;
const int LOOP_DELAY = 4000;
const int OUTPUT_PIN = 13;
const int DELAY_MS = 200;
const int PERIOD_MS = 2000;
const int ON_DURATION_MS = 1000;

void pinHighEventCallback(int index)
{
  digitalWrite(OUTPUT_PIN,HIGH);
}

void pinLowEventCallback(int index)
{
  digitalWrite(OUTPUT_PIN,LOW);
}

void setup()
{
  Serial.begin(BAUDRATE);

  pinMode(OUTPUT_PIN,OUTPUT);
  digitalWrite(OUTPUT_PIN,LOW);

  EventController::event_controller.setup();

  EventController::event_controller.addInfinitePwmUsingDelayPeriodOnDuration(pinHighEventCallback,
                                                                             pinLowEventCallback,
                                                                             DELAY_MS,
                                                                             PERIOD_MS,
                                                                             ON_DURATION_MS);
}


void loop()
{
}
