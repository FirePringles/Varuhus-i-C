#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// Static list of prime numbers used to set how many buckets the hash table should increase to.
static int hash_primtal[] = {17,31,67,127,257,509,1021,2053,4099,8191,16381,24593,49157,98317,196613,393241};
static size_t primtal_length = 16;

typedef union elem elem_t;
union elem
{
  int  i;
  unsigned int u;
  float f;
  bool b;
  void *p;
};

typedef bool(*ioopm_eq_function)(elem_t a, elem_t b);

typedef bool(*ioopm_predicate)(elem_t key, elem_t value, elem_t extra, ioopm_eq_function func);

typedef void(*ioopm_apply_function)(elem_t key, elem_t *value, elem_t extra);

typedef unsigned long(*ioopm_hash_function)(elem_t key);

// 5 functions that check equality of the different types in the elem_t union,
// more is needed if you're using pointers to other things than strings
static bool compare_int_element(elem_t a, elem_t b)
{
return a.i == b.i;
}

static bool compare_unsigned_element(elem_t a, elem_t b)
{
return a.u == b.u;
}

static bool compare_float_element(elem_t a, elem_t b)
{
return a.f == b.f;
}

static bool compare_bool_element(elem_t a, elem_t b)
{
return a.b == b.b;
}

static bool compare_string_element(elem_t a, elem_t b)
{
char *ptr = a.p;
char *ptr2 = b.p;
return !strcmp(ptr,ptr2);
}


typedef struct list ioopm_list_t;
typedef struct link link_t;
struct link
{
  elem_t element;
  link_t *next;
};

struct list
{
  link_t *first;
  link_t *last;
  ioopm_eq_function eq_func;
};

struct iter
{
  link_t *current;
  ioopm_list_t *list;
};

typedef struct entry entry_t;
typedef struct hash_table ioopm_hash_table_t;
struct entry
{
  elem_t key;      
  elem_t value;   
  entry_t *next; 
};

struct hash_table
{
  entry_t *buckets;
  ioopm_hash_function hash_func;
  ioopm_eq_function key_eq_func;
  ioopm_eq_function value_eq_func;
  float load_factor;
  size_t num_buckets;
  size_t num_entries;
};

typedef struct shelf shelf_t;
typedef struct merch merch_t;
typedef struct webstore_backend webstore_backend_t;

struct shelf
{
  char *shelf_num;
  int amount;
};

//linked list contains shelf_t's
struct merch
{
  char *name;
  char *desc;
  int price;
  ioopm_list_t *list;
};

// ht_merch keys are names of merches and the values are the merches themselves
// ht_shelf keys are shelf names e.g "A01" and values are merch names
// ht_carts keys are integers which increase with cart_num everytime a new cart is
// added. Values are hash_tables whos keys are merch names and values are int amount of merches in the cart
struct webstore_backend
{
  ioopm_hash_table_t *ht_merch;
  ioopm_hash_table_t *ht_shelf;
  ioopm_hash_table_t *ht_carts;
  int cart_num;
};

// Hashing functions for each of the five element in elem_t,
// if you're using void * for something else than strings you'll need a new hashing functions that works with that type.
static unsigned long int_hash(elem_t key)
{
  return abs(key.i);
}

static unsigned long unsigned_hash(elem_t key)
{
  return key.u;
}

static unsigned long float_hash(elem_t key)
{
  int x = (int)key.f;
  return abs(x);
}

static unsigned long bool_hash(elem_t key)
{
  return key.b;
}

static unsigned long string_hash(elem_t key)
{
  unsigned long result = 0;
  char *str = key.p;
  do
    {
      result = result * 31 + *str;
    }
  while (*++str != '\0');
  return result;
}

#endif
