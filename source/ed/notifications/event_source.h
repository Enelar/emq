#ifndef _ED_NOTIFICATIONS_EVENT_SOURCE_H_
#define _ED_NOTIFICATIONS_EVENT_SOURCE_H_

#include "../names/reserved.h"
namespace ed
{
  class gateway;
  struct event_source
  {
    int instance, module, event;
    
    event_source() :
      event(reserved::event::BROADCAST), 
      module(reserved::module::BROADCAST),
      instance(reserved::instance::BROADCAST)
    {
    }

    bool operator==( const event_source &a ) const
    {
      return instance == a.instance && module == a.module && event == a.event;
    }
  };
};

#endif