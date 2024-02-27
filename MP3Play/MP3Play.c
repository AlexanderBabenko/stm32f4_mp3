/*!\file MP3Play.c
 *    \author Sanya
 *    \date created on 01 may. 2012
 *    \version 1.0
 *    \brief mp3 player source file.
 */

#include <stdlib.h>
#include <string.h>

#include "MP3Play.h"
#include "ff.h"
#include "stm32f4_discovery_audio.h"
#include "stm32f4_discovery.h"
#include "coder.h"
#include "mp3dec.h"
#include "AudioOut.h"

#include "fatfs.h"

#define MP3_AUDIO_BUF_SZ    (1024)

typedef struct {
    int bytesLeft;
    int offset;
    uint8_t *readPtr;
    uint8_t InBuf[MAINBUF_SIZE];
} mp3InBuf_t;

uint8_t *mp3ID3Buffer;

static HMP3Decoder mp3Decoder;
static MP3FrameInfo mp3FrameInfo;
static mp3InBuf_t mp3InBuf;
static struct tag_info info;

static void mp3Reset(void);
static int mp3Process(FIL *mp3file, audioOut_t *pOut);
static int mp3RefillInBuffer(FIL *mp3file);

static int mp3ParseId3v1(FIL *fd, struct tag_info *info);
static int mp3ParseId3v2(FIL *fd, struct tag_info *info);

int8_t mp3PlayInit(uint8_t *pWorkBuf) {
    //InitAudioOut();
    if(!pWorkBuf) {
        return 1;
    }
    memset(&info, 0, sizeof(info));
    mp3ID3Buffer = pWorkBuf;
    mp3Decoder = MP3InitDecoder();
    mp3Reset();
    return 0;
}

int8_t mp3Play(FIL *mp3file, audioOut_t *pOut) {
    int mp3Result = 0;
    if(!mp3file || !pOut){
        return 1;
    }

    mp3ParseId3v1(mp3file, &info);
    mp3ParseId3v2(mp3file, &info);
    f_lseek(mp3file, info.data_start);

    pOut->pInit();

    while (mp3Result == 0) {

        mp3Result = mp3Process(mp3file, pOut);

        if (BSP_PB_GetState(BUTTON_KEY)) {
            mp3Result = 1;
        }
    }

    pOut->pDeinit();

    mp3Reset();
    mp3Result = 0;
    return 0;
}

static void mp3Reset(void) {
    mp3InBuf.readPtr = NULL;
    mp3InBuf.bytesLeft = 0;
}

static int mp3Process(FIL *mp3file, audioOut_t *pOut) {
    int err;
    short *pBuf;

    if (mp3InBuf.readPtr == NULL) {
        if (mp3RefillInBuffer(mp3file) != 0)
            return -1;
    }

    mp3InBuf.offset = MP3FindSyncWord(mp3InBuf.readPtr, mp3InBuf.bytesLeft);
    if (mp3InBuf.offset < 0) {

        if (mp3RefillInBuffer(mp3file) != 0)
            return -2;
    }

    mp3InBuf.readPtr += mp3InBuf.offset;
    mp3InBuf.bytesLeft -= mp3InBuf.offset;

    // check if this is really a valid frame
    // (the decoder does not seem to calculate CRC, so make some plausibility checks)
    if (MP3GetNextFrameInfo(mp3Decoder, &mp3FrameInfo, mp3InBuf.readPtr) == 0 && mp3FrameInfo.nChans == 2
            && mp3FrameInfo.version == 0) {

    } else {
        // advance data pointer
        // TODO: handle bytes_left == 0
        //assert(bytes_left > 0);
        if (mp3InBuf.bytesLeft > 0) {
            mp3InBuf.bytesLeft -= 1;
            mp3InBuf.readPtr += 1;
        }
        return 0;
    }

    if (mp3InBuf.bytesLeft < 1024) {
        if (mp3RefillInBuffer(mp3file) != 0)
            return -3;
    }

    pBuf = pOut->pGetActiveBuffer();

    BSP_LED_On(LED4);
    err = MP3Decode(mp3Decoder, &mp3InBuf.readPtr, &mp3InBuf.bytesLeft, pBuf, 0);
    BSP_LED_Off(LED4);

    if (err) {
        switch (err) {
        case ERR_MP3_INDATA_UNDERFLOW:
            mp3InBuf.bytesLeft = 0;
            if (mp3RefillInBuffer(mp3file) != 0)
                return -4;
            break;
        case ERR_MP3_MAINDATA_UNDERFLOW:
            // do nothing - next call to decode will provide more mainData
            break;
        default:
            // skip this frame
            if (mp3InBuf.bytesLeft > 0) {
                mp3InBuf.bytesLeft--;
                mp3InBuf.readPtr++;
            }
            break;
        }
    } else {
        // no error
        MP3GetLastFrameInfo(mp3Decoder, &mp3FrameInfo);

        pOut->pPlayActiveBuffer(mp3FrameInfo.samprate, mp3FrameInfo.outputSamps);
    }

    return 0;
}

