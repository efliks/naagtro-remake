#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h> // for memcpy

#include "naagtro.h"

extern "C" {
#include "low.h"
}


class DisplayController
{
public:
    DisplayController(unsigned char*);
    DisplayController();
    ~DisplayController();

    void blur();
    void flip();
    
    unsigned char* data();

private:
    void init();

    const int width_ = 320;
    const int height_ = 200;
    const int buffer_size_ = width_ * height_;

    unsigned char* buffer_;
};

DisplayController::DisplayController()
{
    init();
}

DisplayController::DisplayController(unsigned char* palette)
{
    init();
    set_palette(palette);
}

DisplayController::~DisplayController()
{
    unset_mode13h();
    if (buffer_ != nullptr) {
        delete[] buffer_;
    }
}

void DisplayController::init()
{
    set_mode13h();
    buffer_ = new unsigned char[static_cast<size_t>(buffer_size_)];
}

void DisplayController::blur()
{
    do_blur(buffer_, width_, height_);
}

void DisplayController::flip()
{
    copy_buffer(buffer_);
}

unsigned char* DisplayController::data()
{
    return buffer_;
}

void init_bumpmap(unsigned char* bumpmap)
{
    const int bmap_size = 65536;
    const int bmap_visible_size = 320 * 200;

    const int naaglogo_size = 320 * 70;

    const int filler_size = (bmap_visible_size - naaglogo_size) / 2;

    const int prefill_size = ( (bmap_size - bmap_visible_size) / 2 / 320 ) * 320;
    const int postfill_size = bmap_size - bmap_visible_size - prefill_size;

    unsigned char* ptr_bumpmap = bumpmap;
    for (int i = 0; i < prefill_size + filler_size; i++, ptr_bumpmap++) {
        *ptr_bumpmap = 0;
    }

    //unpack NAAG logo
    unsigned char* ptr_packed_logo = naaglogo;

    for (int i = 0; i < naaglogo_size / 8; i++, ptr_packed_logo++) {
        for (int j = 7; j >= 0; j--, ptr_bumpmap++) {
            if ((*ptr_packed_logo >> j) & 1) {
                *ptr_bumpmap = 0;
            }
            else {
                *ptr_bumpmap = 1;
            }
        }
    }

    for (int i = 0; i < postfill_size; i++, ptr_bumpmap++) {
        *ptr_bumpmap = 0;
    }

    ptr_bumpmap = bumpmap;
    for (int i = 0; i < bmap_size; i++, ptr_bumpmap++) {
        if (*ptr_bumpmap != 0) {
           *ptr_bumpmap = static_cast<unsigned char>(rand() & 255);
        }
    }

    //blur several times
    for (int i = 0; i < 4; i++) {
        do_segment_blur(bumpmap);
    }

    //done initializing bump map
}

void init_envmap(unsigned char* envmap)
{
    unsigned char* ptr_envmap = envmap;

    for (double x = -128; x < 128; x++) {
        for (double y = -128; y < 128; y++) {
            double v = 63 - sqrt(x * x  + y * y) * 63;
            if (v < 0) {
                v = 0;
            }
            else if (v > 63) {
                v = 63;
            }
            *ptr_envmap = static_cast<unsigned char>(v);
            ptr_envmap++;
        }
    }
}

int main(void)
{
    //initialize RNG
    srand(time(nullptr));

    //initialize bump map
    unsigned char* ptr_bumpmap = new unsigned char[65536];
    init_bumpmap(ptr_bumpmap);

    //initialize environ map
    unsigned char* ptr_envmap = new unsigned char[65536];
    init_envmap(ptr_envmap);

    //initialize scroll buffer
    unsigned char* ptr_scroll_buffer = new unsigned char[320 * 8];

    char key;
    DisplayController dc(naagpal);

    //FIXME
    unsigned char* ptr_fb = dc.data();
    memcpy(ptr_fb, ptr_envmap + 320 * 2, 64000);

    dc.flip();

    while (1) {
        if (is_key_pressed()) {
            key = get_key_code();
            if (key == 27) {  // ESC
                break;
            }
        } 
    }

    //clean up
    if (ptr_scroll_buffer != nullptr) {
        delete[] ptr_scroll_buffer;
    }
    if (ptr_envmap != nullptr) {
        delete[] ptr_envmap;
    }
    if (ptr_bumpmap != nullptr) {
        delete[] ptr_bumpmap;
    }

    return 0;
}
