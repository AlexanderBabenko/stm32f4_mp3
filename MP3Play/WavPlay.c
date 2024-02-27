/*
 * WavPlay.c
 *
 *  Created on: Feb 26, 2024
 *      Author: Alexander
 */

#include "WavPlay.h"
#include "AudioOut.h"
#include <stdio.h>
#include "stm32f4_discovery.h"

typedef struct tWAVEFORMATEX{
    uint32_t subChunk1Sz;
    uint16_t AudioFmt;
    uint16_t NumChannels;
    uint32_t SampleRate;
    uint32_t ByteRate;
    uint16_t BlockAlign;
    uint16_t BitsPerSample;
    uint32_t cbSize;
}WAVEFORMATEX;

WAVEFORMATEX winfo;
static uint8_t *buff;

int8_t Init(uint8_t *pWorkBuf){
    buff = pWorkBuf;
    return 0;
}

static char* GetTrackName(void){
    return "No Info\r\n";
}
static uint32_t GetBitrate(void){
    return winfo.ByteRate;
}
static int8_t Deinit(void){
    return 0;
}


static int8_t wavPlay(FIL *wavFile, audioOut_t* pOut)
{
    short *pBuf;
    UINT bytesRead;
    UINT bytesToRead = 20;
    uint8_t needExit = 0;

    if(!wavFile || !pOut){
        return 1;
    }

    if (f_read(wavFile, (BYTE*) buff, bytesToRead, &bytesRead) != FR_OK) {
        return 1;
    }

    if (strncmp("WAVEfmt", (char *)buff + 8, 7) != 0 ) {
        return 1;
    }
    memset(&winfo, 0, sizeof(winfo));

    winfo.subChunk1Sz = *(uint32_t *)(buff + 0x10);

    bytesToRead = winfo.subChunk1Sz;

    if (f_read(wavFile, (BYTE*) buff, bytesToRead, &bytesRead) != FR_OK) {
        return 1;
    }

    winfo.AudioFmt = *(uint16_t *)buff;

    winfo.NumChannels         = *(uint16_t *)(buff+0x02);
    winfo.SampleRate    = *(uint32_t *)(buff+0x04);
    winfo.ByteRate   = *(uint32_t *)(buff+0x08);
    winfo.BlockAlign       = *(uint16_t *)(buff+0x0c);
    winfo.BitsPerSample    = *(uint16_t *)(buff+0x0e);

    bytesToRead = 8;

    if (f_read(wavFile, (BYTE*) buff, bytesToRead, &bytesRead) != FR_OK) {
        return 1;
    }

    if(strncmp("fact", (char *)buff, 4) == 0){
        bytesToRead = *(uint32_t *)(buff + 0x04);
        if (f_read(wavFile, (BYTE*) buff, bytesToRead, &bytesRead) != FR_OK) {
            return 1;
        }

        bytesToRead = 8;

        if (f_read(wavFile, (BYTE*) buff, bytesToRead, &bytesRead) != FR_OK) {
            return 1;
        }
    }

    if (strncmp("data", (char *)buff, 4) != 0 ) {
        return 1;
    }
    winfo.cbSize = *(uint32_t *)(buff+0x04);

    //printf("\nSample Rate = %ul, Channels = %ul, BitsPerSample = %ul, file size = %ulM Bytes\n",
    //        winfo.nSamplesPerSec, winfo.nChannels, winfo.wBitsPerSample, winfo.cbSize/1024/1024);

    if (winfo.BitsPerSample!=16) {
       // printf("8bit format is not support !\n\r");
        return 1;
    }

    pOut->pInit();
    do{
        pBuf = pOut->pGetActiveBuffer();
        bytesToRead = pOut->pGetBufferSize();
        if (f_read(wavFile, (BYTE*) pBuf, bytesToRead, &bytesRead) != FR_OK) {
            return 1;
        }
        if(bytesToRead != bytesRead){
            needExit = 1;
        }

        if (BSP_PB_GetState(BUTTON_KEY)) {
            needExit = 1;
        }

        pOut->pPlayActiveBuffer(winfo.SampleRate, bytesToRead/2);
    } while(!needExit);

    pOut->pDeinit();
    return 0;
}


player_t* GetWavPlay(void){
    static player_t wavPlayer = {
            .pInit = Init,
            .pPlay = wavPlay,
            .pGetTrackName = GetTrackName,
            .pGetBitrate = GetBitrate,
            .pDeinit = Deinit

    };
    return &wavPlayer;
}

