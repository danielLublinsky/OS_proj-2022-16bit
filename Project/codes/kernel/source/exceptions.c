#include "../includes/exceptions.h"

extern Color_params current_color;

void div_by_0_handler(int_frame_32_t *frame)
{
    current_color.foreground = Red;
    puts_color("\n\r****DIVIDE BY 0 ERROR****\n\r", current_color);
    frame->eip++;
}