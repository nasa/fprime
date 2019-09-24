import sys
import argparse
import threading

import fprime_gds.common.adapters.sender
import fprime_gds.common.adapters.ip

# Uses non-standard PIP package pyserial, so test the waters
try:
    import fprime_gds.common.adapters.uart
    UART_AVAILABLE = True
except ImportError:
    UART_AVAILABLE = False

def parse_args(args):
    """
    Parse the arguments to this application, then return the constructed namespace argument.
    :param args: list of arguments to parse
    :return: namespace argument
    """
    parser = argparse.ArgumentParser(description="Connects data from F prime flight software to the GDS tcp server")
    parser.add_argument("--gds-addr", dest="gds_addr", type=str, default="127.0.0.1",
                        help="Address of the GDS layer. Default: %(default)s")
    parser.add_argument("--gds-port", dest="gds_port", type=int, default=50050,
                        help="Port GDS later. Default: %(default)s")
    subparsers = parser.add_subparsers(help="Type of adapter used for processing", dest="subcommand")
    # UART adapter arguments
    if UART_AVAILABLE:
        uparser = subparsers.add_parser("uart", help="Connects to a UART radio")
        uparser.add_argument("-d", "--device", dest="device", type=str, default="/dev/ttyACM0",
                             help="UART device representing the FSW. Default: %(default)s")
        uparser.add_argument("-b", "--baud", dest="baud", type=int, default=115200,
                             help="Baud rate of the serial device. Default: %(default)s")

    # IP adapter arguments
    iparser =  subparsers.add_parser("ip", help="Connects to a IP radio")
    iparser.add_argument("-a", "--addr", dest="addr", type=str, default="0.0.0.0",
                         help="Address of the IP adapter server. Default: %(default)s")
    iparser.add_argument("-p", "--port", dest="port", type=int, default=50000,
                         help="Port of the IP adapter server. Default: %(default)s")
    args = parser.parse_args(args)
    return args

def main(args=sys.argv):
    """
    Main program, degenerates into the run loop
    :param args: arguments
    :return: return code
    """
    args = parse_args(args[1:])
    sender = fprime_gds.common.adapters.sender.TCPSender(args.gds_addr, args.gds_port)
    if args.subcommand == "ip":
        adapter = fprime_gds.common.adapters.ip.IpAdapter(sender, args.addr, args.port)
    elif args.subcommand == "uart":
        adapter = fprime_gds.common.adapters.uart.SerialAdapter(sender, args.device, args.baud)
    else:
        raise Exception("Programming error!")

    threading.Thread(target=adapter.run_uplink).start()
    threading.Thread(target=adapter.run_downlink).start()


if __name__ == "__main__":
    sys.exit(main())