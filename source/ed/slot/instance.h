#ifndef _ED_SLOT_INSTANCE_H_
#define _ED_SLOT_INSTANCE_H_

#include "module.h"
#include "../communications/abstract_connection.h"

namespace ed
{
  namespace slot
  {
    class instance
    {
      std::vector<module> data;
      void Create( int i );
    public:
      com::abstract_connection *con;
      com::abstract_connection &Socket() const;

      module &GetModule( const int i, bool force = false );

      module &GetModule( const event_source &, bool force = false );
      event &GetEvent( const event_source &, bool force = false );

      void AddListener( const listen_message &, bool force = false );
      void RemoveDisconnected( const int instance );

      std::set<int> Subscribed( int slot_data::listener::* type, const event_source &es );
    };
  };
};

#endif