#include <stddef.h>
#include <stdlib.h>
#include <time.h>

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
    unsigned char* ptr_bumpmap = bumpmap;
    for (int i = 0; i < 32000 - 320 * 70 / 2; i++, ptr_bumpmap++) {
        *ptr_bumpmap = 0;
    }

    //unpack NAAG logo
    unsigned char* ptr_packed_logo = naaglogo;

    for (int i = 32000 - 320 * 70 / 2; i < 320 * 70 / 8; i++, ptr_packed_logo++) {
        for (int j = 7; j >= 0; j--, ptr_bumpmap++) {
            if ((*ptr_packed_logo >> j) & 1) {
                //make logo look convex
                *ptr_bumpmap = 0;
            }
            else {
                *ptr_bumpmap = 255;
            }
        }
    }

    for (int i = 0; i < 32000 - 320 * 70 / 2; i++, ptr_bumpmap++) {
        *ptr_bumpmap = 0;
    }

    ptr_bumpmap = bumpmap;
    for (int i = 0; i < 65536; i++, ptr_bumpmap++) {
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

int main(void)
{
    //initialize RNG
    srand(time(nullptr));

    //initialize bump map
    unsigned char* ptr_bumpmap = new unsigned char[65536];
    init_bumpmap(ptr_bumpmap);

    char key;
    DisplayController dc(naagpal);

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
    if (ptr_bumpmap != nullptr) {
        delete[] ptr_bumpmap;
    }
    
    return 0;
}
