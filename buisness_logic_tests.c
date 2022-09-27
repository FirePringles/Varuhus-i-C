#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <CUnit/Basic.h>
#include "buisness_logic.h"

static void destroy_merch(merch_t *merch)
{
  ioopm_linked_list_destroy(merch->list);
  free(merch);
}

static void destroy_all_merch(ioopm_hash_table_t *ht_merch)
{
  for (int i = 0; i < ht_merch->num_buckets; ++i)
    {
      entry_t *current = &ht_merch->buckets[i];
      while (current->next != NULL)
        {
          current = current->next;
          destroy_merch(current->value.p);
        }
    }
}

static void destroy_all_shelfs(ioopm_hash_table_t *ht_merch)
{
  for (int i = 0; i < ht_merch->num_buckets; ++i)
    {
      entry_t *current = &ht_merch->buckets[i];
      while (current->next != NULL)
        {
          current = current->next;
          merch_t *merch = current->value.p;
          link_t *current_link = merch->list->first;
          while (current_link->next != NULL)
            {
              current_link = current_link->next;
              free(current_link->element.p);
            }
        }
    }
}

// compare function for merches used by the hash table functions
// this function doesn't check that the linked lists are the same but that will never happen since two merches can't have the same name
// (which makes this function useless anyway, could just use compare_string_element)
static bool compare_merch_element(elem_t a, elem_t b)
{ 
  merch_t *merch1 = a.p;
  merch_t *merch2 = b.p;
  if (!strcmp(merch1->name, merch2->name) &&
      !strcmp(merch1->desc, merch2->desc) &&
      merch1->price == merch2->price
      )
    return true;
  else
    return false;
}

// compare function for shelves used by the hash table functions
static bool compare_shelf_element(elem_t a, elem_t b)
{
  shelf_t *shelf1 = a.p;
  shelf_t *shelf2 = b.p;
  if (!strcmp(shelf1->shelf_num, shelf2->shelf_num) &&
      shelf1->amount == shelf2->amount)
    return true;
  else
    return false;
}

static merch_t *create_merch(char *name, char *desc, int price)
{
  merch_t *merch = calloc(1, sizeof(merch_t));
  merch->name = name;
  merch->desc = desc;
  merch->price = price;

  ioopm_list_t *new_list = ioopm_linked_list_create(compare_shelf_element);
  merch->list = new_list;

  return merch;
}

static void destroy_all_carts(ioopm_hash_table_t *ht_carts)
{
  for (int i = 0; i < ht_carts->num_buckets; ++i)
    {
      entry_t *current = &ht_carts->buckets[i];
      while (current->next != NULL)
        {
          current = current->next;
          ioopm_hash_table_t *current_ht = current->value.p;
          ioopm_hash_table_destroy(current_ht);
        }
    }
}

int init_suite(void)
{
  return 0;
}

int clean_suite(void)
{
  return 0;
  
}

void test_add_merch_one(void)
{
  webstore_backend_t *wsbe = ioopm_create_webstore_backend();
  
  ioopm_add_merchendise(wsbe, strdup("äpple"), strdup("ett äpple"), 6);
  
  option_t yes = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)(void *)"äpple");
  merch_t *merch = yes.value.p;

  CU_ASSERT(Successful(ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)(void *)"äpple")));
  CU_ASSERT_STRING_EQUAL("ett äpple", (char *)merch->desc);

  ioopm_destroy_webstore_backend(wsbe);
}

void test_add_merch_many(void)
{
  webstore_backend_t *wsbe = ioopm_create_webstore_backend();

  ioopm_add_merchendise(wsbe, strdup("äpple"), strdup("ett äpple"), 6);
  ioopm_add_merchendise(wsbe, strdup("päron"), strdup("grönt"), 12);
  ioopm_add_merchendise(wsbe, strdup("satsumas"), strdup("gul citrus"), 18);
 
  
  option_t yes = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)(void *)"äpple");
  merch_t *merch = yes.value.p;

  CU_ASSERT(Successful(ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)(void *)"satsumas")));
  CU_ASSERT_STRING_EQUAL("ett äpple", merch->desc);
  CU_ASSERT(ioopm_hash_table_size(wsbe->ht_merch) == 3);
  CU_ASSERT_FALSE(ioopm_add_merchendise(wsbe, "äpple", "svenskt", 24));

  ioopm_destroy_webstore_backend(wsbe);
}

