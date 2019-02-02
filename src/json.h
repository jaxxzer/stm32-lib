#ifndef AFDDSSSS
#define AFDDSSSS
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const uint8_t key_length_max = 16; // ascii
static const uint8_t value_length_max = 16; // ascii
extern char key[];
extern char value[];
extern uint8_t parse_errors;
enum {
  new_message,
  wait_start1, // {"                  // wait for {"
  wait_start2,
  wait_name1, // a-Z0-9":             // wait for ":
  wait_name2,
  wait_value, // <number or string>} // wait for }
};

void printJson(const char* key, uint32_t value);

uint8_t parseByte(const char& byte);
#endif