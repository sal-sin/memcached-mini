#ifndef LOGGER_H
#define LOGGER_H

#include "message.hpp"

class Logger
{
public:
    /**
     * @brief initiatize a Logger object and
     * configure with a boolean indicating
     * if logs should be printed to stdout
     *
     * @param[in] print_logs_bool indicates
     * if log should be printed to console
     */
    Logger(bool print_logs_bool);

    /**
     * @brief print log as per Logger configuration
     *
     * @param[in] log the string to be ouput
     */
    Logger operator<<(const std::string &log);

    /**
     * @brief print out a message
     *
     * @param[in] prompt A prompt to include with the message
     * @param[in] msg_p pointer to the message to display
     */
    void display_msg(std::string prompt, msg_t *msg_p);

private:
    /* Flag to be set in constructor */
    bool print_logs;
};

#endif