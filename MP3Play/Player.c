/*
 * FileChoser.c
 *
 *  Created on: Nov 8, 2023
 *      Author: Alexander
 */

#include "Player.h"

#include "usb_host.h"
#include <fatfs.h>
#include "ff.h"
#include "MP3Play.h"

typedef struct {
    FRESULT result;
    DIR dir;
    FIL file;
    FILINFO fileInfo;
    char fullPath[_MAX_LFN + 4]; // 4 it for "0:/" + zero end of string
    char currentDir[_MAX_LFN + 4]; // 4 it for "0:/" + zero end of string
    uint8_t needFindDir;
    char previousName[_MAX_LFN + 4];
} playerData_t;

static const TCHAR RootDir[] = { "0:/" };
static playerData_t playerData;
static struct tag_info info;

static void playerTask(void *param);
static void returnLevelUp(void);

uint8_t playerInit(void) {
    playerData.needFindDir = 0;
    xTaskCreate(playerTask, "Player", 1024, NULL, 1, NULL);
    return 0;
}

static void playerTask(void *param) {
    (void) param;
    mp3PlayInit();
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
                    } else {
                        returnLevelUp(); // need find higher level
                        if (playerData.previousName[0] == 0) { // it is root
                            playerData.needFindDir = 0;
                        }
                    }

                    break;
                }

                if (!(playerData.fileInfo.fattrib & AM_DIR) && !playerData.needFindDir) { /* It is a file */

                    if ((memcmp(&playerData.fileInfo.fname[strlen(playerData.fileInfo.fname) - 3], "mp3", 3) == 0)
                            || (memcmp(&playerData.fileInfo.fname[strlen(playerData.fileInfo.fname) - 3], "MP3", 3) == 0)) {
                        strcpy(playerData.fullPath, playerData.currentDir);
                        uint16_t len = strlen(playerData.currentDir);
                        if (len != 3) {
                            strcat(playerData.fullPath, "/");
                        }
                        strcat(playerData.fullPath, playerData.fileInfo.fname);
                        playerData.result = f_open(&playerData.file, playerData.fullPath, FA_OPEN_EXISTING | FA_READ);
                        if (playerData.result == FR_OK) {
                            f_lseek(&playerData.file, 0UL);

                            mp3ParseId3v1(&playerData.file, &info);
                            mp3ParseId3v2(&playerData.file, &info);

                            f_lseek(&playerData.file, info.data_start);

                            // here need call MP3 decoder
                            mp3TestTask(&playerData.file);

                            f_close(&playerData.file);
                            memset(&info, 0, sizeof(info));
                        }/**/
                        vTaskDelay(200);
                    }
                }

                if (playerData.fileInfo.fattrib & AM_DIR && playerData.needFindDir) {
                    if (!playerData.previousName[0]) {
                        uint16_t len = strlen(playerData.currentDir);
                        if (len != 3) {
                            strcat(playerData.currentDir, "/");
                        }
                        strcat(playerData.currentDir, playerData.fileInfo.fname);

                        playerData.needFindDir = 0;
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

