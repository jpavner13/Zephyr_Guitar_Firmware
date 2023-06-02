#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>
#include <inttypes.h>

// Define thread stack size and priority
#define STACKSIZE 1024
#define BUTTON_PRIORITY 7
#define BLINK_PRIORITY 7

// INITIALIZE THE USER BUTTON
#define SW0_NODE DT_ALIAS(sw0)
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});

// INITIALIZE THE ON BOARD LED
static struct gpio_dt_spec onboardLed = GPIO_DT_SPEC_GET_OR(DT_ALIAS(led0), gpios, {0});

// INITIALIZE THE RED LED
static struct gpio_dt_spec led = GPIO_DT_SPEC_GET_OR(DT_ALIAS(redled), gpios, {0});

// Thread 0 Function
void buttonLED(void)
{
  while (1) 
  {
    int val = gpio_pin_get_dt(&button);

    if (val >= 0) 
    {
      gpio_pin_set_dt(&led, val);
    }
  }
}

// Thread 1 Function
void blink()
{
  while(1)
  {
    if((k_uptime_get() % 1000) == 0)
    {
      gpio_pin_toggle_dt(&onboardLed);
    }
  }
}

// Main funtion (Basically a thread with priority 1)
void main()
{
  // Configure Button as an input
  gpio_pin_configure_dt(&button, GPIO_INPUT);
  gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);

  // Configure LED's as an output
  gpio_pin_configure_dt(&onboardLed, GPIO_OUTPUT);
  gpio_pin_configure_dt(&led, GPIO_OUTPUT);
}

K_THREAD_DEFINE(buttonLED_thread, STACKSIZE, buttonLED, NULL, NULL, NULL, BUTTON_PRIORITY, 0, 0);
K_THREAD_DEFINE(blinkLED_thread, STACKSIZE, blink, NULL, NULL, NULL, BLINK_PRIORITY, 0, 0);