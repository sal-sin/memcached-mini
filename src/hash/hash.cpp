/**
 * @file /src/hash/hash.cpp
 *
 * @brief Implements functions provided in /src/hash/hash.hpp
 */

#include <iostream>
#include "hash.hpp"

unsigned int get_hash(std::string s)
{
    std::hash<std::string> str_hash_func;
    unsigned int hash_val = str_hash_func(s);
    return hash_val;
}

unsigned int get_hash(int num)
{
    std::hash<std::string> str_hash_func;
    std::string s = std::to_string(num);
    unsigned int hash_val = str_hash_func(s);
    return hash_val;
}