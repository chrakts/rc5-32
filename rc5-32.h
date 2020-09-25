#include <avr/io.h>
#include <avr/interrupt.h>
#include "stdbool.h"
#include "rc5-32Hardware.h"

#define	T_0 		51
#define	T_1 		33
#define	T_2 		34
#define	T_3 		35
#define	T_4 		36
#define	T_5 		37
#define	T_6 		38
#define	T_7 		39
#define	T_8 		40
#define	T_9 		41
#define T_ONOFF		11
#define T_REC		204
#define T_PLAY		12
#define T_STOP		3
#define T_PAUSE		13
#define T_KANAL_P	25
#define T_KANAL_M	24
#define T_VOL_P		158
#define T_VOL_M		159
#define T_LETZTER	195
#define T_REW		7
#define T_FF		6
#define T_START_PL	0b11001000
#define T_START_MI	0b11001001
#define T_STOP_PL	0b11001010
#define T_STOP_MI	0b11001011
#define T_DATE_PL	0b11000110
#define T_DATE_MI	0b11000111
#define T_TVPRG_PL	0b00011001   // identisch mit T_KANAL_P
#define T_TVPRG_MI	0b00011000   // identisch mit T_KANAL_M
#define T_PROG_MENU 55
#define T_OK      60




void rc5_init( void );
void falling_edge( void );
void rising_edge( void );
