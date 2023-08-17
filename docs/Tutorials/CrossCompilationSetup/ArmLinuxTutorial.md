# F´ Running on ARM Linux Tutorial

For this tutorial, the assumption is that the ARM Linux machine is available on the network, is running SSH, and the username, password, device address, and host address are known. Without this configuration, users should skip to the next section of the tutorial.

First, in a terminal upload the software to hardware platform. This is done with:

```sh
# For ARM 64-bit hardware
# In: Deployment Folder
scp -r build-artifacts/aarch64-linux <username>@<device-address>:deployment

# For ARM 32-bit hardware
# In: Deployment Folder
scp -r build-artifacts/arm-hf-linux <username>@<device-address>:deployment
```
> Users must fill in the username and device address above.

Next run the F´ GDS without launching the native compilation (`-n`) and with the 
dictionary from the build above (`--dictionary ./build-artifacts/<platform name>/<.xml document>`).

```sh
# For in-person workshops and ARM 64-bit hardware
# In: Deployment Folder
fprime-gds -n --dictionary build-artifacts/aarch64-linux/dict/<App Dictionary>.xml --ip-client --ip-address <device-address>

# For ARM 32-bit hardware
# In: Deployment Folder
fprime-gds -n --dictionary build-artifacts/aarch64-linux/dict/<App Dictionary>.xml --ip-client --ip-address <device-address>
```
> This depends on a flight software deployment that uses TcpServer as the communications driver implementation.

In another terminal SSH into the device and run the uploaded software:
```sh
ssh <username>@<device-address>
sudo deployment/bin/<name-of-deployment> -a 0.0.0.0 -p 50000
```
> User should fill in the username and device address above and ensure the correct executable is supplied.

> If the device does not connect, ensure that the firewall port 50000 is open on the flight computer.

## Troubleshooting

If you are getting errors for missing Libc.c files, make sure when you generate 
that the logs show that it is using the `/opt/toolchains` path and not `/bin`. 
You can additionally verify that the correct toolchain is being used by watching
the logs scroll by when you initially `fprime-util generate <toolchain>`.
