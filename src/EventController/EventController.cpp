// ----------------------------------------------------------------------------
// EventController.cpp
//
//
// Authors:
// Peter Polidoro peter@polidoro.io
// ----------------------------------------------------------------------------
#include "../EventController.h"


bool operator==(const EventId& lhs,
  const EventId& rhs)
{
  return (lhs.index == rhs.index) && (lhs.time_start == rhs.time_start);
}

bool operator==(const EventIdPair& lhs,
  const EventIdPair& rhs)
{
  return (lhs.event_id_0 == rhs.event_id_0) && (lhs.event_id_1 == rhs.event_id_1);
}

bool operator!=(const EventId& lhs,
  const EventId& rhs)
{
  return !(lhs == rhs);
}

bool operator!=(const EventIdPair& lhs,
  const EventIdPair& rhs)
{
  return !(lhs == rhs);
}
