#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <CUnit/Basic.h>
#include "linked_list.h"

int init_suite(void)
{
  return 0;
}

int clean_suite(void)
{
  return 0;
}


// @brief creates a new link
// @param value the value we want the link to have
// @param next_link the link which this link will point to (may be NULL)
// @return a new link
static link_t *link_create(elem_t value, link_t *next_link)
{
  link_t *new_link = calloc(1, sizeof(link_t));
  new_link->element = value;
  new_link->next = next_link;
  return new_link;
}

// Uses an eq_function to determine if two elements are equal, eq_function must be compatible with the element type.
static bool element_equal(elem_t key_ignored, elem_t element, elem_t x, ioopm_eq_function func_equal)
{
  return func_equal(element,x);
}

void test_append_one(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_string_element);
  elem_t test;
  test.p = "hej";
  ioopm_linked_list_append(list, test);
  
  CU_ASSERT_TRUE(list->eq_func(list->first->next->element, test));

  ioopm_linked_list_destroy(list);
}

void test_append_many(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)73);
  ioopm_linked_list_append(list, (elem_t)22); 
  ioopm_linked_list_append(list, (elem_t)1);
  ioopm_linked_list_append(list, (elem_t)13);

  CU_ASSERT_TRUE(list->first->next->element.i == 73);
  CU_ASSERT_TRUE(list->last->element.i == 13);

  ioopm_linked_list_destroy(list);
}

void test_prepend_one(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_prepend(list, (elem_t)2);
  
  CU_ASSERT_TRUE(list->first->next->element.i == 2);

  ioopm_linked_list_destroy(list);
}

void test_prepend_many(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_prepend(list, (elem_t)13);
  ioopm_linked_list_prepend(list, (elem_t)33);
  ioopm_linked_list_prepend(list, (elem_t)212);
  ioopm_linked_list_prepend(list, (elem_t)2);
  
  CU_ASSERT_TRUE(list->first->next->element.i == 2);
  CU_ASSERT_TRUE(list->last->element.i == 13);

  ioopm_linked_list_destroy(list);
}

// test for ioopm_linked_list_insert_last

void test_insert_first(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_prepend(list, (elem_t)13);
  ioopm_linked_list_prepend(list, (elem_t)33);
  ioopm_linked_list_prepend(list, (elem_t)212);

  ioopm_linked_list_insert(list, 0, (elem_t)76);
  
  CU_ASSERT_TRUE(list->first->next->element.i == 76);

  ioopm_linked_list_destroy(list);
}


void test_insert_last(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_prepend(list, (elem_t)13);
  ioopm_linked_list_prepend(list, (elem_t)33);
  ioopm_linked_list_prepend(list, (elem_t)212);

  ioopm_linked_list_insert(list, 3, (elem_t)76);
  
  CU_ASSERT_TRUE(list->last->element.i == 76);

  ioopm_linked_list_destroy(list);
}

void test_remove_first(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)13);
  ioopm_linked_list_append(list, (elem_t)33);
  ioopm_linked_list_append(list, (elem_t)212);
  CU_ASSERT_TRUE(ioopm_linked_list_remove(list, 0).i == 13);
  ioopm_linked_list_destroy(list);
}


void test_remove_last(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)13);
  ioopm_linked_list_append(list, (elem_t)33);
  ioopm_linked_list_append(list, (elem_t)21);  
  
  CU_ASSERT_TRUE(ioopm_linked_list_remove(list, 2).i == 21);
  CU_ASSERT_TRUE(list->last->element.i == 33);
  
  ioopm_linked_list_destroy(list);
}

void test_remove_middle(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)13);
  ioopm_linked_list_append(list, (elem_t)33);
  ioopm_linked_list_append(list, (elem_t)212);
  ioopm_linked_list_append(list, (elem_t)53);
  ioopm_linked_list_append(list, (elem_t)21);

  CU_ASSERT_TRUE(ioopm_linked_list_remove(list, 2).i == 212);
  ioopm_linked_list_destroy(list);
}


void test_get_first(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)3);
  
  CU_ASSERT_TRUE(ioopm_linked_list_get(list, 0).i == 3);

  ioopm_linked_list_destroy(list);
}

