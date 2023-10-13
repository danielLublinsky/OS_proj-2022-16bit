//keyboard.c

#include "../includes/keyboard.h"
key_info_t *key_info = (key_info_t *)KEY_INFO_ADDRESS;
key_info_t get_key(void)
{
    key_info_t output;
    key_info->key = 0;
    while (!key_info->key) {
        __asm__ __volatile__("hlt");
    }
    
    output = *key_info;
    key_info->key = 0;
    
    return output;
}