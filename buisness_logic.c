#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "buisness_logic.h"

static void destroy_merch(merch_t *merch)
{
  ioopm_linked_list_destroy(merch->list);
  free(merch);
}

/// @brief compare function for merches used by the hash table functions
/// this function doesn't check that the linked lists are the same but that will never happen since two merches can't have the same name
/// (which makes this function useless anyway, could just use compare_string_element)
static bool compare_merch_element(elem_t a, elem_t b)
{ 
  merch_t *merch1 = a.p;
  merch_t *merch2 = b.p;
  return (!strcmp(merch1->name, merch2->name) &&
      !strcmp(merch1->desc, merch2->desc) &&
      merch1->price == merch2->price);
}

/// @brief compare function for shelves used by the hash table functions
static bool compare_shelf_element(elem_t a, elem_t b)
{
  shelf_t *shelf1 = a.p;
  shelf_t *shelf2 = b.p;
  return (!strcmp(shelf1->shelf_num, shelf2->shelf_num) &&
          shelf1->amount == shelf2->amount);
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

static void destroy_all_shelves(ioopm_hash_table_t *ht_merch)
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
              shelf_t *shelf = current_link->element.p;
              free(shelf->shelf_num);
              free(shelf);
            }
        }
    }
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

/// since we get names and descriptions through the "ask_question" functions they will be allocated through strdup and thus we need to
/// free them before we destroy the hash_tables with names and descs in them.
static void destroy_all_merch_names_and_descs(ioopm_hash_table_t *ht_merch)
{
  for (int i = 0; i < ht_merch->num_buckets; ++i)
    {
      entry_t *current = &ht_merch->buckets[i];
      while (current->next != NULL)
        {
          current = current->next;
          merch_t *current_merch = current->value.p;
          char *current_name = current_merch->name;
          char *current_desc = current_merch->desc;
          free(current_name);
          free(current_desc);
        }
    }
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

webstore_backend_t *ioopm_create_webstore_backend()
{
  ioopm_hash_table_t *ht_merch = ioopm_hash_table_create(string_hash,
                                                         compare_string_element, compare_merch_element);
  ioopm_hash_table_t *ht_shelf = ioopm_hash_table_create(string_hash,
                                                         compare_string_element, compare_merch_element);
  
  ioopm_hash_table_t *ht_carts = ioopm_hash_table_create(int_hash,
                                                         compare_int_element, compare_int_element);
  webstore_backend_t *new_webstore = calloc(1, sizeof(webstore_backend_t));

  new_webstore->ht_merch = ht_merch;
  new_webstore->ht_shelf = ht_shelf;
  new_webstore->ht_carts = ht_carts;
  new_webstore->cart_num = 1;
  
  return new_webstore;
}

void ioopm_destroy_webstore_backend(webstore_backend_t *wsbe)
{
  destroy_all_shelves(wsbe->ht_merch);
  destroy_all_merch_names_and_descs(wsbe->ht_merch);
  destroy_all_merch(wsbe->ht_merch);
  destroy_all_carts(wsbe->ht_carts);
  ioopm_hash_table_destroy(wsbe->ht_merch);
  ioopm_hash_table_destroy(wsbe->ht_shelf);
  ioopm_hash_table_destroy(wsbe->ht_carts);
  free(wsbe);
}

bool ioopm_add_merchendise(webstore_backend_t *wsbe, char *name, char *desc, int price)
{
  if (Successful(ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)(void *)name)))
    {
      return false;
    }
  merch_t *merch = create_merch(name, desc, price);
  elem_t elem_name = { .p = name };
  elem_t elem_merch = { .p = merch };
  ioopm_hash_table_insert(wsbe->ht_merch, elem_name, elem_merch);
  return true;
}

static int cmpstringp(const void *p1, const void *p2)
{
  return strcmp(* (char * const *) p1, * (char * const *) p2);
}

static void sort_keys(char *keys[], size_t no_keys)
{
  qsort(keys, no_keys, sizeof(char *), cmpstringp);
}

