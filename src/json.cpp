#include "json.h"
#include "printing.h"

char key[key_length_max + 1];
char value[value_length_max + 1];
uint8_t parse_errors;

void printJson(const char* key, uint32_t value)
{
 print("{\"");
 print(key);
 print("\":");
 my_printInt(value);
 print("}");
}

uint8_t parseByte(const char& byte)
{
  // format is : {"key":value}
  // no spaces
  // no nested items
  // numerical/string values only
  // double quotes only

  static uint8_t len = 0;
  static uint8_t state = wait_start1;

  switch (state) {
    case wait_start1:
      if (byte != '{') {
        parse_errors++;
      } else {
        state++;
      }
      break;

    case wait_start2:
      if (byte != '"') {
        parse_errors++;
        state = wait_start1;
      } else {
        len = 0;
        state++;
      }
      break;

    case wait_name1:
      if (byte == '"') { // TODO filter alphanumeric
        key[len] = '\0';
        state++;
      } else {
        if (len == key_length_max) {
          parse_errors++;
          state = wait_start1;
        } else {
          key[len++] = byte;
        }
      }
      break;

    case wait_name2:
      if (byte == ':') {
        len = 0;
        state++;
      } else {
        parse_errors++;
        state = wait_start1;
      }
      break;

    case wait_value:
      if (byte == '}') {
        state = wait_start1;
        value[len] = '\0';
        return new_message;
      } else {
        value[len++] = byte;
      }
      break;
  }
  return state;
}
