#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Ecore_Getopt.h>
#include <check.h>

#include "edoc.h"

#define COPYRIGHT "Copyright © 2017 yohoho <cleanlyj@gmail.com> and various contributors (see AUTHORS)."

static void edoc_test_basic(TCase *tc);

static const struct {
   const char *name;
   void (*build)(TCase *tc);
} tests[] = {
  { "basic", edoc_test_basic }
};

START_TEST(edoc_initialization)
{
   fail_if(edoc_init() != 1);

   edoc_library_call();

   fail_if(edoc_shutdown() != 0);
}
END_TEST

static void
edoc_test_basic(TCase *tc)
{
   tcase_add_test(tc, edoc_initialization);
}

static const Ecore_Getopt optdesc = {
  "edoc",
  "%prog [options]",
  PACKAGE_VERSION,
  COPYRIGHT,
  "BSD with advertisement clause",
  "An EFL edoc program",
  0,
  {
    ECORE_GETOPT_STORE_TRUE('l', "list", "list available tests"),
    ECORE_GETOPT_STORE_STR('t', "test", "test to run"),
    ECORE_GETOPT_LICENSE('L', "license"),
    ECORE_GETOPT_COPYRIGHT('C', "copyright"),
    ECORE_GETOPT_VERSION('V', "version"),
    ECORE_GETOPT_HELP('h', "help"),
    ECORE_GETOPT_SENTINEL
  }
};

int
main(int argc EINA_UNUSED, char **argv EINA_UNUSED)
{
   Suite *s;
   SRunner *sr;
   TCase *tc = NULL;
   char *test = NULL;
   unsigned int i;
   int failed_count = -1;
   int args;
   Eina_Bool quit_option = EINA_FALSE;
   Eina_Bool list_option = EINA_FALSE;

   Ecore_Getopt_Value values[] = {
     ECORE_GETOPT_VALUE_BOOL(list_option),
     ECORE_GETOPT_VALUE_STR(test),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_NONE
   };

   eina_init();

   args = ecore_getopt_parse(&optdesc, values, argc, argv);
   if (args < 0)
     {
	EINA_LOG_CRIT("Could not parse arguments.");
	goto end;
     }
   else if (quit_option)
     {
	goto end;
     }
   else if (list_option)
     {
	fprintf(stdout, "Available tests :\n");
	for (i = 0; i < sizeof (tests) / sizeof (tests[0]); i++)
	  fprintf(stdout, "\t%s\n", tests[i].name);
	goto end;
     }

   s = suite_create("edoc");

   for (i = 0; i < sizeof (tests) / sizeof (tests[0]); i++)
     {
	if (test && strcmp(tests[i].name, test))
	  continue ;

	tc =  tcase_create(tests[i].name);
	tcase_set_timeout(tc, 0);

	tests[i].build(tc);
	suite_add_tcase(s, tc);
     }

   sr = srunner_create(s);
   srunner_set_xml(sr, PACKAGE_BUILD_DIR "/check-results.xml");

   srunner_run_all(sr, CK_ENV);
   failed_count = srunner_ntests_failed(sr);
   srunner_free(sr);

 end:
   eina_shutdown();

   return (failed_count == 0) ? 0 : 255;
}