void test_get_last(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)13);
  ioopm_linked_list_append(list, (elem_t)33);
  ioopm_linked_list_append(list, (elem_t)212);
  ioopm_linked_list_append(list, (elem_t)3);
  
  CU_ASSERT_TRUE(ioopm_linked_list_get(list, 3).i == 3);

  ioopm_linked_list_destroy(list);
}

void test_contains_no(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)13);
  ioopm_linked_list_append(list, (elem_t)33);
  ioopm_linked_list_append(list, (elem_t)212);
  ioopm_linked_list_append(list, (elem_t)3);
  
  CU_ASSERT_FALSE(ioopm_linked_list_contains(list, (elem_t)44));
  ioopm_linked_list_destroy(list);
}

void test_contains_first(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)3);
  
  CU_ASSERT_TRUE(ioopm_linked_list_contains(list, (elem_t)3));

  ioopm_linked_list_destroy(list);
}

void test_contains_last(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)13);
  ioopm_linked_list_append(list, (elem_t)33);
  ioopm_linked_list_append(list, (elem_t)212);
  ioopm_linked_list_append(list, (elem_t)3);
  
  CU_ASSERT_TRUE(ioopm_linked_list_contains(list, (elem_t)3));

  ioopm_linked_list_destroy(list);
}

void test_size_empty(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  
  CU_ASSERT_TRUE(ioopm_linked_list_size(list) == 0);
  ioopm_linked_list_destroy(list);
}
  

void test_size_many(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)13);
  ioopm_linked_list_append(list, (elem_t)33);
  ioopm_linked_list_append(list, (elem_t)212);
  ioopm_linked_list_append(list, (elem_t)3);
  
  CU_ASSERT_TRUE(ioopm_linked_list_size(list) == 4);

  ioopm_linked_list_destroy(list);
}

void test_is_empty(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  
  CU_ASSERT_TRUE(ioopm_linked_list_is_empty(list));

  ioopm_linked_list_destroy(list);
}

void test_is_not_empty(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)3);
  
  CU_ASSERT_FALSE(ioopm_linked_list_is_empty(list));

  ioopm_linked_list_destroy(list);
}

void test_clear_empty(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_clear(list);
  
  CU_ASSERT_TRUE(ioopm_linked_list_is_empty(list));
  CU_ASSERT_TRUE(list->last->next == NULL);

  ioopm_linked_list_destroy(list);
}

void test_clear_full(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)13);
  ioopm_linked_list_append(list, (elem_t)33);
  ioopm_linked_list_append(list, (elem_t)212);
  ioopm_linked_list_append(list, (elem_t)3);
  ioopm_linked_list_clear(list);
  
  CU_ASSERT_TRUE(ioopm_linked_list_is_empty(list));
  CU_ASSERT_TRUE(list->last->next == NULL);

  ioopm_linked_list_destroy(list);
}

//===========================================================================
//test ioopm_linked_list_any_first
void test_any_empty(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  CU_ASSERT_FALSE(ioopm_linked_list_any(list, element_equal, (elem_t)13));

  ioopm_linked_list_destroy(list);
}

void test_any_many(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)13);
  ioopm_linked_list_append(list, (elem_t)33);
  ioopm_linked_list_append(list, (elem_t)34);
  ioopm_linked_list_append(list, (elem_t)433);
  ioopm_linked_list_append(list, (elem_t)3);
  CU_ASSERT(ioopm_linked_list_any(list, element_equal, (elem_t)3));

  ioopm_linked_list_destroy(list);
}

void test_any_many_false(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)13);
  ioopm_linked_list_append(list, (elem_t)33);
  ioopm_linked_list_append(list, (elem_t)34);
  ioopm_linked_list_append(list, (elem_t)433);
  ioopm_linked_list_append(list, (elem_t)7);
  CU_ASSERT_FALSE(ioopm_linked_list_any(list, element_equal, (elem_t)3));

  ioopm_linked_list_destroy(list);
}

void test_all_empty(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  CU_ASSERT_FALSE(ioopm_linked_list_all(list, element_equal, (elem_t)2));

  ioopm_linked_list_destroy(list);
}

void test_all_but_one(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)33);
  ioopm_linked_list_append(list, (elem_t)33);
  ioopm_linked_list_append(list, (elem_t)33);
  ioopm_linked_list_append(list, (elem_t)33);
  ioopm_linked_list_append(list, (elem_t)34);
  CU_ASSERT_FALSE(ioopm_linked_list_all(list, element_equal, (elem_t)33));

  ioopm_linked_list_destroy(list);
}

