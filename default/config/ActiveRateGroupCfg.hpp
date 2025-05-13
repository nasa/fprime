/*
* \author: Tim Canham
* \file:
* \brief
*
* This file has configuration settings for the ActiveRateGroup component.
*
*
*   Copyright 2014-2015, by the California Institute of Technology.
*   ALL RIGHTS RESERVED. United States Government Sponsorship
*   acknowledged.
*
*/

#ifndef ACTIVERATEGROUP_ACTIVERATEGROUPCFG_HPP_
#define ACTIVERATEGROUP_ACTIVERATEGROUPCFG_HPP_

namespace Svc {

    enum {
        //! Number of overruns allowed before overrun event is throttled
        ACTIVE_RATE_GROUP_OVERRUN_THROTTLE = 5,
    };

}



#endif /* ACTIVERATEGROUP_ACTIVERATEGROUPCFG_HPP_ */
