#ifndef EDOC_H_
# define EDOC_H_

#include <Elementary.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef EFL_EDOC_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_EDOC_BUILD */
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif /* ! _WIN32 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * @brief These routines are used for edoc library interaction.
 */

/**
 * @brief Init / shutdown functions.
 * @defgroup Init  Init / Shutdown
 *
 * @{
 *
 * Functions of obligatory usage, handling proper initialization
 * and shutdown routines.
 *
 * Before the usage of any other function, edoc should be properly
 * initialized with @ref edoc_init() and the last call to edoc's
 * functions should be @ref edoc_shutdown(), so everything will
 * be correctly freed.
 *
 * edoc logs everything with Eina Log, using the "edoc" log domain.
 *
 */

/**
 * Initialize edoc.
 *
 * Initializes edoc, its dependencies and modules. Should be the first
 * function of edoc to be called.
 *
 * @return The init counter value.
 *
 * @see edoc_shutdown().
 *
 * @ingroup Init
 */
EAPI int edoc_init(void);

/**
 * Shutdown edoc
 *
 * Shutdown edoc. If init count reaches 0, all the internal structures will
 * be freed. Any edoc library call after this point will leads to an error.
 *
 * @return edoc's init counter value.
 *
 * @see edoc_init().
 *
 * @ingroup Init
 */
EAPI int edoc_shutdown(void);

/**
 * @}
 */

/**
 * @brief Main group API that wont do anything
 * @defgroup Main Main
 *
 * @{
 *
 * @brief A function that doesn't do any good nor any bad
 *
 * @ingroup Main
 */
EAPI void edoc_library_call(void);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* EDOC_H_ */
