/**
 * \file
 * \author T. Canham
 * \brief
 *
 * This contains macros used to document test cases and requirements in unit tests.
 * Borrowed from Insight.
 *
 * \copyright
 * Copyright 2009-2015, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 * <br /><br />
 */

#ifndef TEST_UNITTEST_HPP_
#define TEST_UNITTEST_HPP_

#define TEST_CASE(tc, desc) \
    printf("\n***************************************\n"); \
    printf("TESTCASE %s: " desc "\n", #tc); \
    printf("***************************************\n")

#define REQUIREMENT(str) \
    printf("\n***************************************\n"); \
    printf("(RQ)       %s\n", str); \
    printf("***************************************\n")

#define COMMENT(str) \
    printf("\n***************************************\n"); \
    printf("%s\n", str); \
    printf("***************************************\n")




#endif /* TEST_UNITTEST_HPP_ */
