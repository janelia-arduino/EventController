// ----------------------------------------------------------------------------
// EventController.cpp
//
//
// Authors:
// Peter Polidoro polidorop@janelia.hhmi.org
// ----------------------------------------------------------------------------
#include "EventController.h"


void EventController::setup()
{
  event_array_.fill(Event());
  removeAllEvents();
  startTimer();
}

uint32_t EventController::getTime()
{
  uint32_t time;
  noInterrupts();
  time = millis_;
  interrupts();
  return time;
}

void EventController::setTime(const uint32_t time)
{
  noInterrupts();
  millis_ = time;
  interrupts();
}

EventId EventController::addEvent(const Functor1<int> & callback,
                                  const int arg)
{
  return addEventUsingTime(callback,
                           0,
                           arg);
}

EventId EventController::addRecurringEvent(const Functor1<int> & callback,
                                           const uint32_t period_ms,
                                           const uint16_t count,
                                           const int arg)

{
  return addRecurringEventUsingTime(callback,
                                    0,
                                    period_ms,
                                    count,
                                    arg);
}

EventId EventController::addInfiniteRecurringEvent(const Functor1<int> & callback,
                                                   const uint32_t period_ms,
                                                   const int arg)
{
  return addInfiniteRecurringEventUsingTime(callback,
                                            0,
                                            period_ms,
                                            arg);
}

EventId EventController::addEventUsingTime(const Functor1<int> & callback,
                                           const uint32_t time,
                                           const int arg)
{
  uint32_t time_start = getTime();
  index_t event_index = findAvailableEventIndex();
  if (event_index < EVENT_COUNT_MAX)
  {
    event_array_[event_index].callback = callback;
    event_array_[event_index].time_start = time_start;
    event_array_[event_index].time = time;
    event_array_[event_index].free = false;
    event_array_[event_index].enabled = false;
    event_array_[event_index].infinite = false;
    event_array_[event_index].count = 1;
    event_array_[event_index].period_ms = 0;
    event_array_[event_index].inc = 0;
    event_array_[event_index].arg = arg;
  }
  EventId event_id;
  event_id.index = event_index;
  event_id.time_start = time_start;
  return event_id;
}

EventId EventController::addRecurringEventUsingTime(const Functor1<int> & callback,
                                                    const uint32_t time,
                                                    const uint32_t period_ms,
                                                    const uint16_t count,
                                                    const int arg)
{
  uint32_t time_start = getTime();
  index_t event_index = findAvailableEventIndex();
  if (event_index < EVENT_COUNT_MAX)
  {
    event_array_[event_index].callback = callback;
    event_array_[event_index].time_start = time_start;
    event_array_[event_index].time = time;
    event_array_[event_index].free = false;
    event_array_[event_index].enabled = false;
    event_array_[event_index].infinite = false;
    event_array_[event_index].period_ms = period_ms;
    event_array_[event_index].count = count;
    event_array_[event_index].inc = 0;
    event_array_[event_index].arg = arg;
  }
  EventId event_id;
  event_id.index = event_index;
  event_id.time_start = time_start;
  return event_id;
}

EventId EventController::addInfiniteRecurringEventUsingTime(const Functor1<int> & callback,
                                                            const uint32_t time,
                                                            const uint32_t period_ms,
                                                            const int arg)
{
  uint32_t time_start = getTime();
  index_t event_index = findAvailableEventIndex();
  if (event_index < EVENT_COUNT_MAX)
  {
    event_array_[event_index].callback = callback;
    event_array_[event_index].time_start = time_start;
    event_array_[event_index].time = time;
    event_array_[event_index].free = false;
    event_array_[event_index].enabled = false;
    event_array_[event_index].infinite = true;
    event_array_[event_index].period_ms = period_ms;
    event_array_[event_index].count = 0;
    event_array_[event_index].inc = 0;
    event_array_[event_index].arg = arg;
  }
  EventId event_id;
  event_id.index = event_index;
  event_id.time_start = time_start;
  return event_id;
}

EventId EventController::addEventUsingDelay(const Functor1<int> & callback,
                                            const uint32_t delay,
                                            const int arg)
{
  uint32_t time_now = getTime();
  uint32_t time = time_now + delay;
  return addEventUsingTime(callback,
                           time,
                           arg);
}

