#include <string.h>
#include <stdbool.h>
#include <CUnit/Basic.h>
#include "hash_table.h"

int init_suite(void)
{
  return 0;
}

int clean_suite(void)
{
  return 0;
}


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

// 4 Tests for ioopm_hash_table_insert
//===========================================================================================
void test_insert_one(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element);
  ioopm_hash_table_insert(ht, (elem_t)0, (elem_t)15);
  entry_t *entry = &ht->buckets[0];
  entry = entry->next;
  CU_ASSERT_TRUE(entry->key.i==0);

  ioopm_hash_table_destroy(ht);
}

void test_insert_bigger_than_first_key(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element);
  ioopm_hash_table_insert(ht, (elem_t)1, (elem_t)10);
  ioopm_hash_table_insert(ht, (elem_t)18, (elem_t)10);
  entry_t *entry = &ht->buckets[1];
  entry = entry->next;
  entry = entry->next;  
  CU_ASSERT_TRUE(entry->key.i==18);

  ioopm_hash_table_destroy(ht);
}

void test_insert_change_first_value(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element);
  ioopm_hash_table_insert(ht, (elem_t)1, (elem_t)10);
  ioopm_hash_table_insert(ht, (elem_t)1, (elem_t)20);
  entry_t *entry = &ht->buckets[1];
  entry = entry->next;
  CU_ASSERT_TRUE(entry->value.i == 20);

  ioopm_hash_table_destroy(ht);
}

void test_insert_change_last_value(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element);
  ioopm_hash_table_insert(ht, (elem_t)1, (elem_t)10);
  ioopm_hash_table_insert(ht, (elem_t)18, (elem_t)10);
  ioopm_hash_table_insert(ht, (elem_t)35, (elem_t)10);
  ioopm_hash_table_insert(ht, (elem_t)35, (elem_t)20);
  entry_t *entry = &ht  ->buckets[1];
  entry = entry->next;
  entry = entry->next;
  entry = entry->next;
  CU_ASSERT_TRUE(entry->value.i == 20);

  ioopm_hash_table_destroy(ht);
}

// Hjälpfunktion till testerna
//=============================================================================
ioopm_hash_table_t *create_bucket()
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element);
  ioopm_hash_table_insert(ht, (elem_t)1, (elem_t)10);
  ioopm_hash_table_insert(ht, (elem_t)18, (elem_t)20);
  ioopm_hash_table_insert(ht, (elem_t)52, (elem_t)30);
  return ht;
}

// Tests for ioopm_hash_table_lookup
//============================================================================
void test_lookup_first(void)
{
  ioopm_hash_table_t *ht = create_bucket();
  option_t first_entry = ioopm_hash_table_lookup(ht,(elem_t)1);
  
  CU_ASSERT_TRUE( (first_entry.success) && first_entry.value.i == 10);

  ioopm_hash_table_destroy(ht);
}

void test_lookup_last(void)
{
  ioopm_hash_table_t *ht = create_bucket();
  option_t last_entry = ioopm_hash_table_lookup(ht, (elem_t)52);
  
  CU_ASSERT_TRUE( (last_entry.success) && last_entry.value.i == 30);

  ioopm_hash_table_destroy(ht);
}

void test_lookup_non_existent(void)
{
  ioopm_hash_table_t *ht = create_bucket();
  option_t first_entry_exist = ioopm_hash_table_lookup(ht, (elem_t)35);
  
  CU_ASSERT_FALSE(first_entry_exist.success);

  ioopm_hash_table_destroy(ht);
}

// Tests for ioopm_hash_table_remove
//===========================================================================
void test_delete_first(void)
{
  ioopm_hash_table_t *ht = create_bucket();
  ioopm_hash_table_remove(ht, (elem_t)1);
  option_t lookup = ioopm_hash_table_lookup(ht, (elem_t)1);
  CU_ASSERT_TRUE(Unsuccessful(lookup));

  ioopm_hash_table_destroy(ht);
}

void test_delete_last(void)
{
  ioopm_hash_table_t *ht = create_bucket();
  ioopm_hash_table_remove(ht, (elem_t)52);
  option_t lookup = ioopm_hash_table_lookup(ht, (elem_t)52);
  CU_ASSERT_TRUE(Unsuccessful(lookup));

  ioopm_hash_table_destroy(ht);
}

void test_delete_non_existent(void)
{
  ioopm_hash_table_t *ht = create_bucket();
  option_t remove = ioopm_hash_table_remove(ht, (elem_t)35);
  
  CU_ASSERT_TRUE(Unsuccessful(remove));

  ioopm_hash_table_destroy(ht);
}

