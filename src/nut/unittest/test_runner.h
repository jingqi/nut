﻿
#ifndef ___HEADFILE___C163E413_5234_4AC7_B0E6_1D85282E4D59_
#define ___HEADFILE___C163E413_5234_4AC7_B0E6_1D85282E4D59_

#include "../nut_config.h"
#include "test_logger.h"


namespace nut
{

class NUT_API TestRunner
{
public:
    explicit TestRunner(ITestLogger *logger);

    void run_group(const char *group_name);
    void run_fixture(const char *fixture_name);
    void run_case(const char *fixture_name, const char *case_name);

private:
    ITestLogger *_logger = nullptr;
};

}

#endif /* head file guarder */
