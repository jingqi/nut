/**
 * @file -
 * @author jingqi
 * @date 2010-8-25
 */

#ifndef ___HEADFILE___BFA6BECF_B11A_4738_BF1C_5EDDF7379806_
#define ___HEADFILE___BFA6BECF_B11A_4738_BF1C_5EDDF7379806_

#include "logger.hpp"
#include "log_manager.hpp"

// 可变参数宏使用兼容 c99/vc/gcc 的形式
#   define NUT_LOGGING_DEBUG(logger, fmt, ...) do { (logger)->log(nut::LL_DEBUG, NUT_SOURCE_LOCATION, (fmt), ##__VA_ARGS__); } while (false)
#   define NUT_LOGGING_INFO(logger, fmt, ...) do { (logger)->log(nut::LL_INFO, NUT_SOURCE_LOCATION, (fmt), ##__VA_ARGS__); } while (false)
#   define NUT_LOGGING_WARN(logger, fmt, ...) do { (logger)->log(nut::LL_WARN, NUT_SOURCE_LOCATION, (fmt), ##__VA_ARGS__); } while (false)
#   define NUT_LOGGING_ERROR(logger, fmt, ...) do { (logger)->log(nut::LL_ERROR, NUT_SOURCE_LOCATION, (fmt), ##__VA_ARGS__); } while (false)
#   define NUT_LOGGING_FATAL(logger, fmt, ...) do { (logger)->log(nut::LL_FATAL, NUT_SOURCE_LOCATION, (fmt), ##__VA_ARGS__); } while (false)


#endif /* head file guarder */

