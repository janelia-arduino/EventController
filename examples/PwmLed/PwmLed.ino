#include <Streaming.h>
#include <Functor.h>
#include <EventController.h>


const long BAUD = 115200;
const size_t TIMER_NUMBER = 3;
const int LED_PIN = 13;
const int DELAY_MS = 2000;
const int PERIOD_MS = 2000;
const int ON_DURATION_MS = 1000;

// test count
const int COUNT = 4;

// test infinite
// const int COUNT = -1;

// test edge cases
// const int ON_DURATION_MS = 0;
// const int ON_DURATION_MS = PERIOD_MS;

const int EVENT_COUNT_MAX = 2;
EventController<EVENT_COUNT_MAX> event_controller;

bool get_info = true;


void ledOnEventHandler(int index)
{
  digitalWrite(LED_PIN,HIGH);
  Serial << "led on!\n\n";
}

void ledOffEventHandler(int index)
{
  digitalWrite(LED_PIN,LOW);
  Serial << "led off!\n\n\n";
}

void setGetInfoTrueHandler(int index)
{
  get_info = true;
}

void setGetInfoFalseHandler(int index)
{
  get_info = false;
}

void getInfo()
{
  uint8_t events_active = event_controller.eventsActive();
  Serial << "events_active = " << events_active << "\n";
  uint8_t events_available = event_controller.eventsAvailable();
  Serial << "events_available = " << events_available << "\n\n\n";
}

void setup()
{
  Serial.begin(BAUD);

  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN,LOW);

  event_controller.setup(TIMER_NUMBER);

  EventIdPair event_id_pair = event_controller.addPwmUsingDelay(makeFunctor((Functor1<int> *)0,ledOnEventHandler),
                                                                makeFunctor((Functor1<int> *)0,ledOffEventHandler),
                                                                DELAY_MS,
                                                                PERIOD_MS,
                                                                ON_DURATION_MS,
                                                                COUNT);
  event_controller.addStartFunctor(event_id_pair,makeFunctor((Functor1<int> *)0,setGetInfoFalseHandler));
  event_controller.addStopFunctor(event_id_pair,makeFunctor((Functor1<int> *)0,setGetInfoTrueHandler));
  event_controller.enable(event_id_pair);
}


void loop()
{
  if (get_info)
  {
    getInfo();
    delay(500);
  }
}
