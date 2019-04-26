#include <stdbool.h>
#include <stdint.h>
#include <SEGGER_RTT.h>
#include <SEGGER_RTT_Conf.h>

#define GPIO_BASE_ADDRESS (0x50000000)
#define GPIO_PORT_OUT (*(volatile uint32_t *)(GPIO_BASE_ADDRESS + 0x504)) //Write GPIO port
#define GPIO_PORT_OUT_SET (*(volatile uint32_t *)(GPIO_BASE_ADDRESS + 0x508)) //Set individual bits in GPIO port
#define GPIO_PORT_OUT_CLR (*(volatile uint32_t *)(GPIO_BASE_ADDRESS + 0x50C)) //Clear individual bits in GPIO port
#define GPIO_PORT_IN (*(volatile uint32_t *)(GPIO_BASE_ADDRESS + 0x510)) //Read GPIO port
#define GPIO_PORT_DIR (*(volatile uint32_t *)(GPIO_BASE_ADDRESS + 0x514)) //Direction of GPIO pins
#define GPIO_PORT_DIR_SET (*(volatile uint32_t *)(GPIO_BASE_ADDRESS + 0x518)) //DIR set register
#define GPIO_PORT_DIR_CLR (*(volatile uint32_t *)(GPIO_BASE_ADDRESS + 0x51C)) //DIR clear register
#define GPIO_PORT_LATCH (*(volatile uint32_t *)(GPIO_BASE_ADDRESS + 0x520))) //Latch register indicating what GPIO pins that have met the criteria set in the PIN_CNF[n].SENSE registers
#define GPIO_PORT_DETECT_MODE (*(volatile uint32_t *)(GPIO_BASE_ADDRESS + 0x524)) //Select between default DETECT signal behaviour and LDETECT mode
#define GPIO_PORT_PIN_CNF_5 (*(volatile uint32_t *)(GPIO_BASE_ADDRESS + 0x714))


typedef enum GPIO_PIN_DIR {
    GPIO_PIN_DIR_INPUT,
    GPIO_PIN_DIR_OUTPUT,
} GPIO_PIN_DIR;

static void gpioPinSetDir(GPIO_PIN_DIR pinDir, uint32_t pin)
{
    //assert(pin < 32);
    GPIO_PORT_DIR_SET &= ~(pinDir << pin);
    GPIO_PORT_DIR_SET |= pinDir << pin;
}

static void gpioWritePin(uint32_t pin, bool value)
{
    //assert(pin < 32);
    if (value)
      GPIO_PORT_OUT_SET |= 0x01 << pin;
    else
      GPIO_PORT_OUT_CLR |= 0x01 << pin;
}

#define CLOCK_BASE_ADDRESS (0x40000000)
#define CLOCK_TASKS_HFCLK_START (*(volatile uint32_t *)(CLOCK_BASE_ADDRESS + 0x000)) // Start HFCLK crystal oscillator
#define CLOCK_TASKS_HFCLK_STOP (*(volatile uint32_t *)(CLOCK_BASE_ADDRESS + 0x004)) // Stop HFCLK crystal oscillator
#define CLOCK_TASKS_EVENTS_HFCLK_STARTED (*(volatile uint32_t *)(CLOCK_BASE_ADDRESS + 0x100)) // HFCLK oscillator started
#define CLOCK_HFCLK_RUN (*(volatile uint32_t *)(CLOCK_BASE_ADDRESS + 0x408)) // Status indicating that HFCLKSTART task has been triggered
#define CLOCK_HFCLK_STAT (*(volatile uint32_t *)(CLOCK_BASE_ADDRESS + 0x40C)) // HFCLK status

static void clockInit(void)
{
    CLOCK_TASKS_HFCLK_START |= 0x01;
    while(!(CLOCK_HFCLK_STAT & 0x01 << 16));
}

typedef enum TIMER_BASE_ADDRESS {
    TIMER0_BASE_ADDRESS = 0x40008000,
    TIMER1_BASE_ADDRESS = 0x40009000,
    TIMER2_BASE_ADDRESS = 0x4000A000, 
    TIMER3_BASE_ADDRESS = 0x4001A000, 
    TIMER4_BASE_ADDRESS = 0x4001B000,
} TIMER_BASE_ADDRESS;

