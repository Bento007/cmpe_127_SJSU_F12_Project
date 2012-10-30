#include <stdio.h>      // printf

#include "CommandHandler.hpp"   // Terminal's Command Handler
#include "str.hpp"              // str class

#include "io_functions.h"       // stdio set IO functions
#include "uart0.hpp"            // Interrupt driven UART0 driver
#include "utilities.h"          // PRINT_EXECUTION_SPEED()
#include "handlers.hpp"         // Command-line handlers
#include "io.hpp"               // LED Display API

/**
 * Gets a line of input with backspace support and stores into str s
 * @param maxLen Maximum chars to store into s before leaving this function.
 */
void getLine(str& s, const int maxLen);


void switchled(void* p)
{
    char ledNum = 0;
    while(1)
    {
        vTaskDelay(100);
        LE.setAll(SW.getSwitchValues());

        if(SW.getSwitch(1)) {
            LD.setNumber((AS.getX()+1024)/256);
        }
        if(SW.getSwitch(2)) {
            LD.setNumber((AS.getY()+1024)/256);
        }
        if(SW.getSwitch(3)) {
            LD.setNumber((AS.getZ()+1024)/256);
        }
        if(SW.getSwitch(5)) {
            puts(LS.getValueAsString());
        }
        if(SW.getSwitch(6)) {
            puts(TS.getValueAsString());
            LD.setNumber(TS.getFarenheit());
        }
        if(SW.getSwitch(7)) {
            ledNum++;
            LD.setNumber(ledNum);
        }
        if(SW.getSwitch(8)) {
            ledNum--;
            LD.setNumber(ledNum);
        }
        if(ledNum > 99) {
            ledNum = 0;
        }

        if(IS.isIRCodeReceived())
        {
            unsigned int irCode = IS.getLastIRCode();
            LE.setAll(irCode);
            printf("IR Code: 0x%X\n", irCode);
            vTaskDelay(100);
        }
    }
}

void terminalTask(void* p)
{
    // Initialize Interrupt driven version of getchar & putchar
    UART0& uart0 = UART0::getInstance();
    uart0.init(38400, 32, 256);
    stdio_SetInputCharFunction(uart0.getcharIntrDriven);
    stdio_SetOutputCharFunction(uart0.putcharIntrDriven);

    CommandProcessor cmdProcessor;  // Command processor to process command-line commands
    str input(128);                 // string with 128 byte initial length

    // Add command handlers:
    cmdProcessor.addHandler(taskListHandler, "Info",   "Task/CPU Info.  Use 'Info 200' to get CPU during 200ms");
    cmdProcessor.addHandler(memInfoHandler, "meminfo", "Show System Memory Info");
    cmdProcessor.addHandler(timeHandler, "time",       "Use 'time get' to view time, 'time set MM DD YYYY HH MM SS' to set time");
    cmdProcessor.addHandler(loggerTest, "log",         "Use 'log info', 'log warn', 'log error', 'log flush' for demo");
    // File I/O Handlers:
    cmdProcessor.addHandler(copyHandler, "copy",       "Copy files from/to Flash/SD Card.  Ex: 'copy 0:file.txt 1:file.txt'");
    cmdProcessor.addHandler(lsHandler,   "ls",         "Use 'ls 0:' for Flash, or 'ls 1:' for SD Card");
    cmdProcessor.addHandler(readHandler, "read",       "Read a file.  Ex: 'read 0:file.txt' or 'read 0:file.txt -print' to print to screen");
    cmdProcessor.addHandler(rmHandler,   "rm",         "Remove a file. Ex: 'rm 0:file.txt'");

    puts(cmdProcessor.handleCommand("help"));     // Print list of all commands

    // Process commands forever
    char cmdNum = 0;
    while (1)
    {
        printf("LPC: ");

        // Clear the input str and get new line of input from user terminal
        input.clear();
        getLine(input, input.getCapacity());
        LD.setNumber(++cmdNum%100);

        // If the user did not press enter key, getLen() will be greater than 0
        if (input.getLen() > 0)
        {
            PRINT_EXECUTION_SPEED()
            {
                puts(cmdProcessor.handleCommand(input));
            }
        }
    }
}

void getLine(str& s, const int maxLen)
{
    char c = 0;
    do
    {
        c = getchar();

        switch (c)
        {
            // Backspace 1 char @ terminal and erase last char of string
            case '\b':
                if (s.getLen() > 0)
                {
                    printf("\b \b");
                    s.eraseLast(1);
                }
                else {
                    // Send "Alert" sound to terminal because we can't backspace
                    char bellSound = '\a';
                    putchar(bellSound);
                }
                break;

            // Output these chars but ignore them otherwise
            case '\r':
            case '\n':
                putchar(c);
                break;

            // Store all other characters
            default:
                s += c;
                putchar(c);
                break;
        }
    } while (c != '\n' && s.getLen() < maxLen);
}
