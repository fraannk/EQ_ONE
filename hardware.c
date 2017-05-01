/*****************************************************************************
* University of Southern Denmark
*
* MODULENAME.: hardware.c
*
* PROJECT....: EQ_ONE
*
* DESCRIPTION: Hardware module for EQ_ONE project
*              Project Mode
*                Left-in    PB5 (AIN11)   ADC0
*                Right-in   PB4 (AIN10)   ADC0
*
*                PWM Mode
*                Left-out   PB7 (M0PWM1)  PWM module 0 PWM generator 0
*                Right-out  PB6 (M0PWM0)  PWM module 0 PWM generator 0
*
*                DAC Mode
*                PA2  (SCK)
*                PA3  (CS)
*                PA5  (SDA)
*                PE1  (LDAC)    // Todo:implement
*                PE2  (SHDN)    // Todo:implement
*
*              EMP Mode (with #define EMP) for on board audio circuit
*                in         PE5 (AIN8)    ADC0
*                out        PE4 (M0PWM4)  PWM module 0 PWM generator 2
*
* Change Log:
*****************************************************************************
* Date    Id    Change
* --------------------
* 29. mar. 2017  jorn    Module created.
*
*****************************************************************************/

/***************************** Include files *******************************/
#include "hardware.h"
#include <stdint.h>
#include "global.h"
#include "tm4c123gh6pm.h"
#include "emp_type.h"
#include "macros.h"
#include "systick.h"

/*****************************    Defines    *******************************/

/*****************************   Constants   *******************************/

/*****************************   Variables   *******************************/
INT16U  cycles        = 0;
FP32    adc_pwm_ratio = 1;

/*****************************   Functions   *******************************/
void pwm_clear_interrupt()
/*****************************************************************************
*   Header description
******************************************************************************/
{
#ifdef EMP
  // Clear interrupt flags
  PWM0_ISC_R |= PWM_ISC_INTPWM2;
  PWM0_2_ISC_R |= PWM_2_ISC_INTCNTLOAD;
#else
  // Clear interrupt flags
  PWM0_ISC_R |= PWM_ISC_INTPWM0;
  PWM0_0_ISC_R |= PWM_0_ISC_INTCNTZERO;
#endif
}

void line_in( enum io_state state )
/*****************************************************************************
*   Header description
******************************************************************************/
{
  switch( state )
  {
    case ON:
      ADC0_ACTSS_R |= 0b1000;
#ifndef EMP
      ADC1_ACTSS_R |= 0b1000;
#endif
      break;
    case OFF:
      ADC0_ACTSS_R &= ~0b1000;
#ifndef EMP
      ADC1_ACTSS_R &= ~0b1000;
#endif
      break;
  }
}

void line_out( enum io_state state )
/*****************************************************************************
*   Header description
******************************************************************************/
{
  switch( state )
  {
    case ON:
#ifdef EMP
      // Enable PWM0:PWM4
      bit_set( PWM0_ENABLE_R, PWM_ENABLE_PWM4EN );
#else
      // Enable PWM0:PWM0-PWM1
      bit_set( PWM0_ENABLE_R, PWM_ENABLE_PWM0EN );
      bit_set( PWM0_ENABLE_R, PWM_ENABLE_PWM1EN );
#endif
      break;
    case OFF:
#ifdef EMP
      // Disable PWM0:PWM4
      bit_clear( PWM0_ENABLE_R, PWM_ENABLE_PWM4EN );
#else
      // Disable PWM0:PWM0-PWM1
      bit_clear( PWM0_ENABLE_R, PWM_ENABLE_PWM0EN );
      bit_clear( PWM0_ENABLE_R, PWM_ENABLE_PWM1EN );
#endif
      break;
  }
}