// Tests for ioopm_hash_table_size
//=====================================================================
void test_size_zero(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element);
  size_t result = ioopm_hash_table_size(ht);
  CU_ASSERT_TRUE(result == 0);

  ioopm_hash_table_destroy(ht);
}

void test_size_one(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element);
  ioopm_hash_table_insert(ht, (elem_t)1, (elem_t)10);
  size_t result = ioopm_hash_table_size(ht);
  CU_ASSERT_TRUE(result == 1);

  ioopm_hash_table_destroy(ht);
}

void test_size_many(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element);
  for (int i = 0; i < 20; ++i)
    {
      ioopm_hash_table_insert(ht, (elem_t)i, (elem_t)30);
    }  
  size_t result = ioopm_hash_table_size(ht);  
  CU_ASSERT_TRUE(result == 20);

  ioopm_hash_table_destroy(ht);
}

// Tests for ioopm_hash_table_is_empty
//=======================================================================
void test_empty_empty(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element);
  bool result = ioopm_hash_table_is_empty(ht);
  CU_ASSERT_TRUE(result);

  ioopm_hash_table_destroy(ht);
}

void test_empty_not_empty(void)
{ 
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element);
  ioopm_hash_table_insert(ht, (elem_t)19, (elem_t)10);
  bool result = ioopm_hash_table_is_empty(ht);
  CU_ASSERT_FALSE(result);

  ioopm_hash_table_destroy(ht);
}

// Tests for ioopm_hash_table_clear
// ==========================================================================0
void test_clear_not_empty(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element); 
  for (int i = 0; i < 38; ++i)
    {
      ioopm_hash_table_insert(ht, (elem_t)i, (elem_t)30);
    }
  ioopm_hash_table_clear(ht);
  CU_ASSERT(ioopm_hash_table_is_empty(ht));
  
  ioopm_hash_table_destroy(ht);
}

void test_clear_empty(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element); 
  ioopm_hash_table_clear(ht);
  
  CU_ASSERT(ioopm_hash_table_is_empty(ht));
  
  ioopm_hash_table_destroy(ht);
}


// Tests for ioopm_hash_table_keys
//=======================================================================
void test_keys_empty(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element);
  ioopm_list_t *result = ioopm_hash_table_keys(ht);
  CU_ASSERT_TRUE(result==NULL);

  ioopm_hash_table_destroy(ht);
}

void test_keys_not_empty(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element);
  ioopm_hash_table_insert(ht, (elem_t)1, (elem_t)10);
  ioopm_hash_table_insert(ht, (elem_t)2, (elem_t)20);
  ioopm_hash_table_insert(ht, (elem_t)3, (elem_t)30);
  ioopm_list_t *result = ioopm_hash_table_keys(ht);
  
  CU_ASSERT_TRUE(ioopm_linked_list_get(result, 0).i == 1);
  CU_ASSERT_TRUE(ioopm_linked_list_get(result, 1).i == 2);
  CU_ASSERT_TRUE(ioopm_linked_list_get(result, 2).i == 3);

  ioopm_linked_list_destroy(result);
  ioopm_hash_table_destroy(ht);
}



// Tests for ioopm_hash_table_values
//============================================================================

void test_values_empty(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element);
  ioopm_list_t *result = ioopm_hash_table_values(ht);
  CU_ASSERT_TRUE(result==NULL);

  ioopm_hash_table_destroy(ht);
}

void test_values_not_empty(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element);
  ioopm_hash_table_insert(ht, (elem_t)1, (elem_t)10);
  ioopm_hash_table_insert(ht, (elem_t)2, (elem_t)20);
  ioopm_hash_table_insert(ht, (elem_t)3, (elem_t)30);
  ioopm_list_t *result = ioopm_hash_table_values(ht);
  
  CU_ASSERT_TRUE(ioopm_linked_list_get(result, 0).i == 10);
  CU_ASSERT_TRUE(ioopm_linked_list_get(result, 1).i == 20);
  CU_ASSERT_TRUE(ioopm_linked_list_get(result, 2).i == 30);

  ioopm_linked_list_destroy(result);
  ioopm_hash_table_destroy(ht);
}



// Tests for ioopm_hash_table_has_key
//=========================================================================
void test_has_key_unsuccessful(void) 
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element);
  ioopm_hash_table_insert(ht, (elem_t)1, (elem_t)10);
  ioopm_hash_table_insert(ht, (elem_t)2, (elem_t)20);
  ioopm_hash_table_insert(ht, (elem_t)3, (elem_t)30);
  ioopm_hash_table_insert(ht, (elem_t)4, (elem_t)40);
  
  CU_ASSERT_FALSE(ioopm_hash_table_has_key(ht, (elem_t)5));
  
  
  ioopm_hash_table_destroy(ht);
}

