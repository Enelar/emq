#ifndef _ED_KIT_MODULE_H_
#error Wrong include order module_base.hpp before module_base.h
#endif

namespace ed
{
  template<typename T, typename MODULE>
  void module_base::RegisterPreHandler( bool (MODULE::*f)( const event_context<T> & ), event_source es )
  {
    base_pre_callback_entry *obj = SysCreateHandler<T, MODULE, bool>(f, es);
    impl.AddPreHandler(obj);
  }

  template<typename T, typename MODULE>
  void module_base::RegisterPostHandler( void (MODULE::*f)( const event_context<T> & ), event_source es )
  {
    base_post_callback_entry *obj = SysCreateHandler<T, MODULE, void>(f, es);
    impl.AddPostHandler(obj);
  }

  template<typename MODULE>
  void module_base::UnregisterHandlers( const MODULE *const )
  {
    for (unsigned int i = 0; i < impl.QueryCallbacks.size(); ++i)
    {
      delete impl.QueryCallbacks[i];
      impl.QueryCallbacks[i] = NULL;
    }
    for (unsigned int i = 0; i < impl.EventCallbacks.size(); ++i)
    {
      delete impl.EventCallbacks[i];
      impl.EventCallbacks[i] = NULL;
    }
  }

  template<typename T, typename MODULE, typename RET>
  module_impl::callback_entry<typename RET> *module_base::SysCreateHandler( RET (MODULE::*f)( const event_context<T> & ), event_source es )
  {
    typedef event_handler_convert<MODULE, T, RET> adapterT;
    adapterT *test = NEW adapterT(static_cast<MODULE &>(*this), f);
    return NEW module_impl::callback_entry<RET>(es, test);
  }
};