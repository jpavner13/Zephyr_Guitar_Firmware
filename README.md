# Zephyr RTOS Template

Template to get started with Zephyr RTOS. When flashed, the onboard LED will blink every second.

## More example code can be found at:
https://github.com/zephyrproject-rtos/zephyr/tree/main/samples

## Getting Started:

Before getting started, make sure you have a proper VS Code and Docker
environment. 

- Download and install [VS Code](https://code.visualstudio.com/)
- Install [WSL](https://docs.microsoft.com/en-us/windows/wsl/install)
- Download and install [Docker for Windows](https://docs.docker.com/desktop/windows/install/)
- Download and install [usbipd-win](https://github.com/dorssel/usbipd-win/releases) and follow 
the [instructions](https://github.com/dorssel/usbipd-win/wiki/WSL-support#usbip-client-tools) 
for USB/IP client tools.
- Download and install the [wsl-usb-gui](https://gitlab.com/alelec/wsl-usb-gui/-/releases) tool.

Once complete, open this repository in VS Code. When prompted, re-open in a dev container. 

VS Code will build the Docker image defined in [.devcontainer/Dockerfile](.devcontainer/Dockerfile), 
and may take some time on first setup. The Zephyr repositories will be installed as well.

The main.c file is the entry point of your Zephyr project.

If you need to configure pins on your board, modify the .dts file of the respective board in `/zephyrproject/zephyr/boards`. GPIO pins can usually be configured as led's or buttons. For more information visit https://docs.zephyrproject.org/latest/hardware/peripherals/gpio.html

## Compile code with:
`west build -b $BOARD app`

## Flash code with:
`west flash -d build`

#### Some '$BOARD' examples are
```
nucleo_f303re
nucleo_f429zi
arduino_portenta_h7
```

All boards can be listed with `'west boards'`

### All supported boards can be found in 
`/zephyrproject/zephyr/boards`

Note: This directory can't be accessed in GitLab. In order to access it, clone the repository, build the dev container, and then run `cd zephyrproject/zephyr/boards` in the working directory. Most boards are in the `/arm` folder.