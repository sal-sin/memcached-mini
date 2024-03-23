/**
 * @file /src/hash/hash.hpp
 *
 * @brief This file contains utilities to get the hash value of
 * a string or int
 */

#include <iostream>

/**
 * Given a string, return its hash value
 *
 * @param[in] str Input string
 * @return the hash value
 */
unsigned int getHash(std::string str);

/**
 * Given an integer, return its hash value
 *
 * @param[in] num Input number
 * @return the hash value
 */
unsigned int getHash(int num);