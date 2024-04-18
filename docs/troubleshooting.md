# Troubleshooting

This section will add some known hints to trouble-shooting with the installation of F´. This will hopefully help users install things more effectively.

## General
* [Recommended PIP Versions](#recommended-pip-versions)
* [fprime-util: command not found](#fprime-util-command-not-found)
* [Helper script fpp-redirect-helper exited with reason: Permission denied](#helper-script-fpp-redirect-helper-exited-with-reason-permission-denied)
## Linux
* [Ubuntu, Debian, Java and Python PIP](#ubuntu-debian-java-and-python-pip)
## macOS
* [CMake Command Not Found](#macos-and-cmake-command-not-found)
* [SSL Error with Python 3.8+](#ssl-error-with-python-38-on-macos)

## Recommended PIP Versions
Some of the F´ Python packages are built in a way that it is recommended to install them with modern versions of PIP. Systems not recommended or pip versions less than recommended will require Java and run slower versions of FPP tools. The recommended versions are described below:

|System |Recommended `pip` version|
|-------|:---------------------:|
|macOS (10.9+)| >= 21.0.1|
|x86_64|>= 19.3|
|aarch64|>= 20.3|

## fprime-util: command not found

If the user is using a virtual environment and receives the ‘command not found’, the problem is likely caused by the environment not being sourced in a new terminal. Make sure to source the environment before running:
```
. <path/to/project>/fprime-venv/bin/activate
```
If installing without a virtual environment, PIP occasionally uses `$HOME/.local/bin` as a place to install user tools. Users running without virtual environments should add this directory to the path.

## Helper script ‘fpp-redirect-helper’ exited with reason: Permission denied
This error can occur when the helper-script, (`fprime/cmake/autocoder/fpp-wrapper/fpp-redirect-helper`) loses its execution permission.

To verify that this is the case, change to the directory containing `fpp-redirect-helper` and verify that it is executable.

```
cd fprime/cmake/autocoder/fpp-wrapper/
ls -l
```
If it is not executable, add the permission back.

`chmod 700 fpp-redirect-helper`

## Ubuntu, Debian, Java and Python PIP
Ubuntu and possibly other Debian variants don’t include the pip packages in the default Python 3 installation. To get fully functional, use these commands on Ubuntu and Debian based systems:
```
sudo apt install git cmake default-jre python3 python3-pip python3-venv
```
Now you should be able to run the installation without trouble.

## macOS and CMake Command Not Found
If the user chooses to install CMake directly from the CMake site (not using homebrew nor Mac Ports), then the CMake command-line tools must be added to the user’s PATH or default system libraries. The quickest command to do that is:
```
sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install
```
More information can be found [here](https://stackoverflow.com/questions/30668601/installing-cmake-command-line-tools-on-a-mac).   

## SSL Error with Python 3.8+ on macOS
This fix will not work for Python installed via Homebrew. Try installing Python published at python.org.
The version of openSSL bundled with Python 3.8+ requires access to macOS’s root certificates. If the following error is encountered while installing fprime:
```
Failed find expected download: <urlopen error [SSL: CERTIFICATE_VERIFY_FAILED] certificate verify failed: unable to get  local issuer certificate
```
Then run the following command in a macOS terminal to install necessary certificates:
```
cd /Applications/Python\ 3.X/
./Install\ Certificates.command
```
After running above command, re-try installing `fprime-bootstrap`.
