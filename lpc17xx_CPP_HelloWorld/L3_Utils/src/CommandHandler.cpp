#include "commandHandler.hpp"
#include <string.h> // strlen()


// Define the strings returned for OK, ERROR, Invalid, and other cases:
#define HELP_STR                "HELP" // This must be capital to avoid case insensitive comparison
// Cases don't matter for next strings since returned data will be capitalized
#define NO_HELP_STR             "Help not specified for this command"
#define CMD_INVALID_STR         "Command Invalid"
#define SUPPORTED_COMMANDS_STR  "Supported Commands:"
#define CMD_LEN_ERR_STR         "Command Length Too Large"



void CommandProcessor::addHandler(CmdHandlerFuncPtr pFunc, const char* pPersistantCmdStr, const char* pPersistentCmdHelpStr,
                                  void* pDataParam, int dataParamLen)
{
    CmdProcessorType handler;
    handler.pCommandStr  = (char*)pPersistantCmdStr;
    handler.pCmdHelpText = (char*)pPersistentCmdHelpStr;
    handler.pFunc = pFunc;
    handler.pDataParam = pDataParam;
    handler.dataParamLen = dataParamLen;

    mCmdHandlerVector += handler;
}

const char* CommandProcessor::handleCommand(const char* pCmdStr)
{
    if(strlen(pCmdStr) >= MAX_CMD_LENGTH) {
        mOutputStr = CMD_LEN_ERR_STR;
    }
    else {
        // Copy command to str object, upper-case it and process it
        mInputStr = pCmdStr;
        handleCmd(mInputStr, mOutputStr);
    }

    return mOutputStr();
}






void CommandProcessor::handleCmd(str& input, str& output)
{
    // Note: HELP command cannot simply have a handler because this static handler
    //       will not be able to access the vector of commands
    if(input.beginsWithWholeWordIgnoreCase(HELP_STR))
    {
        pointToParameters(input, HELP_STR);
        getHelpText(input, output);
    }
    else
    {
        unsigned int i=0;
        for(i=0; i < mCmdHandlerVector.size(); i++)
        {
            CmdProcessorType &cp = mCmdHandlerVector[i];

            // If a command matches, return the response from the attached function pointer
            if(input.beginsWithWholeWordIgnoreCase(cp.pCommandStr))
            {
                pointToParameters(input, cp.pCommandStr);
                output.clear();
                cp.pFunc(input, output, cp.pDataParam, cp.dataParamLen);
                break;
            }
        }
        if(i >= mCmdHandlerVector.size()) {
            output = CMD_INVALID_STR;
        }
    }
}

void CommandProcessor::getRegisteredCommandList(str& output)
{
    output = SUPPORTED_COMMANDS_STR;
    for(unsigned int i=0; i<mCmdHandlerVector.size(); i++)
    {
        output += "\n  *  ";
        output += mCmdHandlerVector[i].pCommandStr;
    }
}

void CommandProcessor::getHelpText(str& helpForCmd, str& output)
{
    // If there is a parameter, get help for this specific command
    // where this parameter itself is a command name
    if(helpForCmd.getLen() > 0)
    {
        output = CMD_INVALID_STR;
        for(unsigned int i=0; i < mCmdHandlerVector.size(); i++)
        {
            CmdProcessorType &cp = mCmdHandlerVector[i];
            if(helpForCmd.compareToIgnoreCase(cp.pCommandStr))
            {
                output = (0 == cp.pCmdHelpText || '\0' == cp.pCmdHelpText[0]) ? NO_HELP_STR : cp.pCmdHelpText;
                break;
            }
        }
    }
    else {
        getRegisteredCommandList(output);
    }
}

void CommandProcessor::pointToParameters(str& input, const char* pCmdToRemove)
{
    // There may be a space, so need to copy from after the space if it exists
    const int cmdLen = strlen(pCmdToRemove);
    input.eraseFirst( input.getLen() > cmdLen ? cmdLen+1 : cmdLen);
}
