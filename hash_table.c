#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "hash_table.h"

// @brief check if a key is equal to another key
// @param key key to compare
// @param value_ignored value which is not used
// @param x key to compare
// @param key_eq_func equality function based on the type of the key
// @return true if key==x else false
static bool key_equiv(elem_t key, elem_t value_ignored, elem_t x, ioopm_eq_function key_eq_func)
{
  return key_eq_func(key, x);
}

// @brief check if a value is equal to another value
// @param key_ignored key which is not used
// @param value value to compare
// @param x value to compare
// @param value_eq_func equality function based on the type of the value
// @return true if value==x else false
static bool value_equiv(elem_t key_ignored, elem_t value, elem_t x, ioopm_eq_function value_eq_func)
{
  return value_eq_func(value,x);
}

// @brief find the entry whose hash_func(key) is closest to, but smaller, than the key we send in
// @param entry the dummy entry
// @param key the key we want to find a previous entry for
// @param hash_func the hash_function that works with the type of the keys
// @return the entry_t whose hash_func(key) was closest but smaller than the hash_func(key) we sent in
static entry_t *find_previous_entry_for_key(entry_t *entry, elem_t key, ioopm_hash_function hash_func)
{
  if (entry->next == NULL)
    {
      return entry;
    }
  entry_t *previous_entry = entry;
  entry = entry->next;
  while (hash_func(entry->key) < hash_func(key)  &&  entry->next != NULL)
    {                                               
      entry = entry->next;
      previous_entry = previous_entry->next;
    }
  if (entry->next == NULL && hash_func(entry->key) < hash_func(key)) 
    {
      previous_entry = previous_entry->next;
    }
  return previous_entry;
}

// @brief create a new entry_t
// @param key key for the new entry 
// @param value value for the new entry
// @param entry the entry that the created entry is going to point to (may be NULL)
// @return a new entry
static entry_t *entry_create(elem_t key, elem_t value, entry_t *entry)
{
  entry_t *new_entry = calloc(1, sizeof(entry_t));
  new_entry->key = key;
  new_entry->value = value;
  new_entry->next = entry; 
  return new_entry;
}

// @brief frees the allocated memory for an entry
// @param entry entry whose memory will be free'd
static void entry_destroy(entry_t *entry)
{
  free(entry);
}

// @breif calculates the total number of entrys in a single bucket of a hash table
// @param entry the first entry in the bucket, the dummy
// @return the total number of entrys after the dummy (not including the dummy)
static size_t number_of_entries_in_bucket(entry_t *entry)
{
  size_t counter = 0;
  while (entry->next != NULL)
    {
      entry = entry->next;
      ++counter;
    }
  return counter;
}

// @brief finds the next number in the hash_primtal table to which the ht operated upon should increase it's buckets to
// @param ht the hash table operated upon
// @return the new number of buckets the hash_table should have.
static int ht_new_size(ioopm_hash_table_t *ht)
{
  for (int i = 0; i < primtal_length; ++i)
    {
      if (ht->num_buckets == hash_primtal[i])
        {
          return hash_primtal[i+1];
        }
    }
  return hash_primtal[primtal_length];
}

// @brief rehashes every element in the hash table to be accurate with the new number of buckets
// @param ht the hash table operated upon
// @param old_size the number of buckets the hash_table previously had
static void ht_rehash(ioopm_hash_table_t *ht, int old_size)
{
  for (int i = 0; i < old_size; ++i)
    {
      entry_t *previous = &ht->buckets[i];
      while(previous->next != NULL)
        {
          entry_t *current = previous->next;
          elem_t key = current->key;
          elem_t value = current->value;
          previous->next = current->next;
          free(current);
          --ht->num_entries;
          if (previous->next == NULL)
            {
              ioopm_hash_table_insert(ht, key, value);
              break;
            }
          if (ht->hash_func(key) % ht->num_buckets == (unsigned long)i)
            {
              ioopm_hash_table_insert(ht, key, value);
              previous = previous->next;
            }
          else
            {
              ioopm_hash_table_insert(ht, key, value);    
            }
        }
    }
}

