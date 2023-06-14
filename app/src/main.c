#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/adc.h>
// #include <zephyr/drivers/pwm.h>

// DEFINE THREAT STACK SIZE AND PRIORITY
#define STACKSIZE 1024
#define PLAY_PRIORITY 7

// INITIALIZE ANALOG CHANNELS
#define DT_SPEC_AND_COMMA(node_id, prop, idx) \
	ADC_DT_SPEC_GET_BY_IDX(node_id, idx),

static const struct adc_dt_spec adc_channels[2] = {
	DT_FOREACH_PROP_ELEM(DT_PATH(zephyr_user), io_channels, DT_SPEC_AND_COMMA)
};

uint16_t buf;
struct adc_sequence sequence = {
  .buffer = &buf,
  .buffer_size = sizeof(buf),
};

// INITIALIZE THE SOLENOIDS
static const struct gpio_dt_spec solenoid_1 = GPIO_DT_SPEC_GET_OR(DT_ALIAS(d8), gpios, {0});
int32_t lastTimeFired = 0;

// Function to fire the passed solenoid to play the corresponding string
void fireSolenoid(struct gpio_dt_spec solenoid)
{
  int32_t startTime = k_uptime_get();
  int32_t currTime = k_uptime_get();

  int cooldownTime = 200;
  int32_t fireSpeed = 50;

  if(currTime - lastTimeFired > cooldownTime)
  {
    gpio_pin_set_dt(&solenoid, 0);

    bool doneFiring = false;

    while(!doneFiring)
    {
      if(currTime - startTime > fireSpeed)
      {
        gpio_pin_set_dt(&solenoid, 1);
        lastTimeFired = k_uptime_get(); 
        doneFiring = true;
        break;
      }
      currTime = k_uptime_get();
      k_usleep(10);
    }
  }
}

// Main thread function for controlling playing
void playGuitar()
{
  int photoResistorTolereance = 3200;
  int flexSensorTolerence = 1000;

  gpio_pin_set_dt(&solenoid_1, 1);

  int32_t photoResistor1Value;
  int32_t flexSensor1Value;

  int err;

  while(true)
  {
    // Read the photoresistor
    (void)adc_sequence_init_dt(&adc_channels[0], &sequence);

    // Check to make sure the photo resistor read an expected value
    err = adc_read(adc_channels[0].dev, &sequence);
    if (err < 0) 
    {
      printk("Could not read (%d)\n", err);
      continue;
    }

    photoResistor1Value = (int32_t)buf;

    // Test if laser has been broken
    if(photoResistor1Value < photoResistorTolereance)
    {
      fireSolenoid(solenoid_1);
    }

    // Read the flex sensor
    (void)adc_sequence_init_dt(&adc_channels[1], &sequence);

    // Check to make sure the flex sensor read an expected value
    err = adc_read(adc_channels[1].dev, &sequence);
    if (err < 0) 
    {
      printk("Could not read (%d)\n", err);
      continue;
    }

    flexSensor1Value = (int32_t)buf;

    // Test if flex sensor has been hit
    if(flexSensor1Value < flexSensorTolerence)
    {
      fireSolenoid(solenoid_1);
    }
  }
  return;
}

// Entry point. Configure peripherals
void main()
{
  int err;

  // Configure solenoids
  err = gpio_pin_configure_dt(&solenoid_1, GPIO_OUTPUT);
  if(err < 0)
  {
    printk("Could not configure solenoid 1.");
  }

  // Set up analog inputs (photo resistor, flex sensor)
	for (size_t i = 0U; i < ARRAY_SIZE(adc_channels); i++) {
		if (!device_is_ready(adc_channels[i].dev)) {
			printk("ADC controller device %s not ready\n", adc_channels[i].dev->name);
		}

		err = adc_channel_setup_dt(&adc_channels[i]);
		if (err < 0) 
    {
			printk("Could not setup channel #%d (%d)\n", i, err);
		}
	}
}

// BEGIN THREADS
K_THREAD_DEFINE(playGuitar_thread, STACKSIZE, playGuitar, NULL, NULL, NULL, PLAY_PRIORITY, 0, 0);