#ifndef __UTILS_H__
#define __UTILS_H__
#include <stdbool.h>
typedef union {
  int int_value;
  float float_value;
  char *string_value;
} answer_t;
typedef bool(*check_func)(char[], char *);
typedef answer_t(*convert_func)(char[]);
int read_string(char *buf, int buf_siz);
bool is_number(char *str, char *not_used);
bool is_number_item(char *str, char *not_used);
bool not_empty(char *str, char *not_used);
bool check_for(char *ord, char *valid_inputs);
answer_t ask_question(char *question, check_func check, convert_func convert, char *valid_inputs);
int ask_question_int(char *question);
char *ask_question_string(char *question);
char *ask_question_menu(char *question, char *valid_inputs);
char *ask_question_item(char *question);
void print(char *mening);
void println(char *mening);
#endif
