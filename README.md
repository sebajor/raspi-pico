# raspi-pico

## Building toolflow

To get the pico SDK you need to clone the SDK git repo:
`git clone -b master https://github.com/raspberrypi/pico-sdk.git`

Then you have to move inside the cloned repo and run:
`git submodule update --init`

You need to install the needed packages:
`sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential`

Finally you have to set the enviroment variable (you can add it to the ~/.bashrc file):
`export PICO_SDK_PATH=<where you installed> `

If you want to have the examples code you can clone them from here:
`git clone -b master https://github.com/raspberrypi/pico-examples.git`





