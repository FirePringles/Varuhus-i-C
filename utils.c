#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

bool not_empty(char *str, char *not_used)
{
  return strlen(str) > 0;
}

bool is_number(char *str, char *not_used)
{
  int length = strlen(str) - 1;
  if (length == 0)
    {
      if (!isdigit(str[0]))
        {
          return false;
        }
    }
  for (int i=0; i<=length; i++)
    {
      if (!isdigit(str[i]))
        {
          return false;
        }
    }
  return true;
}

bool is_number_item(char *str, char *not_used)
{
  if (!(strlen(str) == 3))
    {
      return false;
    }
  if (!isalpha(str[0]))
    {
      return false;
    }
  if (!isdigit(str[1]) || !isdigit(str[2]))
    {
      return false;
    }
  return true;
}

answer_t ask_question(char *question, check_func check, convert_func convert, char *valid_inputs)
{
  int buf_siz = 255;
  char buf[buf_siz];
  int read = 0;
  answer_t str;
  do
    {
    printf("%s \n", question);
    read = read_string(buf, buf_siz);

    if (check(buf, valid_inputs) == false)
      {
	printf("Bad input, try again \n");
	read = 0;
      }
    }
  while(read == 0);

  str = convert(buf);
    
    return str;
}

int ask_question_int(char *question)
{
  answer_t answer = ask_question(question, is_number, (convert_func) atoi, "whatever");
  return answer.int_value;
}

int read_string(char *buf, int buf_siz)
{
    int result = 0;
    do
    {
      fgets(buf,255,stdin);
      for (int i=0; i<buf_siz && buf[i] != '\n';i++)
      {
        result = result + 1;
      }
    }
    while (strlen(buf) == 0);

  if (buf[result] == '\n')
  {
   buf[result] = '\0';
  }

  if (result == buf_siz)
  {
    int c;
    do
    {
        c = getchar();
    }
    while (c != '\n');
    
    result = result - 1;
  }

  return result;
}

char *ask_question_string(char *question)
{
  return ask_question(question, not_empty, (convert_func) strdup, "whatever").string_value;
}

char *ask_question_item(char *question)
{
  return ask_question(question, is_number_item, (convert_func) strdup, "whatever").string_value;
}

bool check_for(char *ord, char *valid_inputs)
{
  if (strlen(ord) < 1 || strlen(ord) > 1)
    {
      return false;
    }
  char *p;
  p = strchr(valid_inputs ,ord[0]);
  if (p == NULL)
    {
      return false;
    }
  else
    {
      return true;
    }
}

char *ask_question_menu(char *question, char *valid_inputs)
{
  char *hello = ask_question(question, check_for, (convert_func) strdup, valid_inputs).string_value;
  return hello;
}
