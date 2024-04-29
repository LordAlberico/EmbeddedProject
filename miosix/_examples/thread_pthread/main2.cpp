#include <cstdio>
#include <pthread.h>
#include "miosix.h"
#include "util/software_spi.h"
#include "print_util.h"
#include "lcg.h"

#define T_OUT 0x0C // Address of the temperature
#define INFO1 0x0D // Address of the info 1
#define WHO_AM_I 0x0F // Address of the who am I
#define CTRL_REG4 0x20 // Address of the control register 4
#define CTRL_REG6 0x25 // Address of the control register 5
#define OUT_X_L 0x28 // Address of the X-axis low byte
#define OUT_X_H 0x29 // Address of the X-axis high byte
#define OUT_Y_L 0x2A // Address of the Y-axis low byte
#define OUT_Y_H 0x2B // Address of the Y-axis high byte
#define OUT_Z_L 0x2C // Address of the Z-axis low byte
#define OUT_Z_H 0x2D // Address of the Z-axis high byte
#define FIFO_CTRL 0x2E // Address of the FIFO control register

using namespace std;
using namespace miosix;

typedef Gpio<GPIOD_BASE,13> orangeLed;

// Motion sensor
typedef Gpio<GPIOE_BASE,3>  cs;
typedef Gpio<GPIOA_BASE,5>  sck;
typedef Gpio<GPIOA_BASE,6>  sdo;
typedef Gpio<GPIOA_BASE,7>  sdi;

typedef SoftwareSPI<sdo,sdi,sck,cs, 3> spi;

// K for the motion sensor for +-2g, 16-bit resolution
static const float K = 0.061;

void spiInitLIS3DSH(){
    spi::init();
    Thread::sleep(1); // Wait for the sensor to power up
}

char spiReadLIS3DSH(char address){
	address |= 0x80;
	short input = address << 8;
	spi::ceLow();
	short result = spi::sendRecvShort(input);
	spi::ceHigh();
	return result;
}

void spiWriteLIS3DSH(char address, char data){
    short input = (address << 8) | data;
    spi::ceLow();
    spi::sendRecvShort(input);
    spi::ceHigh();
    Thread::sleep(1); // Wait for the sensor to process the data (i don't why this is necessary and if i can lower this time)
}

void readAllAxis(short &x, short &y, short &z){
    spi::ceLow();
    spi::sendRecvChar(OUT_X_L | 0x80);
    x = spi::sendRecvChar(0) | (spi::sendRecvChar(0) << 8);
    y = spi::sendRecvChar(0) | (spi::sendRecvChar(0) << 8);
    z = spi::sendRecvChar(0) | (spi::sendRecvChar(0) << 8);
    spi::ceHigh();
}

void initializeLIS3DSH(){
    // Set the control register 4
    spiWriteLIS3DSH(CTRL_REG4, 0b01101111); // Enable the X, Y, and Z axis, and set the output data rate to 100Hz, BDU enabled (block data update)
    // Set the control register 6
    spiWriteLIS3DSH(CTRL_REG6, 0b01010000); // FIFO enabled, increment the address for multiple byte reading
    // Set the FIFO control register
    spiWriteLIS3DSH(FIFO_CTRL, 0b01000000); // Set the FIFO mode to stream mode
}

void printSensorTest(){
    // Read the temperature
    char temp = spiReadLIS3DSH(T_OUT);
    printf("Temperature: %d\n", temp);

    // Read the info 1
    char info1 = spiReadLIS3DSH(INFO1);
    printf("Info 1: %d\n", info1);

    // Read the who am I
    char whoAmI = spiReadLIS3DSH(WHO_AM_I);
    printf("Who am I: %d\n", whoAmI);
}

void *ledTest(){
    while(1){
        orangeLed::high();    //Turn the LED on
        Thread::sleep(250); //Wait 500ms
        orangeLed::low();     //Turn the LED off
        Thread::sleep(250); //Wait 500ms
    }
}

void lcgTest(){
    // Random number test
    printf("Random number: %d\n", hardwareRNG(-2,2));
}

pthread_t ledTestThread;

void setup(){
    // Led pin
    orangeLed::mode(Mode::OUTPUT);

    // Initialize the SPI for the motion sensor
    spiInitLIS3DSH();

    // Initialize the motion sensor
    initializeLIS3DSH();

    // Print the sensor test
    printSensorTest();

    // Start a thread for led test
    pthread_create(&ledTestThread, NULL, ledTest, NULL);
}

void mainLoop(){
    // Read the X, Y, and Z axis
    short x, y, z;
    readAllAxis(x, y, z);
    printf("X: %f\n", x * K);
    Thread::sleep(100);

    // Led test
    ledTest();
}

void close(){
    phtread_join(ledTestThread, NULL);
}
int main()
{
    setup();
	for(;;){
	    mainLoop();
    }
    close();
    return 0;
}