// @brief reallocates memory for the hash table to include a larger number of buckets
// Then rehashes all entries to be accurate with the new bucket count
// @param ht the hash table to be operated upon
static void ht_resize(ioopm_hash_table_t *ht)
{
  int new_size = ht_new_size(ht);
  int old_size = ht->num_buckets;
  ht->buckets = realloc(ht->buckets, new_size*sizeof(entry_t));
  ht->num_buckets = new_size;
  for(int i = old_size; i < new_size; ++i)
    {
      ht->buckets[i] = (entry_t){.key = (elem_t)NULL, .value = (elem_t)NULL, .next = NULL};
}
  ht_rehash(ht, old_size);
}

/// @brief Creates a new hash hash table with a desired number of buckets and load_factor
/// @param load_factor the desired load factor for the hash table
/// @param num_buckets the desired number of buckets for the hash_table
/// @param hash_func a hash function that operates on the type that the keys have
/// @param key_eq_func an equal function that operates on the type that the keys have
/// @param value_eq_func an equal function that operates on the type that the values have
/// @return a new empty hash table
static ioopm_hash_table_t *ioopm_hash_table_create_dynamic(float load_factor, size_t num_buckets, ioopm_hash_function hash_func, ioopm_eq_function key_eq_func, ioopm_eq_function value_eq_func)
{
  ioopm_hash_table_t *ht = calloc(1, sizeof(ioopm_hash_table_t));
  ht->hash_func = hash_func;
  ht->key_eq_func = key_eq_func;
  ht->value_eq_func = value_eq_func;
  ht->num_buckets= num_buckets;
  ht->load_factor = load_factor;
  ht->num_entries = 0;
  ht->buckets = calloc(1, num_buckets*sizeof(entry_t));
  return ht;
}

ioopm_hash_table_t *ioopm_hash_table_create(ioopm_hash_function hash_func, ioopm_eq_function key_eq_func, ioopm_eq_function value_eq_func)
{
  return ioopm_hash_table_create_dynamic(0.75, hash_primtal[0], hash_func, key_eq_func, value_eq_func);
}

void ioopm_hash_table_destroy(ioopm_hash_table_t *ht)
{
  ioopm_hash_table_clear(ht); 
  free(ht->buckets);
  free(ht);
}

void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value)
{
  if (ht->num_entries > ht->load_factor * ht->num_buckets)
    {
      ht_resize(ht);
    }
  int bucket = ht->hash_func(key) % ht->num_buckets;
  entry_t *entry = find_previous_entry_for_key(&ht->buckets[bucket], key, ht->hash_func);
  entry_t *next = entry->next;

  if (next != NULL && ht->hash_func(next->key) == ht->hash_func(key))
    {
      next->value = value;
    }
  else
    {
      entry->next = entry_create(key, value, next);
      ++ht->num_entries;
    }
}


option_t ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key)
{
  entry_t *tmp = find_previous_entry_for_key(&ht->buckets[ht->hash_func(key) % ht->num_buckets], key, ht->hash_func);
  entry_t *next = tmp->next;
  
  if (next != NULL && ht->hash_func(next->key) == ht->hash_func(key))
  {
    return Success(next->value);
  }
  else
  {
    return Failure();
    
  }
}

option_t ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key)
{
  option_t exist = ioopm_hash_table_lookup(ht, key);
  if (!exist.success)
    {
      return Failure();
    }
  entry_t *previous_entry = find_previous_entry_for_key(&ht->buckets[ht->hash_func(key) % ht->num_buckets], key, ht->hash_func);
  entry_t *current_entry = previous_entry->next;
  elem_t value = current_entry->value;
  
  previous_entry->next = current_entry->next;
  entry_destroy(current_entry);
  --ht->num_entries;
  
  return Success(value);
}

size_t ioopm_hash_table_size(ioopm_hash_table_t *ht)
{
  size_t counter = 0;
  for (int i = 0; i < ht->num_buckets; ++i )
    {
      counter = counter + number_of_entries_in_bucket(&ht->buckets[i]);
    }
  return counter;
}

bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht)
{
  for (int i = 0; i < ht->num_buckets; ++i)
    {
      entry_t *entry = &ht->buckets[i];
      if (entry->next != NULL)
        {
          return false;
        }
    }
  return true;
}

