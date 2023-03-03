#include <vector>
#include <cstddef>

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
    const int width_ = 320;
    const int height_ = 200;
    const int buffer_size_ = width_ * height_;

    std::vector<unsigned char> buffer_;
};

DisplayController::DisplayController()
{
    set_mode13h();
    buffer_.resize(static_cast<size_t>(buffer_size_));
}

DisplayController::DisplayController(unsigned char* palette)
{
    set_mode13h();
    set_palette(palette);

    buffer_.resize(static_cast<size_t>(buffer_size_));
}

DisplayController::~DisplayController()
{
    unset_mode13h();
}

void DisplayController::blur()
{
    do_blur(buffer_.data(), width_, height_);
}

void DisplayController::flip()
{
    copy_buffer(buffer_.data());
}

unsigned char* DisplayController::data()
{
    return buffer_.data();
}

int main(void)
{
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
    
    return 0;
}
