/*
 * AudioOut.h
 *
 *  Created on: Feb 26, 2024
 *      Author: Alexander
 */

#ifndef AUDIOOUT_H_
#define AUDIOOUT_H_

#include <stdint.h>

typedef struct {
    int8_t(*pInit)(void);
    int8_t(*pDeinit)(void);
    int16_t*(*pGetActiveBuffer)(void);
    uint32_t(*pGetBufferSize)(void);
    void(*pPlayActiveBuffer)(uint32_t samplerate, uint32_t samples);
} audioOut_t;

audioOut_t* GetAudioOut(void);

#endif /* AUDIOOUT_H_ */
