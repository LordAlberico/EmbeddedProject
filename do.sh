set -e
make
st-flash write main.bin 0x08000000
screen /dev/ttyUSB0 115200