void test_remove_one(void)
{
  webstore_backend_t *wsbe = ioopm_create_webstore_backend();

  ioopm_add_merchendise(wsbe, strdup("äpple"), strdup("ett äpple"), 6);

  merch_t *merch = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)(void *)"äpple").value.p;
  ioopm_replenish(wsbe, merch, strdup("A01"), 10);
  ioopm_replenish(wsbe, merch, strdup("A02"), 10);
  
  CU_ASSERT(ioopm_remove_merchendise(wsbe, merch));
  CU_ASSERT(ioopm_hash_table_size(wsbe->ht_merch) == 0);
  CU_ASSERT(ioopm_hash_table_size(wsbe->ht_shelf) == 0);
  
  ioopm_destroy_webstore_backend(wsbe);
}

void test_remove_wrong(void)
{
  webstore_backend_t *wsbe = ioopm_create_webstore_backend();

  ioopm_add_merchendise(wsbe, strdup("äpple"), strdup("ett äpple"), 6);

  merch_t *merch = create_merch("päron", "mat", 2);
  
  CU_ASSERT_FALSE(ioopm_remove_merchendise(wsbe, merch));

  destroy_merch(merch);
  ioopm_destroy_webstore_backend(wsbe);
}

void test_edit_same_name(void)
{
  webstore_backend_t *wsbe = ioopm_create_webstore_backend();

  ioopm_add_merchendise(wsbe, strdup("äpple"), strdup("ett äpple"), 6);

  merch_t *merch = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)(void *)"äpple").value.p;
  ioopm_replenish(wsbe, merch, strdup("A01"), 10);
  ioopm_replenish(wsbe, merch, strdup("A02"), 10);
  ioopm_edit_merchendise(wsbe, merch, strdup("äpple"), strdup("grönt äpple"), 10);
  
  CU_ASSERT(ioopm_hash_table_size(wsbe->ht_merch) == 1);
  CU_ASSERT_STRING_EQUAL(merch->desc, "grönt äpple");
  
  ioopm_destroy_webstore_backend(wsbe);
}

void test_edit_new_name(void)
{
  webstore_backend_t *wsbe = ioopm_create_webstore_backend();

  ioopm_add_merchendise(wsbe, strdup("äpple"), strdup("ett äpple"), 6);

  merch_t *merch = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)(void *)"äpple").value.p;
  ioopm_replenish(wsbe, merch, strdup("A01"), 10);
  ioopm_replenish(wsbe, merch, strdup("A02"), 10);
  ioopm_edit_merchendise(wsbe, merch, strdup("päron"), strdup("grönt päron"), 10);

  merch = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)(void *)"päron").value.p;

  CU_ASSERT(Unsuccessful(ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)(void *)"äpple")));
  CU_ASSERT(ioopm_hash_table_size(wsbe->ht_merch) == 1);
  CU_ASSERT_STRING_EQUAL(merch->desc, "grönt päron");
  
  ioopm_destroy_webstore_backend(wsbe);
}