void test_all_the_same(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)33);
  ioopm_linked_list_append(list, (elem_t)33);
  ioopm_linked_list_append(list, (elem_t)33);
  ioopm_linked_list_append(list, (elem_t)33);
  ioopm_linked_list_append(list, (elem_t)33);
  CU_ASSERT(ioopm_linked_list_all(list, element_equal, (elem_t)33));

  ioopm_linked_list_destroy(list);
}


static void fun(elem_t key, elem_t *value, elem_t extra)
{
  *value = extra;
}

void test_apply_to_all(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)33);
  ioopm_linked_list_append(list, (elem_t)33);
  ioopm_linked_list_append(list, (elem_t)33);
  ioopm_linked_list_append(list, (elem_t)33);
  ioopm_linked_list_append(list, (elem_t)33);
  ioopm_linked_apply_to_all(list, fun, (elem_t)777);
  CU_ASSERT(ioopm_linked_list_all(list, element_equal, (elem_t)777));

  ioopm_linked_list_destroy(list);
}


//===================================================0
// tests for iterator

void test_has_next_true(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)3);
  ioopm_linked_list_append(list, (elem_t)33);
  ioopm_linked_list_append(list, (elem_t)333);

  ioopm_list_iterator_t *iter = ioopm_list_iterator(list);

  CU_ASSERT(ioopm_iterator_has_next(iter));

  ioopm_iterator_destroy(iter);
  ioopm_linked_list_destroy(list);
}

void test_has_next_false(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)3);

  ioopm_list_iterator_t *iter = ioopm_list_iterator(list);

  CU_ASSERT_FALSE(ioopm_iterator_has_next(iter));

  ioopm_iterator_destroy(iter);
  ioopm_linked_list_destroy(list);
}

void test_next(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)1);
  ioopm_linked_list_append(list, (elem_t)2);
  ioopm_linked_list_append(list, (elem_t)3);
  ioopm_linked_list_append(list, (elem_t)4);

  ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
  CU_ASSERT(ioopm_iterator_next(iter).i == 2);
  CU_ASSERT(ioopm_iterator_next(iter).i == 3);
  CU_ASSERT(ioopm_iterator_next(iter).i == 4);
  CU_ASSERT(ioopm_iterator_next(iter).i == 4);
  
  free(iter);
  ioopm_linked_list_destroy(list);
}

void test_current(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)1);
  ioopm_linked_list_append(list, (elem_t)2);

  ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
  CU_ASSERT(ioopm_iterator_current(iter).i == 1);
  ioopm_iterator_next(iter);
  CU_ASSERT(ioopm_iterator_current(iter).i == 2);
  
  free(iter);
  ioopm_linked_list_destroy(list);
}

void test_iter_remove_one(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)1);

  ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
  CU_ASSERT(ioopm_iterator_remove(iter).i == 1);
  CU_ASSERT(iter->list->last->element.i == 1);
  
  
  ioopm_iterator_destroy(iter);
  ioopm_linked_list_destroy(list);
}

void test_iter_remove_last(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)1);
  ioopm_linked_list_append(list, (elem_t)2);

  ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
  ioopm_iterator_next(iter);
  CU_ASSERT(ioopm_iterator_remove(iter).i == 2);
  CU_ASSERT(iter->list->last->element.i == 1);
  
  
  ioopm_iterator_destroy(iter);
  ioopm_linked_list_destroy(list);
}

void test_iter_remove_other(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)1);
  ioopm_linked_list_append(list, (elem_t)2);
  ioopm_linked_list_append(list, (elem_t)3);
  ioopm_linked_list_append(list, (elem_t)4);

  ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
  ioopm_iterator_next(iter);
  CU_ASSERT(ioopm_iterator_remove(iter).i == 2);
  CU_ASSERT(ioopm_iterator_remove(iter).i == 3);
  
  
  ioopm_iterator_destroy(iter);
  ioopm_linked_list_destroy(list);
}

void test_insert(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)1);

  ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
  ioopm_iterator_insert(iter, (elem_t)0);
  
  CU_ASSERT(ioopm_iterator_current(iter).i == 1);
  CU_ASSERT(iter->list->first->element.i == 0);
  CU_ASSERT(ioopm_linked_list_size(list) == 2);
  
  ioopm_iterator_destroy(iter);
  ioopm_linked_list_destroy(list);
}

