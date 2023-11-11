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

#include "usb_host.h"
#include <fatfs.h>

#include "semphr.h"

#define MP3_AUDIO_BUF_SZ    (5 * 1024)

typedef struct {
    int bytesLeft;
    int err;
    int offset;
    uint8_t InBuf[MAINBUF_SIZE];
    uint8_t *readPtr;
} mp3InBuf_t;

typedef struct {
    xSemaphoreHandle sem;
    int16_t buf1[MAX_NSAMP * MAX_NGRAN * MAX_NCHAN];
    int16_t buf2[MAX_NSAMP * MAX_NGRAN * MAX_NCHAN];
    uint8_t activeBuf;
    uint8_t itIsFirstWrite;
} mp3OutBuf_t;

uint8_t mp3_fd_buffer[MP3_AUDIO_BUF_SZ];

static HMP3Decoder mp3Decoder;
static MP3FrameInfo mp3FrameInfo;
static unsigned char *read_ptr;
static int bytes_left = 0, err, offset;
static unsigned char *mp3buf;
static unsigned int mp3buf_size;

static uint8_t InBuf[MAINBUF_SIZE];

static mp3OutBuf_t mp3OutBuf;

static void mp3_init(unsigned char *buffer, unsigned int buffer_size);
static void mp3_reset(void);
static int mp3_process(FIL *mp3file);
static int mp3_refill_inbuffer(FIL *mp3file);

int8_t mp3PlayInit(void) {
    BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_AUTO, 70, AUDIO_FREQUENCY_44K);
    vSemaphoreCreateBinary(mp3OutBuf.sem);
    mp3OutBuf.activeBuf = 0;
    mp3OutBuf.itIsFirstWrite = 1;
    mp3Decoder = MP3InitDecoder();
    mp3_init(InBuf, sizeof(InBuf));
    return 0;
}

void mp3TestTask(FIL *mp3file) {
    int mp3Result = 0;

    while (mp3Result == 0) {

        mp3Result = mp3_process(mp3file);

        if (BSP_PB_GetState(BUTTON_KEY)) {
            mp3Result = 1;
        }
    }
    xSemaphoreTake(mp3OutBuf.sem, portMAX_DELAY);
    BSP_AUDIO_OUT_Stop(CODEC_PDWN_HW);
    mp3_reset();
    mp3OutBuf.itIsFirstWrite = 1;
    xSemaphoreGive(mp3OutBuf.sem);
    mp3Result = 0;
}

static void mp3_init(unsigned char *buffer, unsigned int buffer_size) {
    mp3buf = buffer;
    mp3buf_size = buffer_size;
    mp3_reset();
}

static void mp3_reset(void) {
    read_ptr = NULL;
    bytes_left = 0;
}

static int mp3_process(FIL *mp3file) {

    short *pBuf;

    if (read_ptr == NULL) {
        if (mp3_refill_inbuffer(mp3file) != 0)
            return -1;
    }

    offset = MP3FindSyncWord(read_ptr, bytes_left);
    if (offset < 0) {

        if (mp3_refill_inbuffer(mp3file) != 0)
            return -2;
    }

    read_ptr += offset;
    bytes_left -= offset;

    // check if this is really a valid frame
    // (the decoder does not seem to calculate CRC, so make some plausibility checks)
    if (MP3GetNextFrameInfo(mp3Decoder, &mp3FrameInfo, read_ptr) == 0 && mp3FrameInfo.nChans == 2
            && mp3FrameInfo.version == 0) {

    } else {
        // advance data pointer
        // TODO: handle bytes_left == 0
        //assert(bytes_left > 0);
        if (bytes_left > 0) {
            bytes_left -= 1;
            read_ptr += 1;
        }
        return 0;
    }

    if (bytes_left < 1024) {
        if (mp3_refill_inbuffer(mp3file) != 0)
            return -3;
    }

    if (!mp3OutBuf.activeBuf) {
        pBuf = mp3OutBuf.buf1;
    } else {
        pBuf = mp3OutBuf.buf2;
    }

    BSP_LED_On(LED4);
    err = MP3Decode(mp3Decoder, &read_ptr, &bytes_left, pBuf, 0);
    BSP_LED_Off(LED4);

    if (err) {
        switch (err) {
        case ERR_MP3_INDATA_UNDERFLOW:
            bytes_left = 0;
            if (mp3_refill_inbuffer(mp3file) != 0)
                return -4;
            break;

        case ERR_MP3_MAINDATA_UNDERFLOW:
            // do nothing - next call to decode will provide more mainData
            break;

        default:
            // skip this frame
            if (bytes_left > 0) {
                bytes_left--;
                read_ptr++;
            }
            break;
        }

    } else {
        // no error
        MP3GetLastFrameInfo(mp3Decoder, &mp3FrameInfo);

        xSemaphoreTake(mp3OutBuf.sem, portMAX_DELAY);

        if (mp3OutBuf.itIsFirstWrite == 1) {
            //BSP_AUDIO_OUT_SetFrequency(mp3FrameInfo.samprate);
            BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_AUTO, 70, mp3FrameInfo.samprate);
            BSP_AUDIO_OUT_Play((uint16_t*) pBuf, mp3FrameInfo.outputSamps * 2);
            mp3OutBuf.itIsFirstWrite = 0;
        }

        mp3OutBuf.activeBuf ^= 0xFF;
    }

    return 0;
}

