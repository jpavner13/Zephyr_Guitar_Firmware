#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>

#define SLEEP_TIME 1000

const struct gpio_dt_spec spec =
    GPIO_DT_SPEC_GET_BY_IDX(DT_NODELABEL(green_led_2), gpios, 0);

int main(void) {
  int cnt = 0;

  struct device *dev;
  dev = device_get_binding(spec.port);

  while (1) {
    gpio_pin_set(dev, spec.pin, cnt % 2);
    cnt++;
    k_msleep(SLEEP_TIME);
  }

  return 0;
}
