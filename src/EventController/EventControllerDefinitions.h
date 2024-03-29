// ----------------------------------------------------------------------------
// EventControllerDefinitions.h
//
//
// Authors:
// Peter Polidoro peter@polidoro.io
// ----------------------------------------------------------------------------
#ifndef EVENT_CONTROLLER_DEFINITIONS_H
#define EVENT_CONTROLLER_DEFINITIONS_H


template <uint8_t EVENT_COUNT_MAX>
EventController<EVENT_COUNT_MAX>::EventController()
{
  timer_number_ = 1;
  millis_ = 0;
}

template <uint8_t EVENT_COUNT_MAX>
void EventController<EVENT_COUNT_MAX>::setup(size_t timer_number)
{
  if ((timer_number == 1) || (timer_number == 3))
  {
    timer_number_ = timer_number;
  }
  else
  {
    timer_number_ = 1;
  }
  event_array_.fill(Event());
  removeAllEvents();
  startTimer();
}

template <uint8_t EVENT_COUNT_MAX>
uint32_t EventController<EVENT_COUNT_MAX>::getTime()
{
  uint32_t time;
  noInterrupts();
  time = millis_;
  interrupts();
  return time;
}

template <uint8_t EVENT_COUNT_MAX>
void EventController<EVENT_COUNT_MAX>::setTime(uint32_t time)
{
  noInterrupts();
  millis_ = time;
  interrupts();
}

template <uint8_t EVENT_COUNT_MAX>
EventId EventController<EVENT_COUNT_MAX>::addEvent(const Functor1<int> & functor,
  int arg)
{
  return addEventUsingTime(functor,
    0,
    arg);
}

template <uint8_t EVENT_COUNT_MAX>
EventId EventController<EVENT_COUNT_MAX>::addRecurringEvent(const Functor1<int> & functor,
  uint32_t period_ms,
  int32_t count,
  int arg)

{
  if (count < 0)
  {
    return addInfiniteRecurringEvent(functor,period_ms,arg);
  }
  return addRecurringEventUsingTime(functor,
    0,
    period_ms,
    count,
    arg);
}

template <uint8_t EVENT_COUNT_MAX>
EventId EventController<EVENT_COUNT_MAX>::addInfiniteRecurringEvent(const Functor1<int> & functor,
  uint32_t period_ms,
  int arg)
{
  return addInfiniteRecurringEventUsingTime(functor,
    0,
    period_ms,
    arg);
}

template <uint8_t EVENT_COUNT_MAX>
EventId EventController<EVENT_COUNT_MAX>::addEventUsingTime(const Functor1<int> & functor,
  uint32_t time,
  int arg)
{
  uint32_t time_start = getTime();
  uint8_t event_index = findAvailableEventIndex();
  if (event_index < EVENT_COUNT_MAX)
  {
    Event & event = event_array_[event_index];
    event.functor = functor;
    event.time_start = time_start;
    event.time = time;
    event.free = false;
    event.enabled = false;
    event.infinite = false;
    event.count = 1;
    event.period_ms = 0;
    event.inc = 0;
    event.arg = arg;
  }
  EventId event_id;
  event_id.index = event_index;
  event_id.time_start = time_start;
  return event_id;
}

template <uint8_t EVENT_COUNT_MAX>
EventId EventController<EVENT_COUNT_MAX>::addRecurringEventUsingTime(const Functor1<int> & functor,
  uint32_t time,
  uint32_t period_ms,
  int32_t count,
  int arg)
{
  if (count < 0)
  {
    return addInfiniteRecurringEventUsingTime(functor,time,period_ms,arg);
  }
  uint32_t time_start = getTime();
  uint8_t event_index = findAvailableEventIndex();
  if (event_index < EVENT_COUNT_MAX)
  {
    Event & event = event_array_[event_index];
    event.functor = functor;
    event.time_start = time_start;
    event.time = time;
    event.free = false;
    event.enabled = false;
    event.infinite = false;
    event.period_ms = period_ms;
    event.count = count;
    event.inc = 0;
    event.arg = arg;
  }
  EventId event_id;
  event_id.index = event_index;
  event_id.time_start = time_start;
  return event_id;
}

