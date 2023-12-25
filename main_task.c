#include "main_task.h"
#include "usb_task.h"
#include <stdbool.h>
#include "RingBuffer/lwrb.h"
#include "CH58x_common.h"

__attribute__((aligned(4))) uint8_t device_mode;
tmosTaskID main_task_id;

typedef struct  SendMSG
{
  tmos_event_hdr_t hdr;
  void            *pData;
} SendMSG_t;

uint8 OnBoard_SendMsg(uint8_t registeredTaskID, uint8 event, uint8 state, void *data)
{
    SendMSG_t *msgPtr;

  if ( registeredTaskID != TASK_NO_TASK )
  {
    // Send the address to the task
    msgPtr = ( SendMSG_t * ) tmos_msg_allocate( sizeof(SendMSG_t));
    if ( msgPtr )
    {
      msgPtr->hdr.event = event;
      msgPtr->hdr.status = state;
      msgPtr->pData = data;
      tmos_msg_send( registeredTaskID, ( uint8 * ) msgPtr );

    }
    return ( SUCCESS );
  }
  else
  {
    return ( FAILURE );
  }
}


const uint32_t IOmap[] = {1<<0, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7, 1<<8, 1<<9, 1<<12, 1<<13, 1<<14, 1<<15};

#define Key_Row1                      (R32_PA_PIN&GPIO_Pin_0)
#define Key_Row2                      (R32_PA_PIN&GPIO_Pin_1)
#define Key_Row3                      (R32_PA_PIN&GPIO_Pin_2)
#define Key_Row4                      (R32_PA_PIN&GPIO_Pin_3)
#define Key_Row5                      (R32_PA_PIN&GPIO_Pin_6)

const uint8_t keytale_8b[] =                    //默认的键码表
{
//        R0   R1   R2   R3   R4   R5
        0x00, 0x29, 0x35, 0x2b, 0x39, 0x02, 0x01, //c0                 //esc           ~`              tab         caps-lock  shift-L      ctr-l
        0x00, 0x3a, 0x1e, 0x14, 0x04, 0x1d, 0x08, //c1                 //F1            1!              Q           A          Z            win-l
        0x00, 0x3b, 0x1f, 0x1a, 0x16, 0x1b, 0x04, //c2                 //f2            2@              W           S          X            alt-l
        0x00, 0x3c, 0x20, 0x08, 0x07, 0x06, 0x2c, //c3                 //F3            3#              E           D          C            Space
        0x00, 0x3d, 0x21, 0x15, 0x09, 0x19, 0x40, //c4                 //F4            4$              R           F          V            alt-r
        0x00, 0x3e, 0x22, 0x17, 0x0a, 0x05, 0xfe, //c5                 //F5            5%              T           G          B            Fn

        0x00, 0x3f, 0x23, 0x1c, 0x0b, 0x11, 0x10, //c6                 //F6            6^              Y           H          N
        0x00, 0x40, 0x24, 0x18, 0x0d, 0x10, 0x80, //c7                 //F7            7&              U           J          M            win-r
        0x00, 0x41, 0x25, 0x0c, 0x0e, 0x36, 0x10, //c8                 //F8            8*              I           K          <,           ctr-r
        0x00, 0x42, 0x26, 0x12, 0x0f, 0x37, 0x00, //c9                 //F9            9               O           L          >.
        0x00, 0x43, 0x27, 0x13, 0x33, 0x38, 0x00, //c10                //F10           0               P           ;:         /?
        0x00, 0x44, 0x2d, 0x2f, 0x34, 0x20, 0x00, //c11                //F11           -_              [{          '"         shift-r
        0x00, 0x45, 0x2e, 0x30, 0x31, 0x00, 0x00, //c12                //F12           =+              ]}          \|

        0x00, 0x00, 0x2a, 0x00, 0x28, 0x00, 0x00, //c13                //0x00          Backspace                   Enter-R
        0x00, 0x46, 0x49, 0x4c, 0x00, 0x00, 0x50, //c14                //Print-screen  Insert          Delete      0x00,       0x00,       左
        0x00, 0x47, 0x4a, 0x4d, 0x00, 0x52, 0x51, //c15                //Scroll-Lock   Home            End         0x00,       上                           下
        0x00, 0x48, 0x4b, 0x4e, 0x00, 0x00, 0x4f, //c16                //Pause         Page-Up         Page-Down   0x00        0x00        右
        0x00, 0x00, 0x53, 0x5f, 0x5c, 0x59, 0x00, //c17                //Backlight     Num-lock        7HOME       4(小键盘)   1End       0x00
        0x00, 0x00, 0x54, 0x60, 0x5d, 0x5a, 0x62, //c18                //Locking       /               8(小键盘)   5(小键盘)   2(小键盘)   0Ins
        0x00, 0x00, 0x55, 0x61, 0x5e, 0x5b, 0x63, //c19                //0x00          *               9Pgup       6(小键盘)   3PgDn       =del
        0x00, 0x00, 0x56, 0x57, 0x00, 0x00, 0x58 //c20                 //0x00          -               +           0x00        0x00        Enter-R2
};

