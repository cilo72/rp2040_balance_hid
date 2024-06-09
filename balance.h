#pragma once

#include "cilo72/hw/uart.h"

class Balance
{
public:
    Balance(cilo72::hw::Uart &uart);
    bool weightValueImmediately(char * weight);
private:
    static constexpr int RESPONSE_MAX_LENGTH = 100;
    static constexpr int RESPONSE_MAX_ARGS = 10;
    cilo72::hw::Uart &uart_;
    char *response_;
    char **responseV_;
    uint32_t responseC_;

    uint32_t command(const char *command, const char *response, uint32_t timeout = 10000);
    void split(uint32_t length);
};