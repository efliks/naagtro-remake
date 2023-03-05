/*****************************************
  NAAGTRO version 2

  scroll.c   Scrolling text effect

  (C) March 5, 2023  M. Feliks
*****************************************/

#include <string.h>

#include "scroll.h"
#include "naagtro.h"

static char scrolltext[] = "New Age ASM Group proudly presents a short intro made by Majuma in pure 32-bit Assembler ;) *** If you want to join our group or just have any reason for contacting us, visit our web page: www.naag.prv.pl *** Greetings are scrolling to all members of NAAG: HaRv3sTeR, Klemik, tOudi, SEM, pkmiecik, Overlord, Oolv, Miodzio, asmCode, _TeStON_, Zedd and anybody I forgot... Keep coding !                              ";

static unsigned char scrollbuffer[320 * 8];
static unsigned int curr_char = 0, curr_col = 0;


void init_scroll()
{
    memset(scrollbuffer, 0, 320 * 8);
}

void do_scroll(int y, unsigned char* frame_buffer)
{
    char my_char = scrolltext[curr_char % (sizeof (scrolltext) / sizeof (char))];
    if (my_char > 'a' && my_char < 'z') {
       my_char += ('A' - 'a');
    }

    unsigned char* ptr_char = &chardata[((my_char - 32) & 63) << 3];
    int col_idx = 7 - (curr_col & 7);

    for (int i = 0; i < 8; i++, ptr_char++) {
        if ((*ptr_char >> col_idx) & 1) {
            *(scrollbuffer + i * 320 + 319) = (unsigned char)64;
        }
        else {
            *(scrollbuffer + i * 320 + 319) = 0;
        }
    }

    for (int i = 0; i < 8; i++) {
        unsigned char* ptr_buffer = scrollbuffer + 320 * i;
        for (int j = 0; j < 319; j++, ptr_buffer++) {
            *ptr_buffer = *(ptr_buffer + 1);
        }
    }

    unsigned char* ptr_buffer = scrollbuffer;
    unsigned char* ptr_fb = frame_buffer + y * 320;

    for (int i = 0; i < 320 * 8; i++, ptr_buffer++, ptr_fb++) {
        if (*ptr_buffer != 0) {
           *ptr_fb = *ptr_buffer;
        }
    }

    curr_col++;
    if (curr_col % 8 == 0) {
       curr_char++;
    }
}
