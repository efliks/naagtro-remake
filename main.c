/*****************************************
  NAAGTRO version 2

  main.c   Main program loop

  (C) March 5, 2023  M. Feliks
*****************************************/

#include <stdlib.h>

#include "naagtro.h"
#include "low.h"

#include "bumpmap.h"
#include "scroll.h"

static unsigned char* ptr_frame_buffer;


int main(void)
{
    set_mode13h();
    set_palette(naagpal);

    init_scroll();
    init_bump_mapping();

    ptr_frame_buffer = (unsigned char *)malloc(64000 * sizeof(unsigned char));

    while (1) {
        if (is_key_pressed()) {
            char key = get_key_code();
            if (key == 27) {  // ESC
                break;
            }
        }

        do_bump_mapping(ptr_frame_buffer);
        do_scroll(199 - 8, ptr_frame_buffer);

        copy_buffer(ptr_frame_buffer);
    }
    
    if (ptr_frame_buffer != NULL) {
        free(ptr_frame_buffer);
    }
    deinit_bump_mapping();
    unset_mode13h();

    return 0;
}
