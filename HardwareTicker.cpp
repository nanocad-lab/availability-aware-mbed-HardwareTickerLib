/* HardwareTicker.cpp
 * Tested with mbed board: FRDM-KL46Z
 * Author: Mark Gottscho
 * mgottscho@ucla.edu
 */

#include "mbed.h"
#include "HardwareTicker.h"
//#include "us_ticker_api.h"
#include "PeripheralNames.h"

//Init
uint16_t HardwareTicker::__period = 1000;
bool HardwareTicker::__enabled = false;


void HardwareTicker::enable(float period) {
    if (period <= 0 || period > 60)
        return;
        
    static bool inited = false;
    if (!inited) {
        __init_lptmr();
        inited = true;
    }
    uint16_t __period = period * 1e3; //milliseconds
    __enabled = true;
    __set_lptmr(__period);
}

void HardwareTicker::disable() {
    __reset_lptmr();
    __period = 0;
    __enabled = false;
}

bool HardwareTicker::isEnabled() {
    return __enabled;
}

float HardwareTicker::getPeriod() {
    if (__enabled)
        return __period;
    else
        return 0;   
}

void HardwareTicker::__init_lptmr() {
    //Based on mbed library us_ticker.c code for FRDM-KL46Z
    //Use LPTMR1 (LPTMR0 used by mbed API)
    
    /* Clock the timer */
    SIM->SCGC5 |= SIM_SCGC5_LPTMR_MASK;
    
    __reset_lptmr();
    
    /* Set interrupt handler */
    NVIC_SetVector(LPTimer_IRQn, (uint32_t) __isr_lptmr);
    NVIC_EnableIRQ(LPTimer_IRQn);
    
    /* Clock at (1)MHz -> (1)tick/us */
    //LPTMR0->PSR = LPTMR_PSR_PCS(3);       // OSCERCLK -> 8MHz
    //LPTMR0->PSR |= LPTMR_PSR_PRESCALE(12); // divide by 8192
    
    /* Clock at 1 KHz -> 1 tick/ms */
    SIM->SOPT1 = SIM_SOPT1_OSC32KSEL(0); // 32 KHz clock source: 32 KHz system oscillator (OSC32KCLK)
    LPTMR0->PSR = LPTMR_PSR_PCS(2);       // OSC32KCLK --> 32 KHz
    LPTMR0->PSR |= LPTMR_PSR_PRESCALE(4); // divide by 32 to get 1 KHz
}

void HardwareTicker::__reset_lptmr() {
    /* Reset */
    LPTMR0->CSR = 0;
}

void HardwareTicker::__isr_lptmr() {
    // write 1 to TCF to clear the LPT timer compare flag
    LPTMR0->CSR |= LPTMR_CSR_TCF_MASK;
    __set_lptmr(__period);
}

void HardwareTicker::__set_lptmr(uint16_t count) {
    __reset_lptmr();
    
    /* Set the compare register */
    LPTMR0->CMR = count;
    
    /* Enable interrupt */
    LPTMR0->CSR |= LPTMR_CSR_TIE_MASK;
    
    /* Start the timer */
    LPTMR0->CSR |= LPTMR_CSR_TEN_MASK;
}






void HardwareTicker::__init_tpm() {
    //Enable TPM clock
    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
    
    //Reset TPM    
    __reset_tpm();
    
    //Set interrupt handler
    NVIC_SetVector(TPM0_IRQn, (uint32_t) __isr_tpm);
    NVIC_EnableIRQ(TPM0_IRQn);
    
    //Configure TPM to 48 MHz
    SIM->SOPT2 = SIM_SOPT2_TPMSRC(1); //Set TPM global clock source: MCGFLLCLK
    TPM0->SC = TPM_SC_PS(0); //Prescaler set to 1
}


void HardwareTicker::__reset_tpm() {
    TPM0->SC = 0;
}

void HardwareTicker::__set_tpm(uint16_t count) {  
    //Set the count register
    TPM0->CNT = 0;
     
    //Set the modulo register
    TPM0->MOD = count;
    
    //Enable interrupt
    TPM0->SC |= TPM_SC_TOIE_MASK;
    
    //Start the timer
    TPM0->SC |= TPM_SC_CMOD(1); //Timer will increment on the TPM clock edges, not an external clock
}

void HardwareTicker::__isr_tpm() {
    printf("TPM0 CNT: %u MOD: %u\r\n", TPM0->CNT, TPM0->MOD);
    //__set_tpm(__period);
}