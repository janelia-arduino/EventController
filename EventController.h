// ----------------------------------------------------------------------------
// EventController.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef EVENT_CONTROLLER_H
#define EVENT_CONTROLLER_H
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include "Array.h"
#include "TimerOne.h"
#include "Functor.h"
#include "Streaming.h"

void eventControllerUpdate();
typedef uint8_t index_t;

const int EVENT_COUNT_MAX = 32;
const int DEFAULT_INDEX = 255;
const int MICRO_SEC_PER_MILLI_SEC = 1000;

struct Event
{
  Functor1<int> callback;
  uint32_t time_start;
  uint32_t time;
  boolean free;
  boolean enabled;
  boolean infinite;
  uint32_t period_ms;
  uint16_t count;
  uint16_t inc;
  int arg;
  Functor1<int> callback_start;
  Functor1<int> callback_stop;
};

struct EventId
{
  index_t index;
  uint32_t time_start;
  EventId() :
    index(DEFAULT_INDEX),
    time_start(0) {}
};
const EventId default_event_id;

bool operator==(const EventId& lhs, const EventId& rhs);

struct EventIdPair
{
  EventId event_id_0;
  EventId event_id_1;
  EventIdPair() :
    event_id_0(default_event_id),
    event_id_1(default_event_id) {}
};
const EventIdPair default_event_id_pair;

bool operator==(const EventIdPair& lhs, const EventIdPair& rhs);

class EventController
{
public:
  void setup();
  uint32_t getTime();
  void setTime(const uint32_t time=0);
  EventId addEvent(const Functor1<int> & callback,
                   const int arg=-1);
  EventId addRecurringEvent(const Functor1<int> & callback,
                            const uint32_t period_ms,
                            const uint16_t count,
                            const int arg=-1);
  EventId addInfiniteRecurringEvent(const Functor1<int> & callback,
                                    const uint32_t period_ms,
                                    const int arg=-1);
  EventId addEventUsingTime(const Functor1<int> & callback,
                            const uint32_t time,
                            const int arg=-1);
  EventId addRecurringEventUsingTime(const Functor1<int> & callback,
                                     const uint32_t time,
                                     const uint32_t period_ms,
                                     const uint16_t count,
                                     const int arg=-1);
  EventId addInfiniteRecurringEventUsingTime(const Functor1<int> & callback,
                                             const uint32_t time,
                                             const uint32_t period_ms,
                                             const int arg=-1);
  EventId addEventUsingDelay(const Functor1<int> & callback,
                             const uint32_t delay,
                             const int arg=-1);
  EventId addRecurringEventUsingDelay(const Functor1<int> & callback,
                                      const uint32_t delay,
                                      const uint32_t period_ms,
                                      const uint16_t count,
                                      const int arg=-1);
  EventId addInfiniteRecurringEventUsingDelay(const Functor1<int> & callback,
                                              const uint32_t delay,
                                              const uint32_t period_ms,
                                              const int arg=-1);
  EventId addEventUsingOffset(const Functor1<int> & callback,
                              const EventId event_id_origin,
                              const uint32_t offset,
                              const int arg=-1);
  EventId addRecurringEventUsingOffset(const Functor1<int> & callback,
                                       const EventId event_id_origin,
                                       const uint32_t offset,
                                       const uint32_t period_ms,
                                       const uint16_t count,
                                       const int arg=-1);
  EventId addInfiniteRecurringEventUsingOffset(const Functor1<int> & callback,
                                               const EventId event_id_origin,
                                               const uint32_t offset,
                                               const uint32_t period_ms,
                                               const int arg=-1);
  EventIdPair addPwmUsingTime(const Functor1<int> & callback_0,
                              const Functor1<int> & callback_1,
                              const uint32_t time,
                              const uint32_t period_ms,
                              const uint32_t on_duration_ms,
                              const uint16_t count,
                              const int arg=-1);
  EventIdPair addPwmUsingDelay(const Functor1<int> & callback_0,
                               const Functor1<int> & callback_1,
                               const uint32_t delay,
                               const uint32_t period_ms,
                               const uint32_t on_duration_ms,
                               const uint16_t count,
                               const int arg=-1);
  EventIdPair addPwmUsingOffset(const Functor1<int> & callback_0,
                                const Functor1<int> & callback_1,
                                const EventId event_id_origin,
                                const uint32_t offset,
                                const uint32_t period_ms,
                                const uint32_t on_duration_ms,
                                const uint16_t count,
                                const int arg=-1);
  EventIdPair addInfinitePwmUsingTime(const Functor1<int> & callback_0,
                                      const Functor1<int> & callback_1,
                                      const uint32_t time,
                                      const uint32_t period_ms,
                                      const uint32_t on_duration_ms,
                                      const int arg=-1);
  EventIdPair addInfinitePwmUsingDelay(const Functor1<int> & callback_0,
                                       const Functor1<int> & callback_1,
                                       const uint32_t delay,
                                       const uint32_t period_ms,
                                       const uint32_t on_duration_ms,
                                       const int arg=-1);
  EventIdPair addInfinitePwmUsingOffset(const Functor1<int> & callback_0,
                                        const Functor1<int> & callback_1,
                                        const EventId event_id_origin,
                                        const uint32_t offset,
                                        const uint32_t period_ms,
                                        const uint32_t on_duration_ms,
                                        const int arg=-1);
  void addStartCallback(const EventId event_id, const Functor1<int> & callback);
  void addStopCallback(const EventId event_id, const Functor1<int> & callback);
  void addStartCallback(const EventIdPair event_id_pair, const Functor1<int> & callback);
  void addStopCallback(const EventIdPair event_id_pair, const Functor1<int> & callback);
  void remove(const EventId event_id);
  void remove(const EventIdPair event_id_pair);
  void removeAllEvents();
  void enable(const EventId event_id);
  void enable(const EventIdPair event_id_pair);
  void disable(const EventId event_id);
  void disable(const EventIdPair event_id_pair);
  Event getEvent(const EventId event_id);
  Event getEvent(const index_t event_index);
  void setEventArgToEventIndex(const EventId event_id);
  bool activeEvents();
  int countActiveEvents();
  int getEventCountMax();
private:
  volatile uint32_t millis_;
  Array<Event,EVENT_COUNT_MAX> event_array_;
  bool startTimer();
  index_t findAvailableEventIndex();
  void update();
  void remove(const index_t event_index);
  void enable(const index_t event_index);
  void disable(const index_t event_index);
  friend void eventControllerUpdate();
};

extern EventController event_controller;

inline void eventControllerUpdate() {event_controller.update();}

#endif
