#include "Arduino.h"
#include "Streaming.h"
#include "Array.h"
#include "TimerOne.h"
#include "Functor.h"
#include "EventController.h"


const int BAUDRATE = 9600;
const int OUTPUT_PIN = 13;
const int DELAY_MS = 200;
const int PERIOD_MS = 2000;
const int ON_DURATION_MS = 1000;

const int EVENT_COUNT_MAX = 2;
EventController<EVENT_COUNT_MAX> event_controller;

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

  event_controller.setup();

  EventIdPair event_id_pair = event_controller.addInfinitePwmUsingDelay(makeFunctor((Functor1<int> *)0,pinHighEventCallback),
                                                                        makeFunctor((Functor1<int> *)0,pinLowEventCallback),
                                                                        DELAY_MS,
                                                                        PERIOD_MS,
                                                                        ON_DURATION_MS);
  event_controller.enable(event_id_pair);
}


void loop()
{
}
