//
// Created by mstarch on 12/10/20.
//

#include "PortSelector.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <arpa/inet.h>

namespace Drv {
namespace Test {

U16 get_free_port(bool udp) {
    struct sockaddr_in address;
    NATIVE_INT_TYPE socketFd = -1;
    // Acquire a socket, or return error
    if ((socketFd = ::socket(AF_INET, (udp) ? SOCK_DGRAM : SOCK_STREAM, 0)) == -1) {
        return 0;
    }
    // Set up the address port and name
    address.sin_family = AF_INET;
    address.sin_port = htons(0);

    // First IP address to socket sin_addr
    if (not ::inet_pton(AF_INET, "127.0.0.1", &(address.sin_addr))) {
        ::close(socketFd);
        return 0;
    };

    // When we are setting up for receiving as well, then we must bind to a port
    if (::bind(socketFd, reinterpret_cast<struct sockaddr *>(&address), sizeof(address)) == -1) {
        ::close(socketFd);
        return 0;
    }
    socklen_t size = sizeof(address);
    if (::getsockname(socketFd, reinterpret_cast<struct sockaddr *>(&address), &size) == -1) {
        ::close(socketFd);
        return 0;
    }
    U16 port = ntohs(address.sin_port);
    ::close(socketFd); // Close this recursion's port again, such that we don't infinitely loop
    return port;

}
};
};
