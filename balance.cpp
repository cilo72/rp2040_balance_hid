#include "balance.h"
#include <string.h>
#include <ctype.h>

Balance::Balance(cilo72::hw::Uart &uart)
    : uart_(uart), response_(new char[RESPONSE_MAX_LENGTH]), responseV_(new char *[RESPONSE_MAX_ARGS]), responseC_(0)
{
}

bool Balance::weightValueImmediately(char * weight)
{
    if (command("SIU", "S", 1000))
    {
        if(responseC_ == 4)
        {
            strcpy(weight, responseV_[2]);
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

uint32_t Balance::command(const char *command, const char *response, uint32_t timeout)
{
    static constexpr int RETRIES = 2;
    bool again = true;

    for (int i = 0; i < RETRIES and again; i++)
    {
        bool endFound = false;
        memset(response_, 0, RESPONSE_MAX_LENGTH);
        uart_.transmit(command);
        uart_.transmit("\n");

        uint32_t length = uart_.receive(response_, RESPONSE_MAX_LENGTH - 1, timeout, 100ul, [&](uint8_t *d, uint32_t l) -> bool
                                        { 
             if(l >= 2)
             {
                if(d[l-2] == '\r' and d[l-1] == '\n')
                {
                    endFound = true;
                    return false;
                }
                else
                {
                    return true;
                }
             }
             else
             {
                return true;
             } });

        if (length)
        {
            split(length);
            if (responseC_ >= 1)
            {
                if (strcmp(responseV_[0], response) == 0)
                {
                    if (responseC_ >= 2)
                    {
                        if (not(strcmp(responseV_[1], "I") == 0 or
                                strcmp(responseV_[1], "L") == 0 or
                                strcmp(responseV_[1], "+") == 0 or
                                strcmp(responseV_[1], "-") == 0))
                        {
                            return true;
                        }
                    }
                }
            }
        }
    }

    return false;
}

void Balance::split(uint32_t length)
{
    uint32_t i;
    int lastWasSpace;
    int isSpace;

    for (uint32_t i = 0; i < RESPONSE_MAX_ARGS; i++)
    {
        responseV_[i] = nullptr;
    }
    responseC_ = 0;

    lastWasSpace = 1;

    char *d = response_;

    for (i = 0; (i < length) && (d[i] != 0) && (responseC_ < RESPONSE_MAX_ARGS); i++)
    {
        isSpace = isspace(d[i]);
        if (lastWasSpace != 0 && isSpace == 0)
        {
            responseV_[responseC_] = d + i;
            responseC_++;
        }
        else if (lastWasSpace == 0 && isSpace != 0)
        {
            d[i] = 0;
        }
        lastWasSpace = isSpace;
    }
}