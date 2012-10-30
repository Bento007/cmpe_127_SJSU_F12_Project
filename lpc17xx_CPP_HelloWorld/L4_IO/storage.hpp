/**
 * @file storage.hpp
 */
#ifndef STORAGE_HPP__
#define STORAGE_HPP__

#include "src/FileSystemObject.hpp"



/**
 * Storage class contains the File System Objects
 *
 * This simply gives back the FileSystemObject in a controlled way without
 * allowing the user to create FileSystemObjects.
 *
 * The user can mount the storage medium, format it, and get free space
 * using the FileSystemObjects returned from this class.
 *
 * @ingroup BoardIO
 */
class Storage
{
    public:
        /// @returns Single Flash Drive Object Reference
        static FileSystemObject& getFlashDrive()
        {
            static FileSystemObject* pFlashDrive = new FileSystemObject(flashDriveNum);
            return *pFlashDrive;
        }

        /// @returns Single SD Card Drive Object reference
        static FileSystemObject& getSDDrive()
        {
            static FileSystemObject* pSDCardDrive = new FileSystemObject(sdDriveNum);
            return *pSDCardDrive;
        }

        /**
         * Copies a file
         * @param pExistingFile  Existing file name
         * @param pNewFile       New file name
         * @param pReadTime         Optional: Provide pointer to get the time taken to read the file
         * @param pWriteTime        Optional: Provide pointer to get the time taken to write the file
         * @param pBytesTransferred Optional: Provide pointer to get number of bytes transferred
         */
        static FRESULT copy(const char* pExistingFile, const char* pNewFile,
                            unsigned int* pReadTime=0, unsigned int* pWriteTime=0,
                            unsigned int* pBytesTransferred=0)
        {
            FRESULT status;
            FIL srcFile;
            FIL dstFile;
            unsigned int readTimeMs = 0;
            unsigned int writeTimeMs = 0;

            // Open Existing file
            if (FR_OK != (status = f_open(&srcFile, pExistingFile, FA_OPEN_EXISTING | FA_READ))) {
                return status;
            }

            // Open new file - overwrite if one exists
            if (FR_OK != (status = f_open(&dstFile, pNewFile, FA_CREATE_ALWAYS | FA_WRITE))) {
                f_close(&srcFile);
                return status;
            }

            char buffer[1024];
            unsigned int bytesRead = 0;
            unsigned int bytesWritten = 0;
            unsigned int totalBytesTransferred = 0;

            for (;;)
            {
                unsigned int startTime = xTaskGetTickCount();
                if(FR_OK != (status = f_read(&srcFile, buffer, sizeof(buffer), &bytesRead)) ||
                   0 == bytesRead) {
                    break;
                }
                readTimeMs += xTaskGetTickCount() - startTime;

                startTime = xTaskGetTickCount();
                if(FR_OK != (status = f_write(&dstFile, buffer, bytesRead, &bytesWritten)) ||
                   bytesWritten != bytesRead) {
                    break;
                }
                writeTimeMs += xTaskGetTickCount() - startTime;

                totalBytesTransferred += bytesRead;
            }

            if(0 != pReadTime) {
                *pReadTime = readTimeMs;
            }
            if(0 != pWriteTime) {
                *pWriteTime = writeTimeMs;
            }
            if(0 != pBytesTransferred) {
                *pBytesTransferred = totalBytesTransferred;
            }

            f_close(&srcFile);
            f_close(&dstFile);

            return status;
        }

    private:
        /// Private constructor to restrict object creation
        Storage() {}
};


#endif /* STORAGE_HPP__ */
