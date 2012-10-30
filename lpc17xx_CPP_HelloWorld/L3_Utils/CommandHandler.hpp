/**
 * @brief Provides command handling mapping with a function pointer as handler
 * @ingroup Utilities
 *
 * Version: 07262012    Removed "Version" Handler
 * Version: 06192012    Initial
 */
#ifndef COMMANDHANDLER_HPP_
#define COMMANDHANDLER_HPP_

#include "vector.hpp"
#include "str.hpp"



/**
 * Define the maximum length of a command
 * Command Processor will reject command if it is longer than this.
 * This is to limit the size of memory allocated under input/output strings (str class)
 */
#define MAX_CMD_LENGTH     128



/**
 * This is a function pointer to handle a command.  If the command handler was added
 * for "CPU" and a command is input to command handler as "CPU UTIL 5", then cmdParams
 * will contain "UTIL 5" which are additional parameters for this command.
 * @param cmdParams     The parameter string to your handler
 * @param output        The return text that your command needs to set
 * @param pDataParam    This is the same parameter passed when addHandler() is called
 * @param dataParamLen  The length of data associated with pDataParam
 * @returns One of the responses from CmdRspType
 */
typedef void (*CmdHandlerFuncPtr)(str& cmdParams, str& output, void* pDataParam, int dataParamLen);

/**
 * This macro can be used to declare and/or define a handler function.
 * @see Parameters of CmdHandlerFuncPtr()
 * Example:
 * @code
 * CMD_HANDLER_FUNC(myHandler);
 *
 * CMD_HANDLER_FUNC(myHandler)
 * {
 *    ...
 * }
 * @endcode
 */
#define CMD_HANDLER_FUNC(name) void name(str& cmdParams, str& output, void* pDataParam, int dataParamLen)




/**
 * Command Processor Class
 * @ingroup Utilities
 *
 * This class allows users to add a command string and associated handler for commands.
 * When user inputs a command, it will call the mapped handler.
 * Note that command input is capitalized to make this class case insensitive.
 *
 * One handler is already part of this class:
 *   - "HELP"   : Get list of supported commands
 *
 * The "HELP" handler lists out supported commands,
 * "HELP <command>" will show the help-text associated with the given command.
 *
 * Example Usage:
 * @code
 *      CMD_HANDLER_FUNC(Handler1)
 *      {
 *          // Process the command ...
 *          output = "OK";
 *      }
 *
 *      CMD_HANDLER_FUNC(Handler2)
 *      {
 *          output = "WHOS_THERE?";
 *      }
 *
 *      CommandProcessor cp;
 *      cp.addHandler(Handler1, "MYCMD",       "My Cmd Help");
 *      cp.addHandler(Handler2, "KNOCK_KNOCK", "Returns WHOS_THERE?");
 *
 *      // Now we can process commands:
 *      puts(cp.handleCommand("MYCMD"));       // This will return "OK"
 *      puts(cp.handleCommand("KNOCK_KNOCK")); // This will return "WHOS_THERE?"
 * @endcode
 */
class CommandProcessor
{
    public:
        /**
         * Constructor
         * @param numCmds Optional: Initial list of vector size to avoid memory re-allocation
         * @note addHandler() will grow the vector of command handlers if more commands are added later
         */
        CommandProcessor(int numCmds=8) :
            mCmdHandlerVector(numCmds), mInputStr(MAX_CMD_LENGTH), mOutputStr(MAX_CMD_LENGTH)
        {
        }

        /**
         * Adds a command to the command handler list
         * @param pFunc                 The function pointer of the handler of this command
         * @param pPersistantCmdStr     The persistent data pointer of a command's text
         * @param pPersistentCmdHelpStr The persistent data pointer of this command's help text
         * @param pDataParam            Optional Param: The data parameter pointer to pass to your handler when it gets called
         * @param dataParamLen          Optional Param: The length of the data associated with pDataParam
         * @warning pPersistentCmdStr and pPersistentCmdHelp must always exist in memory without going out of scope because
         *          these strings are not copied internally but their pointer is referenced during comparison
         * @note command is matched while ignoring case.
         */
        void addHandler(CmdHandlerFuncPtr pFunc, const char* pPersistantCmdStr,
                        const char* pPersistentCmdHelpStr=0, void* pDataParam=0, int dataParamLen=0);

        /**
         * @{ Command handling functions
         * Handles an incoming command and @returns null terminated string of the command output
         * @param cmd  Input command either as char* pointer or str object
         * @returns    char* pointer containing the command's response
         * @note "HELP" command will output the list of supported commands.
         */
        const char* handleCommand(const char* cmd);
        const char* handleCommand(str& cmd) { return handleCommand(cmd()); }
        /** @} */

    private:
        /// Structure of a Handler
        typedef struct
        {
            char* pCommandStr;       ///< Pointer to the command text
            char* pCmdHelpText;      ///< Pointer to the command's help
            CmdHandlerFuncPtr pFunc; ///< Pointer to the function pointer handler
            void* pDataParam;        ///< Pointer to the data that should be passed as void pointer to pFunc
            int   dataParamLen;      ///< The size of the data pointed by pData
        } CmdProcessorType;

        VECTOR<CmdProcessorType> mCmdHandlerVector; ///< Vector of the command handlers
        str mInputStr;  ///< input of a command is copied to this string
        str mOutputStr; ///< output of a command is copied to this string


        /// Handles a command stored at input and stores output in output object
        void handleCmd(str& input, str& output);

        /// Gets a list of all registered commands which is used by the "HELP" command
        void getRegisteredCommandList(str& output);

        /**
         * Gets output of a "HELP" command.  If helpForCmd is an empty string, then
         * the list of commands is returned, if it is a command name itself, then
         * the help of THAT command's text is returned.
         * @param helpForCmd The name of the command, or null/empty for top level "HELP"
         * @param output     The output text of the command's help is stored here
         */
        void getHelpText(str& helpForCmd, str& output);

        /// Removes null terminated pCmdToRemove from the "input" str
        void pointToParameters(str& input, const char* pCmdToRemove);
};

#endif /* COMMANDHANDLER_HPP_ */
