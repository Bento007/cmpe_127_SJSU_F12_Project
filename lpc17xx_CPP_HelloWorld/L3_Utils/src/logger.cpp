#include "logger.hpp"
#include <stdlib.h>   // itoa()
#include <stdio.h>    // sprintf()
#include <string.h>   // strlen()




void LoggerBase::flush()
{
    if(mBuffUsage > 0)
    {
        handleCompletedBuffer(mpBuffPtr, mBuffUsage);
        mBuffUsage = 0;
    }

    // Reset our buffer and zero it out.
    // memset(mpBuffPtr, 0, mBuffPtrSize);
}
void LoggerBase::log(const char* pMessage)
{
    // while not enough room to store the message, keep flushing the buffer
    while(mBuffUsage+strlen(pMessage) > mBuffPtrSize)
    {
        const int lenOfMsg = strlen(pMessage);
        const int availableMem = mBuffPtrSize - mBuffUsage;
        const int lenToCopy = lenOfMsg < availableMem ? lenOfMsg : availableMem;

        memcpy(mpBuffPtr+mBuffUsage, pMessage, lenToCopy);
        mBuffUsage += lenToCopy;
        flush();

        pMessage += lenToCopy;
    }

    const int lenOfMsg = strlen(pMessage);
    memcpy(mpBuffPtr+mBuffUsage, pMessage, lenOfMsg);
    mBuffUsage += lenOfMsg;
}





void CSVLogger::logError(const char* pMessage, const char* pFilename, const char* pFuncName, int lineNum)
{
    logCsvMessage("ERROR", pMessage, pFilename, pFuncName, lineNum);
}
void CSVLogger::logWarning(const char* pMessage, const char* pFilename, const char* pFuncName, int lineNum)
{
    logCsvMessage("WARN", pMessage, pFilename, pFuncName, lineNum);
}
void CSVLogger::logInfo(const char* pMessage, const char* pFilename, const char* pFuncName, int lineNum)
{
    logCsvMessage("INFO", pMessage, pFilename, pFuncName, lineNum);
}

void CSVLogger::logCsvMessage(const char* pInfoType, const char* pMessage,
                              const char* pFilename, const char* pFuncName, int lineNum)
{
    semTake();
    {
        const char comma[] = ",";
        const char parens[] = "()";
        const char newline[] = "\n";

        char numStr[16];
        sprintf(numStr, "%i", getTimestamp());
        log(numStr);
        log(comma);

        log(pInfoType);
        log(comma);
        log(pMessage);
        log(comma);

        // Find the back-slash or forward-slash to get filename only, not absolute or relative path
        if(0 != pFilename) {
            char* pSlash = strrchr(pFilename, '/');
            // If forward-slash not found, find back-slash
            if(0 == pSlash) pSlash = strrchr(pFilename, '\\');
            if(0 != pSlash) pFilename = pSlash+1;

            log(pFilename);
        }
        log(comma);

        if(0 != pFuncName) {
            log(pFuncName);
            log(parens);
        }
        log(comma);

        if(0 != lineNum) {
            sprintf(numStr, "%i", lineNum);
            log(numStr);
        }
        log(newline);
    }
    semGive();
}
