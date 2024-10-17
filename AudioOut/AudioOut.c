/*
 * AudioOut.c
 *
 *  Created on: Feb 26, 2024
 *      Author: Alexander
 */

#include "AudioOut.h"

#include "stm32f4_discovery_audio.h"
#include "stm32f4_discovery.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include <memory.h>
#include "../MP3/fixpt/real/coder.h"

typedef struct {
    xSemaphoreHandle sem;
    int16_t buf1[MAX_NSAMP * MAX_NGRAN * MAX_NCHAN];
    int16_t buf2[MAX_NSAMP * MAX_NGRAN * MAX_NCHAN];
    uint32_t outSamples;
    uint32_t sampleRate;
    uint8_t activeBuf;
    uint8_t itIsFirstWrite;
} audioOutBuf_t;

static audioOutBuf_t audioOutBuf;
static uint8_t semCreated = 0;

static int8_t InitAudioOut() {
    //BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_AUTO, 70, AUDIO_FREQUENCY_44K);
    if(!semCreated){
        vSemaphoreCreateBinary(audioOutBuf.sem);
        semCreated = 1;
    }
    audioOutBuf.outSamples = 0;
    audioOutBuf.sampleRate = 0;
    audioOutBuf.activeBuf = 0;
    audioOutBuf.itIsFirstWrite = 1;

    memset(audioOutBuf.buf1, 0, sizeof(audioOutBuf.buf1));
    memset(audioOutBuf.buf2, 0, sizeof(audioOutBuf.buf2));

    return 0;
}

static int8_t DeInitAudioOut() {
    xSemaphoreTake(audioOutBuf.sem, portMAX_DELAY);
    BSP_AUDIO_OUT_Stop(CODEC_PDWN_HW);
    BSP_LED_Off(LED6);
    audioOutBuf.itIsFirstWrite = 1;
    xSemaphoreGive(audioOutBuf.sem);
    return 0;
}

static int16_t* GetActiveBuffer() {
    if (!audioOutBuf.activeBuf) {
        return audioOutBuf.buf1;
    } else {
        return audioOutBuf.buf2;
    }
}

static uint32_t GetBufferSize() {
    return sizeof(audioOutBuf.buf1);
}

static void PlayActiveBuffer(uint32_t samplerate, uint32_t samples) {

    xSemaphoreTake(audioOutBuf.sem, portMAX_DELAY);

    audioOutBuf.outSamples = samples;
    audioOutBuf.sampleRate = samplerate;

    if (audioOutBuf.itIsFirstWrite == 1) {
        BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_AUTO, 70, audioOutBuf.sampleRate);
        BSP_LED_On(LED6);
        BSP_AUDIO_OUT_Play((uint16_t*) ((audioOutBuf.activeBuf) ? audioOutBuf.buf2 : audioOutBuf.buf1),
                audioOutBuf.outSamples * sizeof(short));
        audioOutBuf.itIsFirstWrite = 0;
    }

    audioOutBuf.activeBuf ^= 0xFF;
}

audioOut_t* GetAudioOut(void) {
    static audioOut_t drv = {
            .pInit = InitAudioOut,
            .pDeinit = DeInitAudioOut,
            .pGetActiveBuffer = GetActiveBuffer,
            .pGetBufferSize = GetBufferSize,
            .pPlayActiveBuffer = PlayActiveBuffer };
    return &drv;
}

void BSP_AUDIO_OUT_TransferComplete_CallBack(void) {
    short *pBuf;
    static signed portBASE_TYPE xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    if (!audioOutBuf.activeBuf) {
        pBuf = audioOutBuf.buf1;
    } else {
        pBuf = audioOutBuf.buf2;
    }
    BSP_LED_Toggle(LED6);
    BSP_AUDIO_OUT_Play((uint16_t*) pBuf, audioOutBuf.outSamples * sizeof(short));

    xSemaphoreGiveFromISR(audioOutBuf.sem, &xHigherPriorityTaskWoken);

    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