void test_edit_existing_name(void)
{
  webstore_backend_t *wsbe = ioopm_create_webstore_backend();

  ioopm_add_merchendise(wsbe, strdup("äpple"), strdup("ett äpple"), 6);
  ioopm_add_merchendise(wsbe, strdup("päron"), strdup("blått"), 3);

  merch_t *merch = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)(void *)"äpple").value.p;
  merch_t *merch_pear = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)(void *)"päron").value.p;
  ioopm_replenish(wsbe, merch, strdup("A01"), 10);
  ioopm_replenish(wsbe, merch, strdup("A02"), 10);
  ioopm_replenish(wsbe, merch_pear, strdup("A03"), 10);
  ioopm_replenish(wsbe, merch_pear, strdup("A04"), 10);
  
  CU_ASSERT_FALSE(ioopm_edit_merchendise(wsbe, merch, "päron", "grönt päron", 10));

  CU_ASSERT_STRING_EQUAL(merch_pear->desc, "blått");
  
  ioopm_destroy_webstore_backend(wsbe);
}

void test_calculate_empty(void)
{
  webstore_backend_t *wsbe = ioopm_create_webstore_backend();

  ioopm_add_merchendise(wsbe, strdup("äpple"), strdup("ett äpple"), 6);
  merch_t *merch = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)(void *)"äpple").value.p;
  
  CU_ASSERT(ioopm_calculate_stock(merch) == 0);
  
  ioopm_destroy_webstore_backend(wsbe);
}

void test_calculate_one_shelf(void)
{
  webstore_backend_t *wsbe = ioopm_create_webstore_backend();

  ioopm_add_merchendise(wsbe, strdup("äpple"), strdup("ett äpple"), 6);
  merch_t *merch = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)(void *)"äpple").value.p;
  ioopm_replenish(wsbe, merch, strdup("A01"), 10);
  ioopm_replenish(wsbe, merch, strdup("A01"), 5);
  
  CU_ASSERT(ioopm_calculate_stock(merch) == 15);
  
  ioopm_destroy_webstore_backend(wsbe);
}

void test_calculate_many_shelves(void)
{
  webstore_backend_t *wsbe = ioopm_create_webstore_backend();

  ioopm_add_merchendise(wsbe, strdup("äpple"), strdup("ett äpple"), 6);
  merch_t *merch = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)(void *)"äpple").value.p;
  ioopm_replenish(wsbe, merch, strdup("A01"), 10);
  ioopm_replenish(wsbe, merch, strdup("A02"), 5);
  ioopm_replenish(wsbe, merch, strdup("A03"), 2);
  ioopm_replenish(wsbe, merch, strdup("A04"), 1);
  
  CU_ASSERT(ioopm_calculate_stock(merch) == 18);
  
  ioopm_destroy_webstore_backend(wsbe);
}

void test_replenish_new(void)
{
  webstore_backend_t *wsbe = ioopm_create_webstore_backend();

  ioopm_add_merchendise(wsbe, strdup("äpple"), strdup("ett äpple"), 6);
  merch_t *merch = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)((void *)"äpple")).value.p;
  ioopm_replenish(wsbe, merch, strdup("A34"), 10);

  CU_ASSERT_FALSE(ioopm_replenish(wsbe, merch, strdup("A24"), 0));
  CU_ASSERT_FALSE(ioopm_linked_list_is_empty(merch->list));
  shelf_t *shelf = merch->list->last->element.p;
  CU_ASSERT(shelf->amount == 10);
  
  ioopm_destroy_webstore_backend(wsbe);
}

void test_replenish_existing(void)
{
  webstore_backend_t *wsbe = ioopm_create_webstore_backend();

  ioopm_add_merchendise(wsbe, strdup("äpple"), strdup("ett äpple"), 6);
  merch_t *merch = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)((void *)"äpple")).value.p;
  ioopm_replenish(wsbe, merch, strdup("A34"), 10);
  ioopm_replenish(wsbe, merch, strdup("A34"), 10);

  shelf_t *shelf = merch->list->last->element.p;
  CU_ASSERT(shelf->amount == 20);
  
  ioopm_destroy_webstore_backend(wsbe);
}

