#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
    uint32_t val =  inl(KBD_ADDR);
    kbd->keydown = (val & (KEYDOWN_MASK - 1));
    kbd->keycode = (val & KEYDOWN_MASK);
}
