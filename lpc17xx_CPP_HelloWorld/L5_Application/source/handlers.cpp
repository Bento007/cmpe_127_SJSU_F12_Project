#include <stdio.h>              // printf()
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"               // vTaskList()

#include "CommandHandler.hpp"   // CMD_HANDLER_FUNC()
#include "rtc.h"                // Set and Get System Time
#include "utilities.h"          // printMemoryInfo()
#include "storage.hpp"          // Get Storage Device instances
#include "filelogger.hpp"       // Logger class


CMD_HANDLER_FUNC(taskListHandler)
{
    // Warning: taskListBuffer[] may need additional space if more tasks get added
    // Each task takes roughly 50 characters, so this should be enough for 20 tasks
    char taskListBuffer[1024];             // Buffer to use for vTaskList()
    const int delayInMs = (int)cmdParams;  // cast parameter str to integer

    if(delayInMs > 0) {
        // Need to printf because output str won't get printed until this function exits
        printf("Sleeping for %u ms ...\n", delayInMs);

        vTaskResetRunTimeStats();
        vTaskDelay(OS_MS(delayInMs));
    }

    // Get the tasks' info and CPU usage
    vTaskList((signed char*) &taskListBuffer[0]);

    // Set output str that will get printed when this handler exits
    printf("%10s  S Pr  %5s  # CPU  Ticks\n", "Name", "Stack");
    puts(taskListBuffer);
}

CMD_HANDLER_FUNC(memInfoHandler)
{
    printMemoryInfo();
}

CMD_HANDLER_FUNC(timeHandler)
{
    RTC time;

    /**
     * If cmdParam contains "set" with six spaces, we can parse the time
     * Example: set 11 30 2014 8 25 0
     */
    if(cmdParams.beginsWith("set"))
    {
        if(cmdParams.countOf(" ") !=6) {
            printf("Need time in terms of MM DD YYYY HH MM SS");
            return;
        }

        cmdParams.getToken(" ", true);
        time.month = (int)*cmdParams.getToken();
        time.day   = (int)*cmdParams.getToken();
        time.year  = (int)*cmdParams.getToken();
        time.hour  = (int)*cmdParams.getToken();
        time.min   = (int)*cmdParams.getToken();
        time.sec   = (int)*cmdParams.getToken();

        rtc_settime(&time);
        cmdParams = "get"; // Set to get on purpose to print the time below
    }

    time = rtc_gettime();
    printf("%02u/%02u/%u  --  %02u:%02u:%02u",
            time.month, time.day, time.year,
            time.hour, time.min, time.sec);
}

CMD_HANDLER_FUNC(loggerTest)
{
    if(cmdParams == "info") {
        LOG_INFO("Logger Info Test");
        output = "\nLogged Info";
    }
    else if(cmdParams == "warn") {
        LOG_WARN("Logger Warn Test");
        output = "\nLogged a Warning";
    }
    else if(cmdParams == "error") {
        LOG_ERROR("Logger Error Test");
        output = "\nLogged an Error";
    }
    else if(cmdParams == "flush") {
        FileLogger::getInstance().flush();
    }
    else {
        output.printf("Invalid parameter: |%s|", cmdParams());
    }
}

CMD_HANDLER_FUNC(copyHandler)
{
    int indexOfSpace = cmdParams.firstIndexOf(" ");
    if(indexOfSpace <= 1) {
        puts("Error, Try: copy <src file name> <dst file name>");
        return;
    }

    char srcFile[32] = {0};
    memcpy(srcFile, cmdParams.subString(0, indexOfSpace).c_str(), sizeof(srcFile)-1);

    char dstFile[32] = {0};
    memcpy(dstFile, cmdParams.subString(indexOfSpace+1).c_str(), sizeof(dstFile)-1);

    unsigned int readTimeMs = 0;
    unsigned int writeTimeMs = 0;
    unsigned int bytesTransferred = 0;
    FRESULT copyStatus = Storage::copy(srcFile, dstFile,
                                       &readTimeMs, &writeTimeMs, &bytesTransferred);

    if(FR_OK != copyStatus) {
        printf("Error %u copying |%s| -> |%s|\n", copyStatus, srcFile, dstFile);
    }
    else {
        printf("Finished!  Read: %u Kb/sec, Write: %u Kb/sec\n",
                bytesTransferred/(0 == readTimeMs  ? 1 : readTimeMs),
                bytesTransferred/(0 == writeTimeMs ? 1 : writeTimeMs));
    }
}

