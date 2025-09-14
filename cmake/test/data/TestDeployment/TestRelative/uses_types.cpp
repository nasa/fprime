// ======================================================================
// \title  uses_type.cpp
// \author lestarch
// \brief  fails to compile if the relative FPP sources did not work
// ======================================================================
#include <TestDeployment/TestRelative/FppConstantsAc.hpp>
// Constant definitions will not be defined if the module setup failed
bool new_constant = (relative_constant1 && relative_constant2);