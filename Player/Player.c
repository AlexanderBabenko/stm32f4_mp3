/*
 * FileChoser.c
 *
 *  Created on: Nov 8, 2023
 *      Author: Alexander
 */

#include "Player.h"

#include <ctype.h>

#include "usb_host.h"
#include "fatfs.h"
#include "ff.h"
#include "MP3Play.h"
#include "WavPlay.h"
#include "AacPlayer.h"

#define _MAX_PATH_LEN (_MAX_LFN + 4) // 4 it for "0:/" + zero end of string
#define TASK_STACK_SIZE (2 * 1024) // in words means *4
#define DELAY_BETWEEN_MP3 200
#define FILENAME_EXTENSION_LENGTH 3

typedef struct {
    FRESULT result;
    DIR dir;
    FIL file;
    FILINFO fileInfo;
    char fullPath[_MAX_PATH_LEN];
    char currentDir[_MAX_PATH_LEN];
    uint8_t needFindDir;
    char previousName[_MAX_PATH_LEN];
} playerData_t;

static const TCHAR RootDir[] = { "0:/" };
static playerData_t playerData;
static uint8_t workBuf[2048];

static void playerTask(void *param);
static void returnLevelUp(void);
static uint8_t concatPath(char *path, char *subPath);
static uint8_t checkFileExt(const char *path, const char *ext);

uint8_t playerInit(void) {
    playerData.needFindDir = 0;
    xTaskCreate(playerTask, "Player", TASK_STACK_SIZE, NULL, 1, NULL);
    return 0;
}

static void playerTask(void *param) {
    (void) param;
    //mp3PlayInit();
    // mount disk drive
    f_mount(&USBHFatFS, USBHPath, 0);

    memcpy(playerData.currentDir, RootDir, sizeof(RootDir));

    while (1) {
        playerData.result = f_opendir(&playerData.dir, playerData.currentDir); /* Open the directory */
        if (playerData.result == FR_OK) {

            for (;;) {
                playerData.result = f_readdir(&playerData.dir, &playerData.fileInfo); /* Read a directory item */
                if (playerData.result != FR_OK) {
                    // end of dir or error or file not found
                    break;
                }

                if (playerData.fileInfo.fname[0] == '.') {
                    continue; /* Ignore dot entry */
                }

                if (playerData.fileInfo.fname[0] == 0) {
                    if (!playerData.needFindDir) {
                        playerData.previousName[0] = 0;
                        playerData.needFindDir = 1; // need find next dir
                        f_readdir(&playerData.dir, 0);
                        continue;
                    } else {
                        returnLevelUp(); // need find higher level
                        if (playerData.previousName[0] == 0) { // it is root
                            playerData.needFindDir = 0;
                        }
                        break;
                    }
                }

                if (!(playerData.fileInfo.fattrib & AM_DIR) && !playerData.needFindDir) { /* It is a file */

                    player_t* pPlayer = NULL;

                    if (!checkFileExt(playerData.fileInfo.fname, "mp3")) {
                        pPlayer = GetMp3Player();
                    }

                    if (!checkFileExt(playerData.fileInfo.fname, "wav")) {
                        pPlayer = GetWavPlay();
                    }

                    if (!checkFileExt(playerData.fileInfo.fname, "m4a")) {
                        pPlayer = GetM4APlayer();
                    }

                    if (!checkFileExt(playerData.fileInfo.fname, "aac")) {
                        pPlayer = GetAacPlayer();
                    }

                    if(pPlayer){
                        pPlayer->pInit(workBuf);
                        strcpy(playerData.fullPath, playerData.currentDir);
                        // if we can't concatenate filename just drop it
                        if (!concatPath(playerData.fullPath, playerData.fileInfo.fname)) {
                            playerData.result = f_open(&playerData.file, playerData.fullPath,
                            FA_OPEN_EXISTING | FA_READ);
                            if (playerData.result == FR_OK) {
                                f_lseek(&playerData.file, 0UL);

                                pPlayer->pPlay(&playerData.file, GetAudioOut());

                                f_close(&playerData.file);
                            }
                        }
                        vTaskDelay(DELAY_BETWEEN_MP3);
                        pPlayer->pDeinit();
                    }
                }

                if (playerData.fileInfo.fattrib & AM_DIR && playerData.needFindDir) {
                    if (!playerData.previousName[0]) {
                        // if we can't concatenate path means need return level up
                        if (!concatPath(playerData.currentDir, playerData.fileInfo.fname)) {
                            playerData.needFindDir = 0;
                        }
                        break;
                    } else if (strcmp(playerData.previousName, playerData.fileInfo.fname) == 0) {
                        playerData.previousName[0] = 0; // need next item after that
                    }
                }
            }
            f_closedir(&playerData.dir);
        }
    }
}

void returnLevelUp(void) {
    uint16_t len = strlen(playerData.currentDir);

    for (; len != 0; len--) {
        if (playerData.currentDir[len] == '/') {
            strcpy(playerData.previousName, &playerData.currentDir[len + 1]);
            if (len != 2) { // not root dir
                playerData.currentDir[len] = 0;
            } else {
                playerData.currentDir[len + 1] = 0;
            }
            break;
        }
    }
}

uint8_t concatPath(char *path, char *subPath) {
    uint16_t len = strlen(path);
    if(!path || !subPath) {
        return 2;
    }
    if (len != 3) {
        strcat(path, "/");
        len++;
    }
    if ((len + strlen(subPath) + 1) <= _MAX_PATH_LEN) {
        memcpy(path + len, subPath, strlen(subPath) + 1); // copy len + 1 for zero terminating
    } else {
        return 1;
    }
    return 0;
}

uint8_t checkFileExt(const char *path, const char *ext) {
    char upperFileExt[FILENAME_EXTENSION_LENGTH + 1];
    char upperExt[FILENAME_EXTENSION_LENGTH + 1];
    uint32_t extPos;
    uint8_t i;

    if(!path || !ext){
        return 2;
    }

    extPos = strlen(path) - FILENAME_EXTENSION_LENGTH;

    for (i = 0; i < FILENAME_EXTENSION_LENGTH + 1; i++) {
        upperFileExt[i] = toupper(path[extPos + i]);
        upperExt[i] = toupper(ext[i]);
    }

    if (strcmp(upperFileExt, upperExt) == 0) {
        return 0;
    }

    return 1;
}