void test_replenish_wrong_number(void)
{
  webstore_backend_t *wsbe = ioopm_create_webstore_backend();

  ioopm_add_merchendise(wsbe, strdup("äpple"), strdup("ett äpple"), 6);
  merch_t *merch = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)((void *)"äpple")).value.p;
  ioopm_replenish(wsbe, merch, strdup("A34"), 10);
  
  ioopm_add_merchendise(wsbe, strdup("päron"), strdup("ett päron"), 10);
  merch_t *merch2 = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)((void *)"päron")).value.p;

  CU_ASSERT_FALSE(ioopm_replenish(wsbe, merch2, strdup("A34"), 10));
  
  ioopm_destroy_webstore_backend(wsbe);
}

void test_create_destroy_cart(void)
{
  webstore_backend_t *wsbe = ioopm_create_webstore_backend();

  CU_ASSERT(ioopm_create_cart(wsbe) == 1);     
  CU_ASSERT(wsbe->ht_carts->num_entries == 1);

  ioopm_remove_cart(wsbe, 1);
  CU_ASSERT(wsbe->ht_carts->num_entries == 0);
  
  ioopm_destroy_webstore_backend(wsbe);
}

void test_cart_add_zero(void)
{
  webstore_backend_t *wsbe = ioopm_create_webstore_backend();
  ioopm_add_merchendise(wsbe, strdup("äpple"), strdup("ett äpple"), 6);
  merch_t *merch = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)((void *)"äpple")).value.p;
  ioopm_replenish(wsbe, merch, strdup("A34"), 10);

  ioopm_create_cart(wsbe);
  CU_ASSERT_FALSE(ioopm_add_to_cart(wsbe, 1, strdup("äpple"), 0));

  ioopm_destroy_webstore_backend(wsbe);
}

void test_cart_add_allowed_amount(void)
{
  webstore_backend_t *wsbe = ioopm_create_webstore_backend();
  ioopm_add_merchendise(wsbe, strdup("äpple"), strdup("ett äpple"), 6);
  merch_t *merch = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)((void *)"äpple")).value.p;
  ioopm_replenish(wsbe, merch, strdup("A34"), 10);

  ioopm_create_cart(wsbe);
  CU_ASSERT(ioopm_add_to_cart(wsbe, 1, "äpple", 10));
  ioopm_hash_table_t *cart = ioopm_hash_table_lookup(wsbe->ht_carts, (elem_t)1).value.p;
  
  CU_ASSERT(Successful(ioopm_hash_table_lookup(cart, (elem_t)(void *)"äpple")));

  ioopm_destroy_webstore_backend(wsbe);
}

void test_cart_add_too_much(void)
{
  webstore_backend_t *wsbe = ioopm_create_webstore_backend();
  ioopm_add_merchendise(wsbe, strdup("äpple"), strdup("ett äpple"), 6);
  merch_t *merch = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)((void *)"äpple")).value.p;
  ioopm_replenish(wsbe, merch, strdup("A34"), 10);

  ioopm_create_cart(wsbe);
  CU_ASSERT_FALSE(ioopm_add_to_cart(wsbe, 1, "äpple", 11));
  ioopm_hash_table_t *cart = ioopm_hash_table_lookup(wsbe->ht_carts, (elem_t)1).value.p;
  
  CU_ASSERT_FALSE(Successful(ioopm_hash_table_lookup(cart, (elem_t)(void *)"äpple")));

  ioopm_destroy_webstore_backend(wsbe);
}

void test_cart_add_same_twice(void)
{
  webstore_backend_t *wsbe = ioopm_create_webstore_backend();
  ioopm_add_merchendise(wsbe, strdup("äpple"), strdup("ett äpple"), 6);
  merch_t *merch = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)((void *)"äpple")).value.p;
  ioopm_replenish(wsbe, merch, strdup("A34"), 10);

  ioopm_create_cart(wsbe);
  ioopm_add_to_cart(wsbe, 1, "äpple", 5);
  CU_ASSERT(ioopm_add_to_cart(wsbe, 1, "äpple", 5));
  ioopm_hash_table_t *cart = ioopm_hash_table_lookup(wsbe->ht_carts, (elem_t)1).value.p;
  
  CU_ASSERT(cart->num_entries == 1);
  CU_ASSERT(ioopm_hash_table_lookup(cart, (elem_t)(void *)"äpple").value.i == 10);

  ioopm_destroy_webstore_backend(wsbe);
}

