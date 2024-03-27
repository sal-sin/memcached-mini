#include <iostream>
#include "message.hpp"
#include "logger.hpp"

/**
 * @brief return a string describing a message type
 *
 * @param type `msg_type_t` type of the message
 *
 * @return a string describing the type
 */
std::string mtype_to_str(msg_type_t type)
{
    switch (type)
    {
    case req_get_t:
        return "Get Request";
    case req_put_t:
        return "Put Request";
    case resp_ack_t:
        return "Ack Response";
    case resp_hit_t:
        return "Hit Response";
    case resp_miss_t:
        return "Miss Response";
    default:
        return "Invalid Type";
    }
}

/**
 * @brief initiatize a Logger object and
 * configure with a boolean indicating
 * if logs should be printed to stdout
 *
 * @param[in] print_logs_bool indicates
 * if log should be printed to console
 */
Logger::Logger(bool print_logs_bool)
{
    print_logs = print_logs_bool;
};

/**
 * @brief print log as per Logger configuration
 *
 * @param[in] log the string to be ouput
 */
Logger Logger::operator<<(const std::string &log)
{
    if (print_logs)
    {
        std::cout << log;
    }
    return *this;
}

/**
 * @brief print out a message
 *
 * @param[in] prompt A prompt to include with the message
 * @param[in] msg_p pointer to the message to display
 */
void Logger::display_msg(std::string prompt, msg_t *msg_p)
{
    if (print_logs)
    {
        printf("\n%s:\n", prompt.c_str());
        printf("\tType: %s | Key: \"%s\" | Value: \"%s\"\n",
               mtype_to_str(msg_p->type).c_str(), msg_p->key, msg_p->value);
    }
}