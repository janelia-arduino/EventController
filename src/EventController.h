// ----------------------------------------------------------------------------
// EventController.h
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#ifndef EVENT_CONTROLLER_H
#define EVENT_CONTROLLER_H
#include <Arduino.h>
#include <Array.h>
#include <TimerOne.h>
#include <TimerThree.h>
#include <Functor.h>
#include <FunctorCallbacks.h>


struct Event
{
  Functor1<int> functor;
  uint32_t time_start;
  uint32_t time;
  bool free;
  bool enabled;
  bool infinite;
  uint32_t period_ms;
  uint16_t count;
  uint16_t inc;
  int arg;
  Functor1<int> functor_start;
  Functor1<int> functor_stop;
};
struct EventId
{
  uint8_t index;
  uint32_t time_start;
  EventId() :
    index(255),
    time_start(0) {}
};
struct EventIdPair
{
  EventId event_id_0;
  EventId event_id_1;
  EventIdPair() :
    event_id_0(EventId()),
    event_id_1(EventId()) {}
};

template <uint8_t EVENT_COUNT_MAX>
class EventController
{
public:
  EventController();
  EventController(const size_t timer_number);
  enum{MICRO_SEC_PER_MILLI_SEC=1000};
  void setup(const size_t timer_number=1);
  uint32_t getTime();
  void setTime(const uint32_t time=0);
  EventId addEvent(const Functor1<int> & functor,
                   const int arg=-1);
  EventId addRecurringEvent(const Functor1<int> & functor,
                            const uint32_t period_ms,
                            const int32_t count,
                            const int arg=-1);
  EventId addInfiniteRecurringEvent(const Functor1<int> & functor,
                                    const uint32_t period_ms,
                                    const int arg=-1);
  EventId addEventUsingTime(const Functor1<int> & functor,
                            const uint32_t time,
                            const int arg=-1);
  EventId addRecurringEventUsingTime(const Functor1<int> & functor,
                                     const uint32_t time,
                                     const uint32_t period_ms,
                                     const int32_t count,
                                     const int arg=-1);
  EventId addInfiniteRecurringEventUsingTime(const Functor1<int> & functor,
                                             const uint32_t time,
                                             const uint32_t period_ms,
                                             const int arg=-1);
  EventId addEventUsingDelay(const Functor1<int> & functor,
                             const uint32_t delay,
                             const int arg=-1);
  EventId addRecurringEventUsingDelay(const Functor1<int> & functor,
                                      const uint32_t delay,
                                      const uint32_t period_ms,
                                      const int32_t count,
                                      const int arg=-1);
  EventId addInfiniteRecurringEventUsingDelay(const Functor1<int> & functor,
                                              const uint32_t delay,
                                              const uint32_t period_ms,
                                              const int arg=-1);
  EventId addEventUsingOffset(const Functor1<int> & functor,
                              const EventId event_id_origin,
                              const uint32_t offset,
                              const int arg=-1);
  EventId addRecurringEventUsingOffset(const Functor1<int> & functor,
                                       const EventId event_id_origin,
                                       const uint32_t offset,
                                       const uint32_t period_ms,
                                       const int32_t count,
                                       const int arg=-1);
  EventId addInfiniteRecurringEventUsingOffset(const Functor1<int> & functor,
                                               const EventId event_id_origin,
                                               const uint32_t offset,
                                               const uint32_t period_ms,
                                               const int arg=-1);
  EventIdPair addPwmUsingTime(const Functor1<int> & functor_0,
                              const Functor1<int> & functor_1,
                              const uint32_t time,
                              const uint32_t period_ms,
                              const uint32_t on_duration_ms,
                              const int32_t count,
                              const int arg=-1);
  EventIdPair addPwmUsingDelay(const Functor1<int> & functor_0,
                               const Functor1<int> & functor_1,
                               const uint32_t delay,
                               const uint32_t period_ms,
                               const uint32_t on_duration_ms,
                               const int32_t count,
                               const int arg=-1);
  EventIdPair addPwmUsingOffset(const Functor1<int> & functor_0,
                                const Functor1<int> & functor_1,
                                const EventId event_id_origin,
                                const uint32_t offset,
                                const uint32_t period_ms,
                                const uint32_t on_duration_ms,
                                const int32_t count,
                                const int arg=-1);
  EventIdPair addInfinitePwmUsingTime(const Functor1<int> & functor_0,
                                      const Functor1<int> & functor_1,
                                      const uint32_t time,
                                      const uint32_t period_ms,
                                      const uint32_t on_duration_ms,
                                      const int arg=-1);
  EventIdPair addInfinitePwmUsingDelay(const Functor1<int> & functor_0,
                                       const Functor1<int> & functor_1,
                                       const uint32_t delay,
                                       const uint32_t period_ms,
                                       const uint32_t on_duration_ms,
                                       const int arg=-1);
  EventIdPair addInfinitePwmUsingOffset(const Functor1<int> & functor_0,
                                        const Functor1<int> & functor_1,
                                        const EventId event_id_origin,
                                        const uint32_t offset,
                                        const uint32_t period_ms,
                                        const uint32_t on_duration_ms,
                                        const int arg=-1);
  void addStartFunctor(const EventId event_id, const Functor1<int> & functor);
  void addStopFunctor(const EventId event_id, const Functor1<int> & functor);
  void addStartFunctor(const EventIdPair event_id_pair, const Functor1<int> & functor);
  void addStopFunctor(const EventIdPair event_id_pair, const Functor1<int> & functor);
  void remove(const EventId event_id);
  void remove(const EventIdPair event_id_pair);
  void removeAllEvents();
  void clear(const EventId event_id);
  void clear(const EventIdPair event_id_pair);
  void clearAllEvents();
  void enable(const EventId event_id);
  void enable(const EventIdPair event_id_pair);
  void disable(const EventId event_id);
  void disable(const EventIdPair event_id_pair);
  Event getEvent(const EventId event_id);
  Event getEvent(const uint8_t event_index);
  void setEventArgToEventIndex(const EventId event_id);
  uint8_t eventsActive();
  uint8_t eventsAvailable();
  Array<Event,EVENT_COUNT_MAX> getEventArray();
private:
  volatile uint32_t millis_;
  Array<Event,EVENT_COUNT_MAX> event_array_;
  const Functor1<int> functor_dummy_;
  size_t timer_number_;

  void startTimer();
  uint8_t findAvailableEventIndex();
  void update();
  void remove(const uint8_t event_index);
  void clear(const uint8_t event_index);
  void enable(const uint8_t event_index);
  void disable(const uint8_t event_index);
};

bool operator==(const EventId& lhs, const EventId& rhs);
bool operator==(const EventIdPair& lhs, const EventIdPair& rhs);

#include "EventController/EventControllerDefinitions.h"

#endif
