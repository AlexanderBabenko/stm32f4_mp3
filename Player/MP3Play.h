/*!\file MP3Play.h
 *    \author Sanya
 *    \date created on 01 may. 2012
 *    \version 1.0
 *    \brief mp3 player header file.
 */

#ifndef MP3PLAY_H_
#define MP3PLAY_H_

#include "FreeRTOS.h"
#include "task.h"
#include "ff.h"
#include "Player.h"

/* music tag information structure */
typedef struct tag_info
{
    char title [40];        /* music title              */
    char artist[40];        /* music artist             */

    uint32_t duration;   /* music total duration (second)   */
    uint32_t position;   /* music current position (second) */

    uint32_t bit_rate;   /* bit rate                 */
    uint32_t sampling;   /* sampling                 */
    uint32_t data_start; /* start position of data   */
} tag_info_t;


player_t* GetMp3Player(void);

#endif //MP3PLAY_H_
