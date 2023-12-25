#include "CH58x_common.h"
#include "usbd_core.h"
#include <stdio.h>
#include <string.h>
#include "CH58xBLE_LIB.h"
#include "RingBuffer/lwrb.h"
#include "hid_keyboard.c"

void keyInit(void)
{
    GPIOA_ModeCfg(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6, GPIO_ModeIN_PU);
    GPIOB_ModeCfg(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 
        | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15
        , GPIO_ModeOut_PP_5mA);
}


int main(void)
{
    extern void board_init(void);
    board_init();
    printf("CherryUSB device hid keyboard example\n");
    extern void hid_keyboard_init(void);
    hid_keyboard_init();

    ring_buffer_init();
    keyInit();

    // Wait until configured
    while (!usb_device_is_configured()) {
    }

    while (1) {
        // rf_sync();
        TMOS_SystemProcess();
    }

    // static uint32_t wait_ct = 100000;
    // // Everything is interrupt driven so just loop here
    // while (1) {
    //     extern void hid_keyboard_test(void);
    //     hid_keyboard_test();
    //     while (wait_ct--) {
    //     }
    //     uint8_t sendbuffer[8] = { 0 };
    //     usbd_ep_start_write(HID_INT_EP, sendbuffer, 8);
    //     wait_ct = 400000;
    //     while (wait_ct--) {
    //     }
    // }
    return 0;
}