static int mp3_refill_inbuffer(FIL *mp3file) {
    UINT bytes_read;
    UINT bytes_to_read;

    if (bytes_left > 0) {
        memmove(mp3buf, read_ptr, bytes_left);
    }

    bytes_to_read = mp3buf_size - bytes_left;
    BSP_LED_Toggle(LED5);
    if (f_read(mp3file, (BYTE*) mp3buf + bytes_left, bytes_to_read, &bytes_read) != FR_OK) {
        return -1;
    }
    if (bytes_read == bytes_to_read) {
        read_ptr = mp3buf;
        offset = 0;
        bytes_left = mp3buf_size;
        return 0;
    } else {
        return -1;
    }
}

void BSP_AUDIO_OUT_TransferComplete_CallBack(void) {
    short *pBuf;
    static signed portBASE_TYPE xHigherPriorityTaskWoken;
    xHigherPriorityTaskWoken = pdFALSE;
    if (!mp3OutBuf.activeBuf) {
        pBuf = mp3OutBuf.buf1;
    } else {
        pBuf = mp3OutBuf.buf2;
    }

    BSP_AUDIO_OUT_Play((uint16_t*) pBuf, mp3FrameInfo.outputSamps * 2);

    xSemaphoreGiveFromISR(mp3OutBuf.sem, &xHigherPriorityTaskWoken);

    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

int mp3ParseId3v1(FIL *fd, struct tag_info *info) {
    FRESULT res;
    UINT bytes_read;

    res = f_lseek(fd, f_tell(fd) - 128);
    if (res == FR_OK) {
        res = f_read(fd, (char*) mp3_fd_buffer, 128, &bytes_read);
        if (res == FR_OK) {
            /* ID3v1 */
            if (strncmp("TAG", (char*) mp3_fd_buffer, 3) == 0) {
                strncpy(info->title, (char*) mp3_fd_buffer + 3, 30);
                info->title[31] = 0;
                strncpy(info->artist, (char*) mp3_fd_buffer + 3 + 30, 30);
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
    UINT bytes_read;

    f_rewind(fd);
    f_read(fd, (char*) mp3_fd_buffer, sizeof(mp3_fd_buffer), &bytes_read);

    if (strncmp("ID3", (char*) mp3_fd_buffer, 3) == 0) {
        uint32_t tag_size, frame_size, i;
        uint8_t version_major;
        int frame_header_size;

        tag_size = ((uint32_t) mp3_fd_buffer[6] << 21) | ((uint32_t) mp3_fd_buffer[7] << 14)
                | ((uint16_t) mp3_fd_buffer[8] << 7) | mp3_fd_buffer[9];
        info->data_start = tag_size;
        version_major = mp3_fd_buffer[3];
        if (version_major >= 3) {
            frame_header_size = 10;
        } else {
            frame_header_size = 6;
        }
        i = p = 10;

        // iterate through frames
        while (p < tag_size) {
            if (version_major >= 3) {
                frame_size = ((uint32_t) mp3_fd_buffer[i + 4] << 24) | ((uint32_t) mp3_fd_buffer[i + 5] << 16)
                        | ((uint16_t) mp3_fd_buffer[i + 6] << 8) | mp3_fd_buffer[i + 7];
            } else {
                frame_size = ((uint32_t) mp3_fd_buffer[i + 3] << 14) | ((uint16_t) mp3_fd_buffer[i + 4] << 7)
                        | mp3_fd_buffer[i + 5];
            }
            if (i + frame_size + frame_header_size + frame_header_size >= sizeof(mp3_fd_buffer)) {
                if (frame_size + frame_header_size > sizeof(mp3_fd_buffer)) {
                    f_lseek(fd, f_tell(fd) + p + frame_size + frame_header_size);

                    f_read(fd, (char*) mp3_fd_buffer, sizeof(mp3_fd_buffer), &bytes_read);
                    p += frame_size + frame_header_size;
                    i = 0;
                    continue;
                } else {
                    int r = sizeof(mp3_fd_buffer) - i;
                    memmove(mp3_fd_buffer, mp3_fd_buffer + i, r);
                    f_read(fd, (char*) mp3_fd_buffer + r, i, &bytes_read);
                    i = 0;
                }
            }

            if (strncmp("TT2", (char*) mp3_fd_buffer + i, 3) == 0
                    || strncmp("TIT2", (char*) mp3_fd_buffer + i, 4) == 0) {
                //strncpy(info->title, (char *) mp3_fd_buffer + i + frame_header_size + 1, MIN(frame_size - 1, sizeof(info->title) - 1));
            } else if (strncmp("TP1", (char*) mp3_fd_buffer + i, 3) == 0
                    || strncmp("TPE1", (char*) mp3_fd_buffer + i, 4) == 0) {
                //strncpy(info->artist, (char *) mp3_fd_buffer + i + frame_header_size + 1, MIN(frame_size - 1, sizeof(info->artist) - 1));
            }

            p += frame_size + frame_header_size;
            i += frame_size + frame_header_size;
        }

        return 0;
    }

    return -1;
}