template <uint8_t EVENT_COUNT_MAX>
EventId EventController<EVENT_COUNT_MAX>::addInfiniteRecurringEventUsingTime(const Functor1<int> & functor,
  uint32_t time,
  uint32_t period_ms,
  int arg)
{
  uint32_t time_start = getTime();
  uint8_t event_index = findAvailableEventIndex();
  if (event_index < EVENT_COUNT_MAX)
  {
    Event & event = event_array_[event_index];
    event.functor = functor;
    event.time_start = time_start;
    event.time = time;
    event.free = false;
    event.enabled = false;
    event.infinite = true;
    event.period_ms = period_ms;
    event.count = 0;
    event.inc = 0;
    event.arg = arg;
  }
  EventId event_id;
  event_id.index = event_index;
  event_id.time_start = time_start;
  return event_id;
}

template <uint8_t EVENT_COUNT_MAX>
EventId EventController<EVENT_COUNT_MAX>::addEventUsingDelay(const Functor1<int> & functor,
  uint32_t delay,
  int arg)
{
  uint32_t time_now = getTime();
  uint32_t time = time_now + delay;
  return addEventUsingTime(functor,
    time,
    arg);
}

template <uint8_t EVENT_COUNT_MAX>
EventId EventController<EVENT_COUNT_MAX>::addRecurringEventUsingDelay(const Functor1<int> & functor,
  uint32_t delay,
  uint32_t period_ms,
  int32_t count,
  int arg)
{
  if (count < 0)
  {
    return addInfiniteRecurringEventUsingDelay(functor,delay,period_ms,arg);
  }
  uint32_t time_now = getTime();
  uint32_t time = time_now + delay;
  return addRecurringEventUsingTime(functor,
    time,
    period_ms,
    count,
    arg);
}

template <uint8_t EVENT_COUNT_MAX>
EventId EventController<EVENT_COUNT_MAX>::addInfiniteRecurringEventUsingDelay(const Functor1<int> & functor,
  uint32_t delay,
  uint32_t period_ms,
  int arg)
{
  uint32_t time_now = getTime();
  uint32_t time = time_now + delay;
  return addInfiniteRecurringEventUsingTime(functor,
    time,
    period_ms,
    arg);
}

template <uint8_t EVENT_COUNT_MAX>
EventId EventController<EVENT_COUNT_MAX>::addEventUsingOffset(const Functor1<int> & functor,
  const EventId event_id_origin,
  uint32_t offset,
  int arg)
{
  uint8_t event_index_origin = event_id_origin.index;
  if (event_index_origin < EVENT_COUNT_MAX)
  {
    uint32_t time_origin = event_array_[event_index_origin].time;
    uint32_t time = time_origin + offset;
    return addEventUsingTime(functor,
      time,
      arg);
  }
  else
  {
    return EventId();
  }
}

template <uint8_t EVENT_COUNT_MAX>
EventId EventController<EVENT_COUNT_MAX>::addRecurringEventUsingOffset(const Functor1<int> & functor,
  const EventId event_id_origin,
  uint32_t offset,
  uint32_t period_ms,
  int32_t count,
  int arg)
{
  if (count < 0)
  {
    return addInfiniteRecurringEventUsingOffset(functor,event_id_origin,offset,period_ms,arg);
  }
  uint8_t event_index_origin = event_id_origin.index;
  if (event_index_origin < EVENT_COUNT_MAX)
  {
    uint32_t time_origin = event_array_[event_index_origin].time;
    uint32_t time = time_origin + offset;
    return addRecurringEventUsingTime(functor,
      time,
      period_ms,
      count,
      arg);
  }
  else
  {
    return EventId();
  }
}

template <uint8_t EVENT_COUNT_MAX>
EventId EventController<EVENT_COUNT_MAX>::addInfiniteRecurringEventUsingOffset(const Functor1<int> & functor,
  const EventId event_id_origin,
  uint32_t offset,
  uint32_t period_ms,
  int arg)
{
  uint8_t event_index_origin = event_id_origin.index;
  if (event_index_origin < EVENT_COUNT_MAX)
  {
    uint32_t time_origin = event_array_[event_index_origin].time;
    uint32_t time = time_origin + offset;
    return addInfiniteRecurringEventUsingTime(functor,
      time,
      period_ms,
      arg);
  }
  else
  {
    return EventId();
  }
}