void test_cart_add_many_carts(void)
{
  webstore_backend_t *wsbe = ioopm_create_webstore_backend();
  ioopm_add_merchendise(wsbe, strdup("äpple"), strdup("ett äpple"), 3);
  merch_t *merch1 = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)((void *)"äpple")).value.p;
  ioopm_replenish(wsbe, merch1, strdup("A34"), 10);

  ioopm_add_merchendise(wsbe, strdup("päron"), strdup("ett päron"), 5);
  merch_t *merch2 = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)((void *)"päron")).value.p;
  ioopm_replenish(wsbe, merch2, strdup("A35"), 10);

  ioopm_add_merchendise(wsbe, strdup("citron"), strdup("en citron"), 7);
  merch_t *merch3 = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)((void *)"citron")).value.p;
  ioopm_replenish(wsbe, merch3, strdup("A36"), 10);

  ioopm_create_cart(wsbe);
  ioopm_create_cart(wsbe);
  ioopm_create_cart(wsbe);
  ioopm_add_to_cart(wsbe, 1, "äpple", 5);
  ioopm_add_to_cart(wsbe, 2, "äpple", 5);
  CU_ASSERT_FALSE(ioopm_add_to_cart(wsbe, 3, "äpple", 5));
  ioopm_add_to_cart(wsbe, 1, "päron", 5);
  ioopm_add_to_cart(wsbe, 1, "citron", 5);
  ioopm_add_to_cart(wsbe, 2, "päron", 5);
  ioopm_add_to_cart(wsbe, 3, "citron", 2);
  ioopm_add_to_cart(wsbe, 3, "citron", 3);
  
  ioopm_hash_table_t *cart1 = ioopm_hash_table_lookup(wsbe->ht_carts, (elem_t)1).value.p;
  ioopm_hash_table_t *cart2 = ioopm_hash_table_lookup(wsbe->ht_carts, (elem_t)2).value.p;
  ioopm_hash_table_t *cart3 = ioopm_hash_table_lookup(wsbe->ht_carts, (elem_t)3).value.p;

  CU_ASSERT(cart1->num_entries == 3);
  CU_ASSERT(cart2->num_entries == 2);
  CU_ASSERT(cart3->num_entries == 1);

  ioopm_destroy_webstore_backend(wsbe);
}

void test_calc_cost_zero(void)
{
  webstore_backend_t *wsbe = ioopm_create_webstore_backend();
  ioopm_add_merchendise(wsbe, strdup("äpple"), strdup("ett äpple"), 6);
  merch_t *merch = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)((void *)"äpple")).value.p;
  ioopm_replenish(wsbe, merch, strdup("A34"), 10);

  ioopm_create_cart(wsbe);
  
  CU_ASSERT(ioopm_calculate_cost(wsbe, 1) == 0);

  ioopm_destroy_webstore_backend(wsbe);
}

void test_calc_cost_many(void)
{
  webstore_backend_t *wsbe = ioopm_create_webstore_backend();
  
  ioopm_add_merchendise(wsbe, strdup("äpple"), strdup("ett äpple"), 3);
  merch_t *merch1 = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)((void *)"äpple")).value.p;
  ioopm_replenish(wsbe, merch1, strdup("A34"), 10);

  ioopm_add_merchendise(wsbe, strdup("päron"), strdup("ett päron"), 7);
  merch_t *merch2 = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)((void *)"päron")).value.p;
  ioopm_replenish(wsbe, merch2, strdup("A35"), 10);

  ioopm_create_cart(wsbe);
  ioopm_add_to_cart(wsbe, 1, "äpple", 6);
  ioopm_add_to_cart(wsbe, 1, "päron", 6);
  
  CU_ASSERT(ioopm_calculate_cost(wsbe, 1) == 60);

  ioopm_destroy_webstore_backend(wsbe);
}

