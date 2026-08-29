# Drv::LinuxRawEther SDD

Passive ByteStreamDriver for raw L2 over AF_PACKET (Linux). Boards sharing eth without IP stack send custom-framed data. Same Fw.Buffer ports as Udp/Tcp so topology picks raw-ether vs UART by wiring.

## Usage
```fpp
instance rawEther: Drv.LinuxRawEther base id 0x1000
rawEther.open("eth0", 2048); rawEther.start()
```

## Design
- Linux: AF_PACKET SOCK_RAW ETH_P_ALL, SIOCGIFINDEX bind, needs CAP_NET_RAW
- VxWorks: second impl behind same FPP (MUX/END layer)
- Hermetic: bounded recv, no heap post-init, explicit error events, stats tlm BytesSent/Recv