void ioopm_hash_table_clear(ioopm_hash_table_t *ht)
{
  for (int current_bucket = 0; current_bucket < ht->num_buckets; ++current_bucket)
    {
      entry_t *current_entry = &ht->buckets[current_bucket];
      if (current_entry->next != NULL)
        {
          current_entry = current_entry->next;
          while(current_entry->next != NULL)
            {
              entry_t *remove_entry = current_entry;
              current_entry = current_entry->next;
              entry_destroy(remove_entry);
            }
          entry_destroy(current_entry);
        }
      entry_t *dummy = &ht->buckets[current_bucket];
      dummy->next = NULL;
    }
}

ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht)
{
  if (ioopm_hash_table_is_empty(ht))
    {
      return NULL;
    }
  ioopm_list_t *list_of_keys = ioopm_linked_list_create(ht->key_eq_func);
  for (int current_bucket = 0; current_bucket < ht->num_buckets; ++current_bucket)
    {     
      entry_t *current_entry = &ht->buckets[current_bucket];
      elem_t current_key;
      while(current_entry->next != NULL)
        {
          current_entry = current_entry->next;
          current_key = current_entry->key;
          ioopm_linked_list_append(list_of_keys, current_key);
        }
    }
  return list_of_keys;
}

ioopm_list_t *ioopm_hash_table_values(ioopm_hash_table_t *ht)
{
  if (ioopm_hash_table_is_empty(ht))
    {
      return NULL;
    }
  ioopm_list_t *list_of_values = ioopm_linked_list_create(ht->value_eq_func);
  for (int current_bucket = 0; current_bucket < ht->num_buckets; ++current_bucket)
    {     
      entry_t *current_entry = &ht->buckets[current_bucket];
      elem_t current_value;
      while(current_entry->next != NULL)
        {
          current_entry = current_entry->next;
          current_value = current_entry->value;
          ioopm_linked_list_append(list_of_values, current_value);
        }
    }
  return list_of_values;
}

bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key)
{
  return ioopm_hash_table_any(ht, key_equiv, key, ht->key_eq_func);
}

bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, elem_t value)
{
  return ioopm_hash_table_any(ht, value_equiv, value, ht->value_eq_func);
}

bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate pred, elem_t arg, ioopm_eq_function eq_func)
{
  if (ioopm_hash_table_is_empty(ht))
    {
      return false;
    }
  size_t size = ioopm_hash_table_size(ht);
  ioopm_list_t *keys = ioopm_hash_table_keys(ht);
  ioopm_list_t *values = ioopm_hash_table_values(ht);

  bool result = false;
  ioopm_list_iterator_t *iter_keys = ioopm_list_iterator(keys);
  ioopm_list_iterator_t *iter_values = ioopm_list_iterator(values);

  for (int i = 0; i < size && !result; ++i)
    {
      result = result || pred(ioopm_iterator_current(iter_keys), ioopm_iterator_current(iter_values), arg, eq_func);
      ioopm_iterator_next(iter_keys);
      ioopm_iterator_next(iter_values);
    }
  
  ioopm_iterator_destroy(iter_keys);
  ioopm_iterator_destroy(iter_values);
  ioopm_linked_list_destroy(keys);
  ioopm_linked_list_destroy(values);
  
  return result;
}

bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate pred, elem_t arg, ioopm_eq_function eq_func)
{
  if (ioopm_hash_table_is_empty(ht))
    {
      return false;
    }
  size_t size = ioopm_hash_table_size(ht);
  ioopm_list_t *keys = ioopm_hash_table_keys(ht);
  ioopm_list_t *values = ioopm_hash_table_values(ht);
  bool result = true;

  ioopm_list_iterator_t *iter_keys = ioopm_list_iterator(keys);
  ioopm_list_iterator_t *iter_values = ioopm_list_iterator(values);
  
  for (int i = 0; i < size && result; ++i)
    {
      result = result && pred(ioopm_iterator_current(iter_keys), ioopm_iterator_current(iter_values), arg, eq_func);
      ioopm_iterator_next(iter_keys);
      ioopm_iterator_next(iter_values);
    }
  ioopm_iterator_destroy(iter_keys);
  ioopm_iterator_destroy(iter_values);
  ioopm_linked_list_destroy(keys);
  ioopm_linked_list_destroy(values);

  return result;
}


void ioopm_hash_table_apply_to_all(ioopm_hash_table_t *ht, ioopm_apply_function apply_fun, elem_t arg)
{
  for (int current_bucket = 0; current_bucket < ht->num_buckets; ++current_bucket)
    {     
      entry_t *current_entry = &ht->buckets[current_bucket];
      if (current_entry->next != NULL)
        {
          do
            {
              current_entry = current_entry->next;
              apply_fun(current_entry->key, &current_entry->value, arg);
            }
          while(current_entry->next != NULL);
        }
    }
}


