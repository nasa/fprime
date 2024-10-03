// ======================================================================
// \title  SocketComponentHelper.hpp
// \author mstarch
// \brief  hpp file for SocketComponentHelper implementation class
//
// \copyright
// Copyright 2009-2020, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================
#ifndef DRV_SocketComponentHelper_HPP
#define DRV_SocketComponentHelper_HPP

#include <Fw/Buffer/Buffer.hpp>
#include <Drv/Ip/IpSocket.hpp>
#include <Os/Task.hpp>
#include <Os/Mutex.hpp>

namespace Drv {
/**
 * \brief supports a task to read a given socket adaptation
 *
 * Defines an Os::Task task to read a socket and send out the data. This represents the task itself, which is capable of
 * reading the data from the socket, sending the data out, and reopening the connection should a non-retry error occur.
 *
 */
class SocketComponentHelper {
  public:
    /**
     * \brief constructs the socket read task
     */
    SocketComponentHelper();

    /**
     * \brief destructor of the socket read task
     */
    virtual ~SocketComponentHelper();

    /**
     * \brief start the socket read task to start producing data
     *
     * Starts up the socket reading task and opens socket. This should be called before send calls are expected to
     * work. Will connect to the previously configured port and host. priority, stack, and cpuAffinity are provided
     * to the Os::Task::start call.
     *
     * \param name: name of the task
     * \param reconnect: automatically reconnect socket when closed. Default: true.
     * \param priority: priority of the started task. See: Os::Task::start. Default: TASK_DEFAULT, not prioritized
     * \param stack: stack size provided to the task. See: Os::Task::start. Default: TASK_DEFAULT, posix threads default
     * \param cpuAffinity: cpu affinity provided to task. See: Os::Task::start. Default: TASK_DEFAULT, don't care
     */
    void start(const Fw::StringBase &name,
                         const bool reconnect = true,
                         const Os::Task::ParamType priority = Os::Task::TASK_DEFAULT,
                         const Os::Task::ParamType stack = Os::Task::TASK_DEFAULT,
                         const Os::Task::ParamType cpuAffinity = Os::Task::TASK_DEFAULT);

    /**
     * \brief startup the socket for communications
     *
     * Status of the socket handler.
     *
     * Note: this just delegates to the handler
     *
     * \return status of open, SOCK_SUCCESS for success, something else on error
     */
    SocketIpStatus startup();

    /**
     * \brief Returns true when the socket is started
     *
     * Returns true when the socket is started up sufficiently to be actively listening to clients. Returns false
     * otherwise. This means `startup()` was called and returned success.
     */
    bool isStarted();

    /**
     * \brief open the socket for communications
     *
     * Typically the socket read task will open the connection and keep it open. However, in cases where the read task
     * will not be started, this function may be used to open the socket.
     *
     * Note: this just delegates to the handler
     *
     * \return status of open, SOCK_SUCCESS for success, something else on error
     */
    SocketIpStatus open();

     /**
     * \brief check if IP socket has previously been opened
     *
     * Check if this IpSocket has previously been opened. In the case of Udp this will check for outgoing transmissions
     * and (if configured) incoming transmissions as well. This does not guarantee errors will not occur when using this
     * socket as the remote component may have disconnected.
     *
     * \return true if socket is open, false otherwise
     */
    bool isOpened();
    
     /**
     * \brief Re-open port if it has been disconnected
     *
     *
     * \return status of reconnect, SOCK_SUCCESS for success, something else on error
     */
    SocketIpStatus reconnect();

    /**
     * \brief send data to the IP socket from the given buffer
     *
     *
     * \param data: pointer to data to send
     * \param size: size of data to send
     * \return status of send, SOCK_SUCCESS for success, something else on error
     */
    SocketIpStatus send(const U8* const data, const U32 size);

    /**
     * \brief receive data from the IP socket from the given buffer
     *
     *
     * \param data: pointer to data to fill with received data
     * \param size: maximum size of data buffer to fill
     * \return status of the send, SOCK_DISCONNECTED to reopen, SOCK_SUCCESS on success, something else on error
     */
    SocketIpStatus recv(U8* data, U32 &size);

    /**
     * \brief close the socket communications
     *
     * Typically stopping the socket read task will shutdown the connection. However, in cases where the read task
     * will not be started, this function may be used to close the socket. This calls a full `close` on the client
     * socket.
     *
     * Note: this just delegates to the handler
     */
    void close();

    /**
     * \brief shutdown the socket communications
     *
     * Typically stopping the socket read task will shutdown the connection. However, in cases where the read task
     * will not be started, this function may be used to close the socket. This calls a full `shutdown` on the client
     * socket.
     *
     * Note: this just delegates to the handler
     */
    void shutdown();

    /**
     * \brief stop the socket read task and close the associated socket.
     *
     * Called to stop the socket read task. It is an error to call this before the thread has been started using the
     * startSocketTask call. This will stop the read task and close the client socket.
     */
    void stop();

    /**
     * \brief joins to the stopping read task to wait for it to close
     *
     * Called to join with the read socket task. This will block and return after the task has been stopped with a call
     * to the stopSocketTask method.
     * \param value_ptr: a pointer to fill with data. Passed to the Os::Task::join call. NULL to ignore.
     * \return: Os::Task::Status passed back from the Os::Task::join call.
     */
    Os::Task::Status join();


  PROTECTED:
    /**
     * \brief returns a reference to the socket handler
     *
     * Gets a reference to the current socket handler in order to operate generically on the IpSocket instance. Used for
     * receive, and open calls.
     *
     * Note: this must be implemented by the inheritor
     *
     * \return IpSocket reference
     */
    virtual IpSocket& getSocketHandler() = 0;

    /**
     * \brief returns a buffer to fill with data
     *
     * Gets a reference to a buffer to fill with data. This allows the component to determine how to provide a
     * buffer and the socket read task just fills said buffer.
     *
     * Note: this must be implemented by the inheritor
     *
     * \return Fw::Buffer to fill with data
     */
    virtual Fw::Buffer getBuffer() = 0;

    /**
     * \brief sends a buffer to be filled with data
     *
     * Sends the buffer gotten by getBuffer that has now been filled with data. This is used to delegate to the
     * component how to send back the buffer.
     *
     * Note: this must be implemented by the inheritor
     *
     * \return Fw::Buffer filled with data to send out
     */
    virtual void sendBuffer(Fw::Buffer buffer, SocketIpStatus status) = 0;

    /**
     * \brief called when the IPv4 system has been connected
     */
    virtual void connected() = 0;

    /**
     * \brief a task designed to read from the socket and output incoming data
     *
     * \param pointer: pointer to "this" component
     */
    static void readTask(void* pointer);

    Os::Task m_task;
    Os::Mutex m_lock;
    NATIVE_INT_TYPE m_fd;
    bool m_reconnect; //!< Force reconnection
    bool m_stop; //!< Stops the task when set to true
    bool m_started; //!< Have we successfully started the socket
    bool m_open; //!< Have we successfully opened


};
}
#endif  // DRV_SocketComponentHelper_HPP
