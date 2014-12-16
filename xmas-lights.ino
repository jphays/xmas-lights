#include <SPI.h>
#include <TCL.h>

const int LEDS = 50;
const int NUM_COLORS = 8;
const int FPS = 25;

const bool MIXIN_POTS = false;

const float UPDATE_SPEED = 1.5;
const float UPDATE_VAR = 0.1;

const int BLACK = 0;
const int RED = 1;
const int ORANGE = 2;
const int YELLOW = 3;
const int GREEN = 4;
const int TEAL = 5;
const int BLUE = 6;
const int PURPLE = 7;
const int WHITE = 8;

const int R = 0;
const int G = 1;
const int B = 2;

byte color_values[NUM_COLORS][3];
long change_time[LEDS];
int current_color[LEDS];
byte strand[LEDS][3];

int delay_time;
int update_time_min;
int update_time_max;

void setup()
{
    TCL.begin();
    TCL.setupDeveloperShield();

    init_times();
    init_colors();

    clear_strand();

    rainbow_init();
    update_strand();
}

void loop()
{
    rainbow_loop();
    update_strand();
    process_inputs();
    delay(delay_time);
}


/* loop functions */

void rainbow_loop()
{

    unsigned long time = millis();

    for (int i = 0; i < LEDS; i++)
    {
        if (time >= change_time[i])
        {
            change_time[i] = time + random(update_time_min, update_time_max);
            {
                current_color[i] = (current_color[i] + 1) % NUM_COLORS;
            }
        }
    }

    for (int i = 0; i < LEDS; i++)
    {
        assign_color(i, current_color[i]);
    }

}

void xmas_loop()
{
    unsigned long time = millis();

    for(int i = 0; i < LEDS; i++)
    {
        if (change_time[i] < time)
        {
            change_time[i] = time + random(update_time_min, update_time_max);

            if (current_color[i] == BLACK)
            {
                current_color[i] = random(1, NUM_COLORS);
            }
            else
            {
                current_color[i] = BLACK;
            }

            strand[i][0] = color_values[current_color[i]][0];
            strand[i][1] = color_values[current_color[i]][1];
            strand[i][2] = color_values[current_color[i]][2];
        }
        else
        {
            //if (current_color[i] == -1)
            {
                strand[i][0] = (strand[i][0] & 0xfe) >> 1;
                strand[i][1] = (strand[i][1] & 0xfe) >> 1;
                strand[i][2] = (strand[i][2] & 0xfe) >> 1;
            }
        }
    }
}


/* Utility functions */

void update_strand()
{
    int i;
    int color;

    TCL.sendEmptyFrame();

    for(int i = 0; i < LEDS; i++)
    {
        TCL.sendColor(strand[i][R], strand[i][G], strand[i][B]);
    }

    TCL.sendEmptyFrame();
}

void clear_strand()
{
    unsigned long time = millis();

    for(int i = 0; i < LEDS; i++)
    {
        change_time[i] = time + random(update_time_min, update_time_max);
        current_color[i] = BLACK;
        assign_color(i, BLACK);
    }
}

void assign_color(int led, int color)
{
    int tick = change_time[i] - millis();

    if (tick < 200)
    {
        strand[led][R] = (strand[led][R] & 0xfe) >> 1;
        strand[led][G] = (strand[led][G] & 0xfe) >> 1;
        strand[led][B] = (strand[led][B] & 0xfe) >> 1;
    }
    else if (MIXIN_POTS)
    {
        strand[led][R] = (color_values[color][R] + (analogRead(TCL_POT1) >> 2)) % 256;
        strand[led][G] = (color_values[color][G] + (analogRead(TCL_POT2) >> 2)) % 256;
        strand[led][B] = (color_values[color][B] + (analogRead(TCL_POT3) >> 2)) % 256;
    }
    else
    {
        strand[led][R] = color_values[color][R];
        strand[led][G] = color_values[color][G];
        strand[led][B] = color_values[color][B];
    }
}

void process_inputs()
{
    // button 1: reset
    if (digitalRead(TCL_MOMENTARY1) == LOW)
    {
        rainbow_init();
    }
}


/* initialization functions */

void rainbow_init()
{
    int color = BLACK;

    for (int i = 0; i < LEDS; i++)
    {
        current_color[i] = color;
        assign_color(i, color);

        if (i % 5 == 4)
        {
            //color = (color + 1) % NUM_COLORS;
            color = random(0, NUM_COLORS);
        }

        change_time[i] = millis();
    }

}

void init_times()
{
    delay_time = 1000 / FPS;

    int update_time = 1000 / UPDATE_SPEED;
    int update_var = 1000 * UPDATE_VAR;

    update_time_min = update_time - update_var;
    update_time_max = update_time + update_var;
}

void init_colors()
{
    color_values[BLACK][0] = 0x00;
    color_values[BLACK][1] = 0x00;
    color_values[BLACK][2] = 0x00;

    color_values[RED][0]=0xb0;
    color_values[RED][1]=0x00;
    color_values[RED][2]=0x00;

    color_values[ORANGE][0]=0xa0;
    color_values[ORANGE][1]=0x20;
    color_values[ORANGE][2]=0x00;

    color_values[YELLOW][0]=0x80;
    color_values[YELLOW][1]=0x60;
    color_values[YELLOW][2]=0x00;

    color_values[GREEN][0]=0x00;
    color_values[GREEN][1]=0x50;
    color_values[GREEN][2]=0x00;

    color_values[TEAL][0]=0x00;
    color_values[TEAL][1]=0x40;
    color_values[TEAL][2]=0x20;

    color_values[BLUE][0]=0x00;
    color_values[BLUE][1]=0x00;
    color_values[BLUE][2]=0xa0;

    color_values[PURPLE][0]=0x30;
    color_values[PURPLE][1]=0x00;
    color_values[PURPLE][2]=0x80;

    color_values[WHITE][0]=0xa0;
    color_values[WHITE][1]=0xa0;
    color_values[WHITE][2]=0x90;
}
