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
*   acknowledged. Any commercial use must be negotiated with the Office
*   of Technology Transfer at the California Institute of Technology.
*
*   Information included herein is controlled under the International
*   Traffic in Arms Regulations ("ITAR") by the U.S. Department of State.
*   Export or transfer of this information to a Foreign Person or foreign
*   entity requires an export license issued by the U.S. State Department
*   or an ITAR exemption prior to the export or transfer.
*/

#ifndef ACTIVERATEGROUP_ACTIVERATEGROUPIMPLCFG_HPP_
#define ACTIVERATEGROUP_ACTIVERATEGROUPIMPLCFG_HPP_

namespace Svc {

    enum {
        //! Number of overruns allowed before overrun event is throttled
        ACTIVE_RATE_GROUP_OVERRUN_THROTTLE = 5,
    };

}



#endif /* ACTIVERATEGROUP_ACTIVERATEGROUPIMPLCFG_HPP_ */