void test_has_key_first(void)
  
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element);
  ioopm_hash_table_insert(ht, (elem_t)1,  (elem_t)10);
  ioopm_hash_table_insert(ht,  (elem_t)2,  (elem_t)20);
  
  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, (elem_t)1));
   
  ioopm_hash_table_destroy(ht);
}


void test_has_key_last(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element);
  ioopm_hash_table_insert(ht, (elem_t)0, (elem_t)10);
  ioopm_hash_table_insert(ht, (elem_t)17, (elem_t)20);
  ioopm_hash_table_insert(ht, (elem_t)34, (elem_t)30);
  ioopm_hash_table_insert(ht, (elem_t)51, (elem_t)40);
  ioopm_hash_table_insert(ht, (elem_t)68, (elem_t)50);
  
  CU_ASSERT_TRUE(ioopm_hash_table_has_key(ht, (elem_t)68));
  ioopm_hash_table_destroy(ht);
}

// Tests for ioopm_hash_table_has_value
//=========================================================================

void test_has_value_unsuccessful(void) 
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element);
  ioopm_hash_table_insert(ht, (elem_t)1, (elem_t)10);
  ioopm_hash_table_insert(ht, (elem_t)2, (elem_t)20);
  ioopm_hash_table_insert(ht, (elem_t)3, (elem_t)30);
  ioopm_hash_table_insert(ht, (elem_t)4, (elem_t)40);
  
  CU_ASSERT_FALSE(ioopm_hash_table_has_value(ht, (elem_t)50));
  
  
  ioopm_hash_table_destroy(ht);
}

void test_has_value_first(void)
  
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element);
  ioopm_hash_table_insert(ht, (elem_t)1,  (elem_t)10);
  ioopm_hash_table_insert(ht, (elem_t)2,  (elem_t)20);
  
  CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht, (elem_t)10));
   
  ioopm_hash_table_destroy(ht);
}


void test_has_value_last(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element);
  ioopm_hash_table_insert(ht, (elem_t)0, (elem_t)10);
  ioopm_hash_table_insert(ht, (elem_t)17, (elem_t)20);
  ioopm_hash_table_insert(ht, (elem_t)34, (elem_t)30);
  ioopm_hash_table_insert(ht, (elem_t)51, (elem_t)40);
  ioopm_hash_table_insert(ht, (elem_t)68, (elem_t)50);
  
  CU_ASSERT_TRUE(ioopm_hash_table_has_value(ht, (elem_t)50));
  ioopm_hash_table_destroy(ht);
}


// Tests for ioopm_hash_table_all
//=========================================================================
void test_has_all_key_one(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element);
  ioopm_hash_table_insert(ht, (elem_t)1, (elem_t)10);

  CU_ASSERT_TRUE(ioopm_hash_table_all(ht, key_equiv, (elem_t)1, ht->key_eq_func));

  ioopm_hash_table_destroy(ht);
}

void test_has_all_value_wrong(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element);
  ioopm_hash_table_insert(ht, (elem_t)0, (elem_t)10);
  ioopm_hash_table_insert(ht, (elem_t)1, (elem_t)20);
  ioopm_hash_table_insert(ht, (elem_t)31, (elem_t)40);
  ioopm_hash_table_insert(ht, (elem_t)68, (elem_t)50);

  CU_ASSERT_FALSE(ioopm_hash_table_all(ht, value_equiv, (elem_t)10, ht->value_eq_func));

  ioopm_hash_table_destroy(ht);
}

void test_has_all_value_many(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element);
  ioopm_hash_table_insert(ht, (elem_t)0, (elem_t)10);
  ioopm_hash_table_insert(ht, (elem_t)1, (elem_t)10);
  ioopm_hash_table_insert(ht, (elem_t)31, (elem_t)10);
  ioopm_hash_table_insert(ht, (elem_t)68, (elem_t)10);
 
  CU_ASSERT_TRUE(ioopm_hash_table_all(ht, value_equiv, (elem_t)10, ht->value_eq_func));

  ioopm_hash_table_destroy(ht);
}



// Tests for ioopm_hash_table_apply_to_all
//=========================================================================
static void fun(elem_t key, elem_t *value, elem_t extra)
{
  *value = extra;
}

