//
// Created by mstarch on 12/10/20.
//
#include <FpConfig.hpp>
#include <Fw//Buffer/Buffer.hpp>
#include <Drv/Ip/IpSocket.hpp>

#ifndef DRV_TEST_SOCKETHELPER_HPP
#define DRV_TEST_SOCKETHELPER_HPP

// Drv::Test namespace
namespace Drv {
namespace Test {


/**
 * Force a receive timeout on a socket such that it will not hang our testing despite the normal recv behavior of
 * "block forever" until it gets data.
 * @param socket: socket to make timeout
 */
void force_recv_timeout(Drv::IpSocket &socket);

/**
 * Validate random data from data against truth
 * @param data: data to validate
 * @param truth: truth data to validate
 * @param size: size to validate
 */
void validate_random_data(U8 *data, U8 *truth, U32 size);

/**
 * Fills in the given data buffer with randomly picked data.
 * @param data: data to file
 * @param size: size of fill
 */
void fill_random_data(U8 *data, U32 size);

/**
 * Validates a given buffer against the data provided.
 * @param buffer: buffer to validate
 * @param truth: correct data to validate against
 */
void validate_random_buffer(Fw::Buffer &buffer, U8 *data);

/**
 * Fill random data into the buffer (using a random length).
 * @param buffer: buffer to fill.
 */
void fill_random_buffer(Fw::Buffer &buffer);

/**
 * Send/receive pair.
 * @param sender: sender of the pair
 * @param receiver: receiver of pair
 */
void send_recv(Drv::IpSocket& sender, Drv::IpSocket& receiver);

/**
 * Wait on socket change.
 */
bool wait_on_change(Drv::IpSocket &socket, bool open, U32 iterations);

};
};
#endif
