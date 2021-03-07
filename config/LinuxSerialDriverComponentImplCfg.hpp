/*
 * LinuxSerialDriverComponentImplCfg.hpp
 *
 *  Created on: Nov 29, 2016
 *      Author: tcanham
 */

#ifndef LINUXSERIALDRIVER_BLSPSERIALDRIVERCOMPONENTIMPLCFG_HPP_
#define LINUXSERIALDRIVER_BLSPSERIALDRIVERCOMPONENTIMPLCFG_HPP_

enum {
    DR_MAX_NUM_BUFFERS = 20,		// Increasing this from 10 b/c RceAdapter couldn't always keep up with just 10 for reads
};

#endif /* LINUXSERIALDRIVER_BLSPSERIALDRIVERCOMPONENTIMPLCFG_HPP_ */