void audio_out(sample_type sample)
/*****************************************************************************
*   Header description
******************************************************************************/
{
  if(cycles)
  {
#ifdef EMP
    INT32U mono = (sample.left + sample.right)>>2;
    PWM0_2_CMPA_R = (INT32U)(adc_pwm_ratio * mono);
#else
#ifdef PWM
    PWM0_0_CMPA_R = (INT32U)(adc_pwm_ratio * (sample.right));
    PWM0_0_CMPB_R = (INT32U)(adc_pwm_ratio * (sample.left));
#else
    // Set right channel outA
    static INT32U reg;
    reg = 0b0111 << 12;       // A, buffered, 1xGain, On
    reg |= sample.left;
    SSI0_DR_R = reg;

    reg = 0b1111 << 12;       // B, buffered, 1xGain, On
    reg |= sample.right;
    SSI0_DR_R = reg;
#endif

#endif
  }
}

void audio_in(sample_type *sample)
/*****************************************************************************
*   Header description
******************************************************************************/
{
#ifdef EMP
  INT16U value;

  if( !( ADC0_SSFSTAT3_R & ADC_SSFSTAT3_EMPTY) )
    value = (INT16U)(ADC0_SSFIFO3_R);
  else
    value = 2048;

  sample->left = value;
  sample->right = value;

  // initiate a sample for SS3
  ADC0_PSSI_R |= ADC_PSSI_SS3;

  // wait for the ADC0 to complete
  //while( ADC0_ACTSS_R & ADC_ACTSS_BUSY ) ;

#else
  if( !( ADC0_SSFSTAT3_R & ADC_SSFSTAT3_EMPTY) )
    sample->left = (INT16U)(ADC0_SSFIFO3_R);
  else
    sample->left = 2048;

  if( !( ADC1_SSFSTAT3_R & ADC_SSFSTAT3_EMPTY) )
    sample->right = (INT16U)(ADC1_SSFIFO3_R);
  else
    sample->right = 2048;

  // initiate a sample for SS3 with Global sync
  //ADC0_PSSI_R |= ADC_PSSI_SYNCWAIT & ADC_PSSI_SS3;
  //ADC1_PSSI_R |= ADC_PSSI_GSYNC & ADC_PSSI_SS3;

  ADC0_PSSI_R |= ADC_PSSI_SS3;
  ADC1_PSSI_R |= ADC_PSSI_SS3;

  // wait for the ADC0 & ADC1 to complete
  //while( (ADC0_ACTSS_R & ADC_ACTSS_BUSY) || (ADC1_ACTSS_R & ADC_ACTSS_BUSY) ) ;
#endif
}

