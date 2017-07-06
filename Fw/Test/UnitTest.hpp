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
 * acknowledged. Any commercial use must be negotiated with the Office
 * of Technology Transfer at the California Institute of Technology.
 * <br /><br />
 * This software may be subject to U.S. export control laws and
 * regulations.  By accepting this document, the user agrees to comply
 * with all U.S. export laws and regulations.  User has the
 * responsibility to obtain export licenses, or other export authority
 * as may be required before exporting such information to foreign
 * countries or providing access to foreign persons.
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