void test_reset(void)
{
  ioopm_list_t *list = ioopm_linked_list_create(compare_int_element);
  ioopm_linked_list_append(list, (elem_t)1);
  ioopm_linked_list_append(list, (elem_t)2);
  ioopm_linked_list_append(list, (elem_t)3);
  ioopm_linked_list_append(list, (elem_t)4);
  
  ioopm_list_iterator_t *iter = ioopm_list_iterator(list);
  ioopm_iterator_next(iter);
  ioopm_iterator_next(iter);
  ioopm_iterator_next(iter);
  ioopm_iterator_reset(iter);

  CU_ASSERT(ioopm_iterator_current(iter).i == 1);
  
  ioopm_iterator_destroy(iter);
  ioopm_linked_list_destroy(list);
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
    (NULL == CU_add_test(test_suite1, "test_append_one", test_append_one)) ||
    (NULL == CU_add_test(test_suite1, "test_append_many", test_append_many)) ||
    (NULL == CU_add_test(test_suite1, "test_prepend_one", test_prepend_one)) ||
    (NULL == CU_add_test(test_suite1, "test_prepend_many", test_prepend_many)) ||
    (NULL == CU_add_test(test_suite1, "test_insert_first", test_insert_first)) ||
    (NULL == CU_add_test(test_suite1, "test_insert_last", test_insert_last)) ||
    (NULL == CU_add_test(test_suite1, "test_remove_first", test_remove_first)) ||
    (NULL == CU_add_test(test_suite1, "test_remove_middle", test_remove_middle)) ||
    (NULL == CU_add_test(test_suite1, "test_remove_last", test_remove_last)) ||
    (NULL == CU_add_test(test_suite1, "test_get_first", test_get_first)) ||
    (NULL == CU_add_test(test_suite1, "test_get_last", test_get_last)) ||
    (NULL == CU_add_test(test_suite1, "test_contains_no", test_contains_no)) ||
    (NULL == CU_add_test(test_suite1, "test_contains_first", test_contains_first)) ||
    (NULL == CU_add_test(test_suite1, "test_contains_last", test_contains_last)) ||
    (NULL == CU_add_test(test_suite1, "test_size_empty", test_size_empty)) ||
    (NULL == CU_add_test(test_suite1, "test_size_many", test_size_many)) ||
    (NULL == CU_add_test(test_suite1, "test_is_empty", test_is_empty)) ||
    (NULL == CU_add_test(test_suite1, "test_is_not_empty", test_is_not_empty)) ||
    (NULL == CU_add_test(test_suite1, "test_clear_empty", test_clear_empty)) ||
    (NULL == CU_add_test(test_suite1, "test_clear_full", test_clear_full)) ||
    (NULL == CU_add_test(test_suite1, "test_any_empty", test_any_empty)) ||
    (NULL == CU_add_test(test_suite1, "test_any_many", test_any_many)) ||
    (NULL == CU_add_test(test_suite1, "test_any_many_false", test_any_many_false)) ||
    (NULL == CU_add_test(test_suite1, "test_all_empty", test_all_empty)) ||
    (NULL == CU_add_test(test_suite1, "test_all_but_one", test_all_but_one)) ||
    (NULL == CU_add_test(test_suite1, "test_all_the_same", test_all_the_same)) ||
    (NULL == CU_add_test(test_suite1, "test_apply_to_all", test_apply_to_all)) ||
    (NULL == CU_add_test(test_suite1, "test_has_next_true", test_has_next_true)) ||
    (NULL == CU_add_test(test_suite1, "test_has_next_false", test_has_next_false)) ||
    (NULL == CU_add_test(test_suite1, "test_next", test_next)) ||
    (NULL == CU_add_test(test_suite1, "test_current", test_current)) ||
    (NULL == CU_add_test(test_suite1, "test_iter_remove_one", test_iter_remove_one)) ||
    (NULL == CU_add_test(test_suite1, "test_iter_remove_last", test_iter_remove_last)) ||
    (NULL == CU_add_test(test_suite1, "test_iter_remove_other", test_iter_remove_other)) ||
    (NULL == CU_add_test(test_suite1, "test_insert", test_insert)) ||
    (NULL == CU_add_test(test_suite1, "test_reset", test_reset))
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
