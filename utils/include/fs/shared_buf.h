#pragma once

#include <string>
#include <thread>
#include <vector>

#include "fs/shared_fd.h"

namespace redroid {

/**
 * Reads from fd until it is closed or errors, storing all data in buf.
 *
 * On a successful read, returns the number of bytes read.
 *
 * If a read error is encountered, returns -1. buf will contain any data read
 * up until that point and errno will be set.
 */
ssize_t ReadAll(SharedFD fd, std::string* buf);

/**
 * Reads from fd until reading buf->size() bytes or errors.
 *
 * On a successful read, returns buf->size().
 *
 * If a read error is encountered, returns -1. buf will contain any data read
 * up until that point and errno will be set.
 */
ssize_t ReadExact(SharedFD fd, std::string* buf);

/**
 * Reads from fd until reading buf->size() bytes or errors.
 *
 * On a successful read, returns buf->size().
 *
 * If a read error is encountered, returns -1. buf will contain any data read
 * up until that point and errno will be set.
 */
ssize_t ReadExact(SharedFD fd, std::vector<char>* buf);

/**
 * Reads from fd until reading `size` bytes or errors.
 *
 * On a successful read, returns buf->size().
 *
 * If a read error is encountered, returns -1. buf will contain any data read
 * up until that point and errno will be set.
 */
ssize_t ReadExact(SharedFD fd, char* buf, size_t size);

/*
 * Reads from fd until reading `sizeof(T)` bytes or errors.
 *
 * On a successful read, returns `sizeof(T)`.
 *
 * If a read error is encountered, returns -1. buf will contain any data read
 * up until that point and errno will be set.
 */
template<typename T>
ssize_t ReadExactBinary(SharedFD fd, T* binary_data) {
  return ReadExact(fd, (char*) binary_data, sizeof(*binary_data));
}

/**
 * Writes to fd until writing all bytes in buf.
 *
 * On a successful write, returns buf.size().
 *
 * If a write error is encountered, returns -1. Some data may have already been
 * written to fd at that point.
 */
ssize_t WriteAll(SharedFD fd, const std::string& buf);

/**
 * Writes to fd until writing all bytes in buf.
 *
 * On a successful write, returns buf.size().
 *
 * If a write error is encountered, returns -1. Some data may have already been
 * written to fd at that point.
 */
ssize_t WriteAll(SharedFD fd, const std::vector<char>& buf);

/**
 * Writes to fd until `size` bytes are written from `buf`.
 *
 * On a successful write, returns `size`.
 *
 * If a write error is encountered, returns -1. Some data may have already been
 * written to fd at that point.
 */
ssize_t WriteAll(SharedFD fd, const char* buf, size_t size);

/**
 * Writes to fd until `sizeof(T)` bytes are written from binary_data.
 *
 * On a successful write, returns `sizeof(T)`.
 *
 * If a write error is encountered, returns -1. Some data may have already been
 * written to fd at that point.
 */
template<typename T>
ssize_t WriteAllBinary(SharedFD fd, const T* binary_data) {
  return WriteAll(fd, (const char*) binary_data, sizeof(*binary_data));
}

/**
 * Sends contents of msg through sock, checking for socket error conditions
 *
 * On successful Send, returns true
 *
 * If a Send error is encountered, returns false. Some data may have already
 * been written to 'sock' at that point.
 */
bool SendAll(SharedFD sock, const std::string& msg);

/**
 * Receives 'count' bytes from sock, checking for socket error conditions
 *
 * On successful Recv, returns a string containing the received data
 *
 * If a Recv error is encountered, returns the empty string
 */
std::string RecvAll(SharedFD sock, const size_t count);

} // namespace redroid