static char **create_array_of_merch(webstore_backend_t *wsbe)
{
  char **array = calloc(1, (wsbe->ht_merch->num_entries)*sizeof(char *));
  int counter = 0;
  for (int i = 0; i < wsbe->ht_merch->num_buckets; ++i)
    {
      entry_t *current = &wsbe->ht_merch->buckets[i];
      while (current->next != NULL)
        {
          current = current->next;
          array[counter] = (char *)current->key.p;
          ++counter;
        }
    }
  sort_keys(array, wsbe->ht_merch->num_entries);
  return array;
}

void ioopm_list_merchendise(webstore_backend_t *wsbe, input_func func)
{
  if (wsbe->ht_merch->num_entries < 1)
    {
      puts("There is no merchendise in the store yet, add some with \"A\"");
      return;
    }
  int counter = 1;
  printf("======Merchendise======\n");
  bool result = true;
  char **array_of_merch = create_array_of_merch(wsbe);
  for (int i = 0; i < wsbe->ht_merch->num_entries && result; ++i)
    {
      printf("%d. %s\n", counter, array_of_merch[counter-1]);
      if (counter > 1 && counter % 20 == 0)
        {
          result = func(wsbe);
        }
      ++counter;
    }
  free(array_of_merch);
  if (!result)
    {
      return;
    }
  int ending = func(wsbe);
  if (ending)
    {
      puts("sorry, no more merchendise to view");
    }
  else
    return;
}

bool ioopm_remove_merchendise(webstore_backend_t *wsbe, merch_t *merch)
{
  if (Unsuccessful(ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)(void *)merch->name)))
    {
      return false;
    }
  link_t *current = merch->list->first;
  while (current->next != NULL)
    {
      current = current->next;
      shelf_t *shelf = current->element.p;
      char *shelf_num = shelf->shelf_num;
      ioopm_hash_table_remove(wsbe->ht_shelf, (elem_t)(void *)shelf_num);
      free(shelf_num);
      free(shelf);
    }
  ioopm_hash_table_remove(wsbe->ht_merch, (elem_t)(void *)merch->name);
  free(merch->name);
  free(merch->desc);
  destroy_merch(merch);
  return true;
}

/// @brief skapa en ny merch, gå igenom gamla merch listan, för varje shelf spara shelfen i den nya merchens lista och byt ut merchen på shelfens plats i ht_shelf, destroya merch stoppa in nya merchen i ht_merch.
bool ioopm_edit_merchendise(webstore_backend_t *wsbe, merch_t *old_merch, char *new_name, char *new_desc, int new_price)
{
  if (!strcmp(old_merch->name, new_name)) //om dom är samma
    {
      free(old_merch->desc);
      free(new_name);
      old_merch->desc = new_desc;
      old_merch->price = new_price;
      return true;
    }
  if (Successful(ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)(void *)new_name)))
    {
      return false;
    }
  merch_t *new_merch = create_merch(new_name, new_desc, new_price);
  link_t *current_link = old_merch->list->first;
  while (current_link->next != NULL)
    {
      current_link = current_link->next;
      shelf_t *current_shelf = current_link->element.p;
      ioopm_linked_list_prepend(new_merch->list, (elem_t)(void *)current_shelf);
      ioopm_hash_table_remove(wsbe->ht_shelf, (elem_t)(void *)current_shelf->shelf_num);
      ioopm_hash_table_insert(wsbe->ht_shelf, (elem_t)(void *)current_shelf->shelf_num, (elem_t)(void *)new_merch);
    }
  ioopm_hash_table_remove(wsbe->ht_merch, (elem_t)(void *)old_merch->name);
  free(old_merch->name);
  free(old_merch->desc);
  ioopm_hash_table_insert(wsbe->ht_merch, (elem_t)(void *)new_merch->name, (elem_t)(void *)new_merch);
  destroy_merch(old_merch);
  return true;
}