template <uint8_t EVENT_COUNT_MAX>
EventIdPair EventController<EVENT_COUNT_MAX>::addPwmUsingTime(const Functor1<int> & functor_0,
  const Functor1<int> & functor_1,
  uint32_t time,
  uint32_t period_ms,
  uint32_t on_duration_ms,
  int32_t count,
  int arg)
{
  if (count < 0)
  {
    return addInfinitePwmUsingTime(functor_0,functor_1,time,period_ms,on_duration_ms,arg);
  }
  EventIdPair event_id_pair;
  if ((period_ms == 0) && (on_duration_ms == 0))
  {
    return event_id_pair;
  }
  if (period_ms < on_duration_ms)
  {
    period_ms = on_duration_ms;
  }
  if ((on_duration_ms > 0) && (on_duration_ms < period_ms))
  {
    event_id_pair.event_id_0 = addRecurringEventUsingTime(functor_0,
      time,
      period_ms,
      count,
      arg);
    event_id_pair.event_id_1 = addRecurringEventUsingOffset(functor_1,
      event_id_pair.event_id_0,
      on_duration_ms,
      period_ms,
      count,
      arg);
  }
  else if (on_duration_ms == 0)
  {
    event_id_pair.event_id_0 = addRecurringEventUsingTime(functor_1,
      time,
      period_ms,
      count,
      arg);
  }
  else
  {
    event_id_pair.event_id_0 = addRecurringEventUsingTime(functor_0,
      time,
      period_ms,
      count,
      arg);
  }
  return event_id_pair;
}

template <uint8_t EVENT_COUNT_MAX>
EventIdPair EventController<EVENT_COUNT_MAX>::addPwmUsingDelay(const Functor1<int> & functor_0,
  const Functor1<int> & functor_1,
  uint32_t delay,
  uint32_t period_ms,
  uint32_t on_duration_ms,
  int32_t count,
  int arg)
{
  if (count < 0)
  {
    return addInfinitePwmUsingDelay(functor_0,functor_1,delay,period_ms,on_duration_ms,arg);
  }
  uint32_t time_now = getTime();
  uint32_t time = time_now + delay;
  return addPwmUsingTime(functor_0,
    functor_1,
    time,
    period_ms,
    on_duration_ms,
    count,
    arg);
}

template <uint8_t EVENT_COUNT_MAX>
EventIdPair EventController<EVENT_COUNT_MAX>::addPwmUsingOffset(const Functor1<int> & functor_0,
  const Functor1<int> & functor_1,
  const EventId event_id_origin,
  uint32_t offset,
  uint32_t period_ms,
  uint32_t on_duration_ms,
  int32_t count,
  int arg)
{
  if (count < 0)
  {
    return addInfinitePwmUsingOffset(functor_0,functor_1,event_id_origin,offset,period_ms,on_duration_ms,arg);
  }
  uint8_t event_index_origin = event_id_origin.index;
  if (event_index_origin < EVENT_COUNT_MAX)
  {
    uint32_t time_origin = event_array_[event_index_origin].time;
    uint32_t time = time_origin + offset;
    return addPwmUsingTime(functor_0,
      functor_1,
      time,
      period_ms,
      on_duration_ms,
      count,
      arg);
  }
  else
  {
    return EventIdPair();
  }
}

template <uint8_t EVENT_COUNT_MAX>
EventIdPair EventController<EVENT_COUNT_MAX>::addInfinitePwmUsingTime(const Functor1<int> & functor_0,
  const Functor1<int> & functor_1,
  uint32_t time,
  uint32_t period_ms,
  uint32_t on_duration_ms,
  int arg)
{
  EventIdPair event_id_pair;
  if ((on_duration_ms > 0) && (on_duration_ms < period_ms))
  {
    event_id_pair.event_id_0 = addInfiniteRecurringEventUsingTime(functor_0,
      time,
      period_ms,
      arg);
    event_id_pair.event_id_1 = addInfiniteRecurringEventUsingOffset(functor_1,
      event_id_pair.event_id_0,
      on_duration_ms,
      period_ms,
      arg);
  }
  else if (on_duration_ms == 0)
  {
    event_id_pair.event_id_0 = addInfiniteRecurringEventUsingTime(functor_1,
      time,
      period_ms,
      arg);
  }
  else
  {
    event_id_pair.event_id_0 = addInfiniteRecurringEventUsingTime(functor_0,
      time,
      period_ms,
      arg);
  }
  return event_id_pair;
}

