/**
 * @file fileLogger.hpp
 * @brief This is a logger that logs data to a file on the system such as an SD Card.
 * @ingroup Utilities
 *
 * Version: 06192012    Initial
 */
#ifndef FILE_LOGGER_HPP__
#define FILE_LOGGER_HPP__



#define LPC_LOGGER              1            ///< Set to non-zero if used with Chan's FATFS Library
#define LOGGER_BUFFER_SIZE      512          ///< The size of the buffer, multiples of 512 recommended for File Logger
#define LOGGER_FILE_NAME        "0:log.csv"  ///< The filename to use for logging


/// Macros to log a message that picks up filename, function name, and line number that logged the message
#define LOG_ERROR(pMSG) FileLogger::getInstance().logError  (pMSG, __FILE__, __FUNCTION__, __LINE__)
#define LOG_WARN(pMSG)  FileLogger::getInstance().logWarning(pMSG, __FILE__, __FUNCTION__, __LINE__)
#define LOG_INFO(pMSG)  FileLogger::getInstance().logInfo   (pMSG, __FILE__, __FUNCTION__, __LINE__)



#include <stdio.h>          // Error message output
#include "src/logger.hpp"   // Base class
#include "singletonTemplate.hpp"
#if LPC_LOGGER
#include "FreeRTOS.h"
#include "semphr.h"     // FreeRTOS Semaphore for logger
#include "fat/ff.h"
#endif



/**
 * File Logger Class
 * @ingroup Utilities
 *
 * This is an instance of logger that will print the logged data once
 * the CSVLogger becomes full.
 *
 * The constructor opens a file in append mode, and when logger buffer becomes
 * full, the data is saved in the file.  This class also provides semaphore
 * and getTimestamp() function to complete the abstract logger class.
 *
 * @warning DO NOT LOG IF YOU ARE INSIDE AN ISR because if Logger becomes full
 *          then we might try to use FATFS API which uses FreeRTOS API
 */
class FileLogger : public CSVLogger, public SingletonTemplate<FileLogger>
{
    public:
    // No public API here; CSVLogger exposes public API

    protected:
        /// Protected Constructor for Singleton class
        FileLogger() : CSVLogger(mBuff,LOGGER_BUFFER_SIZE), mFileOpened(false)
        {
#if LPC_LOGGER
            mSemHandle = xSemaphoreCreateMutex();

            // Open the logger file and seek to end of file
            if(FR_OK == f_open(&mOutFile, LOGGER_FILE_NAME, FA_OPEN_ALWAYS|FA_WRITE))
            {
                if(FR_OK == f_lseek(&mOutFile, f_size(&mOutFile)))
                {
                    mFileOpened = true;
                }
            }
            // Leave the file opened to maximize performance
#endif
        }

        /// Pure-virtual base class function implementation
        void handleCompletedBuffer(char* pBuff, unsigned int size)
        {
            bool success = false;
            if(mFileOpened)
            {
#if LPC_LOGGER
                unsigned int bytesWritten = 0;
                if(FR_OK == f_write(&mOutFile, pBuff, size, &bytesWritten))
                {
                    success = (size == bytesWritten);
                    if(FR_OK != f_sync(&mOutFile)) {
                        success = false;
                    }
                }
#endif
            }
            if(!success) {
                puts("Error logging data to file, here is a printout of the data: ");
                for(unsigned int i=0; i<size; i++) {
                    putchar(pBuff[i]);
                }
            }
        }

#if LPC_LOGGER
        /// @{ \name Virtual function overrides:
            void semTake(){ xSemaphoreTake(mSemHandle, portMAX_DELAY); }
            void semGive(){ xSemaphoreGive(mSemHandle);                }
            unsigned int getTimestamp() { return xTaskGetTickCountFromISR(); }
        /// @}
#endif

    private:
        bool mFileOpened;            ///< Set by constructor if logger file was opened successfully
#if LPC_LOGGER
        FIL mOutFile;                ///< The filehandle of the logger file
        xSemaphoreHandle mSemHandle; ///< Semaphore for the CSVLogger
#endif

        /// The buffer to log data until it gets full
        char mBuff[LOGGER_BUFFER_SIZE];

        /// Friend class used for Singleton Template
        friend class SingletonTemplate<FileLogger>;
};



#endif /* FILE_LOGGER_HPP__ */
