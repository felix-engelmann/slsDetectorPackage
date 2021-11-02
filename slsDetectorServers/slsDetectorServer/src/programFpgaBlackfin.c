// SPDX-License-Identifier: LGPL-3.0-or-other
// Copyright (C) 2021 Contributors to the SLS Detector Package
#include "programFpgaBlackfin.h"
#include "clogger.h"
#include "common.h"
#include "sls/ansi.h"
#include "slsDetectorServer_defs.h"

#include <string.h>
#include <sys/sysinfo.h>
#include <unistd.h> // usleep

/* global variables */
// clang-format off
#define MAX_TIME_FPGA_TOUCH_FLASH_US (10 * 1000 * 1000) // 10s
#define CMD_GET_FLASH    "awk \'$4== \"\\\"bitfile(spi)\\\"\" {print $1}\' /proc/mtd"
#define CMD_FPGA_PICKED_STATUS  "cat /sys/class/gpio/gpio7/value"
#define FLASH_BUFFER_MEMORY_SIZE (128 * 1024) // 500 KB
// clang-format on

#define FLASH_DRIVE_NAME_SIZE 16
char flashDriveName[FLASH_DRIVE_NAME_SIZE] = {0};
int gpioDefined = 0;

extern int executeCommand(char *command, char *result, enum TLogLevel level);

int latestKernelVerified = -1;
#define KERNEL_DATE_VRSN_3GPIO "Fri Oct 29 07:04:21 2021"

void defineGPIOpins() {
#ifdef VIRTUAL
    return;
#endif
    if (latestKernelVerified == -1) {
        if (FAIL == validateKernelVersion(KERNEL_DATE_VRSN_3GPIO)) {
            latestKernelVerified = 0;
            LOG(logWARNING,
                ("Kernel too old to use gpio 3 pins. Not the end "
                 "of the world. Continuing with current kernel.\n"));
        } else {
            latestKernelVerified = 1;
        }
    }
    if (!gpioDefined) {
        // define the gpio pins
        system("echo 7 > /sys/class/gpio/export");
        LOG(logINFO, ("\tgpio7: defined\n"));
        system("echo in  > /sys/class/gpio/gpio7/direction");
        LOG(logINFO, ("\tgpio7: setting intput\n"));
        system("echo 9 > /sys/class/gpio/export");
        LOG(logINFO, ("\tgpio9: defined\n"));

        if (latestKernelVerified == 1) {
            // gpio 3 = not chip enable
            system("echo 3 > /sys/class/gpio/export");
            LOG(logINFO, ("\tgpio3: defined\n"));
        }
        gpioDefined = 1;
    } else
        LOG(logDEBUG1, ("gpio pins already defined earlier\n"));
}

void FPGAdontTouchFlash() {
#ifdef VIRTUAL
    return;
#endif
    // define as output pins
    system("echo out > /sys/class/gpio/gpio9/direction");
    LOG(logINFO, ("\tgpio9: setting output\n"));
    if (latestKernelVerified == 1) {
        // gpio 3 = not chip enable
        system("echo out > /sys/class/gpio/gpio3/direction");
        LOG(logINFO, ("\tgpio3: setting output\n"));
    }

    // tell FPGA to not touch flash
    system("echo 0 > /sys/class/gpio/gpio9/value");
        LOG(logINFO, ("\tgpio9: fpga dont touch flash\n"));
    if (latestKernelVerified == 1) {
        system("echo 1 > /sys/class/gpio/gpio3/value");
        LOG(logINFO, ("\tgpio3: fpga dont touch flash\n"));
    }
    // usleep(100*1000);
}

void FPGATouchFlash() {
#ifdef VIRTUAL
    return;
#endif
    // tell FPGA to touch flash to program itself
    system("echo in  > /sys/class/gpio/gpio9/direction");
    LOG(logINFO, ("\tgpio9: setting input\n"));
    if (latestKernelVerified == 1) {
        system("echo in  > /sys/class/gpio/gpio3/direction");
        LOG(logINFO, ("\tgpio3: setting input\n"));
    }
}

void resetFPGA() {
    LOG(logINFOBLUE, ("Reseting FPGA\n"));
#ifdef VIRTUAL
    return;
#endif
    FPGAdontTouchFlash();
    FPGATouchFlash();
    usleep(CTRL_SRVR_INIT_TIME_US);
}

int deleteOldFile(char *mess) {
    char cmd[MAX_STR_LENGTH] = {0};
    char retvals[MAX_STR_LENGTH] = {0};

    char *format = "rm -fr %s";
    if (snprintf(cmd, MAX_STR_LENGTH, format, TEMP_PROG_FILE_NAME) >=
        MAX_STR_LENGTH) {
        strcpy(
            mess,
            "Could not program fpga. Command to delete old file is too long\n");
        LOG(logERROR, (mess));
        return FAIL;
    }
    if (executeCommand(cmd, retvals, logDEBUG1) == FAIL) {
        snprintf(mess, MAX_STR_LENGTH,
                 "Could not program fpga. (could not delete old file: %s)\n",
                 retvals);
        // LOG(logERROR, (mess)); already printed in executecommand
        return FAIL;
    }
    LOG(logINFO,
        ("\tDeleted old programming file (%s)\n", TEMP_PROG_FILE_NAME));
    return OK;
}