void init_ADC( )
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Initialize the ADC
******************************************************************************/
{
  // EMP board is controled by define EMP in the define section
  //  ANALOG_IN mono  -> PE5 (AIN8) on the EMP Board
  //  or
  //  ANALOG_IN LEFT  -> PB5 (AIN11)
  //            RIGHT -> PE4 (AIN10)

#ifdef EMP
  // Module Init
  // Enable ADC0 Clock
  SYSCTL_RCGCADC_R = SYSCTL_RCGCADC_R0;

  // Enable GPIO clock for port E
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGC2_GPIOE;

  // Enable the alternate function on PortE pin 5 PE5, high==AFSEL
  GPIO_PORTE_AFSEL_R |= 1<<5;

  // Set input as analog, clear bit 5 on port E
  GPIO_PORTE_DEN_R &= ~(1<<5);

  // Select analog mode AMSEL on PortE bit 5
  GPIO_PORTE_AMSEL_R |= (1<<5);

  // Sample Sequencer Configuration

  // Disable ASEN3:0
  ADC0_ACTSS_R &= ~0b1111;

  // Configure trigger event for SS ADCEMUX, set EM3 : Processor event (ADCPSSI)
  ADC0_EMUX_R &= 0x0000;

  // Config input source for each sample i SS3, AIN8
  ADC0_SSMUX3_R = 8;

  // Set Sample Sequence Control 3
  // 3 : TS0  - Temp Sensor
  // 2 : IE0  - Interrupt Enable
  // 1 : END0 - End of Sequence
  // 0 : D0   - Sample Diff input Select
  ADC0_SSCTL3_R = 0b0010;

  // Set ADC Interrupt Mask (if used)
  ADC0_IM_R = 0b0000;
#else

  // Module Init
  // Enable ADC0 & ADC1 Clock
  SYSCTL_RCGCADC_R |= (SYSCTL_RCGCADC_R0 | SYSCTL_RCGCADC_R1);

  // Enable GPIO clock for port B
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;

  // Enable the alternate function on PB5 and PB4, high==AFSEL
  GPIO_PORTB_AFSEL_R |= (1<<4 & 1<<5) ;

  // Set input as analog, clear bit 2 and 3 on port E
  GPIO_PORTB_DEN_R &= ~(1<<4 & 1<<5);

  // Select analog mode AMSEL on PortE bit 2 and 3
  GPIO_PORTB_AMSEL_R |= (1<<4 & 1<<5);

  // Sample Sequencer Configuration

  // Disable ASEN3:0
  ADC0_ACTSS_R &= ~0b1111;
  ADC1_ACTSS_R &= ~0b1111;

  // Configure trigger event for SS ADCEMUX, set EM3 : Processor event (ADCPSSI)
  ADC0_EMUX_R &= 0x0000;
  ADC1_EMUX_R &= 0x0000;

  // Config input source for each sample i SS3, AIN11 & AIN10
  ADC0_SSMUX3_R = 11;
  ADC1_SSMUX3_R = 10;

  // Set Sample Sequence Control 3
  // 3 : TS0  - Temp Sensor
  // 2 : IE0  - Interrupt Enable
  // 1 : END0 - End of Sequence
  // 0 : D0   - Sample Diff input Select
  ADC0_SSCTL3_R = 0b0010;
  ADC1_SSCTL3_R = 0b0010;

  // Set ADC Interrupt Mask (if used)
  ADC0_IM_R = 0b0000;
  ADC1_IM_R = 0b0000;
#endif
}

void set_80Mhz_clock()
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Set the processor frequency to 80Mhz
******************************************************************************/
{
  INT32U reg;

  // Enable the use of RCC2
  SYSCTL_RCC2_R |= SYSCTL_RCC2_USERCC2;     // Set USERCC2

  // Bypass PLL ans system clock divider
  SYSCTL_RCC_R |= SYSCTL_RCC_BYPASS;        // Set Bypass on RCC
  SYSCTL_RCC2_R |= SYSCTL_RCC2_BYPASS2;     // Set Bypass on RCC2
  SYSCTL_RCC_R &= ~SYSCTL_RCC_USESYSDIV;    // Clear USESYS

  // Set XTAL to 16Mhz
  reg = SYSCTL_RCC_R;
  reg &= ~(0b11111<<6);
  reg |= SYSCTL_RCC_XTAL_16MHZ;
  SYSCTL_RCC_R = reg;

  // Set Oscillator Source OSCSRC
  SYSCTL_RCC2_R &= ~0b1110000;  // Set MOSC

  // Clear the PWRDN
  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_PWRDN2;


  // Set DIV400 bit
  SYSCTL_RCC2_R |= SYSCTL_RCC2_DIV400;

  // Set system divider SYSDIV to 0x2 and SSYSDIV2LSB to 0
  reg = SYSCTL_RCC2_R;
  reg &= ~(0b1111111<<22);
  reg |= (0b100<<22);
  SYSCTL_RCC2_R = reg;

  // Enable the new divider by setting USESYS
  SYSCTL_RCC_R |= SYSCTL_RCC_USESYSDIV;    // Set USESYS

  // Wait for PLL lock, value=1 when PLL is locked
  while( !(SYSCTL_RIS_R & SYSCTL_RIS_PLLLRIS) );

  // Enable PLL
  SYSCTL_RCC2_R &= ~SYSCTL_RCC2_BYPASS2;  // Clear Bypass2

}

