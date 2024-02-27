/*
 * Player.h
 *
 *  Created on: Nov 8, 2023
 *      Author: Alexander
 */

#ifndef PLAYER_H_
#define PLAYER_H_

#include <stdint.h>
#include "ff.h"
#include "AudioOut.h"

typedef struct {
    int8_t(*pInit)(uint8_t *pWorkBuf);
    int8_t(*pPlay)(FIL *fl, audioOut_t* pOut);
    char*(*pGetTrackName)(void);
    uint32_t(*pGetBitrate)(void);
    int8_t(*pDeinit)(void);
} player_t;

uint8_t playerInit();

#endif /* PLAYER_H_ */
