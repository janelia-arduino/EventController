#include "Arduino.h"
#include "Streaming.h"
#include "Array.h"
#include "TimerOne.h"
#include "Functor.h"
#include "FunctorCallbacks.h"
#include "EventController.h"


const int BAUDRATE = 9600;
const int LOOP_DELAY = 1000;
const int CLOCK_PERIOD = 1000;
const int CLOCK_START_DELAY = 2000;
const int CLOCK_MAX_COUNT = 25;
const int COUNTER_LIMIT = 10;
const int COUNTER_START_DELAY = 5000;
const int TRIGGER_OFFSET = 2000;
const int LED_PIN = 13;
const int LED_DELAY = 3000;
const int LED_PERIOD = 500;
const int LED_ON_DURATION = 250;
const int LED_COUNT = 5;
const int LED_2_DELAY = LED_DELAY*2 + LED_PERIOD*LED_COUNT;
const int LED_2_PERIOD = LED_PERIOD*2;
const int LED_2_ON_DURATION = LED_ON_DURATION*2;
const int LED_2_END_CLOCK = CLOCK_MAX_COUNT-5;

const int EVENT_COUNT_MAX = 32;
EventController<EVENT_COUNT_MAX> event_controller;

int clock = 0;
int counter = 0;
bool triggered = false;

EventId clock_event_id;
EventId counter_event_id;
EventIdPair led_2_event_id_pair;

void clockUpdateEventHandler(int arg=-1)
{
  ++clock;
}

void counterUpdateEventHandler(int arg=-1)
{
  ++counter;
}

void triggerEventHandler(int arg=-1)
{
  triggered = true;
}

void ledOnEventHandler(int arg=-1)
{
  digitalWrite(LED_PIN, HIGH);
}

void ledOffEventHandler(int arg=-1)
{
  digitalWrite(LED_PIN, LOW);
}

void ledPwmStartEventHandler(int arg=-1)
{
  Serial << "led pwm start!" << endl;
}

void ledPwmStopEventHandler(int arg=-1)
{
  Serial << "led pwm stop!" << endl;
}

void setup()
{
  Serial.begin(BAUDRATE);
  delay(1000);

  pinMode(LED_PIN, OUTPUT);
  ledOffEventHandler();

  event_controller.setup();
  clock_event_id = event_controller.addInfiniteRecurringEventUsingDelay(makeFunctor((Functor1<int> *)0,clockUpdateEventHandler),
                                                                        CLOCK_START_DELAY,
                                                                        CLOCK_PERIOD);
  event_controller.enable(clock_event_id);
  counter_event_id = event_controller.addRecurringEventUsingDelay(makeFunctor((Functor1<int> *)0,counterUpdateEventHandler),
                                                                  COUNTER_START_DELAY,
                                                                  CLOCK_PERIOD,
                                                                  COUNTER_LIMIT);
  event_controller.enable(counter_event_id);
  EventId trigger_event_id = event_controller.addEventUsingOffset(makeFunctor((Functor1<int> *)0,triggerEventHandler),
                                                                  counter_event_id,
                                                                  TRIGGER_OFFSET);
  event_controller.enable(trigger_event_id);
  EventIdPair led_event_id_pair = event_controller.addPwmUsingDelay(makeFunctor((Functor1<int> *)0,ledOnEventHandler),
                                                                    makeFunctor((Functor1<int> *)0,ledOffEventHandler),
                                                                    LED_DELAY,
                                                                    LED_PERIOD,
                                                                    LED_ON_DURATION,
                                                                    LED_COUNT);
  event_controller.addStartFunctor(led_event_id_pair,
                                    makeFunctor((Functor1<int> *)0,ledPwmStartEventHandler));
  event_controller.addStopFunctor(led_event_id_pair,
                                   makeFunctor((Functor1<int> *)0,ledPwmStopEventHandler));
  event_controller.enable(led_event_id_pair);
  led_2_event_id_pair = event_controller.addInfinitePwmUsingDelay(makeFunctor((Functor1<int> *)0,ledOnEventHandler),
                                                                  makeFunctor((Functor1<int> *)0,ledOffEventHandler),
                                                                  LED_2_DELAY,
                                                                  LED_2_PERIOD,
                                                                  LED_2_ON_DURATION);
  event_controller.enable(led_2_event_id_pair);
}


void loop()
{
  Serial << "clock: " << clock << endl;
  Serial << "counter: " << counter << endl;
  Serial << "triggered: " << triggered << endl;
  Serial << "countActiveEvents: " << event_controller.countActiveEvents() << endl;
  Serial << endl;
  if (clock == CLOCK_MAX_COUNT)
  {
    event_controller.remove(clock_event_id);
  }
  if (clock >= LED_2_END_CLOCK)
  {
    event_controller.remove(led_2_event_id_pair);
  }
  delay(LOOP_DELAY);
}
