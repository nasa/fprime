/**
 * ConsoleDefault.cpp:
 *
 * This file ensures that the Os::Console has a default instance. This means it will be created in static space, and
 * registered as the default implementation. If the user does not intend to use the default implementation of
 * Os::Console, then this file Should be excluded from the build.
 */
#include <Os/Console.hpp>
Os::Console __default_console__; // Create a default instance which will register itself in the constructor