#define TIMER_TASKS_START *((volatile uint32_t *) (TIMER0_BASE_ADDRESS + 0x000)) // Start Timer
#define TIMER_TASKS_STOP *((volatile uint32_t *) (TIMER0_BASE_ADDRESS + 0x004)) // Stop Timer
#define TIMER_TASKS_COUNT *((volatile uint32_t *) (TIMER0_BASE_ADDRESS + 0x008)) // Increment Timer (Counter mode only)
#define TIMER_TASKS_CLEAR *((volatile uint32_t *) (TIMER0_BASE_ADDRESS + 0x00C)) // Clear time
#define TIMER_TASKS_SHUTDOWN *((volatile uint32_t *) (TIMER0_BASE_ADDRESS + 0x010)) // Shut down timer

//TASKS_CAPTURE[0] 0x040 Capture Timer value to CC[0] register
//TASKS_CAPTURE[1] 0x044 Capture Timer value to CC[1] register
//TASKS_CAPTURE[2] 0x048 Capture Timer value to CC[2] register
//TASKS_CAPTURE[3] 0x04C Capture Timer value to CC[3] register
//TASKS_CAPTURE[4] 0x050 Capture Timer value to CC[4] register
//TASKS_CAPTURE[5] 0x054 Capture Timer value to CC[5] register
//EVENTS_COMPARE[0] 0x140 Compare event on CC[0] match
//EVENTS_COMPARE[1] 0x144 Compare event on CC[1] match
//EVENTS_COMPARE[2] 0x148 Compare event on CC[2] match
//EVENTS_COMPARE[3] 0x14C Compare event on CC[3] match
//EVENTS_COMPARE[4] 0x150 Compare event on CC[4] match
//EVENTS_COMPARE[5] 0x154 Compare event on CC[5] match

#define TIMER_SHORTS *((volatile uint32_t *) (TIMER0_BASE_ADDRESS + 0x200)) // Shortcut register
#define TIMER_INTENSET *((volatile uint32_t *) (TIMER0_BASE_ADDRESS + 0x304)) // Enable interrupt
#define TIMER_INTENCLR *((volatile uint32_t *) (TIMER0_BASE_ADDRESS + 0x308)) // Disable interrupt
#define TIMER_MODE *((volatile uint32_t *) (TIMER0_BASE_ADDRESS + 0x504)) // Timer mode selection
#define TIMER_BITMODE *((volatile uint32_t *) (TIMER0_BASE_ADDRESS + 0x508)) // Configure the number of bits used by the TIMER
#define TIMER_PRESCALER *((volatile uint32_t *) (TIMER0_BASE_ADDRESS + 0x510)) // Timer prescaler register

#define TIMER_CAPTURE_COMPARE0 *((volatile uint32_t *) (TIMER0_BASE_ADDRESS + 0x540)) // Capture/Compare register 0
//CC[1] 0x544 Capture/Compare register 1
//CC[2] 0x548 Capture/Compare register 2
//CC[3] 0x54C Capture/Compare register 3
//CC[4] 0x550 Capture/Compare register 4
//CC[5] 0x554 Capture/Compare register 5

// fTIMER = 16 MHz / (2PRESCALER)
static timerInit(void)
{
    TIMER_TASKS_STOP |= 0x01;
    TIMER_TASKS_CLEAR |= 0x01;
    TIMER_MODE &= 0x00; // Timer 0 Select Timer mode
    TIMER_BITMODE = 0x03; // 3 - 32 bit timer bit width
    TIMER_PRESCALER = 0x02; // 2 divider
    TIMER_CAPTURE_COMPARE0 = 4000000; // blink every 0.5 sec
    TIMER_INTENSET |= 0x01 << 16; // enable CC[0] interrupt
}

static startTimer(void)
{
    TIMER_TASKS_STOP |= 0x01;
    TIMER_TASKS_CLEAR |= 0x01;
    TIMER_TASKS_START |= 0x01;
}

static stopTimer(void)
{
    TIMER_TASKS_STOP |= 0x01;
}

uint32_t state = 0x01;

int main(void)
{
    clockInit();
    timerInit();
    startTimer();
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
    SEGGER_RTT_WriteString(0, "SEGGER Real-Time-Terminal Sample\r\n\r\n");
    gpioPinSetDir(GPIO_PIN_DIR_OUTPUT, 5);
    
    /* Toggle LEDs. */
    while (true)
    {
        //gpioWritePin(5, state);
       // for (volatile uint32_t i = 0; i < 1000000; i++);
        //state ^= 0x01;        
    }
}

void TIMER0_IRQHandler(void)
{
    gpioWritePin(5, state ^= 0x01);
}


/*************************** End of file ****************************/
