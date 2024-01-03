#include <gb/gb.h>
#include <stdint.h>
#include <gb/drawing.h>
#include <rand.h>
#include <stdbool.h>
#include <time.h>
static inline void set_hello_sprite(const uint8_t a[8*8],uint8_t sp[8]) {
    
    for (int i=0;i<sizeof(a);i++) {//0x08 1000
        int index=(i>>3);
        sp[index]=a[i]<<(i&0x7);
    }
}
const uint8_t hello_sprite[] = {
    0x7e,//0111 1110
    0x81,//1000 0001
    0x81,
    0x81,
  
    0x81,
    0x81,
    0x81,
    0x7e,
};

struct Dot {
    uint8_t x,y;
    bool alive;
};
const uint8_t PADDING=4;
uint8_t get_rand_x(void) {
return((rand())%(GRAPHICS_WIDTH-(PADDING+8)))+8;
}
uint8_t get_rand_y(void) {
    return ((rand())%(GRAPHICS_HEIGHT-(PADDING+16)))+16;
}
void main(void) {
    uint8_t x=8,y=16;
    uint8_t N_DOTS=8;
    struct Dot dots[UINT8_MAX];
    initrand(sys_time);
    for (int i=0;i<N_DOTS;i++) {
        dots[i].x=get_rand_x();
        // if (dots[i].x<8){dots[i].x+=8;}
        dots[i].y=get_rand_y();
        // if (dots[i].y<16){dots[i].y+=16;}
        dots[i].alive=1;
    }
    const uint8_t STATE[8*2*2] = {
        0x80,0,0,0, 0,0,0,0,
        0x00,0,0,0, 0,0,0,0,
        0x00,0,0,0, 0,0,0,0,
        0x00,0,0,0, 0,0,0,0
    };
    
    uint8_t which = 0;

    /* Initialize sprite palette */
    OBP1_REG = 0xE0U;
    set_sprite_1bpp_data(0,1,hello_sprite);
    set_sprite_tile(0,0);
    set_sprite_prop(0,0x10);//color 1
    set_sprite_2bpp_data(1,2,STATE);
    for (int i=0;i<N_DOTS;i++) {
        set_sprite_tile(i+1,1);
        move_sprite(i+1,dots[i].x,dots[i].y);
    }
    uint8_t dots_alive=N_DOTS;
    SHOW_SPRITES;
    color(DMG_LITE_GRAY,DMG_BLACK,XOR);
    while (1) {
        vsync();
        uint8_t key = joypad();
        if (key&J_RIGHT) {x++;}
        if (key&J_LEFT) {x--;}
        if (key&J_DOWN) {y++;}
        if (key&J_UP) {y--;}
        if (key&J_A) {x=dots[which].x;y=dots[which].y;}
        if (key&J_B) {which++;which&=0x7;}
        move_sprite(0,x,y);
        for (int i=0;i<N_DOTS;i++) {
            if (dots[i].alive&&
                (dots[i].x>x && dots[i].x<x+8)&&
                (dots[i].y>y && dots[i].y<y+8)
            ) {dots[i].alive=false;dots_alive--;}
            set_sprite_tile(i+1,dots[i].alive?1:2);
        }
        if (dots_alive==0) {
            initrand(sys_time);
            for (int i=0;i<N_DOTS;i++) {
                dots[i].alive=true;
                dots[i].x=get_rand_x();
                dots[i].y=get_rand_y();
                move_sprite(i+1,dots[i].x,dots[i].y);
                dots_alive=N_DOTS;
            }
        }
        // delay(16);
        gotogxy(0,0);
        gprintf("alive:%d",dots_alive);
    }
    
}