void init_PWM( INT16U cycles )
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Initialize the PWM as audio output based on the cycles
*              between each sample.
******************************************************************************/
{
#ifdef EMP
  // PB2 (T3CCP0)
  /* Not used anymore

  INT32U reg;

  // Activate PORTB Clock
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;

  // 3 clockcycle wait after enabling the clock
  __asm__("nop");__asm__("nop");__asm__("nop");

  // Enable the alternate function on PortB pin 2 PB2, high==AFSEL
  GPIO_PORTB_AFSEL_R |= 1<<2;

  // GPIO portcontrol, activate T3CCP0 on pin PB2
  reg = GPIO_PORTB_PCTL_R;
  reg &= 0xF00;                                     // Clear bits 11:8
  GPIO_PORTB_PCTL_R |= GPIO_PCTL_PB2_T3CCP0;        // Set

  // Activate Timer3 Clock
  SYSCTL_RCGC1_R |= SYSCTL_RCGC1_TIMER3;

  // 3 clockcycle wait after enabling the clock
  __asm__("nop");__asm__("nop");__asm__("nop");

  // Disable Timer 3
  bit_clear( TIMER3_CTL_R, TIMER_CTL_TAEN);

  // Select 16/32 bit mode for 32/64bit timer
  TIMER3_CFG_R = 0x4;

  //In the GPTM Timer Mode (GPTMTnMR) register, set the
  // TnAMS bit to 0x1  - PWM mode enabled
  // TnCMR bit to 0x0  - Edge count mode
  // TnMR field to 0x2 - Periodic time mode
  TIMER3_TAMR_R &= ~(0xF);
  TIMER3_TAMR_R |= 0b1010;

  // Configure the output state of the PWM signal
  // (whether or not it is inverted) in the TnPWML field
  // of the GPTM Control (GPTMCTL) register.

  bit_clear( TIMER3_CTL_R, TIMER_CTL_TAPWML ); // PWM signal is not inverted

  //  If a prescaler is to be used, write the prescale value to the
  //  GPTM Timer n Prescale Register (GPTMTnPR).

  // TIMER3_TAPR_R = 0;

  // Load the timer start value into the GPTM Timer n Interval Load
  // (GPTMTnILR) register.

  TIMER3_TAILR_R = cycles;

  //  Load the GPTM Timer n Match (GPTMTnMATCHR) register with
  //  the match value.
  TIMER3_TAMATCHR_R = 0;

  // Enable NVIC interrupt 35
  NVIC_EN1_R |= 1<<3;

  // Enable Timer3 Interrupt on Timer A Falling Edge
  TIMER3_CTL_R &=  ~TIMER_CTL_TAEVENT_M;
  TIMER3_CTL_R |= TIMER_CTL_TAEVENT_NEG;
  TIMER3_TAMR_R |= TIMER_TAMR_TAPWMIE;

  // Timer A freezes counting while the processor is halted by the   debugger.
  bit_set( TIMER3_CTL_R, TIMER_CTL_TASTALL);

  TIMER3_ICR_R |= TIMER_ICR_CAECINT;

  */

  // PE4 (M0PWM4)

  INT32U reg;

  // Activate PORTE Clock (R4)
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;

  // Digital enable pins
  GPIO_PORTE_DEN_R |= 1<<4;

  // Direction
  GPIO_PORTE_DIR_R |= 1<<4;

  // Enable the alternate function on PE4, high==AFSEL
  GPIO_PORTE_AFSEL_R |= 1<<4;

  // GPIO portcontrol, activate M0PWM4 on PE4
  reg = GPIO_PORTE_PCTL_R;
  reg &= 0x000F0000;
  GPIO_PORTE_PCTL_R |= GPIO_PCTL_PE4_M0PWM4;      // Set

  // Activate PWM0 clock
  SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R0;

  // Set PWM divide on the RCC to disabled
  bit_clear( SYSCTL_RCC_R, SYSCTL_RCC_USEPWMDIV);

  // Disable PWM block
  bit_clear( PWM0_2_CTL_R, PWM_0_CTL_ENABLE );

  // Disable PWM0:PWM4
  bit_clear( PWM0_ENABLE_R, PWM_ENABLE_PWM4EN );

  // Set PWM Count-down mode
  bit_clear( PWM0_2_CTL_R, PWM_0_CTL_MODE );
  bit_set( PWM0_2_CTL_R, PWM_0_CTL_DEBUG);

  // set PWM0 Generator A control
  // Comp A down -> Drive pwmA Low
  //   -> Drive pwmA High
  bit_clear(PWM0_2_GENA_R, 0xFFF ) ;       // clear settings
  bit_set( PWM0_2_GENA_R, (PWM_0_GENA_ACTCMPAD_ZERO | PWM_0_GENA_ACTLOAD_ONE) );

  // set the PWM0 load
  PWM0_2_LOAD_R = (INT16U)(cycles);

  // invert M0PWM4 signals
  bit_set( PWM0_INVERT_R, 1<<4 );

  // set initial CompA and CompB values
  PWM0_2_CMPA_R = 0;

  // Enable PWM0 interrupt  28
  NVIC_EN0_R |= 1<<12;

  // enable interrupt on PWM0
  bit_set( PWM0_INTEN_R, 0b0100 );

  // enable interrupt for PWM0 when counter is zero
  bit_set( PWM0_2_INTEN_R, PWM_2_INTEN_INTCNTLOAD);

  // Enable PWM block
  bit_set( PWM0_2_CTL_R, PWM_2_CTL_ENABLE );

#else

  // PB6 (M0PWM0)  & PB7 (M0PWM1)

   INT32U reg;

   // Activate PORTB Clock
   SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;

   // Digital enable pins
   GPIO_PORTB_DEN_R |= (1<<6 | 1<<7);

   // Direction
   GPIO_PORTB_DIR_R |= (1<<6 | 1<<7);

   // Enable the alternate function on PB6 & PB7, high==AFSEL
   GPIO_PORTB_AFSEL_R |= (1<<6 | 1<<7);

   // GPIO portcontrol, activate M0PWM0 on PB6 & M0PWM1 on PB7
   reg = GPIO_PORTB_PCTL_R;
   reg &= 0xFF000000;                                // Clear bits 31:24
   GPIO_PORTB_PCTL_R |= GPIO_PCTL_PB7_M0PWM1
                        | GPIO_PCTL_PB6_M0PWM0;      // Set

   // Activate PWM0 clock
   SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R0;

   // Set PWM divide on the RCC to disabled
   bit_clear( SYSCTL_RCC_R, SYSCTL_RCC_USEPWMDIV);

   // Disable PWM block
   bit_clear( PWM0_0_CTL_R, PWM_0_CTL_ENABLE );

   // Disable PWM0:PWM0-PWM1
   bit_clear( PWM0_ENABLE_R, PWM_ENABLE_PWM0EN );
   bit_clear( PWM0_ENABLE_R, PWM_ENABLE_PWM1EN );

   // Set PWM Count-up/Count-down mode
   bit_set( PWM0_0_CTL_R, PWM_0_CTL_MODE );
   bit_set( PWM0_0_CTL_R, PWM_0_CTL_DEBUG);

   // set PWM0 Generator A control
   // Comp A down -> Drive pwmA Low
   // Comp A up   -> Drive pwmA High
   bit_set( PWM0_0_GENA_R, (PWM_0_GENA_ACTCMPAD_ZERO | PWM_0_GENA_ACTCMPAU_ONE) );

   // set PWM0 Generator B control
   // Comp A down -> Drive pwmB Low
   // Comp A up   -> Drive pwmB High
   bit_set( PWM0_0_GENB_R, (PWM_0_GENB_ACTCMPBD_ZERO | PWM_0_GENB_ACTCMPBU_ONE) );

   // set the PWM0 load
   PWM0_0_LOAD_R = (INT16U)(cycles>>1);

   // invert M0PWM1 and M0PWM0 signals
   //bit_set( PWM0_INVERT_R, 0b11 );

   // set initial CompA and CompB values
   PWM0_0_CMPA_R = 0;
   PWM0_0_CMPB_R = 0;

   // Enable PWM0 interrupt  26
   NVIC_EN0_R |= 1<<10;

   // enable interrupt on PWM0
   bit_set( PWM0_INTEN_R, 0b0001 );

   // enable interrupt for PWM0 when counter is zero
   bit_set( PWM0_0_INTEN_R, PWM_0_INTEN_INTCNTZERO);

   // Enable PWM block
   bit_set( PWM0_0_CTL_R, PWM_0_CTL_ENABLE );

   #endif

}

