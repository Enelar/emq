#ifndef _ED_KIT_EVENT_DATA_H_
#define _ED_KIT_EVENT_DATA_H_

#include "../messages/message.h"

namespace ed
{
  struct event_data
  {
    const message origin;

    operator const message &() const
    {
      return origin;
    }

    const message &operator->() const
    {
      return origin;
    }

    const event_data &const Base() const
    {
      return *this;
    }

    event_data &Base() const
    {
      return *this;
    }
  }
};

#endif