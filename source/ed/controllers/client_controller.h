#ifndef _ED_CONTROLLERS_CLIENT_CONTROLLER_H_
#define _ED_CONTROLLERS_CLIENT_CONTROLLER_H_

#include "../def.h"

namespace ed
{
  template<class connection>
  struct __declspec(dllexport) client_controller
  {
  private:
    connection *c;
  public:
    client_controller( connection * );

    operator connection &() const;

    ~client_controller();
  };
};

#ifndef _ED_CONTROLLERS_CLIENT_CONTROLLER_IMPL_
namespace ed
{
  #include "client_controller.cpp"
};
#else
using namespace ed;
#endif

#endif