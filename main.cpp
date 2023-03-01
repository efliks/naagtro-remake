#include <vector>
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
    void flip_clear();
    
    unsigned char* data();

private:
    std::vector<unsigned char> buffer_;
};

DisplayController::DisplayController()
{
    set_mode13h();
    buffer_.resize(0xffff);
}

DisplayController::DisplayController(unsigned char* palette)
{
    set_mode13h();
    set_palette(palette);
    buffer_.resize(0xffff);
}

DisplayController::~DisplayController()
{
    unset_mode13h();
}

void DisplayController::blur()
{
    soft_segment(buffer_.data());
}

void DisplayController::flip()
{
    copy_buffer(buffer_.data());
}

void DisplayController::flip_clear()
{
    copy_buffer(buffer_.data());
    clear_buffer(buffer_.data());
}

unsigned char* DisplayController::data()
{
    return buffer_.data();
}


int main(void)
{
    char key;
    DisplayController dc(naagpal);

    dc.flip_clear();

    while (1) {
        if (is_key_pressed()) {
            key = get_key_code();
            if (key == 27) {  // ESC
                break;
            }
        } 
    }
    
    return 0;
}