void init_tiva_board()
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : INitialize the Tiva board
******************************************************************************/
{
  // Set GPIO'S on Run Mode Clock Gating Control Register on PORTF & PORTD
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5|  SYSCTL_RCGCGPIO_R3;

  // GPIO Direction (GPIODR)
  GPIO_PORTF_DIR_R = 0x0E;    // 0b00001110
  // Set the direction as output (PD6).
  GPIO_PORTD_DIR_R = 0x40;    // 0b01000000

  // GPIO Digital Enable (GPIODEN)
  GPIO_PORTF_DEN_R = 0x1F;    // 0b00011111
  // Enable the GPIO pins for digital function (PD6).
  GPIO_PORTD_DEN_R = 0x40;    // 0b01000000

  // GPIO PullUp Resistor (GPIOPUR)
  GPIO_PORTF_PUR_R = 0x11;    // 0b00010001
}

INT16U cycles_per_sample ( INT16U sample_freq )
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Calculates the sample interval in CPU cycles on a 80Mhz system
******************************************************************************/
{
  INT16U cycles = CPU_F / sample_freq;
  return cycles;
}

void enable_FPU()
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Enable the floating point unit (FPU)
******************************************************************************/
{
  INT32U reg;

  reg = NVIC_CPAC_R;
  reg |= (0xF << 20);
  NVIC_CPAC_R = reg;

  __asm__("DSB");
  __asm__("ISB");
}

