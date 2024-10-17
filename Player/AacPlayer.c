/*
 * AacPlayer.c
 *
 *  Created on: Mar 2, 2024
 *      Author: Alexander
 */

#include "AacPlayer.h"
#include <stdint.h>
#include "FreeRTOS.h"
#include "AudioOut.h"
#include "ff.h"
#include "stm32f4_discovery.h"
#include "aacdec.h"
#include <assert.h>
#include <string.h>

static HAACDecoder hAACDecoder;
static AACFrameInfo aacFrameInfo;
static unsigned char *readPtr;
static int bytesLeft = 0, bytesLeftBeforeDecoding = 0, err, offset;
static unsigned char *aacbuf;
static unsigned int aacbuf_size;
static unsigned char allocated = 0;
static int raw = 0;

void aac_reset() {
    readPtr = NULL;
    bytesLeftBeforeDecoding = bytesLeft = 0;
}

void aac_init(unsigned char *buffer, unsigned int buffer_size) {
    aacbuf = buffer;
    aacbuf_size = buffer_size;
    aac_reset();
}

void aac_alloc() {
    if (!allocated)
        assert(hAACDecoder = AACInitDecoder());
    allocated = 1;
}

void aac_free() {
    if (allocated)
        AACFreeDecoder(hAACDecoder);
    allocated = 0;
}

void aac_setup_raw() {
    memset(&aacFrameInfo, 0, sizeof(AACFrameInfo));
    aacFrameInfo.nChans = 2;
    aacFrameInfo.sampRateCore = 44100;
    aacFrameInfo.profile = AAC_PROFILE_LC;
    assert(AACSetRawBlockParams(hAACDecoder, 0, &aacFrameInfo) == 0);
}

int aac_process(FIL *aacfile, audioOut_t *pOut) {
    UINT bytes_read;
    short *pBuf;

    if (readPtr == NULL) {
        BSP_LED_On(LED5);
        assert(f_read(aacfile, (BYTE* )aacbuf, aacbuf_size, &bytes_read) == FR_OK);
        BSP_LED_Off(LED5);
        if (bytes_read == aacbuf_size) {
            readPtr = aacbuf;
            offset = 0;
            bytesLeft = aacbuf_size;
        } else {
            return -1;
        }
    }

    if (!raw) {
        offset = AACFindSyncWord(readPtr, bytesLeft);
        if (offset < 0) {

            // read more data
            BSP_LED_On(LED5);
            assert(f_read(aacfile, (BYTE* )aacbuf, aacbuf_size, &bytes_read) == FR_OK);
            BSP_LED_Off(LED5);
            if (bytes_read == aacbuf_size) {
                readPtr = aacbuf;
                offset = 0;
                bytesLeft = aacbuf_size;
                return 0;
            } else {
                return -1;
            }
        }

        readPtr += offset;
        bytesLeft -= offset;
    }

    bytesLeftBeforeDecoding = bytesLeft;

    // check if this is really a valid frame
    // (the decoder does not seem to calculate CRC, so make some plausibility checks)
    /*
     if (AACGetNextFrameInfo(hAACDecoder, &aacFrameInfo, readPtr) == 0 &&
     aacFrameInfo.sampRateOut == 44100 &&
     aacFrameInfo.nChans == 2) {
     debug_printf("Found a frame at offset %x\n", offset + readPtr - aacbuf + aacfile->FilePtr);
     } else {
     iprintf("this is no valid frame\n");
     // advance data pointer
     // TODO: what if bytesLeft == 0?
     assert(bytesLeft > 0);
     bytesLeft -= 1;
     readPtr += 1;
     return 0;
     }
     */

    if (bytesLeft < 1024) {
        memmove(aacbuf, readPtr, bytesLeft);
        BSP_LED_On(LED5);
        assert(f_read(aacfile, (BYTE* )aacbuf + bytesLeft, aacbuf_size - bytesLeft, &bytes_read) == FR_OK);
        BSP_LED_Off(LED5);
        if (bytes_read == aacbuf_size - bytesLeft) {
            readPtr = aacbuf;
            offset = 0;
            bytesLeft = aacbuf_size;
            return 0;
        } else {
            return -1;
        }
    }

    pBuf = pOut->pGetActiveBuffer();

    BSP_LED_On(LED4);
    err = AACDecode(hAACDecoder, &readPtr, &bytesLeft, pBuf);
    BSP_LED_Off(LED4);

    if (err) {
        UINT bytesToRead;

        switch (err) {
        case ERR_AAC_INDATA_UNDERFLOW:
            memmove(aacbuf, readPtr, bytesLeft);

            bytesToRead = aacbuf_size - bytesLeft;
            BSP_LED_On(LED5);
            if (f_read(aacfile, (BYTE*) aacbuf + bytesLeft, bytesToRead, &bytes_read) != FR_OK) {
                BSP_LED_Off(LED5);
                return -1;
            }
            BSP_LED_Off(LED5);

            if (bytes_read == bytesToRead) {
                readPtr = aacbuf;
                offset = 0;
                bytesLeft = aacbuf_size;
            } else {
                return -1;
            }
            break;

        default:
            // skip this frame
            if (bytesLeft > 0) {
                bytesLeft--;
                readPtr++;
            } else {
                // TODO
                assert(0);
            }
            break;
        }
    } else {
        /* no error */
        AACGetLastFrameInfo(hAACDecoder, &aacFrameInfo);

        pOut->pPlayActiveBuffer(aacFrameInfo.sampRateOut, aacFrameInfo.outputSamps);
    }

    return 0;
}