void test_checkout_empty(void)
{
  webstore_backend_t *wsbe = ioopm_create_webstore_backend();

  ioopm_create_cart(wsbe);
  ioopm_checkout_cart(wsbe, 1);
  
  CU_ASSERT(wsbe->ht_carts->num_entries == 0);

  ioopm_destroy_webstore_backend(wsbe);
}

void test_checkout_one_item(void)
{
  webstore_backend_t *wsbe = ioopm_create_webstore_backend();
  
  ioopm_add_merchendise(wsbe, strdup("äpple"), strdup("ett äpple"), 3);
  merch_t *merch1 = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)((void *)"äpple")).value.p;
  ioopm_replenish(wsbe, merch1, strdup("A34"), 10);
  ioopm_replenish(wsbe, merch1, strdup("A35"), 10);

  ioopm_create_cart(wsbe);
  ioopm_add_to_cart(wsbe, 1, "äpple", 15);
  ioopm_checkout_cart(wsbe, 1);

  CU_ASSERT(ioopm_calculate_stock(merch1) == 5);

  ioopm_destroy_webstore_backend(wsbe);
}

void test_checkout_many_items(void)
{
  webstore_backend_t *wsbe = ioopm_create_webstore_backend();
  
  ioopm_add_merchendise(wsbe, strdup("äpple"), strdup("ett äpple"), 3);
  merch_t *merch1 = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)((void *)"äpple")).value.p;
  ioopm_replenish(wsbe, merch1, strdup("A34"), 10);
  ioopm_replenish(wsbe, merch1, strdup("B34"), 10);

  ioopm_add_merchendise(wsbe, strdup("päron"), strdup("ett päron"), 5);
  merch_t *merch2 = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)((void *)"päron")).value.p;
  ioopm_replenish(wsbe, merch2, strdup("A35"), 10);
  ioopm_replenish(wsbe, merch2, strdup("B35"), 10);

  ioopm_add_merchendise(wsbe, strdup("citron"), strdup("en citron"), 7);
  merch_t *merch3 = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)((void *)"citron")).value.p;
  ioopm_replenish(wsbe, merch3, strdup("A36"), 10);
  ioopm_replenish(wsbe, merch3, strdup("B36"), 10);
  
  ioopm_create_cart(wsbe);
  ioopm_add_to_cart(wsbe, 1, "äpple", 15);
  ioopm_add_to_cart(wsbe, 1, "äpple", 5);
  ioopm_add_to_cart(wsbe, 1, "päron", 9);
  ioopm_add_to_cart(wsbe, 1, "citron", 21);
  CU_ASSERT(ioopm_calculate_stock(merch1) == 20);
  CU_ASSERT(ioopm_calculate_stock(merch2) == 20);
  CU_ASSERT(ioopm_calculate_stock(merch3) == 20);
  ioopm_checkout_cart(wsbe, 1);

  CU_ASSERT(ioopm_calculate_stock(merch1) == 0);
  CU_ASSERT(ioopm_calculate_stock(merch2) == 11);
  CU_ASSERT(ioopm_calculate_stock(merch3) == 20);

  ioopm_destroy_webstore_backend(wsbe);
}

