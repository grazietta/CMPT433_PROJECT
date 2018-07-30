/* Reads analog input voltage 0 on the BeagleBone
Converts reading to different brightness levels which
change the brightness of the LEDs. 
*/
 
#ifndef _POT_H_
#define _POT_H_

void *startListening(void *args);

void pot_init(void);

void pot_cleanup(void);

#endif