void init_debug_pins()
/*****************************************************************************
*   Input    : -
*   Output   : -
*   Function : Initialize the debug pins PB0, PB1 and PB3
******************************************************************************/
{
  // Activate PORTB Clock
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;

  // GPIO Direction (GPIOB)
  GPIO_PORTB_DIR_R |= ( 1<<0 | 1<<1 | 1<<3 );

  // GPIO Digital Enable (GPIODEN)
  GPIO_PORTB_DEN_R |= ( 1<<0 | 1<<1 | 1<<3 ) ;
}

void delay_us(INT32U time)
/*****************************************************************************
*   Header description
******************************************************************************/
{
  //Make sure Timer2A is stopped
  bit_clear(TIMER2_CTL_R, TIMER_CTL_TAEN);

  //Set the start value of timer2 A
  TIMER2_TAILR_R = time * CPU_MULTIPLEX;

  // Start timer2 A
  //bit_set(TIMER2_CTL_R, TIMER_CTL_TASTALL);  // stall timer2 in debug mode
  bit_set(TIMER2_CTL_R, TIMER_CTL_TAEN);

  // poll for time-out on timer2 A
  while(!(TIMER2_RIS_R & TIMER_RIS_TATORIS));

  // clear flag
  bit_set(TIMER2_ICR_R, TIMER_ICR_TATOCINT);
}

void delay_init()
/*****************************************************************************
*   Header description
******************************************************************************/
{
    // Activate Timer2 Clock
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R2;

    // Disable timer2 A before changing any value
    bit_clear(TIMER2_CTL_R, TIMER_CTL_TAEN);

    // 32-bit timer for Timer2 A.
    TIMER2_CFG_R = 0x00;

    // One-Shot Timer mode on Timer2 A
    TIMER2_TAMR_R = 0x01;
}

void timer_init()
{
  // Activate Timer4 Clock
  SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R4;

  // Disable timer4 A before changing any value
  bit_clear(TIMER4_CTL_R, TIMER_CTL_TAEN);

  // 32-bit timer for Timer4 A.
  TIMER4_CFG_R = 0x00;

  // Enable stall in debug mode
  bit_set(TIMER4_CTL_R, TIMER_CTL_TASTALL);

  // One-Shot Timer mode on Timer4 A
  TIMER4_TAMR_R =  (TIMER_TAMR_TACDIR | TIMER_TAMR_TAMR_PERIOD) ;

  bit_set(TIMER4_CTL_R, TIMER_CTL_TAEN);
}

