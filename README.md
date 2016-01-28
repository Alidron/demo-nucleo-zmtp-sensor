Demo of a sensor node with a LED
================================

[![Gitter](https://badges.gitter.im/gitterHQ/gitter.svg)](https://gitter.im/Alidron/talk)

Part of the demo run at FOSDEM talk.

This demo use a Nucleo-Spirit1 node made of eval boards. It uses a ARM Cortex-M3 at 32MHz with a sub-1GHz RF module.

The program of this demo serve sensor data to connecting peers (another node and a computer program) and interact with them. It uses the ZMTP protocol with PUB/SUB sockets. Send an Alidron-like signal event when the user push the blue button of the Nucleo board. And it waits for alidron-like signal events to turn on/off or toggle a blue LED.

The blue LED has to be mounted as open-drain with a pull-up on pin A4.

You need to modify following files in Contiki to adapt to this blue LED:
* platform/stm32nucleo-spirit1/stm32cube-lib/drivers/stm32l1xx_nucleo/stm32l1xx_nucleo.c
* platform/stm32nucleo-spirit1/stm32cube-lib/drivers/stm32l1xx_nucleo/stm32l1xx_nucleo.c
* platform/stm32nucleo-spirit1/dev/leds-arch.c

The replacement files are provided in the folder `contiki-modifications` of this repository.

License and contribution policy
===============================

This project is licensed under LGPLv3.

Contiki-zmtp is licensed under MPLv2.

To contribute, please, follow the [C4.1](http://rfc.zeromq.org/spec:22) contribution policy.
