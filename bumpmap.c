#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "low.h"
#include "naagtro.h"

#define WAYTABLE_SIZE 256
unsigned char way_table[WAYTABLE_SIZE];

unsigned char* ptr_bumpmap, *ptr_envmap;
int light_pos_x, light_pos_y;


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
           *ptr_bumpmap = (unsigned char)(rand() & 255);
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
            *ptr_envmap = (unsigned char)v;
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

void init_bump_mapping()
{
    //initialize RNG
    srand(time(NULL));

    //initialize bump map
    ptr_bumpmap = (unsigned char *)malloc(65536 * sizeof(unsigned char));
    init_bumpmap(ptr_bumpmap);

    //initialize environ map
    ptr_envmap = (unsigned char *)malloc(65536 * sizeof(unsigned char));
    init_envmap(ptr_envmap);
}

void deinit_bump_mapping()
{
    if (ptr_envmap != NULL) {
        free(ptr_envmap);
    }
    if (ptr_bumpmap != NULL) {
        free(ptr_bumpmap);
    }
}

void do_bump_mapping(unsigned char* frame_buffer)
{
}
