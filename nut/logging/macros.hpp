/**
 * @file -
 * @author jingqi
 * @date 2010-8-25
 */

#ifndef ___HEADFILE___BFA6BECF_B11A_4738_BF1C_5EDDF7379806_
#define ___HEADFILE___BFA6BECF_B11A_4738_BF1C_5EDDF7379806_

#include "Logger.h"
#include "LogManager.h"

#if defined(_MSC_VER) /* c99,vc */

#   if defined(NO_MINOR_LOGGING) /* remove logging */
#       define LOGGING_DEBUG(logger,fmt,...) ((void)0)
#   else /* NO_MINOR_LOGGING */
#       define LOGGING_DEBUG(logger,fmt,...) (logger).log(logging::LL_DEBUG,DEBUGGING_SOURCE_LOCATION,(fmt),__VA_ARGS__)
#   endif /* NO_MINOR_LOGGING */

#   define LOGGING_INFO(logger,fmt,...) (logger).log(logging::LL_INFO,DEBUGGING_SOURCE_LOCATION,(fmt),__VA_ARGS__)
#   define LOGGING_WARN(logger,fmt,...) (logger).log(logging::LL_WARN,DEBUGGING_SOURCE_LOCATION,(fmt),__VA_ARGS__)
#   define LOGGING_ERROR(logger,fmt,...) (logger).log(logging::LL_ERROR,DEBUGGING_SOURCE_LOCATION,(fmt),__VA_ARGS__)
#   define LOGGING_FATAL(logger,fmt,...) (logger).log(logging::LL_FATAL,DEBUGGING_SOURCE_LOCATION,(fmt),__VA_ARGS__)

#elif defined(__GNUC__) || defined(__GNUC_MINOR__) || (__MINGW32__) /* gcc */

#   if defined(NO_MINOR_LOGGING) /* remove logging */
#       define LOGGING_DEBUG(logger,fmt,args...) ((void)0)
#   else /* NO_MINOR_LOGGING */
#       define LOGGING_DEBUG(logger,fmt,args...) (logger).log(logging::LL_DEBUG,DEBUGGING_SOURCE_LOCATION,(fmt),##args)
#   endif /* NO_MINOR_LOGGING */

#   define LOGGING_INFO(logger,fmt,args...) (logger).log(logging::LL_INFO,DEBUGGING_SOURCE_LOCATION,(fmt),##args)
#   define LOGGING_WARN(logger,fmt,args...) (logger).log(logging::LL_WARN,DEBUGGING_SOURCE_LOCATION,(fmt),##args)
#   define LOGGING_ERROR(logger,fmt,args...) (logger).log(logging::LL_ERROR,DEBUGGING_SOURCE_LOCATION,(fmt),##args)
#   define LOGGING_FATAL(logger,fmt,args...) (logger).log(logging::LL_FATAL,DEBUGGING_SOURCE_LOCATION,(fmt),##args)

#endif

#define LOGGING_ASSERT_WARN(logger,ex) do { if (!(ex)) (logger).log(logging::LL_WARN,DEBUGGING_SOURCE_LOCATION,"Assertion failed : %s",#ex); } while (false)
#define LOGGING_ASSERT_ERROR(logger,ex) do { if (!(ex)) (logger).log(logging::LL_WARN,DEBUGGING_SOURCE_LOCATION,"Assertion failed : %s",#ex); } while (false)
#define LOGGING_ASSERT_FATAL(logger,ex) do { if (!(ex)) (logger).log(logging::LL_WARN,DEBUGGING_SOURCE_LOCATION,"Assertion failed : %s",#ex); } while (false)

#endif /* head file guarder */