template <uint8_t EVENT_COUNT_MAX>
EventIdPair EventController<EVENT_COUNT_MAX>::addInfinitePwmUsingDelay(const Functor1<int> & functor_0,
  const Functor1<int> & functor_1,
  uint32_t delay,
  uint32_t period_ms,
  uint32_t on_duration_ms,
  int arg)
{
  uint32_t time_now = getTime();
  uint32_t time = time_now + delay;
  return addInfinitePwmUsingTime(functor_0,
    functor_1,
    time,
    period_ms,
    on_duration_ms,
    arg);
}

template <uint8_t EVENT_COUNT_MAX>
EventIdPair EventController<EVENT_COUNT_MAX>::addInfinitePwmUsingOffset(const Functor1<int> & functor_0,
  const Functor1<int> & functor_1,
  const EventId event_id_origin,
  uint32_t offset,
  uint32_t period_ms,
  uint32_t on_duration_ms,
  int arg)
{
  uint8_t event_index_origin = event_id_origin.index;
  if (event_index_origin < EVENT_COUNT_MAX)
  {
    uint32_t time_origin = event_array_[event_index_origin].time;
    uint32_t time = time_origin + offset;
    return addInfinitePwmUsingTime(functor_0,
      functor_1,
      time,
      period_ms,
      on_duration_ms,
      arg);
  }
  else
  {
    return EventIdPair();
  }
}

template <uint8_t EVENT_COUNT_MAX>
void EventController<EVENT_COUNT_MAX>::addStartFunctor(const EventId event_id,
  const Functor1<int> & functor)
{
  uint8_t event_index = event_id.index;
  if ((event_index < EVENT_COUNT_MAX) &&
    (event_array_[event_index].time_start == event_id.time_start) &&
    !event_array_[event_index].free)
  {
    event_array_[event_index].functor_start = functor;
  }
}

template <uint8_t EVENT_COUNT_MAX>
void EventController<EVENT_COUNT_MAX>::addStopFunctor(const EventId event_id,
  const Functor1<int> & functor)
{
  uint8_t event_index = event_id.index;
  if ((event_index < EVENT_COUNT_MAX) &&
    (event_array_[event_index].time_start == event_id.time_start) &&
    !event_array_[event_index].free)
  {
    event_array_[event_index].functor_stop = functor;
  }
}

template <uint8_t EVENT_COUNT_MAX>
void EventController<EVENT_COUNT_MAX>::replaceFunctor(const EventId event_id,
  const Functor1<int> & functor)
{
  uint8_t event_index = event_id.index;
  if ((event_index < EVENT_COUNT_MAX) &&
    (event_array_[event_index].time_start == event_id.time_start) &&
    !event_array_[event_index].free)
  {
    event_array_[event_index].functor = functor;
  }
}

template <uint8_t EVENT_COUNT_MAX>
void EventController<EVENT_COUNT_MAX>::addStartFunctor(const EventIdPair event_id_pair,
  const Functor1<int> & functor)
{
  const EventId & event_id = event_id_pair.event_id_0;
  uint8_t event_index = event_id.index;
  if ((event_index < EVENT_COUNT_MAX) &&
    (event_array_[event_index].time_start == event_id.time_start) &&
    !event_array_[event_index].free)
  {
    event_array_[event_index].functor_start = functor;
  }
}

template <uint8_t EVENT_COUNT_MAX>
void EventController<EVENT_COUNT_MAX>::addStopFunctor(const EventIdPair event_id_pair,
  const Functor1<int> & functor)
{
  const EventId & event_id = event_id_pair.event_id_0;
  uint8_t event_index = event_id.index;
  if ((event_index < EVENT_COUNT_MAX) &&
    (event_array_[event_index].time_start == event_id.time_start) &&
    !event_array_[event_index].free)
  {
    event_array_[event_index].functor_stop = functor;
  }
}

template <uint8_t EVENT_COUNT_MAX>
void EventController<EVENT_COUNT_MAX>::replaceFunctors(const EventIdPair event_id_pair,
  const Functor1<int> & functor_0,
  const Functor1<int> & functor_1)
{
  const EventId & event_id_0 = event_id_pair.event_id_0;
  uint8_t event_index_0 = event_id_0.index;
  if ((event_index_0 < EVENT_COUNT_MAX) &&
    (event_array_[event_index_0].time_start == event_id_0.time_start) &&
    !event_array_[event_index_0].free)
  {
    event_array_[event_index_0].functor = functor_0;
  }

  const EventId & event_id_1 = event_id_pair.event_id_1;
  uint8_t event_index_1 = event_id_1.index;
  if ((event_index_1 < EVENT_COUNT_MAX) &&
    (event_array_[event_index_1].time_start == event_id_1.time_start) &&
    !event_array_[event_index_1].free)
  {
    event_array_[event_index_1].functor = functor_1;
  }
}

