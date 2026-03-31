# FppTest Topology

This test is an integration/system test meant to test fpp
code-generation for topologies. The basic principle is to use
two components, Sender and Receiver and to ping-pong messages
between them and validate.

Both sender and receiver include a set of typed ports with
most permutations of arguments. All port instance specifiers
have 3 items. The first two are meant for [typed port tests](#typed-port-tests)
while the third is meant for [serial port tests](#serial-port-tests).

## Typed Port Tests

Typed port tests represent the connections between
typed-port -> typed-port. There are two for each typed port.
The receiver will implement a handler to serialize the values
coming in on the port and send out a `replyOut` back to the sender
which will verify that what the receiver got was the same as
what the sender sent.

## Serial Port Tests

Because testing serial ports requires testing two permutations of
all types of serial connections (i.e. serial -> typed and typed -> serial (serial -> serial not handled here)),
we utilize the existing [typed port tests](#typed-port-tests) to
implement these tests. Instead of connecting `sender.typedPort[2]` to
`receiver.typedPort[2]` we instead go through two sets of serial ports:

```fpp
sender.typedPort[2] -> receiver.serialIn[TYPED_PORT_ID]
receiver.serialReplyOut -> sender.serialReplyIn

sender.serialOut[TYPED_PORT_ID] -> receiver.typedPort[2]
receiver.replyOut -> sender.replyIn
```

The above connections are listed in the order in which the message is passed
between the components. We end up testing both typed to serial (first connection)
and serial to typed (third connection).

### Serial to Serial

Serial to serial is a simple special case of serial port tests which is manually implemented.