int main()
{
  CU_pSuite test_suite_add = NULL;
  CU_pSuite test_suite_remove = NULL;
  CU_pSuite test_suite_edit = NULL;
  CU_pSuite test_suite_calculate = NULL;
  CU_pSuite test_suite_replenish = NULL;
  CU_pSuite test_suite_cart = NULL;
  CU_pSuite test_suite_calc_cost = NULL;
  CU_pSuite test_suite_checkout = NULL;

  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  test_suite_add = CU_add_suite("Test Suite add", init_suite, clean_suite);
  test_suite_remove = CU_add_suite("Test Suite remove", init_suite, clean_suite);
  test_suite_edit = CU_add_suite("Test Suite edit", init_suite, clean_suite);
  test_suite_calculate = CU_add_suite("Test Suite calculate stock", init_suite, clean_suite);
  test_suite_replenish = CU_add_suite("Test Suite replenish", init_suite, clean_suite);
  test_suite_cart = CU_add_suite("Test Suite cart", init_suite, clean_suite);
  test_suite_calc_cost = CU_add_suite("Test Suite calculate cost", init_suite, clean_suite);
  test_suite_checkout = CU_add_suite("Test Suite checkout", init_suite, clean_suite);
  
  if (NULL == test_suite_add)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  if (NULL == test_suite_remove)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  if (NULL == test_suite_edit)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  if (NULL == test_suite_calculate)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  if (NULL == test_suite_replenish)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  if (NULL == test_suite_cart)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  if (NULL == test_suite_calc_cost)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  if (NULL == test_suite_checkout)
    {
      CU_cleanup_registry();
      return CU_get_error();
    }

  if (
      (NULL == CU_add_test(test_suite_add, "test_add_merch_one", test_add_merch_one)) ||
      (NULL == CU_add_test(test_suite_add, "test_add_merch_many", test_add_merch_many)) ||
      (NULL == CU_add_test(test_suite_remove, "test_remove_one", test_remove_one)) ||
      (NULL == CU_add_test(test_suite_remove, "test_remove_wrong", test_remove_wrong)) ||
      (NULL == CU_add_test(test_suite_edit, "test_edit_same_name", test_edit_same_name)) ||
      (NULL == CU_add_test(test_suite_edit, "test_edit_new_name", test_edit_new_name)) ||
      (NULL == CU_add_test(test_suite_edit, "test_edit_existing_name", test_edit_existing_name)) ||
      (NULL == CU_add_test(test_suite_calculate, "test_calculate_empty", test_calculate_empty)) ||
      (NULL == CU_add_test(test_suite_calculate, "test_calculate_one_shelf", test_calculate_one_shelf)) ||
      (NULL == CU_add_test(test_suite_calculate, "test_calculate_many_shelves", test_calculate_many_shelves)) ||
      (NULL == CU_add_test(test_suite_replenish, "test_replenish_new", test_replenish_new)) ||
      (NULL == CU_add_test(test_suite_replenish, "test_replenish_existing", test_replenish_existing)) ||
      (NULL == CU_add_test(test_suite_replenish, "test_replenish_wrong_number", test_replenish_wrong_number)) ||
      (NULL == CU_add_test(test_suite_cart, "test_create_destroy_cart", test_create_destroy_cart)) ||
      (NULL == CU_add_test(test_suite_cart, "test_cart_add_zero", test_cart_add_zero)) ||
      (NULL == CU_add_test(test_suite_cart, "test_cart_add_allowed_amount", test_cart_add_allowed_amount)) ||
      (NULL == CU_add_test(test_suite_cart, "test_cart_add_too_much", test_cart_add_too_much)) ||
      (NULL == CU_add_test(test_suite_cart, "test_cart_add_same_twice", test_cart_add_same_twice)) ||
      (NULL == CU_add_test(test_suite_cart, "test_cart_add_many_carts", test_cart_add_many_carts)) ||
      (NULL == CU_add_test(test_suite_calc_cost, "test_calc_cost_zero", test_calc_cost_zero)) ||
      (NULL == CU_add_test(test_suite_calc_cost, "test_calc_cost_many", test_calc_cost_many)) ||
      (NULL == CU_add_test(test_suite_checkout, "test_checkout_empty", test_checkout_empty)) ||
      (NULL == CU_add_test(test_suite_checkout, "test_checkout_one_item", test_checkout_one_item)) ||
      (NULL == CU_add_test(test_suite_checkout, "test_checkout_many_items", test_checkout_many_items))
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
