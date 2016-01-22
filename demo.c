#include "sys/autostart.h"
#include "sys/etimer.h"
#include "zmq.h"
#include "sub.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dev/button-sensor.h"
#include "dev/leds.h"
#include "dev/radio-sensor.h"
#include "dev/sensor-common.h"

#include "st-lib.h"

#ifdef X_NUCLEO_IKS01A1
#include "dev/temperature-sensor.h"
#include "dev/humidity-sensor.h"
#include "dev/pressure-sensor.h"
#include "dev/magneto-sensor.h"
#include "dev/acceleration-sensor.h"
#include "dev/gyroscope-sensor.h"
#endif /*X_NUCLEO_IKS01A1*/

PROCESS(demo, "Sensor demo");
AUTOSTART_PROCESSES(&demo);

zmq_socket_t pub, sub;
struct etimer etimer;
LIST(read_list);

static const char *uri_blue_led_toggle = "action://nucleo-demo/led/blue/toggle";
static const char *uri_blue_led_on = "action://nucleo-demo/led/blue/on";
static const char *uri_blue_led_off = "action://nucleo-demo/led/blue/off";

#define VALUE_UPDATE(sock, uri, value) \
    do{ \
        msg = zmq_msg_from_const_data(0, uri "\0" value, strlen(uri) + 1 +strlen(value)); \
        PT_WAIT_THREAD(process_pt, sock.send(&sock, msg)); \
        zmq_msg_destroy(&msg); \
    }while(0)

#define CMP_MSG_STR(msg, s) ( \
    (zmq_msg_size(msg) == strlen(s)) && \
    !strncmp((const char *) zmq_msg_data(msg), s, strlen(s)) \
  )

PROCESS_THREAD(demo, ev, data) {
    static int sensor_value = 0;
    static zmq_msg_t *msg = NULL;

    PROCESS_BEGIN();

    SENSORS_ACTIVATE(button_sensor);

    SENSORS_ACTIVATE(radio_sensor);

  #ifdef X_NUCLEO_IKS01A1
    SENSORS_ACTIVATE(temperature_sensor);
    SENSORS_ACTIVATE(humidity_sensor);
    SENSORS_ACTIVATE(pressure_sensor);
    SENSORS_ACTIVATE(magneto_sensor);
    SENSORS_ACTIVATE(acceleration_sensor);
    SENSORS_ACTIVATE(gyroscope_sensor);
  #endif /*X_NUCLEO_IKS01A1*/

    zmq_init();
    zmq_socket_init(&sub, ZMQ_SUB);
    zmq_bind(&sub, 8888);

    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_POLL); // Wait for a publisher to connect
    // This is actually a defect of the exemple (and of the subscribe implementation)
    // because the subscribe message will only be sent to the first publisher connected.

    printf("Doing subscription\r\n");
    PT_WAIT_THREAD(process_pt, zmq_sub_subscribe(&sub, ""));

    zmq_socket_init(&pub, ZMQ_PUB);
    zmq_bind(&pub, 9999);

    while(1) {
        // if(etimer_expired(&etimer))
        //     etimer_set(&etimer, CLOCK_SECOND);

        PROCESS_WAIT_EVENT_UNTIL((ev == sensors_event) || (ev == zmq_socket_input_activity));

        printf("ev %d %p %p ", ev, data, &button_sensor);
        print_event_name(ev);
        printf("\r\n");
        if((ev == sensors_event) && (data == &button_sensor)) {
          printf("Sensor event detected: Button Pressed.\r\n");
          VALUE_UPDATE(pub, "sensor://nucleo-demo/button", "pressed");

          // BSP_LED_Toggle(LED3);
        }

        if(ev == zmq_socket_input_activity) {
            PT_WAIT_THREAD(process_pt, sub.recv_multipart(&sub, read_list));

            msg = list_pop(read_list);
            if(CMP_MSG_STR(msg, uri_blue_led_toggle))
                BSP_LED_Toggle(LED3);
            else if(CMP_MSG_STR(msg, uri_blue_led_on))
                BSP_LED_Off(LED3); // Inversed
            else if(CMP_MSG_STR(msg, uri_blue_led_off))
                BSP_LED_On(LED3); // Inversed

            while(msg != NULL) {
                printf("Received: ");
                uint8_t *data = zmq_msg_data(msg);
                uint8_t *pos = data;
                size_t size = zmq_msg_size(msg);
                while(pos < (data + size))
                    printf("%c", *pos++);
                printf("\r\n");

                zmq_msg_destroy(&msg);
                msg = list_pop(read_list);
            }
        }
    }

    PROCESS_END();
}
