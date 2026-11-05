// ======================================================================
// \title  ParamValid.hpp
// \author F Prime
// \brief  Helpers for checking parameter validity
//
// \copyright
// Copyright (C) 2026 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FW_PRM_PARAM_VALID_HPP
#define FW_PRM_PARAM_VALID_HPP

#include <Fw/Prm/ParamValidEnumAc.hpp>

//! Check whether a parameter value is usable
//!
//! A parameter value is usable when it was successfully loaded from the
//! parameter database or when its default value was applied.
#define FW_PARAM_OK(paramValid) (((paramValid) == Fw::ParamValid::DEFAULT) || ((paramValid) == Fw::ParamValid::VALID))

#endif  // FW_PRM_PARAM_VALID_HPP
