#ifndef LIS3DSH_H
#define LIS3DSH_H

#include "miosix.h"
#include "util/software_spi.h"
#include "print_util.h"

#define T_OUT 0x0C     // Address of the temperature
#define INFO1 0x0D     // Address of the info 1
#define WHO_AM_I 0x0F  // Address of the who am I
#define LC_L 0x16      // Address of the low byte of the LC
#define STAT 0x18      // Address of the status
#define CTRL_REG4 0x20 // Address of the control register 4
#define CTRL_REG1 0x21 // Address of the control register 1
#define CTRL_REG3 0x23 // Address of the control register 3
#define CTRL_REG5 0x24 // Address of the control register 5
#define CTRL_REG6 0x25 // Address of the control register 6
#define STATUS 0x27    // Address of the status register
#define OUT_X_L 0x28   // Address of the X-axis low byte
#define OUT_X_H 0x29   // Address of the X-axis high byte
#define OUT_Y_L 0x2A   // Address of the Y-axis low byte
#define OUT_Y_H 0x2B   // Address of the Y-axis high byte
#define OUT_Z_L 0x2C   // Address of the Z-axis low byte
#define OUT_Z_H 0x2D   // Address of the Z-axis high byte
#define FIFO_CTRL 0x2E // Address of the FIFO control register
#define TIM3_1 0x51    // Address of the time 3 1
#define TIM1_1_L 0x54  // Address of the time 1 1 low byte
#define TIM1_1_H 0x55  // Address of the time 1 1 high byte
#define THRS2_1 0x56   // Address of the threshold 2 1
#define THRS1_1 0x57   // Address of the threshold 1 1
#define MASK1_B 0x59   // Address of the mask 1 B
#define MASK1_A 0x5A   // Address of the mask 1 A
#define SETT1 0x5B     // Address of the SETT 1
#define PR1 0x5C       // Address of the PR 1
#define OUTS_1 0x5F    // Address of the output SM 1

using namespace miosix;

namespace lis3dsh{
    // Status machine registers
    #define ST1_START 0x40
    #define ST2_START 0x60

    typedef Gpio<GPIOE_BASE,3>  cs;
    typedef Gpio<GPIOA_BASE,5>  sck;
    typedef Gpio<GPIOA_BASE,6>  sdo;
    typedef Gpio<GPIOA_BASE,7>  sdi;
    typedef Gpio<GPIOE_BASE,0>  int1;
    typedef Gpio<GPIOE_BASE,1>  int2;

    typedef SoftwareSPI<sdo,sdi,sck,cs, 3> spi;

    // K for the motion sensor for +-2g, 16-bit resolution
    static const float K = 0.061;

    void spiInitLIS3DSH();

    char spiReadLIS3DSH(char address);

    void spiWriteLIS3DSH(char address, char data);

    void resetSM();

    void setWakeUpSM();

    void setToggleSM();

    void setResetSM();

    int getInt1();

    int getInt2();

    bool int1Flag();
    
    void waitUntilInt1();

    /**
     * Reads the values of all three axes (X, Y, Z) from the LIS3DSH sensor.
     *
     * @param x Reference to the variable where the X-axis value will be stored.
     * @param y Reference to the variable where the Y-axis value will be stored.
     * @param z Reference to the variable where the Z-axis value will be stored.
     */
    void readAllAxis(short &x, short &y, short &z);

    void initializeLIS3DSH();

    void printSensorTest();

    void printRegisters();

    void printSM1();
}

#endif