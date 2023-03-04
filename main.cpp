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

/*
class BumpMappingController
{
public:
    BumpMappingController(DisplayController*, unsigned char*, unsigned char*, double*, int);
    ~BumpMappingController();

    void do_bump();

private:
    DisplayController* ptr_display_;

    unsigned char* ptr_bumpmap_;
    unsigned char* ptr_envmap_;

    double* ptr_waytable_;
    int waytable_size_;

    int light_pos_x;
    int light_pos_y;
};

BumpMappingController::BumpMappingController(DisplayController* ptr_display, unsigned char* ptr_bumpmap, unsigned char* ptr_envmap, double* ptr_waytable, int waytable_size)
    : ptr_display_(ptr_display)
    , ptr_bumpmap_(ptr_bumpmap)
    , ptr_envmap_(ptr_envmap)
    , ptr_waytable_(ptr_waytable)
    , waytable_size_(waytable_size)
    , light_pos_x(0)
    , light_pos_y(0)
{
}

BumpMappingController::~BumpMappingController()
{
}
*/

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

    //add random noise
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

void init_way(double* way_table, int size_table)
{
    double* ptr_table = way_table;

    for (double i = 0; i < size_table; i++) {
        *ptr_table = sin(i * M_PI / (size_table / 2)) * size_table / 2;
        ptr_table++;
    }
}

/*
struct BumpMappingConfig
{
    DisplayController* ptr_display;

    unsigned char* ptr_bumpmap;
    unsigned char* ptr_envmap;

    double* ptr_waytable;
    int waytable_size;

    int light_pos_x;
    int light_pos_y;
};
*/

void do_bump_mapping(unsigned char* ptr_fb, unsigned char* ptr_bumpmap, unsigned char* ptr_envmap, unsigned char* ptr_waytable, int waytable_size, int& light_x, int& light_y)
{
    light_x += 2;
    light_x %= waytable_size;
    double tmp_x = ptr_waytable[light_x] * 2 + 320 / 2;

    light_y += 1;
    light_y %= waytable_size;
    double tmp_y = ptr_waytable[light_y] / 2 + 200 / 2;

    int light_pos_x = static_cast<int>(tmp_x);
    int light_pos_y = static_cast<int>(tmp_y);

    /*
    config->light_pos_x += 2;
    config->light_pos_x %= config->waytable_size;

    config->light_pos_y += 1;
    config->light_pos_y %= config->waytable_size;

    double tmp_x = config->ptr_waytable[config->light_pos_x] * 2 + 320 / 2;
    double tmp_y = config->ptr_waytable[config->light_pos_y] / 2 + 200 / 2;

    int light_x = static_cast<int>(tmp_x);
    int light_y = static_cast<int>(tmp_y);

    unsigned char* ptr_bumpmap = config->ptr_bumpmap + 320;
    unsigned char* ptr_frame_buffer = config->ptr_display->data() + 320;
    */

    for (int i = 1; i < 199; i++) {
        for (int j = 0; j < 320; j++) {

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

    double way_table[256];
    init_way(way_table, 256);

    char key;
    DisplayController dc(naagpal);

    //BumpMappingConfig bump_mapping_config{&dc, ptr_bumpmap, ptr_envmap, way_table, 256, 0, 0};

    while (1) {
        if (is_key_pressed()) {
            key = get_key_code();
            if (key == 27) {  // ESC
                break;
            }
        } 

        //do_bump_mapping(&bump_mapping_config);
        dc.flip();
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