template <uint8_t EVENT_COUNT_MAX>
void EventController<EVENT_COUNT_MAX>::remove(const EventId event_id)
{
  uint8_t event_index = event_id.index;
  if ((event_index < EVENT_COUNT_MAX) && (event_array_[event_index].time_start == event_id.time_start))
  {
    remove(event_index);
  }
}

template <uint8_t EVENT_COUNT_MAX>
void EventController<EVENT_COUNT_MAX>::remove(const EventIdPair event_id_pair)
{
  remove(event_id_pair.event_id_0);
  remove(event_id_pair.event_id_1);
}

template <uint8_t EVENT_COUNT_MAX>
void EventController<EVENT_COUNT_MAX>::remove(uint8_t event_index)
{
  if (event_index < EVENT_COUNT_MAX)
  {
    Event & event = event_array_[event_index];
    if (event.functor_stop)
    {
      event.functor_stop(event.arg);
    }
    clear(event_index);
  }
}

template <uint8_t EVENT_COUNT_MAX>
void EventController<EVENT_COUNT_MAX>::removeAllEvents()
{
  for (size_t i=0; i<EVENT_COUNT_MAX; ++i)
  {
    remove(i);
  }
  millis_ = 0;
}

template <uint8_t EVENT_COUNT_MAX>
void EventController<EVENT_COUNT_MAX>::clear(const EventId event_id)
{
  uint8_t event_index = event_id.index;
  if ((event_index < EVENT_COUNT_MAX) && (event_array_[event_index].time_start == event_id.time_start))
  {
    clear(event_index);
  }
}

template <uint8_t EVENT_COUNT_MAX>
void EventController<EVENT_COUNT_MAX>::clear(const EventIdPair event_id_pair)
{
  clear(event_id_pair.event_id_0);
  clear(event_id_pair.event_id_1);
}

template <uint8_t EVENT_COUNT_MAX>
void EventController<EVENT_COUNT_MAX>::clear(uint8_t event_index)
{
  if (event_index < EVENT_COUNT_MAX)
  {
    Event & event = event_array_[event_index];
    event.functor = functor_dummy_;
    event.time_start = 0;
    event.time = 0;
    event.free = true;
    event.enabled = false;
    event.infinite = false;
    event.period_ms = 0;
    event.count = 0;
    event.inc = 0;
    event.arg = -1;
    event.functor_start = functor_dummy_;
    event.functor_stop = functor_dummy_;
  }
}

template <uint8_t EVENT_COUNT_MAX>
void EventController<EVENT_COUNT_MAX>::clearAllEvents()
{
  for (size_t i=0; i<EVENT_COUNT_MAX; ++i)
  {
    clear(i);
  }
  millis_ = 0;
}

template <uint8_t EVENT_COUNT_MAX>
void EventController<EVENT_COUNT_MAX>::enable(const EventId event_id)
{
  uint8_t event_index = event_id.index;
  if ((event_index < EVENT_COUNT_MAX) &&
    (event_array_[event_index].time_start == event_id.time_start) &&
    !event_array_[event_index].free)
  {
    event_array_[event_index].enabled = true;
  }
}

template <uint8_t EVENT_COUNT_MAX>
void EventController<EVENT_COUNT_MAX>::enable(const EventIdPair event_id_pair)
{
  enable(event_id_pair.event_id_0);
  enable(event_id_pair.event_id_1);
}

template <uint8_t EVENT_COUNT_MAX>
void EventController<EVENT_COUNT_MAX>::enable(uint8_t event_index)
{
  if ((event_index < EVENT_COUNT_MAX) && !event_array_[event_index].free)
  {
    event_array_[event_index].enabled = true;
  }
}

template <uint8_t EVENT_COUNT_MAX>
void EventController<EVENT_COUNT_MAX>::disable(const EventId event_id)
{
  uint8_t event_index = event_id.index;
  if ((event_index < EVENT_COUNT_MAX) &&
    (event_array_[event_index].time_start == event_id.time_start) &&
    !event_array_[event_index].free)
  {
    event_array_[event_index].enabled = false;
  }
}

