/*
  Copyright (c) 2023 Daniel Zwirner
  SPDX-License-Identifier: MIT-0
*/

#include <stdio.h>
#include "balance.h"
#include "hid.h"
#include "cilo72/hw/blink_forever.h"
#include "cilo72/hw/gpiokey.h"
#include "cilo72/hw/uart.h"
#include "cilo72/hw/elapsed_timer_ms.h"
#include "cilo72/ic/ws2812.h"
#include "cilo72/graphic/color.h"
#include <string.h>

/*
 1 : UART0 TX/I2C0 SDA/SPI0  RX/GP00   -> UART TX
 2 : UART0 RX/I2C0 SCL/SPI0  CS/GP01   <- UART RX
 3 :                             GND
 4 :          I2C1 SDA/SPI0 SCK/GP02
 5 :          I2C1 SCL/SPI0  TX/GP03
 6 : UART1 TX/I2C0 SDA/SPI0  RX/GP04
 7 : UART1 RX/I2C0 SCL/SPI0  CS/GP05
 8 :                             GND
 9 :          I2C1 SDA/SPI0 SCK/GP06
10 :          I2C1 SCL/SPI0  TX/GP07
11 : UART1 TX/I2C0 SDA/SPI1  RX/GP08
12 : UART1 RX/I2C0 SCL/SPI1  CS/GP09
13 :                             GND
14 :          I2C1 SDA/SPI1 SCK/GP10
15 :          I2C1 SCL/SPI1  TX/GP11
16 : UART0 TX/I2C0 SDA/SPI1  RX/GP12
17 : UART0 RX/I2C0 SCL/SPI1  CS/GP13  <- Key TAB
18 :                             GND
19 :          I2C1 SDA/SPI1 SCK/GP14  <- Key ENTER
20 :          I2C1 SCL/SPI1  TX/GP15  <- Key

21 : UART0 TX/I2C0 SDA/SPI0  RX/GP16  -> Neopixel
22 : UART0 RX/I2C0 SCL/SPI0  CS/GP17
23 :                             GND
24 :          I2C1 SDA/SPI0 SCK/GP18
25 :          I2C1 SCL/SPI0  TX/GP19
26 :          I2C0 SDA/        /GP20
27 :          I2C0 SCL/        /GP21
28 :                             GND
29 :                            GP22
30 :                             RUN
31 :          I2C1 SDA/ADC0    /GP26
32 :          I2C1 SCL/ADC1    /GP27
33 :                             GND
34 :                  ADC2     /GP28
35 :                  ADC_VREF
36 :                         3V3_OUT    -> 3V3
37 :                          3V3_EN
38 :                             GND
39 :                            VSYS    <- 5V
40 :                            VBUS
*/

static constexpr uint8_t PIN_UART_TX = 0;
static constexpr uint8_t PIN_UART_RX = 1;

static constexpr uint8_t PIN_KEY_TAB   = 13;
static constexpr uint8_t PIN_KEY_ENTER = 14;
static constexpr uint8_t PIN_KEY       = 15;

static constexpr uint8_t PIN_NEO_PIXEL = 16;

int main()
{
  cilo72::hw::BlinkForever blink(PICO_DEFAULT_LED_PIN, 1);
  cilo72::hw::GpioKey keyTab(PIN_KEY_TAB, cilo72::hw::Gpio::Pull::Up);
  cilo72::hw::GpioKey keyEnter(PIN_KEY_ENTER, cilo72::hw::Gpio::Pull::Up);
  cilo72::hw::GpioKey key(PIN_KEY, cilo72::hw::Gpio::Pull::Up);
  cilo72::hw::Uart uart(PIN_UART_RX, PIN_UART_TX, 9600, 8, 1, UART_PARITY_NONE);
  cilo72::ic::WS2812 neoPixel(PIN_NEO_PIXEL, 1);
  cilo72::hw::ElapsedTimer_ms timer;
  Balance balance(uart);
  cilo72::graphic::Color color(cilo72::graphic::Color::green);

  key.pressed();
  keyTab.pressed();
  keyEnter.pressed();

  neoPixel.set(0, 128, 0);
  neoPixel.update();
  hid_init();

  std::function<void(const char *)> doit = [&](const char *postfix) -> void
  {
    char weight[20];
    weight[0] = 0;
    neoPixel.set(0, 0, 128);
    neoPixel.update();
    timer.start();
    if (balance.weightValueImmediately(weight))
    {
      color = cilo72::graphic::Color::green;
      strcat(weight, postfix);
      hid_send(weight);
    }
    else
    {
      color = cilo72::graphic::Color::red;
    }

    //Clear any pressed button
    key.pressed();
    keyTab.pressed();
    keyEnter.pressed();
  };

  while (1)
  {
    if (key.pressed())
    {
      doit("");
    }

    if (keyTab.pressed())
    {
      doit("\t");
    }

    if (keyEnter.pressed())
    {
      doit("\n");
    }
    //uart.transmit("abc\n");

    if(timer.isValid() and timer.elapsed() > 1000)
    {
      neoPixel.set(color.r(), color.g(), color.b());
      neoPixel.update();
      timer.invalidate();
    }
  }
}
