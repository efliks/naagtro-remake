/*****************************************
  NAAGTRO version 2

  low.c   Low-level access to hardware

  (C) March 5, 2023  M. Feliks
*****************************************/

#include <dos.h>
#include <dpmi.h>
#include <mem.h>
#include <sys/nearptr.h>

#include <string.h> // for memset

#include <conio.h>


int is_key_pressed()
{
    return kbhit() ? 1 : 0;
}

char get_key_code()
{
    return getch();
}

void set_mode13h()
{
    __dpmi_regs r;

    memset(&r, 0, sizeof(__dpmi_regs));
    r.x.ax = 0x13;
    __dpmi_int(0x10, &r);
    __djgpp_nearptr_enable();
}

void unset_mode13h()
{
    __dpmi_regs r;

    memset(&r, 0, sizeof(__dpmi_regs));
    r.x.ax = 0x03;
    __dpmi_int(0x10, &r);
    __djgpp_nearptr_disable();
}

void copy_buffer(unsigned char* frame_buffer)
{
    unsigned char* ptr_vidmem;

    ptr_vidmem = (unsigned char*)0x0a0000 + __djgpp_conventional_base;
    memcpy(ptr_vidmem, frame_buffer, 64000);
}

void screen_retrace()
{
    while (!(inp(0x03da) & 8))
        ;
    while ((inp(0x03da) & 8))
        ;
}

void set_palette(unsigned char* palette)
{
    int i;

    outp(0x03c8, 0);
    for (i = 0; i < 768; i++) {
        outp(0x03c9, palette[i]);
    }
}

void load_palette(unsigned char* palette)
{
    int i;

    outp(0x03c7, 0);
    for (i = 0; i < 768; i++) {
        palette[i] = inp(0x03c9);
    }
}

void do_blur(unsigned char* frame_buffer, int width, int height)
{
    int i;
    int color;
    unsigned char* pbf = frame_buffer;

    for (i = 0; i < width; i++) {
        *pbf = 0;
        pbf++;
    }

    for (i = 0; i < width * (height - 2); i++) {
        color = *(pbf - 1);
        color += *(pbf + 1);
        color += *(pbf - width);
        color += *(pbf + width);
        color >>= 2;
        *pbf = (unsigned char)color;
        pbf++;
    }

    for (i = 0; i < width; i++) {
        *pbf = 0;
        pbf++;
    }
}

void do_segment_blur(unsigned char* frame_buffer, int width)
{
    int i;
    int color;

    for (i = 0; i <= 0xffff; i++) {
        color = *(frame_buffer + ((i - 1) & 0xffff));
        color += *(frame_buffer + ((i + 1) & 0xffff));
        color += *(frame_buffer + ((i - width) & 0xffff));
        color += *(frame_buffer + ((i + width) & 0xffff));
        color >>= 2;
        *(frame_buffer + i) = (unsigned char)color;
    }
}
