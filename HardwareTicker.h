/* HardwareTicker.h
 * Tested with mbed board: FRDM-KL46Z
 * Author: Mark Gottscho
 * mgottscho@ucla.edu
 */

#ifndef HARDWARETICKER_H
#define HARDWARETICKER_H

#include "mbed.h"

/**
 * Note: This ticker is NOT virtualized like the mbed's API timers.
 * Millisecond accuracy.
 */
class HardwareTicker {
    public:        
        /**
         * Enables the HardwareTicker.
         * @param period in seconds (must be greater than 0 and less than or equal to 60)
         */
        static void enable(float period);
        
        /**
         * Disables the HardwareTicker.
         */
        static void disable();
        
        /**
         * @returns true if the HardwareTicker is enabled.
         */
        static bool isEnabled();  
        
        /**
         * @returns the period in seconds
         */
        static float getPeriod();    
        
    private:
        /**
         * Initializes the hardware LPTMR timer.
         */
        static void __init_lptmr();
        
        /** 
         * Resets the hardware LPTMR timer.
         */
        static void __reset_lptmr();
        
        /**
         * Sets the hardware LPTMR timer.
         */
        static void __set_lptmr(uint16_t count);
        
        /**
         * Interrupt service routine for LPTMR
         */
        static void __isr_lptmr();
        
        /**
         * Initializes the hardware TPM timer.
         */
        static void __init_tpm();
        
        /** 
         * Resets the hardware TPM timer.
         */
        static void __reset_tpm();
        
        /**
         * Sets the hardware TPM timer.
         */
        static void __set_tpm(uint16_t count);
        
        /**
         * Interrupt service routine for TPM
         */
        static void __isr_tpm();
        
        static uint16_t __period; //in ms
        static bool __enabled;
};

#endif