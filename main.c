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

int main(void)
{
    clockInit();
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
    SEGGER_RTT_WriteString(0, "SEGGER Real-Time-Terminal Sample\r\n\r\n");
    gpioPinSetDir(GPIO_PIN_DIR_OUTPUT, 5);
    uint32_t state = 0x01;
    /* Toggle LEDs. */
    while (true)
    {
        gpioWritePin(5, state);
        for (volatile uint32_t i = 0; i < 1000000; i++);
        state ^= 0x01;        
    }
}

/*************************** End of file ****************************/