CMD_HANDLER_FUNC(readHandler)
{
    // If -print was present, we will print the string using putchar()
    bool printToScreen = cmdParams.erase("-print");
    cmdParams.trimStart(" ");
    cmdParams.trimEnd(" ");

    FIL file;
    if(FR_OK != f_open(&file, cmdParams(), FA_OPEN_EXISTING | FA_READ))
    {
        printf("Failed to open: %s\n", cmdParams());
    }
    else
    {
        char buffer[1024*2];
        unsigned int bytesRead = 0;
        unsigned int totalBytesRead = 0;

        unsigned int startTime = xTaskGetTickCount();
        while(FR_OK == f_read(&file, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0)
        {
            totalBytesRead += bytesRead;

            if(printToScreen) {
                for(unsigned int i=0; i<bytesRead; i++) {
                    putchar(buffer[i]);
                }
            }
        }
        f_close(&file);

        if(!printToScreen) {
            unsigned int timeTaken = xTaskGetTickCount() - startTime;
            // Divide by zero check
            if(0 == timeTaken) {
                timeTaken = 1;
            }
            printf("Read %u bytes @ %u Kb/sec\n", totalBytesRead, totalBytesRead/timeTaken);
        }
    }
}

CMD_HANDLER_FUNC(lsHandler)
{
    DIR Dir;
    FILINFO Finfo;
    FATFS *fs;
    FRESULT returnCode = FR_OK;

    unsigned int fileBytesTotal, numFiles, numDirs;
    fileBytesTotal = numFiles = numDirs = 0;
    #if _USE_LFN
        char Lfname[512];
    #endif

    char* dirPath = (char*)cmdParams();
    if (FR_OK != (returnCode = f_opendir(&Dir, dirPath))) {
        printf("Invalid directory: |%s|\n", dirPath);
        return;
    }

    printf("Directory listing of: %s\n\n", dirPath);
    for (;;)
    {
        #if _USE_LFN
            Finfo.lfname = Lfname;
            Finfo.lfsize = sizeof(Lfname);
        #endif

        char returnCode = f_readdir(&Dir, &Finfo);
        if ( (FR_OK != returnCode) || !Finfo.fname[0])
            break;
        if (Finfo.fattrib & AM_DIR){
            numDirs++;
        }
        else{
            numFiles++;
            fileBytesTotal += Finfo.fsize;
        }
        printf("%c%c%c%c%c %u/%2u/%2u %2u:%2u %10lu %13s",
                (Finfo.fattrib & AM_DIR) ? 'D' : '-',
                (Finfo.fattrib & AM_RDO) ? 'R' : '-',
                (Finfo.fattrib & AM_HID) ? 'H' : '-',
                (Finfo.fattrib & AM_SYS) ? 'S' : '-',
                (Finfo.fattrib & AM_ARC) ? 'A' : '-',
                (Finfo.fdate >> 9) + 1980, (Finfo.fdate >> 5) & 15, Finfo.fdate & 31,
                (Finfo.ftime >> 11), (Finfo.ftime >> 5) & 63,
                Finfo.fsize, &(Finfo.fname[0]));
        #if _USE_LFN
                printf(" -- %s", Lfname);
        #endif
        printf("\n");
    }
    printf("\n%4u File(s), %10u bytes total\n%4u Dir(s)", numFiles, fileBytesTotal, numDirs);

    if (f_getfree(dirPath, (DWORD*) &fileBytesTotal, &fs) == FR_OK)
    {
        printf(", %10uK bytes free\n", fileBytesTotal * fs->csize / 2);
    }
}

CMD_HANDLER_FUNC(rmHandler)
{
    printf("Delete '%s' : %s", cmdParams(), FR_OK == f_unlink(cmdParams()) ? "OK" : "ERROR");
}


