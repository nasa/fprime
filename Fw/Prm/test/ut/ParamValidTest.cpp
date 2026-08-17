#include <gtest/gtest.h>

#include <Fw/Prm/ParamValid.hpp>

TEST(FwPrmParamValid, ReportsUsableParameterStatuses) {
    EXPECT_FALSE(FW_PARAM_OK(Fw::ParamValid::UNINIT));
    EXPECT_TRUE(FW_PARAM_OK(Fw::ParamValid::VALID));
    EXPECT_FALSE(FW_PARAM_OK(Fw::ParamValid::INVALID));
    EXPECT_TRUE(FW_PARAM_OK(Fw::ParamValid::DEFAULT));
}