EventId EventController::addRecurringEventUsingDelay(const Functor1<int> & callback,
                                                     const uint32_t delay,
                                                     const uint32_t period_ms,
                                                     const uint16_t count,
                                                     const int arg)
{
  uint32_t time_now = getTime();
  uint32_t time = time_now + delay;
  return addRecurringEventUsingTime(callback,
                                    time,
                                    period_ms,
                                    count,
                                    arg);
}

EventId EventController::addInfiniteRecurringEventUsingDelay(const Functor1<int> & callback,
                                                             const uint32_t delay,
                                                             const uint32_t period_ms,
                                                             const int arg)
{
  uint32_t time_now = getTime();
  uint32_t time = time_now + delay;
  return addInfiniteRecurringEventUsingTime(callback,
                                            time,
                                            period_ms,
                                            arg);
}

EventId EventController::addEventUsingOffset(const Functor1<int> & callback,
                                             const EventId event_id_origin,
                                             const uint32_t offset,
                                             const int arg)
{
  index_t event_index_origin = event_id_origin.index;
  if (event_index_origin < EVENT_COUNT_MAX)
  {
    uint32_t time_origin = event_array_[event_index_origin].time;
    uint32_t time = time_origin + offset;
    return addEventUsingTime(callback,
                             time,
                             arg);
  }
  else
  {
    return default_event_id;
  }
}

EventId EventController::addRecurringEventUsingOffset(const Functor1<int> & callback,
                                                      const EventId event_id_origin,
                                                      const uint32_t offset,
                                                      const uint32_t period_ms,
                                                      const uint16_t count,
                                                      const int arg)
{
  index_t event_index_origin = event_id_origin.index;
  if (event_index_origin < EVENT_COUNT_MAX)
  {
    uint32_t time_origin = event_array_[event_index_origin].time;
    uint32_t time = time_origin + offset;
    return addRecurringEventUsingTime(callback,
                                      time,
                                      period_ms,
                                      count,
                                      arg);
  }
  else
  {
    return default_event_id;
  }
}

EventId EventController::addInfiniteRecurringEventUsingOffset(const Functor1<int> & callback,
                                                              const EventId event_id_origin,
                                                              const uint32_t offset,
                                                              const uint32_t period_ms,
                                                              const int arg)
{
  index_t event_index_origin = event_id_origin.index;
  if (event_index_origin < EVENT_COUNT_MAX)
  {
    uint32_t time_origin = event_array_[event_index_origin].time;
    uint32_t time = time_origin + offset;
    return addInfiniteRecurringEventUsingTime(callback,
                                              time,
                                              period_ms,
                                              arg);
  }
  else
  {
    return default_event_id;
  }
}

EventIdPair EventController::addPwmUsingTime(const Functor1<int> & callback_0,
                                             const Functor1<int> & callback_1,
                                             const uint32_t time,
                                             const uint32_t period_ms,
                                             const uint32_t on_duration_ms,
                                             const uint16_t count,
                                             const int arg)
{
  EventIdPair event_id_pair;
  event_id_pair.event_id_0 = addRecurringEventUsingTime(callback_0,
                                                        time,
                                                        period_ms,
                                                        count,
                                                        arg);
  event_id_pair.event_id_1 = addRecurringEventUsingOffset(callback_1,
                                                          event_id_pair.event_id_0,
                                                          on_duration_ms,
                                                          period_ms,
                                                          count,
                                                          arg);
  return event_id_pair;
}

EventIdPair EventController::addPwmUsingDelay(const Functor1<int> & callback_0,
                                              const Functor1<int> & callback_1,
                                              const uint32_t delay,
                                              const uint32_t period_ms,
                                              const uint32_t on_duration_ms,
                                              const uint16_t count,
                                              const int arg)
{
  uint32_t time_now = getTime();
  uint32_t time = time_now + delay;
  return addPwmUsingTime(callback_0,
                         callback_1,
                         time,
                         period_ms,
                         on_duration_ms,
                         count,
                         arg);
}

EventIdPair EventController::addPwmUsingOffset(const Functor1<int> & callback_0,
                                               const Functor1<int> & callback_1,
                                               const EventId event_id_origin,
                                               const uint32_t offset,
                                               const uint32_t period_ms,
                                               const uint32_t on_duration_ms,
                                               const uint16_t count,
                                               const int arg)
{
  index_t event_index_origin = event_id_origin.index;
  if (event_index_origin < EVENT_COUNT_MAX)
  {
    uint32_t time_origin = event_array_[event_index_origin].time;
    uint32_t time = time_origin + offset;
    return addPwmUsingTime(callback_0,
                           callback_1,
                           time,
                           period_ms,
                           on_duration_ms,
                           count,
                           arg);
  }
  else
  {
    return default_event_id_pair;
  }
}