int preparetoCopyFPGAProgram(FILE **fd, uint64_t fsize, char *mess) {

    if (deleteOldFile(mess) == FAIL) {
        return FAIL;
    }

    // check available memory to copy program
    {
        struct sysinfo info;
        sysinfo(&info);
        if (fsize >= info.freeram) {
            sprintf(mess,
                    "Could not program fpga. Not enough memory to copy "
                    "program. [File size:%ldMB, free RAM: %ldMB]\n",
                    (long int)(fsize / (1024 * 1024)),
                    (long int)(info.freeram / (1024 * 1024)));
            LOG(logERROR, (mess));
            return FAIL;
        }
    }

    // open file to copy program
    *fd = fopen(TEMP_PROG_FILE_NAME, "w");
    if (*fd == NULL) {
        sprintf(mess, "Unable to open %s in write mode\n", TEMP_PROG_FILE_NAME);
        LOG(logERROR, (mess));
        return FAIL;
    }
    LOG(logINFO, ("\tGoing to copy program to %s\n", TEMP_PROG_FILE_NAME));
    return OK;
}

int copyToFlash(ssize_t fsize, char *clientChecksum, char *mess) {

    if (getDrive(mess) == FAIL) {
        return FAIL;
    }

    FILE *flashfd = NULL;
    FILE *srcfd = NULL;
    if (openFileForFlash(&flashfd, &srcfd, mess) == FAIL) {
        return FAIL;
    }

    if (eraseFlash(mess) == FAIL) {
        fclose(flashfd);
        fclose(srcfd);
        return FAIL;
    }

    if (writeToFlash(fsize, flashfd, srcfd, mess) == FAIL) {
        return FAIL;
    }

    if (deleteOldFile(mess) == FAIL) {
        return FAIL;
    }

    /* ignoring this until flash fixed
    if (verifyChecksumFromFlash(mess, clientChecksum, flashDriveName, fsize) == FAIL) {     return FAIL;
    }
    */
    if (waitForFPGAtoTouchFlash(mess) == FAIL) {
        return FAIL;
    }

    return OK;
}

int getDrive(char *mess) {
#ifdef VIRTUAL
    strcpy(flashDriveName, "/tmp/SLS_mtd3");
    return OK;
#endif
    LOG(logDEBUG1, ("Finding flash drive...\n"));
    // getting the drive
    // root:/>  cat /proc/mtd
    // dev:    size   erasesize  name
    // mtd0: 00040000 00020000 "bootloader(nor)"
    // mtd1: 00100000 00020000 "linux kernel(nor)"
    // mtd2: 002c0000 00020000 "file system(nor)"
    // mtd3: 01000000 00010000 "bitfile(spi)"

    char cmd[MAX_STR_LENGTH] = {0};
    char retvals[MAX_STR_LENGTH] = {0};

    strcpy(cmd, CMD_GET_FLASH);
    if (executeCommand(cmd, retvals, logDEBUG1) == FAIL) {
        snprintf(mess, MAX_STR_LENGTH,
                 "Could not program fpga. (could not get flash drive: %s)\n",
                 retvals);
        // LOG(logERROR, (mess)); already printed in executecommand
        return FAIL;
    }

    char *pch = strtok(retvals, ":");
    if (pch == NULL) {
        strcpy(mess, "Could not get mtd drive to flash (strtok fail).\n");
        LOG(logERROR, (mess));
        return FAIL;
    }

    memset(flashDriveName, 0, sizeof(flashDriveName));
    strcpy(flashDriveName, "/dev/");
    strcat(flashDriveName, pch);
    LOG(logINFO, ("\tFlash drive found: %s\n", flashDriveName));
    return OK;
}

int openFileForFlash(FILE **flashfd, FILE **srcfd, char *mess) {
    FPGAdontTouchFlash();

    // open src file
    *srcfd = fopen(TEMP_PROG_FILE_NAME, "r");
    if (*srcfd == NULL) {
        sprintf(mess,
                "Could not flash. Unable to open temp program file %s in read "
                "mode\n",
                TEMP_PROG_FILE_NAME);
        LOG(logERROR, (mess));
        return FAIL;
    }
    LOG(logDEBUG1, ("Temp file ready for reading\n"));

    // open flash drive for writing
    *flashfd = fopen(flashDriveName, "w");
    if (*flashfd == NULL) {
        fclose(*srcfd);
        sprintf(mess, "Unable to open flash drive %s in write mode\n",
                flashDriveName);
        LOG(logERROR, (mess));
        return FAIL;
    }
    LOG(logINFO, ("\tFlash ready for writing\n"));
    return OK;
}

