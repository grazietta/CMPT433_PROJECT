/*
When the joystick is moved up, down, righ left,
or pushed down, it selects one of 5 modes which
represent a different animation on the LEDs
*/

#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

void joystick_init(void);

void joystick_cleanup(void);

#endif