EventIdPair EventController::addInfinitePwmUsingTime(const Functor1<int> & callback_0,
                                                     const Functor1<int> & callback_1,
                                                     const uint32_t time,
                                                     const uint32_t period_ms,
                                                     const uint32_t on_duration_ms,
                                                     const int arg)
{
  EventIdPair event_id_pair;
  event_id_pair.event_id_0 = addInfiniteRecurringEventUsingTime(callback_0,
                                                                time,
                                                                period_ms,
                                                                arg);
  event_id_pair.event_id_1 = addInfiniteRecurringEventUsingOffset(callback_1,
                                                                  event_id_pair.event_id_0,
                                                                  on_duration_ms,
                                                                  period_ms,
                                                                  arg);
  return event_id_pair;
}

EventIdPair EventController::addInfinitePwmUsingDelay(const Functor1<int> & callback_0,
                                                      const Functor1<int> & callback_1,
                                                      const uint32_t delay,
                                                      const uint32_t period_ms,
                                                      const uint32_t on_duration_ms,
                                                      const int arg)
{
  uint32_t time_now = getTime();
  uint32_t time = time_now + delay;
  return addInfinitePwmUsingTime(callback_0,
                                 callback_1,
                                 time,
                                 period_ms,
                                 on_duration_ms,
                                 arg);
}

EventIdPair EventController::addInfinitePwmUsingOffset(const Functor1<int> & callback_0,
                                                       const Functor1<int> & callback_1,
                                                       const EventId event_id_origin,
                                                       const uint32_t offset,
                                                       const uint32_t period_ms,
                                                       const uint32_t on_duration_ms,
                                                       const int arg)
{
  index_t event_index_origin = event_id_origin.index;
  if (event_index_origin < EVENT_COUNT_MAX)
  {
    uint32_t time_origin = event_array_[event_index_origin].time;
    uint32_t time = time_origin + offset;
    return addInfinitePwmUsingTime(callback_0,
                                   callback_1,
                                   time,
                                   period_ms,
                                   on_duration_ms,
                                   arg);
  }
  else
  {
    return default_event_id_pair;
  }
}

void EventController::addStartCallback(const EventId event_id, const Functor1<int> & callback)
{
  index_t event_index = event_id.index;
  if ((event_index < EVENT_COUNT_MAX) &&
      (event_array_[event_index].time_start == event_id.time_start) &&
      !event_array_[event_index].free)
  {
    event_array_[event_index].callback_start = callback;
  }
}

void EventController::addStopCallback(const EventId event_id, const Functor1<int> & callback)
{
  index_t event_index = event_id.index;
  if ((event_index < EVENT_COUNT_MAX) &&
      (event_array_[event_index].time_start == event_id.time_start) &&
      !event_array_[event_index].free)
  {
    event_array_[event_index].callback_stop = callback;
  }
}

void EventController::addStartCallback(const EventIdPair event_id_pair, const Functor1<int> & callback)
{
  const EventId & event_id = event_id_pair.event_id_0;
  index_t event_index = event_id.index;
  if ((event_index < EVENT_COUNT_MAX) &&
      (event_array_[event_index].time_start == event_id.time_start) &&
      !event_array_[event_index].free)
  {
    event_array_[event_index].callback_start = callback;
  }
}

void EventController::addStopCallback(const EventIdPair event_id_pair, const Functor1<int> & callback)
{
  const EventId & event_id = event_id_pair.event_id_0;
  index_t event_index = event_id.index;
  if ((event_index < EVENT_COUNT_MAX) &&
      (event_array_[event_index].time_start == event_id.time_start) &&
      !event_array_[event_index].free)
  {
    event_array_[event_index].callback_stop = callback;
  }
}

void EventController::remove(const EventId event_id)
{
  index_t event_index = event_id.index;
  if ((event_index < EVENT_COUNT_MAX) && (event_array_[event_index].time_start == event_id.time_start))
  {
    remove(event_index);
  }
}

void EventController::remove(const EventIdPair event_id_pair)
{
  remove(event_id_pair.event_id_0);
  remove(event_id_pair.event_id_1);
}