int eraseFlash(char *mess) {
    LOG(logINFO, ("\tErasing Flash...\n"));

#ifdef VIRTUAL
    return OK;
#endif
    char cmd[MAX_STR_LENGTH] = {0};
    char retvals[MAX_STR_LENGTH] = {0};
    char *format = "flash_eraseall %s";
    if (snprintf(cmd, MAX_STR_LENGTH, format, flashDriveName) >=
        MAX_STR_LENGTH) {
        strcpy(mess,
               "Could not program fpga. Command to erase flash is too long\n");
        LOG(logERROR, (mess));
        return FAIL;
    }
    if (executeCommand(cmd, retvals, logDEBUG1) == FAIL) {
        snprintf(mess, MAX_STR_LENGTH,
                 "Could not program fpga. (could not erase flash: %s)\n",
                 retvals);
        // LOG(logERROR, (mess)); already printed in executecommand
        return FAIL;
    }
    LOG(logINFO, ("\tFlash erased\n"));
    return OK;
}

int writeToFlash(ssize_t fsize, FILE *flashfd, FILE *srcfd, char *mess) {
    LOG(logDEBUG1, ("writing to flash\n"));

    char *buffer = malloc(FLASH_BUFFER_MEMORY_SIZE);
    if (buffer == NULL) {
        fclose(flashfd);
        fclose(srcfd);
        strcpy(mess, "Could not program fpga. Memory allocation to write to "
                     "flash failed.\n");
        LOG(logERROR, (mess));
        return FAIL;
    }
    LOG(logINFO, ("\tWriting to Flash...\n"));

    int oldProgress = 0;
    ssize_t totalBytes = 0;
    ssize_t bytes =
        fread((void *)buffer, sizeof(char), FLASH_BUFFER_MEMORY_SIZE, srcfd);

    while (bytes > 0) {

        ssize_t bytesWritten =
            fwrite((void *)buffer, sizeof(char), bytes, flashfd);
        totalBytes += bytesWritten;

        if (bytesWritten != bytes) {
            free(buffer);
            fclose(flashfd);
            fclose(srcfd);
            sprintf(mess,
                    "Could not write to flash (bytes written:%ld, expected: "
                    "%ld, total written:%ld)\n",
                    (long int)bytesWritten, (long int)bytes,
                    (long int)totalBytes);
            LOG(logERROR, (mess));
            return FAIL;
        }

        // print progress
        if (fsize > 0) {
            int progress = (int)(((double)(totalBytes) / fsize) * 100);
            if (oldProgress != progress) {
                printf("%d%%\r", progress);
                fflush(stdout);
                oldProgress = progress;
            }
        } else
            printf(".");

        bytes = fread((void *)buffer, sizeof(char), FLASH_BUFFER_MEMORY_SIZE,
                      srcfd);
    }
    if (fsize <= 0) {
        printf("\n");
    }
    free(buffer);
    fclose(flashfd);
    fclose(srcfd);
    LOG(logINFO, ("\tWrote %ld bytes to flash\n", totalBytes));

    if (totalBytes != fsize) {
        sprintf(mess,
                "Could not program fpga. Incorrect bytes written to flash %lu "
                "[expected: %lu]\n",
                totalBytes, fsize);
        LOG(logERROR, (mess));
        return FAIL;
    }
    return OK;
}

int waitForFPGAtoTouchFlash(char *mess) {
    // touch and program
    FPGATouchFlash();

#ifdef VIRTUAL
    return OK;
#endif
    LOG(logINFO, ("\tWaiting for FPGA to program from flash\n"));
    int timeSpent = 0;

    int result = 0;
    while (result == 0) {
        // time taken for fpga to pick up from flash
        usleep(1000);
        timeSpent += 1000;
        if (timeSpent >= MAX_TIME_FPGA_TOUCH_FLASH_US) {
            sprintf(
                mess,
                "Could not program fpga. (exceeded max time allowed: %ds)\n",
                MAX_TIME_FPGA_TOUCH_FLASH_US / (1000 * 1000));
            LOG(logERROR, (mess));
            return FAIL;
        }

        // read gpio status
        char retvals[MAX_STR_LENGTH] = {0};
        if (FAIL ==
            executeCommand(CMD_FPGA_PICKED_STATUS, retvals, logDEBUG1)) {
            snprintf(
                mess, MAX_STR_LENGTH,
                "Could not program fpga. (could not read gpio status: %s)\n",
                retvals);
            // LOG(logERROR, (mess)); already printed in executecommand
            return FAIL;
        }

        // convert to int
        if (sscanf(retvals, "%d\n", &result) != 1) {
            snprintf(mess, MAX_STR_LENGTH,
                     "Could not program fpga. (could not scan int for gpio "
                     "status: [%s])\n",
                     retvals);
            LOG(logERROR, (mess));
            return FAIL;
        }
        LOG(logDEBUG1, ("gpi07 returned %d\n", result));
    }
    LOG(logINFO, ("\tFPGA has picked up the program from flash\n"));
    return OK;
}
