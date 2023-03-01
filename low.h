#ifndef _LOW_H
#define _LOW_H

int is_key_pressed();
char get_key_code();

void set_mode13h();
void unset_mode13h();

void screen_retrace();

void copy_buffer(unsigned char*);
void clear_buffer(unsigned char*);

void set_palette(unsigned char*);
void load_palette(unsigned char*);

void soft_segment(unsigned char*);

#endif
