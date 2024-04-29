#include "lis3dsh.h"

namespace lis3dsh{
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
        // Read the X, Y, and Z axis
        spi::ceLow();
        spi::sendRecvChar(OUT_X_L | 0x80);
        x = spi::sendRecvChar(0) | (spi::sendRecvChar(0) << 8);
        y = spi::sendRecvChar(0) | (spi::sendRecvChar(0) << 8);
        z = spi::sendRecvChar(0) | (spi::sendRecvChar(0) << 8);
        spi::ceHigh();
    }

    void initializeLIS3DSH(){
        // Set the control register 4
        spiWriteLIS3DSH(CTRL_REG4, 0b10011111); // Enable the X, Y, and Z axis, and set the output data rate to 1600Hz, BDU enabled (block data update)
        // Set the control register 3
        spiWriteLIS3DSH(CTRL_REG3, 0b11001000); // DR_EN to 1 (enable data ready signal on INT1 pin), IEA to 1 (interrupt active high), INT1_EN to 1 (enable interrupt on INT1 pin)
        // Set the control register 6
        spiWriteLIS3DSH(CTRL_REG6, 0b01010000); // FIFO enabled, increment the address for multiple byte reading
        // Set the FIFO control register
        spiWriteLIS3DSH(FIFO_CTRL, 0b01000000); // Set the FIFO mode to stream mode

        resetSM();
    }

    void resetSM(){
        // Set control register 1
        spiWriteLIS3DSH(CTRL_REG1, 0b00000000);
        // Set control register 5
        spiWriteLIS3DSH(CTRL_REG5, 0b00000000);
        // Set the control register 3
        spiWriteLIS3DSH(CTRL_REG3, 0b11001000);
        // Set SETT
        spiWriteLIS3DSH(SETT1, 0b00000000);
        // Set MASK1_A
        spiWriteLIS3DSH(MASK1_A, 0x0);
        // Set MASK1_B
        spiWriteLIS3DSH(MASK1_B, 0x0);

        // Set to 0 all the SM registers
        for(int i = 0; i < 16; i++){
            spiWriteLIS3DSH(ST1_START + i, 0x00);
        }
        for (int i = 0; i < 16; i++){
            spiWriteLIS3DSH(ST2_START + i, 0x00);
        }
    }

    void setToggleSM(){
        // Set SETT
        spiWriteLIS3DSH(SETT1, 0b00000001); // Set the SITR to 1 (STOP and CONT generate an interrupt)
        // Set control register 3
        spiWriteLIS3DSH(CTRL_REG3, 0b01001000); // Set IEA to 1 (interrupt active high), INT1_EN to 1 (enable interrupt on INT1 pin)
        // Set control register 5
        spiWriteLIS3DSH(CTRL_REG5, 0b00000000);
        // Set TM3_1
        spiWriteLIS3DSH(TIM3_1, 0x05); // Set the time to 3

        // SM registers
        spiWriteLIS3DSH(ST1_START, 0x03); // Clock TIMER3_1 elapsed
        spiWriteLIS3DSH(ST1_START + 1, 0x11); // CONT

        // Set control register 1
        spiWriteLIS3DSH(CTRL_REG1, 0b00000001); // Set the SM 1 to enable
    }

    void setWakeUpSM(){
        // Set SITT
        spiWriteLIS3DSH(SETT1, 0b00000001); // Set the SITR to 1 (STOP and CONT generate an interrupt)
        // Set control register 3
        spiWriteLIS3DSH(CTRL_REG3, 0b01001000); // Set IEA to 1 (interrupt active high), INT1_EN to 1 (enable interrupt on INT1 pin)
        // Set control register 5
        spiWriteLIS3DSH(CTRL_REG5, 0b00000000);
        // Set the threshold
        spiWriteLIS3DSH(THRS1_1, 0x5F); // Set the threshold to 95
        // Set MASK1_A
        spiWriteLIS3DSH(MASK1_A, 0b11111111); // Set the mask to 1 (all axis are considered for the detection)

        // SM registers
        spiWriteLIS3DSH(ST1_START, 0x05); // Any/triggered axis greater than THRS1
        spiWriteLIS3DSH(ST1_START + 1, 0x11); // CONT

        // Set control register 1
        spiWriteLIS3DSH(CTRL_REG1, 0b00000001); // Set the SM 1 to enable
    }

    void setResetSM(){
        // Set SITT
        spiWriteLIS3DSH(SETT1, 0b00100001); // Set signed thrs, set the SITR to 1 (STOP and CONT generate an interrupt)
        // Set control register 3
        spiWriteLIS3DSH(CTRL_REG3, 0b01001000); // Set IEA to 1 (interrupt active high), INT1_EN to 1 (enable interrupt on INT1 pin)
        // Set control register 5
        spiWriteLIS3DSH(CTRL_REG5, 0b00000000);
        // Set the threshold
        spiWriteLIS3DSH(THRS1_1, 0b11001110); // Set the threshold to -50
        // Set the timer
        spiWriteLIS3DSH(TIM1_1_L, 0x80);
        spiWriteLIS3DSH(TIM1_1_H, 0x0C); // Set the time to 2000ms (if ODR is 1600Hz, 2000ms is 3200 samples)
        // Set MASKs
        spiWriteLIS3DSH(MASK1_A, 0b00001000); // Set the mask to only z axis

        // SM registers
        spiWriteLIS3DSH(ST1_START, 0x07); // CONT: any axis less than THRS1
        spiWriteLIS3DSH(ST1_START + 1, 0x51); // RESET: any axis greater then THRS1, CONT: timer elapsed
        spiWriteLIS3DSH(ST1_START + 2, 0x11); // CONT

        // Set control register 1
        spiWriteLIS3DSH(CTRL_REG1, 0b00000001); // Set the SM 1 to enable
    }

    int getInt1(){
        return int1::value();
    }

    int getInt2(){
        return int2::value();
    }

    bool int1Flag(){
        return spiReadLIS3DSH(STAT) & 0b00001000;
    }

    void waitUntilInt1(){
        while(!int1Flag());
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

    void printRegisters(){
        // Print the registers
        char reg1 = spiReadLIS3DSH(CTRL_REG1);
        printf("Control register 1: 0x%X\n", reg1);
        char reg3 = spiReadLIS3DSH(CTRL_REG3);
        printf("Control register 3: 0x%X\n", reg3);
        char reg4 = spiReadLIS3DSH(CTRL_REG4);
        printf("Control register 4: 0x%X\n", reg4);
        char reg5 = spiReadLIS3DSH(CTRL_REG5);
        printf("Control register 5: 0x%X\n", reg5);
        char reg6 = spiReadLIS3DSH(CTRL_REG6);
        printf("Control register 6: 0x%X\n", reg6);
        char fifo = spiReadLIS3DSH(FIFO_CTRL);
        printf("FIFO control register: 0x%X\n", fifo);
        char stat = spiReadLIS3DSH(STAT);
        printf("Status register: "); printChar(stat);
        char sitt1 = spiReadLIS3DSH(SETT1);
        printf("SITT 1: 0x%X\n", sitt1);
    }

    void printSM1(){
        char pr_1 = spiReadLIS3DSH(PR1);
        printf("PR 1: 0x%X\n", pr_1);
        char tim3_1 = spiReadLIS3DSH(TIM3_1);
        printf("Time 3 1: 0x%X\n", tim3_1);
        char thrs1_1 = spiReadLIS3DSH(THRS1_1);
        printf("Threshold 1 1: 0x%X\n", thrs1_1);
        char thrs2_1 = spiReadLIS3DSH(THRS2_1);
        printf("Threshold 2 1: 0x%X\n", thrs2_1);
        for(int i = 0; i < 16; i++){
            char reg = spiReadLIS3DSH(ST1_START + i);
            printf("SM 1 register %d: 0x%X\n", i, reg);
        }
    }
}