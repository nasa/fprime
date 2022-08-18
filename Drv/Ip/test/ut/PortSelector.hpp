//
// Created by mstarch on 12/10/20.
//
#include <FpConfig.hpp>

#ifndef DRV_TEST_PORTSELECTOR_HPP
#define DRV_TEST_PORTSELECTOR_HPP

namespace Drv {
namespace Test {
/**
 * \brief returns a (currently) unused port
 *
 * Tests working with TCP often need ports to be unused. This presents a problem when looking to bind to a port that has
 * not been used anywhere on the system.  This function will walk the process through to the point of getting a bind,
 * and use the port 0 to have the OS assign one.  At this point, the assigned port will be inspected and the fd will be
 * closed without a connection allowing something else to bind to it e.g the test code.
 *
 * Note: this is test code only as there is a known race condition from the moment of closing the port, to when the
 * recipient binds it again.
 *
 * \param is_udp: is this a UDP port
 * \return 0 on error, or a free port on success
 */
U16 get_free_port(bool is_udp = false);
};
};
#endif  // DRV_TEST_PORTSELECTOR_HPP
