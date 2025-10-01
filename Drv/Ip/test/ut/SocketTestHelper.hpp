//
// Created by mstarch on 12/10/20.
//
#include <Drv/Ip/IpSocket.hpp>
#include <Fw/Buffer/Buffer.hpp>
#include <Fw/FPrimeBasicTypes.hpp>

#ifndef DRV_TEST_SOCKETHELPER_HPP
#define DRV_TEST_SOCKETHELPER_HPP

// Drv::Test namespace
namespace Drv {
namespace Test {

static constexpr U16 MAX_ITER = 10;
/**
 * Force a receive timeout on a socket such that it will not hang our testing despite the normal recv behavior of
 * "block forever" until it gets data.
 * @param fd: socket file descriptor
 * @param socket: socket to make timeout
 */
void force_recv_timeout(int fd, Drv::IpSocket& socket);

/**
 * Validate random data from data against truth
 * @param data: data to validate
 * @param truth: truth data to validate
 * @param size: size to validate
 */
void validate_random_data(U8* data, U8* truth, FwSizeType size);

/**
 * Fills in the given data buffer with randomly picked data.
 * @param data: data to file
 * @param size: size of fill
 */
void fill_random_data(U8* data, FwSizeType size);

/**
 * Validates a given buffer against the data provided.
 * @param buffer: buffer to validate
 * @param truth: correct data to validate against
 */
void validate_random_buffer(Fw::Buffer& buffer, U8* data);

/**
 * Fill random data into the buffer (using a random length).
 * @param buffer: buffer to fill.
 */
FwSizeType fill_random_buffer(Fw::Buffer& buffer);

/**
 * Send/receive pair.
 * @param sender: sender of the pair
 * @param receiver: receiver of pair
 * @param sender_fd: file descriptor for sender
 * @param receiver_fd: file descriptor for receiver
 */
void send_recv(Drv::IpSocket& sender,
               Drv::IpSocket& receiver,
               Drv::SocketDescriptor& sender_fd,
               Drv::SocketDescriptor& receiver_fd);

/**
 * Drain bytes from the socket until disconnect received.
 * @warning: must have called shutdown on the remote before calling this
 * @param drain_fd: file descriptor for draining
 */
void drain(Drv::IpSocket& receiver, Drv::SocketDescriptor& drain_fd);

/**
 * Receive all data, reassembling the frame
 * @param receiver: receiver
 * @param receiver_fd: receiver descriptor
 * @param buffer: buffer
 * @param size: size to receive
 */
void receive_all(Drv::IpSocket& receiver, Drv::SocketDescriptor& receiver_fd, U8* buffer, FwSizeType size);

/**
 * Wait on socket change.
 */
bool wait_on_change(Drv::IpSocket& socket, bool open, U32 iterations);

/**
 * Wait on started
 */
bool wait_on_started(Drv::IpSocket& socket, bool open, U32 iterations);

/**
 * Get the configured delay, converted to milliseconds
 * @return SOCKET_RETRY_INTERVAL converted to milliseconds
 */
U64 get_configured_delay_ms();

}  // namespace Test
}  // namespace Drv
#endif
