//#pragma once
#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "linked_list.h"

#define Success(v)      (option_t) { .success = true, .value = v };
#define Failure()       (option_t) { .success = false };
#define Successful(o)   (o.success == true)
#define Unsuccessful(o) (o.success == false)

typedef struct option option_t;
struct option
{
  bool success;
  elem_t value;
};

/**
 * @file hash_table.h
 * @author Alexander Lahti och Behrooz Etemadi
 * @date 17 Sept 2019 - 9 Oct 2019
 * @brief Simple hash table that maps keys to values.
 *
 */

/// @brief Create a new hash table
/// @param hash_func a hash function that operates on the type that the keys have
/// @param key_eq_func an equal function that operates on the type that the keys have
/// @param value_eq_func an equal function that operates on the type that the values have
/// @return A new empty hash table
ioopm_hash_table_t *ioopm_hash_table_create(ioopm_hash_function hash_func, ioopm_eq_function key_eq_func, ioopm_eq_function value_eq_function);


/// @brief Delete a hash table and free its memory
/// param ht a hash table to be deleted
void ioopm_hash_table_destroy(ioopm_hash_table_t *ht); 

/// @brief add key => value entry in hash table ht
/// @param ht hash table operated upon
/// @param key key to insert
/// @param value value to insert
void ioopm_hash_table_insert(ioopm_hash_table_t *ht, elem_t key, elem_t value);

/// @brief lookup value for key in hash table ht
/// @param ht hash table operated upon
/// @param key key to lookup
/// @return the value mapped to by key if option_t.success is true, otherwise otion_t.value is unknown
option_t ioopm_hash_table_lookup(ioopm_hash_table_t *ht, elem_t key);

/// @brief remove any mapping from key to a value
/// @param ht hash table operated upon
/// @param key key to remove
/// @return the value mapped to by key i option_t.success is true (=something was removed), otherwise otion_t.value is unknown
option_t ioopm_hash_table_remove(ioopm_hash_table_t *ht, elem_t key);

/// @brief returns the number of key => value entries in the hash table
/// @param ht hash table operated upon
/// @return the number of key => value entries in the hash table
size_t ioopm_hash_table_size(ioopm_hash_table_t *ht);

/// @brief checks if the hash table is empty
/// @param ht hash table operated upon
/// @return true if empty, else false
bool ioopm_hash_table_is_empty(ioopm_hash_table_t *ht);

/// @brief clear all the entries in a hash table and return their memory
/// @param h hash table operated upon
void ioopm_hash_table_clear(ioopm_hash_table_t *ht);

/// @brief return the keys for all entries in a hash map (in no particular order, but same as ioopm_hash_table_values) if no keys are found, returns NULL
/// @param h hash table operated upon
/// @return a linked list of keys for hash table h
ioopm_list_t *ioopm_hash_table_keys(ioopm_hash_table_t *ht);

/// @brief return the values for all entries in a hash map (in no particular order, but same as ioopm_hash_table_keys) if no values are found, returns NULL
/// @param h hash table operated upon
/// @return a linked list of values for hash table h
ioopm_list_t *ioopm_hash_table_values(ioopm_hash_table_t *ht);

/// @brief check if a hash table has an entry with a given key
/// @param h hash table operated upon
/// @param key the key sought
/// @return true if the hash table had the key
bool ioopm_hash_table_has_key(ioopm_hash_table_t *ht, elem_t key);

/// @brief check if a hash table has an entry with a given value
/// @param h hash table operated upon
/// @param value the value sought
/// @return true if the hash table had the value
bool ioopm_hash_table_has_value(ioopm_hash_table_t *ht, elem_t value);

/// @brief check if a predicate is satisfied by all entries in a hash table
/// @param h hash table operated upon
/// @param pred the predicate
/// @param arg extra argument to pred
/// @param eq_func extra function to pred based on the type of the entries operated upon
/// @return true if the all entries satisfied the predicate
bool ioopm_hash_table_all(ioopm_hash_table_t *ht, ioopm_predicate pred, elem_t arg, ioopm_eq_function eq_fun);

/// @brief check if a predicate is satisfied by any entry in a hash table
/// @param h hash table operated upon
/// @param pred the predicate
/// @param arg extra argument to pred
/// @param eq_func extra function to pred based on the type of the entries operated upon
/// @return true if the any entries satisfied the predicate
bool ioopm_hash_table_any(ioopm_hash_table_t *ht, ioopm_predicate pred, elem_t arg, ioopm_eq_function eq_func);

/// @brief apply a function to all entries in a hash table
/// @param h hash table operated upon
/// @param apply_fun the function to be applied to all elements
/// @param arg extra argument to apply_fun
void ioopm_hash_table_apply_to_all(ioopm_hash_table_t *ht, ioopm_apply_function apply_fun, elem_t arg);

#endif
