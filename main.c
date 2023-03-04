#include <stdlib.h>

#include "naagtro.h"
#include "bumpmap.h"
#include "low.h"

unsigned char* ptr_frame_buffer;


int main(void)
{
    set_mode13h();
    set_palette(naagpal);

    init_bump_mapping();
    
    ptr_frame_buffer = (unsigned char *)malloc(64000 * sizeof(unsigned char));

    while (1) {
        if (is_key_pressed()) {
            char key = get_key_code();
            if (key == 27) {  // ESC
                break;
            }
        } 
    }
    
    if (ptr_frame_buffer != NULL) {
        free(ptr_frame_buffer);
    }
    deinit_bump_mapping();
    unset_mode13h();

    return 0;
}
