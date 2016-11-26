
#ifndef ___HEADFILE___C163E413_5234_4AC7_B0E6_1D85282E4D59_
#define ___HEADFILE___C163E413_5234_4AC7_B0E6_1D85282E4D59_

#include "../nut_config.h"
#include "test_logger.h"


/* magic number, do NOT change them */
#define __NUT_UNITTEST_SUM_CASES_OP -2
#define __NUT_UNITTEST_RUN_NAMED_CASE_OP -1

namespace nut
{

class NUT_API TestRunner
{
    ITestLogger *_logger = NULL;

public:
    TestRunner(ITestLogger *logger);

    void run_group(const char *group_name);
    void run_fixture(const char *fixture_name);
    void run_case(const char *fixture_name, const char *case_name);
};

}

#endif /* head file guarder */