void EventController::remove(const index_t event_index)
{
  if (event_index < EVENT_COUNT_MAX)
  {
    event_array_[event_index].time_start = 0;
    event_array_[event_index].time = 0;
    event_array_[event_index].free = true;
    event_array_[event_index].enabled = false;
    event_array_[event_index].infinite = false;
    event_array_[event_index].period_ms = 0;
    event_array_[event_index].count = 0;
    event_array_[event_index].inc = 0;
    event_array_[event_index].arg = -1;
  }
}

void EventController::removeAllEvents()
{
  for (size_t i=0; i<EVENT_COUNT_MAX; ++i)
  {
    remove(i);
  }
}

void EventController::enable(const EventId event_id)
{
  index_t event_index = event_id.index;
  if ((event_index < EVENT_COUNT_MAX) &&
      (event_array_[event_index].time_start == event_id.time_start) &&
      !event_array_[event_index].free)
  {
    event_array_[event_index].enabled = true;
  }
}

void EventController::enable(const EventIdPair event_id_pair)
{
  enable(event_id_pair.event_id_0);
  enable(event_id_pair.event_id_1);
}

void EventController::enable(const index_t event_index)
{
  if ((event_index < EVENT_COUNT_MAX) && !event_array_[event_index].free)
  {
    event_array_[event_index].enabled = true;
  }
}

void EventController::disable(const EventId event_id)
{
  index_t event_index = event_id.index;
  if ((event_index < EVENT_COUNT_MAX) &&
      (event_array_[event_index].time_start == event_id.time_start) &&
      !event_array_[event_index].free)
  {
    event_array_[event_index].enabled = false;
  }
}

void EventController::disable(const EventIdPair event_id_pair)
{
  disable(event_id_pair.event_id_0);
  disable(event_id_pair.event_id_1);
}

void EventController::disable(const index_t event_index)
{
  if ((event_index < EVENT_COUNT_MAX) && !event_array_[event_index].free)
  {
    event_array_[event_index].enabled = false;
  }
}

Event EventController::getEvent(const EventId event_id)
{
  index_t event_index = event_id.index;
  if (event_index < EVENT_COUNT_MAX)
  {
    return event_array_[event_index];
  }
  else
  {
    return Event();
  }
}

Event EventController::getEvent(const index_t event_index)
{
  if (event_index < EVENT_COUNT_MAX)
  {
    return event_array_[event_index];
  }
  else
  {
    return Event();
  }
}

void EventController::setEventArgToEventIndex(const EventId event_id)
{
  index_t event_index = event_id.index;
  if (event_index < EVENT_COUNT_MAX)
  {
    event_array_[event_index].arg = event_index;
  }
}

bool EventController::activeEvents()
{
  return (countActiveEvents() > 0);
}

int EventController::countActiveEvents()
{
  int active_events = 0;
  for (index_t event_index=0; event_index<event_array_.size(); ++event_index)
  {
    if ((!event_array_[event_index].free) && event_array_[event_index].enabled)
    {
      ++active_events;
    }
  }
  return active_events;
}

bool EventController::startTimer()
{
  noInterrupts();
  Timer1.initialize(MICRO_SEC_PER_MILLI_SEC);
  Timer1.attachInterrupt(eventControllerUpdate);
  interrupts();
}

index_t EventController::findAvailableEventIndex()
{
  index_t event_index = 0;
  while ((event_index < EVENT_COUNT_MAX) && !event_array_[event_index].free)
  {
    ++event_index;
  }
  return event_index;
}

void EventController::update()
{
  noInterrupts();
  ++millis_;
  interrupts();

  for (index_t event_index = 0; event_index < EVENT_COUNT_MAX; ++event_index)
  {
    Event& event = event_array_[event_index];
    if ((!event.free) && (event.time <= millis_))
    {
      if ((event.infinite) || (event.inc < event.count))
      {
        while ((event.period_ms > 0) &&
               (event.time <= millis_))
        {
          event.time += event.period_ms;
        }
        if (event.enabled)
        {
          if (event.callback_start && (event.inc == 0))
          {
            event.callback_start(event.arg);
          }
          event.callback(event.arg);
          ++event.inc;
        }
      }
      else
      {
        if (event.callback_stop)
        {
          event.callback_stop(event.arg);
        }
        remove(event_index);
      }
    }
  }
}

EventController event_controller;

bool operator==(const EventId& lhs, const EventId& rhs)
{
  return (lhs.index == rhs.index) && (lhs.time_start == rhs.time_start);
}

bool operator==(const EventIdPair& lhs, const EventIdPair& rhs)
{
  return (lhs.event_id_0 == rhs.event_id_0) && (lhs.event_id_1 == rhs.event_id_1);
}