int ioopm_calculate_stock(merch_t *merch)
{
  int counter = 0;
  link_t *current_link = merch->list->first;
  while (current_link->next != NULL)
    {
      current_link = current_link->next;
      shelf_t *shelf = current_link->element.p;
      counter = counter + shelf->amount;
    }
  return counter;
}

/// Brutit ner påfyllnads algortimen i ioopm_replenish
static void replenish_stock(merch_t *merch, char *shelf_num, size_t amount)
{
  link_t *current = merch->list->first;
  while (current->next != NULL)
    {
      current = current->next;
      shelf_t *current_shelf = current->element.p;
      if (!strcmp(current_shelf->shelf_num, shelf_num))
        {
          shelf_t *shelf = current->element.p;
          shelf->amount = shelf->amount + amount;
          current->element = (elem_t)(void *)shelf;
        }
    }
}

/* @brief Kollar om shelfen finns i hash tabellen och om den tillhör "vår" merch, gör den inte det
   men finns så returnar den false för då måste man ge en ny shelf.
   Fanns den och tillhörde vår merch fyller vi på med så mycket amount vi behöver
   Fanns den inte gör vi en ny shelf i både ht_shelf och hos merchens lista
*/
bool ioopm_replenish(webstore_backend_t *wsbe, merch_t *merch, char *shelf_num, size_t amount)
{
  if (amount == 0)
    {
      puts("You have to replenish with more than 0");
      free(shelf_num);
      return false;
    }
  if (ioopm_hash_table_has_key(wsbe->ht_shelf, (elem_t)(void *)shelf_num) && 
      compare_merch_element(ioopm_hash_table_lookup(wsbe->ht_shelf, (elem_t)(void *)shelf_num).value, (elem_t)(void *)merch))
    {
      replenish_stock(merch, shelf_num, amount);
      free(shelf_num);
      return true;  
    }
  if (ioopm_hash_table_has_key(wsbe->ht_shelf, (elem_t)(void *)shelf_num))
    {
      puts("There is another merchendise on that shelf right now, try another shelf");
      free(shelf_num);
      return false;
    }
  ioopm_hash_table_insert(wsbe->ht_shelf, (elem_t)(void *)shelf_num, (elem_t)(void *)merch);
  shelf_t *new_shelf = calloc(1, sizeof(shelf_t));
  new_shelf->shelf_num = shelf_num;
  new_shelf->amount = amount;
  ioopm_linked_list_prepend(merch->list, (elem_t)(void *)new_shelf);
  return true;
}

int ioopm_create_cart(webstore_backend_t *wsbe)
{
  ioopm_hash_table_t *new_cart = ioopm_hash_table_create(string_hash, compare_string_element, compare_int_element);
  ioopm_hash_table_insert(wsbe->ht_carts, (elem_t)wsbe->cart_num, (elem_t)(void *)new_cart);
  ++wsbe->cart_num;
  return wsbe->cart_num-1;
}

void ioopm_remove_cart(webstore_backend_t *wsbe, int cart_num)
{
  ioopm_hash_table_t *ht_to_destroy = ioopm_hash_table_lookup(wsbe->ht_carts, (elem_t)cart_num).value.p;
  ioopm_hash_table_destroy(ht_to_destroy);
  ioopm_hash_table_remove(wsbe->ht_carts, (elem_t)cart_num);
}

/// @brief calculates the amount of a merch in all of the shopping carts in a hash table
static int calc_amount_saved_in_carts(ioopm_hash_table_t *ht, char *merch_name)
{
  int amount_in_carts = 0;
  for (int i = 0; i < ht->num_buckets; ++i)
    {
      entry_t *current = &ht->buckets[i];
      while (current->next != NULL)
        {
          current = current->next;
          ioopm_hash_table_t *cart = current->value.p;
          option_t lookup = ioopm_hash_table_lookup(cart, (elem_t)(void *)merch_name);
          if (Successful(lookup))
            {
              amount_in_carts = amount_in_carts + lookup.value.i;
            }
        }
    }
  return amount_in_carts;
}