void index2keyVal_8(uint8_t *index, uint8_t *keyVal, uint8_t len)
{

    for (int b = 0, idx = 0; b < len; b++) {
        if(!keytale_8b[index[b]]) continue;
        keyVal[2 + idx++] = keytale_8b[index[b]];
    }
}

__HIGH_CODE
void keyScan(uint8_t *pbuf, uint8_t *key_num)
{
    uint8_t KeyNum;
    static uint8_t secbuf[120];

    uint8_t firstbuf[120] = { 0 };

    KeyNum = 0;

    for(uint8_t i = 0; i < 14; i++){
        GPIOB_ResetBits(IOmap[i]);
        __nop();__nop();
        {
            if (Key_Row1 == 0) {
                firstbuf[KeyNum++] = i * 7 + 1;
            }
            if (Key_Row2 == 0) {
                firstbuf[KeyNum++] = i * 7 + 2;
            }
            if (Key_Row3 == 0) {
                firstbuf[KeyNum++] = i * 7 + 3;
            }
            if (Key_Row4 == 0) {
                firstbuf[KeyNum++] = i * 7 + 4;
            }
            if (Key_Row5 == 0) {
                firstbuf[KeyNum++] = i * 7 + 5;
            }

        }
        GPIOB_SetBits(IOmap[i]);

        while(!(Key_Row1 && Key_Row2 && Key_Row3 && Key_Row4 && Key_Row5)) {
            continue;
        }
    }

    if (tmos_memcmp(firstbuf, secbuf, sizeof(firstbuf)) == true)
    {
        tmos_memcpy(pbuf, secbuf, sizeof(firstbuf));
        *key_num = KeyNum;
    }

    tmos_memcpy(secbuf, firstbuf, sizeof(firstbuf));
}


bool readKeyVal(void) {

    static uint8_t current_key_index[120] = { 0 };
    static uint8_t last_key_index[120] = { 0 };
    uint8_t key_num = 0;
    uint8_t save_key16[16] = { 0 };
    uint8_t save_key8[8] = { 0 };

    keyScan(current_key_index, &key_num);

    if (tmos_memcmp(current_key_index, last_key_index, sizeof(current_key_index)) == true) { // key not changed
        return false;
    }
    tmos_memcpy(last_key_index, current_key_index, sizeof(current_key_index));

#define  KEY_MODE    8

#if KEY_MODE==8
    //hotkey deal
    // hotkeydeal(current_key_index, save_key8, key_num);

    index2keyVal_8(current_key_index, save_key8, key_num);

    // PRINT("key=[");
    // for(int i = 0; i < 8; i++){
    //     if(i) PRINT(" ");
    //     PRINT("%#x", save_key8[i]);
    // }PRINT("]\n");

    // static bool isFnpress = false;
    // if(SpecialKey(save_key8)){  // after fn press must be a key release
    //     isFnpress = true;
    //     return SpecialKey_Deal();
    // } else{
    //     if(isFnpress) {
    //         isFnpress = false;
    //         return false;  // no keys change
    //     }
    // }

    uint8_t report_id = 0;
    lwrb_write(&KEY_buff, &report_id, 1);
    size_t wl = lwrb_write(&KEY_buff, save_key8, 8);
    if(wl != 8)
        lwrb_skip(&KEY_buff, wl);


#elif KEY_MODE==16
    index2keyVal_16(current_key_index, save_key16, key_num);
    uint8_t report_id = KEYBIT_ID;
    lwrb_write(&KEY_buff, &report_id, 1);
    size_t wl = lwrb_write(&KEY_buff, save_key16, 16);
    if(wl != 16)
        lwrb_skip(&KEY_buff, wl);
#endif

    return true;
}


tmosEvents main_process_event( tmosTaskID task_id, tmosEvents events ) {
    uint8 * msg_ptr;
    if ( events & SYS_EVENT_MSG ) {
        if (readKeyVal()){
            switch(device_mode) {
            case MODE_USB:
                OnBoard_SendMsg(usb_task_id, KEY_MESSAGE, 1, NULL);
                break;
            default:
                break;
            }
        }
        return events ^ SYS_EVENT_MSG;
    }
}

void main_task_init() {
    main_task_id = TMOS_ProcessEventRegister( main_process_event );
}