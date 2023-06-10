/**
 * LogDefault.cpp:
 *
 * This file ensures that the Os::Log has a default instance. This means it will be created in static space here, and
 * registered as the default implementation. If the user does not intend to use the default implementation of Os::Log,
 * then this file can safely be ignored.
 */
#include <Os/Log.hpp>
Os::Log __default_logger__; // Create a default instance which will register itself in the constructor