/// @brief calculates the amount of merch stored on all shelves connected to that merch
static int calc_amount_stored(merch_t *merch)
{
  int amount_stored = 0;
  link_t *link = merch->list->first;
  while(link->next != NULL)
    {
      link = link->next;
      shelf_t *shelf = link->element.p;
      amount_stored = amount_stored + shelf->amount;
    }
  return amount_stored;
}

bool ioopm_add_to_cart(webstore_backend_t *wsbe, int cart_num, char *merch_name, size_t amount)
{
  if (amount == 0)
    {
      puts("you can't add 0 to your cart\n");
      free(merch_name);
      return false;
    }
  int amount_saved_in_carts = calc_amount_saved_in_carts(wsbe->ht_carts, merch_name);
  
  merch_t *merch = ioopm_hash_table_lookup(wsbe->ht_merch,(elem_t)(void *)merch_name).value.p;
  int amount_stored = calc_amount_stored(merch);
  
  if (amount+amount_saved_in_carts > amount_stored)
    {
      printf("You can only add a maximum of %d %s\'s to you cart, try again\n", amount_stored-amount_saved_in_carts, merch_name);
      return false;
    }
  ioopm_hash_table_t *our_cart = ioopm_hash_table_lookup(wsbe->ht_carts, (elem_t)cart_num).value.p;
  option_t lookup = ioopm_hash_table_lookup(our_cart, (elem_t)(void *)merch_name);
  // checks if we already had the merch in the shopping cart
  if (Successful(lookup))
    {
      amount = amount + lookup.value.i;
      ioopm_hash_table_insert(our_cart, (elem_t)(void *)merch_name, (elem_t)(int)amount);
      return true;
    }
  ioopm_hash_table_insert(our_cart, (elem_t)(void *)merch_name, (elem_t)(int)amount);
  return true;
}

int ioopm_calculate_cost(webstore_backend_t *wsbe, int cart_num)
{
  ioopm_hash_table_t *cart = ioopm_hash_table_lookup(wsbe->ht_carts, (elem_t)cart_num).value.p;
  int total_amount = 0;
  for (int i = 0; i < cart->num_buckets; ++i)
    {
      entry_t *current = &cart->buckets[i];
      while (current->next != NULL)
        {
          current = current->next;
          char *merch_name = current->key.p;
          merch_t *merch = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)(void *)merch_name).value.p;
          total_amount = total_amount + merch->price * current->value.i;
        }
    }
  return total_amount;
}

/* @brief gå igenom carten, för varje entry, spara antalet varor och namnet, gå in i ht_merch för det namnet och ta bort x antal varor, överstiger antalet antalet som finns på första hyllan, gå till nästa och ta bort dom istället. frigör carten och remova från ht_carts
 */
void ioopm_checkout_cart(webstore_backend_t *wsbe, int cart_num)
{
  ioopm_hash_table_t *cart = ioopm_hash_table_lookup(wsbe->ht_carts, (elem_t)cart_num).value.p;
  for (int i = 0; i < cart->num_buckets; ++i)
    {
      entry_t *current = &cart->buckets[i];
      while (current->next != NULL)
        {
          current = current->next;
          int tot_amount = current->value.i;
          merch_t *merch = ioopm_hash_table_lookup(wsbe->ht_merch, current->key).value.p;
          link_t *link = merch->list->first;
          while (link->next != NULL)
            {
              link = link->next;
              shelf_t *shelf = link->element.p;
              if (shelf->amount >= tot_amount)
                {
                  shelf->amount = shelf->amount - tot_amount;
                  link = merch->list->last; //makes the while loop break
                }
              else
                {
                  tot_amount = tot_amount - shelf->amount;
                  shelf->amount = 0;
                }
            }
        }
    }
  ioopm_hash_table_destroy(cart);
  ioopm_hash_table_remove(wsbe->ht_carts, (elem_t)cart_num);
}