void timer_set(INT32U time )
{
  TIMER4_TAV_R = time;

}

INT32U timer_get()
{
  return (TIMER4_TAR_R);
}

void spi_init()
{
#ifndef EMP
  // activate the SSI module clock for SSI0
  bit_set( SYSCTL_RCGCSSI_R, SYSCTL_RCGCSSI_R0 );

  // activate GPIO port A click
  bit_set( SYSCTL_RCGCGPIO_R, SYSCTL_RCGCGPIO_R0);

  // Digital enable pins
  GPIO_PORTA_DEN_R |= (1<<2 | 1<<3 | 1<<4 | 1<<5);

  // Direction
  GPIO_PORTA_DIR_R |= (1<<2 | 1<<3 | 1<<5);

  // GPIO PullUp Resistor (GPIOPUR)
  GPIO_PORTA_PUR_R |=  (1<<4);    // pull up on SSI0RX pin 4

  // Enable the alternate function on PA2, PA3, PA4 & PA5, high==AFSEL
  GPIO_PORTA_AFSEL_R |= (1<<2 | 1<<3 | 1<<4 | 1<<5 );

  // GPIO portcontrol,
  INT32U reg = GPIO_PORTA_PCTL_R;
  reg &= 0x00FFFF00;                             // Clear bits
  GPIO_PORTA_PCTL_R |= GPIO_PCTL_PA2_SSI0CLK
                    | GPIO_PCTL_PA3_SSI0FSS
                    | GPIO_PCTL_PA4_SSI0RX
                    | GPIO_PCTL_PA5_SSI0TX;      // Set bits

  // SSI 0 master mode and disable SSI
  SSI0_CR1_R = 0;

  // Configure the SSI clock source (SSICC)
  SSI0_CC_R = 0;                  // Use system clock

  // Configure the clock prescale divisor (SSICPSR)
  // SSInClk = SysClk / (CPSDVSR * (1 + SCR))
  // SSInClk = 10Mhz
  SSI0_CPSR_R = 10;       // CPSDVSR
                          // SCR is deafult 0

  // Write the SSICR0 register with
  // Serial clock rate (SCR)
  // Desired clock phase/polarity, if using Freescale SPI mode (SPH and SPO)
  // The protocol mode: Freescale SPI
  // The data size (DSS)

  bit_clear (SSI0_CR0_R, SSI_CR0_SPH);    // SPH = 0
  bit_set (SSI0_CR0_R, SSI_CR0_SPO);    // SPO = 0
  bit_clear (SSI0_CR0_R, 0b110000 );      // Freescale SPI Frame Format
  bit_set (SSI0_CR0_R, SSI_CR0_DSS_16);   // DDS = 16 bit

  // Enable SSI
  bit_set (SSI0_CR1_R, SSI_CR1_SSE);
#endif
}

void hardware_init( INT16U sample_freq )
/*****************************************************************************
*   Header description
******************************************************************************/
{
  // disable global interrupt
  disable_global_int();

  // Set processor frequency to max
  set_80Mhz_clock();

  // Enable the FPU
  enable_FPU();

  // Initialize debug pins on portB
  init_debug_pins();

  // Initialize the Tiva board
  init_tiva_board();

  // Calculate the number off clock-cycles between each sample
  cycles = cycles_per_sample( sample_freq );

  //
  #ifdef EMP
    adc_pwm_ratio = (FP32)cycles/4096;
  #else
    adc_pwm_ratio = (FP32)cycles/8192;
  #endif

  init_ADC();

  init_PWM( cycles );

  delay_init();

  timer_init();

  systick_init();

  spi_init();

  // Enable global interrupt
  enable_global_int();

}

/****************************** End Of Module *******************************/
