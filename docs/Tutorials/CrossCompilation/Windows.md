# Microsoft Windows 10/11 WSL

Windows 10 ships with a technology known as WSL. WSL allows users to run Linux virtual machines transparently within the Windows 10 operating system.

**Powershell: Install WSL with Default Ubuntu**
```powershell
wsl --install
```

To start Ubuntu under WSL, search for Ubuntu in the start menu and select the "Ubuntu on Windows" app. All class commands should be run on these Ubuntu terminals.

> Full instructions and troubleshooting help is available in the 
> [Microsoft documentation](https://learn.microsoft.com/en-us/windows/wsl/install).

Lastly, Windows users must open up a firewall port and forward that port to WSL to 
ensure the hardware can call back into F' ground data system running in WSL. First we'll 
need to note the IP address of the WSL machine. This is done with the following 
command *in an administrator PowerShell*.

```powershell
wsl hostname -I
```

> Record the output of this command for the next step. For this guide, we will use 
> the value `127.0.0.1`.

Next, we will add a firewall rule and forward it to the WSL instance. This is done with the following commands:

> Warning: these commands work with the Windows firewall. Security and antivirus tools 
> can run extra firewall rules. Users must allow the port `50000` (Or whichever port that 
> is going to be used) or disable these extra firewall settings.

**PowerShell: Add and Forward External Firewall Rule**
```PowerShell
New-NetFirewallRule -DisplayName "fprime" -Direction inbound -Profile Any -Action Allow -LocalPort 50000 -Protocol TCP
```

> Remember to change `127.0.0.1` to your recorded ip address as discovered with 
> `wsl hostname -I`. Users are advised to remove this rule after the class has been completed.
 

**IMPORTANT:**

Go to the [Ubuntu 20.04 / 22.04 / Generic](./Linux.md) Linux to finish setting up your WSL environment.
