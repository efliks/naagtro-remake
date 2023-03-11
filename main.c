/*****************************************
  NAAGTRO version 2

  main.c   Main program loop

  (C) March 5, 2023  M. Feliks
*****************************************/

#include <stdlib.h>
#include <string.h>

#include "naagtro.h"
#include "low.h"
#include "ticker.h"

#include "bumpmap.h"
#include "tunnel.h"
#include "scroll.h"

static unsigned char* ptr_frame_buffer;
static unsigned char palette_current[768], palette_target[768];

static int flash_count = 64;


void init_flash()
{
    if (flash_count > 63) {
       flash_count = 0;
    }
}

void do_flash()
{
    if (flash_count > 63) {
        return;
    }
    if (flash_count == 0) {
        load_palette(palette_target);

        // set all white palette
        memset(palette_current, 63, 768);
        set_palette(palette_current);
    }
    else {
        load_palette(palette_current);

        for (int i = 0; i < 768; i++) {
            if (palette_current[i] > palette_target[i]) {
               palette_current[i]--;
            }
        }
        set_palette(palette_current);
    }
    flash_count++;
}

int main(void)
{
    int do_what = 0;
    int switch_count = 800;

    ticker_init();

    set_mode13h();
    set_palette(naagpal);

    init_scroll();
    init_tunnel();
    init_bump_mapping();

    ptr_frame_buffer = (unsigned char *)malloc(64000 * sizeof(unsigned char));

    while (1) {
        ticker_start(34);  // 34 ms ~ 30 fps

        int do_switch = 0;
        if (is_key_pressed()) {
            char key = get_key_code();
            if (key == 27) {  // ESC
                break;
            }
            else if (key == 32) {  // space
                do_switch = 1;
            }
        }

        switch_count--;
        if (switch_count == 0) {
            do_switch = 1;
        }

        if (do_switch) {
            switch_count = 800;
            do_what++;
            if (do_what > 1) {
               do_what = 0;
            }
            init_flash();
        }

        if (do_what == 0) {
            do_bump_mapping(ptr_frame_buffer);
        }
        else {
            do_tunnel(ptr_frame_buffer);
        }

        do_flash();
        do_scroll(199 - 8, ptr_frame_buffer);
        copy_buffer(ptr_frame_buffer);

        ticker_wait();
    }
    
    if (ptr_frame_buffer != NULL) {
        free(ptr_frame_buffer);
    }
    deinit_bump_mapping();
    deinit_tunnel();
    
    unset_mode13h();
    ticker_done();

    return 0;
}