void test_apply_to_all(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(string_hash, compare_string_element, compare_int_element);
  elem_t key1;
  key1.p = "hej1";
  elem_t key2;
  key2.p = "hej2";
  elem_t key3;
  key3.p = "hej3";
  
  ioopm_hash_table_insert(ht, key1, (elem_t)1);
  ioopm_hash_table_insert(ht, key2, (elem_t)2);
  ioopm_hash_table_insert(ht, key3, (elem_t)3);

  
  ioopm_hash_table_apply_to_all(ht, fun, (elem_t)5);
  CU_ASSERT(ioopm_hash_table_all(ht, value_equiv, (elem_t)5, ht->value_eq_func));

  ioopm_hash_table_destroy(ht);
}


void test_dynamic_hash_table(void)
{
  ioopm_hash_table_t *ht = ioopm_hash_table_create(int_hash, compare_int_element, compare_int_element);
  for (int i = 0; i < 2000; ++i)
    {
      ioopm_hash_table_insert(ht, (elem_t)(i*50), (elem_t)30);
    }    
  CU_ASSERT_TRUE(ht->num_entries == 2000);
  CU_ASSERT_TRUE(ht->num_buckets == 4099);
  
  ioopm_hash_table_destroy(ht);
}

int main()
{
  CU_pSuite test_suite1 = NULL;

  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  test_suite1 = CU_add_suite("Test Suite 1", init_suite, clean_suite);
  if (NULL == test_suite1)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  if (
      (NULL == CU_add_test(test_suite1, "test_insert_one", test_insert_one)) ||
      (NULL == CU_add_test(test_suite1, "test_insert_bigger_than_first_key", test_insert_bigger_than_first_key)) ||
      (NULL == CU_add_test(test_suite1, "test_insert_change_first_value", test_insert_change_first_value)) ||
      (NULL == CU_add_test(test_suite1, "test_insert_change_last_value", test_insert_change_last_value)) ||
      (NULL == CU_add_test(test_suite1, "test_lookup_first", test_lookup_first)) ||
      (NULL == CU_add_test(test_suite1, "test_lookup_last", test_lookup_last)) ||
      (NULL == CU_add_test(test_suite1, "test_lookup_non_existent", test_lookup_non_existent)) ||
      (NULL == CU_add_test(test_suite1, "test_delete_first", test_delete_first)) ||
      (NULL == CU_add_test(test_suite1, "test_delete_last", test_delete_last)) ||
      (NULL == CU_add_test(test_suite1, "test_delete_non_existent", test_delete_non_existent)) ||
      (NULL == CU_add_test(test_suite1, "test_size_zero", test_size_zero))  ||
      (NULL == CU_add_test(test_suite1, "test_size_one", test_size_one)) ||
      (NULL == CU_add_test(test_suite1, "test_size_many", test_size_many)) ||
      (NULL == CU_add_test(test_suite1, "test_empty_empty", test_empty_empty)) ||
      (NULL == CU_add_test(test_suite1, "test_empty_not_empty", test_empty_not_empty)) ||
      (NULL == CU_add_test(test_suite1, "test_clear_not_empty", test_clear_not_empty)) ||
      (NULL == CU_add_test(test_suite1, "test_clear_empty", test_clear_empty)) ||
      (NULL == CU_add_test(test_suite1, "test_keys_empty", test_keys_empty)) ||
      (NULL == CU_add_test(test_suite1, "test_keys_not_empty", test_keys_not_empty)) ||
      (NULL == CU_add_test(test_suite1, "test_values_empty", test_values_empty)) ||
      (NULL == CU_add_test(test_suite1, "test_values_not_empty", test_values_not_empty)) ||
      (NULL == CU_add_test(test_suite1, "test_has_key_unsuccessful",test_has_key_unsuccessful)) ||
      (NULL == CU_add_test(test_suite1, "test_has_key_first",test_has_key_first)) ||
      (NULL == CU_add_test(test_suite1, "test_has_key_last",test_has_key_last)) ||
      (NULL == CU_add_test(test_suite1, "test_has_value_unsuccessful",test_has_value_unsuccessful)) ||
      (NULL == CU_add_test(test_suite1, "test_has_value_first",test_has_value_first)) ||
      (NULL == CU_add_test(test_suite1, "test_has_value_last",test_has_value_last)) ||
      (NULL == CU_add_test(test_suite1, "test_has_all_key_one",test_has_all_key_one)) ||
      (NULL == CU_add_test(test_suite1, "test_has_all_value_wrong",test_has_all_value_wrong)) ||
      (NULL == CU_add_test(test_suite1, "test_has_all_value_many",test_has_all_value_many)) ||
      (NULL == CU_add_test(test_suite1, "test_apply_to_all",test_apply_to_all)) ||
      (NULL == CU_add_test(test_suite1, "test_dynamic_hash_table",test_dynamic_hash_table))  
   
      )
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();
  return CU_get_error();
}
