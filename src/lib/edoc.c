#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "edoc.h"

#include "edoc_private.h"

static int _edoc_init = 0;
int _edoc_lib_log_dom = -1;

EAPI int
edoc_init(void)
{
   _edoc_init++;
   if (_edoc_init > 1) return _edoc_init;

   eina_init();

   _edoc_lib_log_dom = eina_log_domain_register("edoc", EINA_COLOR_CYAN);
   if (_edoc_lib_log_dom < 0)
     {
	EINA_LOG_ERR("edoc can not create its log domain.");
	goto shutdown_eina;
     }

   // Put here your initialization logic of your library

   eina_log_timing(_edoc_lib_log_dom, EINA_LOG_STATE_STOP, EINA_LOG_STATE_INIT);

   return _edoc_init;

 shutdown_eina:
   eina_shutdown();
   _edoc_init--;

   return _edoc_init;
}

EAPI int
edoc_shutdown(void)
{
   _edoc_init--;
   if (_edoc_init != 0) return _edoc_init;

   eina_log_timing(_edoc_lib_log_dom,
		   EINA_LOG_STATE_START,
		   EINA_LOG_STATE_SHUTDOWN);

   // Put here your shutdown logic

   eina_log_domain_unregister(_edoc_lib_log_dom);
   _edoc_lib_log_dom = -1;

   eina_shutdown();

   return _edoc_init;
}

EAPI void
edoc_library_call(void)
{
   INF("Not really doing anything useful.");
}