static int mp3RefillInBuffer(FIL *mp3file) {
    UINT bytesRead;
    UINT bytesToRead;

    if (mp3InBuf.bytesLeft > 0) {
        memmove(mp3InBuf.InBuf, mp3InBuf.readPtr, mp3InBuf.bytesLeft);
    }

    bytesToRead = sizeof(mp3InBuf.InBuf) - mp3InBuf.bytesLeft;
    BSP_LED_On(LED5);
    if (f_read(mp3file, (BYTE*) mp3InBuf.InBuf + mp3InBuf.bytesLeft, bytesToRead, &bytesRead) != FR_OK) {
        BSP_LED_Off(LED5);
        return -1;
    }
    BSP_LED_Off(LED5);
    if (bytesRead == bytesToRead) {
        mp3InBuf.readPtr = mp3InBuf.InBuf;
        mp3InBuf.offset = 0;
        mp3InBuf.bytesLeft = sizeof(mp3InBuf.InBuf);
        return 0;
    } else {
        return -1;
    }
}

int mp3ParseId3v1(FIL *fd, struct tag_info *info) {
    FRESULT res;
    UINT bytes_read;

    res = f_lseek(fd, f_tell(fd) - 128);
    if (res == FR_OK) {
        res = f_read(fd, (char*) mp3ID3Buffer, 128, &bytes_read);
        if (res == FR_OK) {
            /* ID3v1 */
            if (strncmp("TAG", (char*) mp3ID3Buffer, 3) == 0) {
                strncpy(info->title, (char*) mp3ID3Buffer + 3, 30);
                info->title[31] = 0;
                strncpy(info->artist, (char*) mp3ID3Buffer + 3 + 30, 30);
                info->artist[31] = 0;
                return 0;
            }
            return -1;
        }
        return -3;
    }
    return -2;
}

int mp3ParseId3v2(FIL *fd, struct tag_info *info) {
    uint32_t p = 0;
    UINT bytesRead;

    f_rewind(fd);
    f_read(fd, (char*) mp3ID3Buffer, 1024, &bytesRead);

    if (strncmp("ID3", (char*) mp3ID3Buffer, 3) == 0) {
        uint32_t tagSize, frameSize, i;
        uint8_t versionMajor;
        int frameHeaderSize;

        tagSize = ((uint32_t) mp3ID3Buffer[6] << 21) | ((uint32_t) mp3ID3Buffer[7] << 14)
                | ((uint16_t) mp3ID3Buffer[8] << 7) | mp3ID3Buffer[9];
        info->data_start = tagSize;
        versionMajor = mp3ID3Buffer[3];
        if (versionMajor >= 3) {
            frameHeaderSize = 10;
        } else {
            frameHeaderSize = 6;
        }
        i = p = 10;

        // iterate through frames
        while (p < tagSize) {
            if (versionMajor >= 3) {
                frameSize = ((uint32_t) mp3ID3Buffer[i + 4] << 24) | ((uint32_t) mp3ID3Buffer[i + 5] << 16)
                        | ((uint16_t) mp3ID3Buffer[i + 6] << 8) | mp3ID3Buffer[i + 7];
            } else {
                frameSize = ((uint32_t) mp3ID3Buffer[i + 3] << 14) | ((uint16_t) mp3ID3Buffer[i + 4] << 7)
                        | mp3ID3Buffer[i + 5];
            }
            if (i + frameSize + frameHeaderSize + frameHeaderSize >= sizeof(mp3ID3Buffer)) {
                if (frameSize + frameHeaderSize > sizeof(mp3ID3Buffer)) {
                    f_lseek(fd, f_tell(fd) + p + frameSize + frameHeaderSize);

                    f_read(fd, (char*) mp3ID3Buffer, sizeof(mp3ID3Buffer), &bytesRead);
                    p += frameSize + frameHeaderSize;
                    i = 0;
                    continue;
                } else {
                    int r = sizeof(mp3ID3Buffer) - i;
                    memmove(mp3ID3Buffer, mp3ID3Buffer + i, r);
                    f_read(fd, (char*) mp3ID3Buffer + r, i, &bytesRead);
                    i = 0;
                }
            }

            if (strncmp("TT2", (char*) mp3ID3Buffer + i, 3) == 0
                    || strncmp("TIT2", (char*) mp3ID3Buffer + i, 4) == 0) {
                strncpy(info->title, (char *) mp3ID3Buffer + i + frameHeaderSize + 1, MIN(frameSize - 1, sizeof(info->title) - 1));
            } else if (strncmp("TP1", (char*) mp3ID3Buffer + i, 3) == 0
                    || strncmp("TPE1", (char*) mp3ID3Buffer + i, 4) == 0) {
                strncpy(info->artist, (char *) mp3ID3Buffer + i + frameHeaderSize + 1, MIN(frameSize - 1, sizeof(info->artist) - 1));
            }

            p += frameSize + frameHeaderSize;
            i += frameSize + frameHeaderSize;
        }

        return 0;
    }

    return -1;
}

static char* GetTrackName(void){
    return info.title;
}

static uint32_t GetBitrate(void){
    return info.bit_rate;
}

static int8_t Deinit(void) {
    memset(&info, 0, sizeof(info));
    return 0;
}

player_t* GetMp3Player(void){
    static player_t mp3 = {
            .pInit = mp3PlayInit,
            .pPlay = mp3Play,
            .pGetTrackName = GetTrackName,
            .pGetBitrate = GetBitrate,
            .pDeinit = Deinit
    };
    return &mp3;
}