//=============================================================================
static int8_t InitAAC(uint8_t *pWorkBuf) {
    if (!pWorkBuf) {
        return 1;
    }
    aac_alloc();
    aac_init(pWorkBuf, 2048);
    raw = 0;
    return 0;
}

static int8_t InitM4A(uint8_t *pWorkBuf) {
    if (!pWorkBuf) {
        return 1;
    }

    if (!InitAAC(pWorkBuf)) {
        aac_setup_raw();
        raw = 1;
    }
    return 0;
}

static char* GetTrackName(void) {
    return "No Info\r\n";
}
static uint32_t GetBitrate(void) {
    return aacFrameInfo.sampRateOut;
}
static int8_t Deinit(void) {
    aac_free();
    return 0;
}

static int8_t aacPlay(FIL *aacFile, audioOut_t *pOut) {
    int aacResult = 0;
    if (!aacFile || !pOut) {
        return 1;
    }

    pOut->pInit();

    while (aacResult == 0) {
        aacResult = aac_process(aacFile, pOut);

        if (BSP_PB_GetState(BUTTON_KEY)) {
            aacResult = 1;
        }
    }

    pOut->pDeinit();

    return 0;
}

static int8_t aacPlayRaw(FIL *aacFile, audioOut_t *pOut) {
    UINT bytes_read;
    uint8_t found = 0;
    do {
        if (f_read(aacFile, aacbuf, 8, &bytes_read) == FR_OK) {
            if (memcmp(aacbuf + sizeof(uint32_t), "mdat", 4) == 0) {
                found = 1;
            } else {
                uint32_t len = *(uint32_t*) (aacbuf);
                len = __REV(len);
                f_lseek(aacFile, f_tell(aacFile) + len - 8);
            }
        } else {
            return 1;
        }
    } while (!found);

    return aacPlay(aacFile, pOut);
}

player_t* GetAacPlayer(void) {
    static player_t aacPl = { .pInit = InitAAC, .pPlay = aacPlay, .pGetTrackName = GetTrackName, .pGetBitrate =
            GetBitrate, .pDeinit = Deinit };
    return &aacPl;
}

player_t* GetM4APlayer(void) {
    static player_t aacPl = { .pInit = InitM4A, .pPlay = aacPlayRaw, .pGetTrackName = GetTrackName, .pGetBitrate =
            GetBitrate, .pDeinit = Deinit };
    return &aacPl;
}
