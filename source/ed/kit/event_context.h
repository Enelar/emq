#ifndef _ED_KIT_EVENT_CONTEXT_H_
#define _ED_KIT_EVENT_CONTEXT_H_

#include "event_data.h"

namespace ed
{
  template<typename dataT = event_data>
  struct event_context;

  template<>
  struct event_context<event_data>
  {
    typedef event_data dataT;
    int event_local_id;
    const event_source source;
    dataT *const payload;

    event_context( int _event_local_id, const event_source &_source, dataT *const _payload  )
      : event_local_id(_event_local_id), source(_source), payload(_payload)
    {
      throw_assert(payload);
    }

    event_context( const event_context &a )
      : event_local_id(a.event_local_id), source(a.source), payload(NEW event_data(*a.payload))
    {

    }

    ~event_context()
    {
      delete payload;
    }
  };

  template<typename dataT>
  struct event_context
  {
    int event_local_id;
    const event_source source;
    dataT *const payload;

    event_context( const event_context<> &a )
      : event_local_id(a.event_local_id), source(a.source), payload(NEW dataT(convert<dataT>(*a.payload)))      
    {
    }

    ~event_context()
    {
      delete payload;
    }
  };
};

#endif