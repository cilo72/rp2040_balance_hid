#include "hid.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "pico/util/queue.h"
#include "pico/multicore.h"
#include "cilo72/hw/elapsed_timer_ms.h"

typedef struct
{
    uint8_t keycode[6];
    uint8_t modifier;
} queue_entry_t;

bool sendNextKey = true;
queue_t call_queue;

void hid_entry()
{
    static cilo72::hw::ElapsedTimer_ms timer;
    queue_entry_t entry;
    bool keyAvailable = false;
    tusb_init();
    timer.start();

    while (1)
    {
        tud_task(); // tinyusb device task

        if (timer.hasExpired(1))
        {
            timer.start();

            if(sendNextKey == true and keyAvailable == false)
            {
                if(queue_try_remove(&call_queue, &entry))
                {
                    keyAvailable = true;
                }
            }

            if (tud_suspended() && keyAvailable)
            {
                // Wake up host if we are in suspend mode
                // and REMOTE_WAKEUP feature is enabled by host
                tud_remote_wakeup();
            }
            else
            {
                if (keyAvailable)
                {
                    keyAvailable = false;
                    sendNextKey  = false;

                    if (entry.keycode[0] == HID_KEY_NONE)
                    {
                        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
                    }
                    else
                    {
                        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, entry.modifier, entry.keycode);
                    }
                    
                }
            }
        }
    }
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint16_t len)
{
    (void)instance;
    (void)len;

    sendNextKey = true;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
    // TODO not Implemented
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
    (void)instance;

    if (report_type == HID_REPORT_TYPE_OUTPUT)
    {
        // Set keyboard LED e.g Capslock, Numlock etc...
        if (report_id == REPORT_ID_KEYBOARD)
        {
            // bufsize should be (at least) 1
            if (bufsize < 1)
                return;

            uint8_t const kbd_leds = buffer[0];

            if (kbd_leds & KEYBOARD_LED_CAPSLOCK)
            {
            }
            else
            {
            }
        }
    }
}

void charToCode(char c, uint8_t * code, uint8_t * modifier)
{
    *modifier = 0;
    switch(c)
    {
        case '0'  : *code = HID_KEY_0;                                                   return;
        case '1'  : *code = HID_KEY_1;                                                   return;
        case '2'  : *code = HID_KEY_2;                                                   return;
        case '3'  : *code = HID_KEY_3;                                                   return;
        case '4'  : *code = HID_KEY_4;                                                   return;
        case '5'  : *code = HID_KEY_5;                                                   return;
        case '6'  : *code = HID_KEY_6;                                                   return;
        case '7'  : *code = HID_KEY_7;                                                   return;
        case '8'  : *code = HID_KEY_8;                                                   return;
        case '9'  : *code = HID_KEY_9;                                                   return;
        case ' '  : *code = HID_KEY_SPACE;                                               return;
        case '\n' : *code = HID_KEY_ENTER;                                               return;
        case '\t' : *code = HID_KEY_TAB;                                                 return;
        case ','  : *code = HID_KEY_COMMA;                                               return;
        case ';'  : *code = HID_KEY_SEMICOLON;                                           return;
        case '.'  : *code = HID_KEY_PERIOD;                                              return;
        case ':'  : *code = HID_KEY_SEMICOLON;  *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case '-'  : *code = HID_KEY_MINUS;                                               return;
        case '_'  : *code = HID_KEY_MINUS;      *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case '='  : *code = HID_KEY_EQUAL;                                               return;
        case '+'  : *code = HID_KEY_EQUAL;      *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case '*'  : *code = HID_KEY_8;                                                   return;
        case 'a'  : *code = HID_KEY_A;                                                   return;
        case 'A'  : *code = HID_KEY_A;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case 'b'  : *code = HID_KEY_B;                                                   return;
        case 'B'  : *code = HID_KEY_B;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case 'c'  : *code = HID_KEY_C;                                                   return;
        case 'C'  : *code = HID_KEY_C;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case 'd'  : *code = HID_KEY_D;                                                   return;
        case 'D'  : *code = HID_KEY_D;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case 'e'  : *code = HID_KEY_E;                                                   return;
        case 'E'  : *code = HID_KEY_E;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case 'f'  : *code = HID_KEY_F;                                                   return;
        case 'F'  : *code = HID_KEY_F;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case 'g'  : *code = HID_KEY_G;                                                   return;
        case 'G'  : *code = HID_KEY_G;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case 'h'  : *code = HID_KEY_H;                                                   return;
        case 'H'  : *code = HID_KEY_H;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case 'i'  : *code = HID_KEY_I;                                                   return;
        case 'I'  : *code = HID_KEY_I;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case 'j'  : *code = HID_KEY_J;                                                   return;
        case 'J'  : *code = HID_KEY_J;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case 'k'  : *code = HID_KEY_K;                                                   return;
        case 'K'  : *code = HID_KEY_K;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case 'l'  : *code = HID_KEY_L;                                                   return;
        case 'L'  : *code = HID_KEY_L;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case 'm'  : *code = HID_KEY_M;                                                   return;
        case 'M'  : *code = HID_KEY_M;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case 'n'  : *code = HID_KEY_N;                                                   return;
        case 'N'  : *code = HID_KEY_N;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case 'o'  : *code = HID_KEY_O;                                                   return;
        case 'O'  : *code = HID_KEY_O;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case 'p'  : *code = HID_KEY_P;                                                   return;
        case 'P'  : *code = HID_KEY_P;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case 'q'  : *code = HID_KEY_Q;                                                   return;
        case 'Q'  : *code = HID_KEY_Q;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case 'r'  : *code = HID_KEY_R;                                                   return;
        case 'R'  : *code = HID_KEY_R;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case 's'  : *code = HID_KEY_S;                                                   return;
        case 'S'  : *code = HID_KEY_S;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case 't'  : *code = HID_KEY_T;                                                   return;
        case 'T'  : *code = HID_KEY_T;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case 'u'  : *code = HID_KEY_U;                                                   return;
        case 'U'  : *code = HID_KEY_U;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case 'v'  : *code = HID_KEY_V;                                                   return;
        case 'V'  : *code = HID_KEY_V;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case 'w'  : *code = HID_KEY_W;                                                   return;
        case 'W'  : *code = HID_KEY_W;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case 'x'  : *code = HID_KEY_X;                                                   return;
        case 'X'  : *code = HID_KEY_X;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case 'y'  : *code = HID_KEY_Y;                                                   return;
        case 'Y'  : *code = HID_KEY_Y;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
        case 'z'  : *code = HID_KEY_Z;                                                   return;
        case 'Z'  : *code = HID_KEY_Z;          *modifier = KEYBOARD_MODIFIER_LEFTSHIFT; return;
    }
}

void hid_send(const char *s)
{
    queue_entry_t entry = {0};
    for (const char *p = s; *p; p++)
    {
        charToCode(*p, entry.keycode, &entry.modifier);
        queue_try_add(&call_queue, &entry);
        entry.keycode[0] = HID_KEY_NONE;
        queue_try_add(&call_queue, &entry);
    }
}

void hid_init()
{
    queue_init(&call_queue, sizeof(queue_entry_t), 100);
    multicore_launch_core1(hid_entry);
}