template <uint8_t EVENT_COUNT_MAX>
void EventController<EVENT_COUNT_MAX>::disable(const EventIdPair event_id_pair)
{
  disable(event_id_pair.event_id_0);
  disable(event_id_pair.event_id_1);
}

template <uint8_t EVENT_COUNT_MAX>
void EventController<EVENT_COUNT_MAX>::disable(uint8_t event_index)
{
  if ((event_index < EVENT_COUNT_MAX) && !event_array_[event_index].free)
  {
    event_array_[event_index].enabled = false;
  }
}

template <uint8_t EVENT_COUNT_MAX>
Event EventController<EVENT_COUNT_MAX>::getEvent(const EventId event_id)
{
  uint8_t event_index = event_id.index;
  if (event_index < EVENT_COUNT_MAX)
  {
    return event_array_[event_index];
  }
  else
  {
    return Event();
  }
}

template <uint8_t EVENT_COUNT_MAX>
Event EventController<EVENT_COUNT_MAX>::getEvent(uint8_t event_index)
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

template <uint8_t EVENT_COUNT_MAX>
void EventController<EVENT_COUNT_MAX>::setEventArgToEventIndex(const EventId event_id)
{
  uint8_t event_index = event_id.index;
  if (event_index < EVENT_COUNT_MAX)
  {
    event_array_[event_index].arg = event_index;
  }
}

template <uint8_t EVENT_COUNT_MAX>
uint8_t EventController<EVENT_COUNT_MAX>::eventsActive()
{
  uint8_t events_active = 0;
  for (uint8_t event_index=0; event_index<event_array_.size(); ++event_index)
  {
    if ((!event_array_[event_index].free) && event_array_[event_index].enabled)
    {
      ++events_active;
    }
  }
  return events_active;
}

template <uint8_t EVENT_COUNT_MAX>
uint8_t EventController<EVENT_COUNT_MAX>::eventsAvailable()
{
  uint8_t events_available = 0;
  for (uint8_t event_index=0; event_index<event_array_.size(); ++event_index)
  {
    if (event_array_[event_index].free)
    {
      ++events_available;
    }
  }
  return events_available;
}

template <uint8_t EVENT_COUNT_MAX>
Array<Event,EVENT_COUNT_MAX> EventController<EVENT_COUNT_MAX>::getEventArray()
{
  return event_array_;
}

template <uint8_t EVENT_COUNT_MAX>
void EventController<EVENT_COUNT_MAX>::startTimer()
{
  noInterrupts();
  if (timer_number_ == 1)
  {
    Timer1.initialize(MICRO_SEC_PER_MILLI_SEC);
  }
  else if (timer_number_ == 3)
  {
    Timer3.initialize(MICRO_SEC_PER_MILLI_SEC);
  }
  FunctorCallbacks::Callback callback = FunctorCallbacks::add(makeFunctor((Functor0 *)0,*this,&EventController<EVENT_COUNT_MAX>::update));
  if (callback)
  {
    if (timer_number_ == 1)
    {
      Timer1.attachInterrupt(callback);
    }
    else if (timer_number_ == 3)
    {
      Timer3.attachInterrupt(callback);
    }
  }
  interrupts();
}

template <uint8_t EVENT_COUNT_MAX>
uint8_t EventController<EVENT_COUNT_MAX>::findAvailableEventIndex()
{
  uint8_t event_index = 0;
  while ((event_index < EVENT_COUNT_MAX) && !event_array_[event_index].free)
  {
    ++event_index;
  }
  return event_index;
}

template <uint8_t EVENT_COUNT_MAX>
void EventController<EVENT_COUNT_MAX>::update()
{
  noInterrupts();
  ++millis_;
  interrupts();

  for (uint8_t event_index = 0; event_index < EVENT_COUNT_MAX; ++event_index)
  {
    Event& event = event_array_[event_index];
    if ((!event.free) && (event.time <= millis_))
    {
      if ((event.enabled) && ((event.infinite) || (event.inc < event.count)))
      {
        while ((event.period_ms > 0) &&
          (event.time <= millis_))
        {
          event.time += event.period_ms;
        }
        if (event.functor_start && (event.inc == 0))
        {
          event.functor_start(event.arg);
        }
        if (event.functor)
        {
          event.functor(event.arg);
        }
        ++event.inc;
      }
      else
      {
        remove(event_index);
      }
    }
  }
}

#endif
