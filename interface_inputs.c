#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "interface_inputs.h"

static int cmpstringp(const void *p1, const void *p2)
{
  return strcmp(* (char * const *) p1, * (char * const *) p2);
}

static void sort_keys(char *keys[], size_t no_keys)
{
  qsort(keys, no_keys, sizeof(char *), cmpstringp);
}

static char **create_array_of_keys(ioopm_hash_table_t *ht)
{
  char **array = calloc(1, (ht->num_entries)*sizeof(char *));
  int counter = 0;
  for (int i = 0; i < ht->num_buckets; ++i)
    {
      entry_t *current = &ht->buckets[i];
      while (current->next != NULL)
        {
          current = current->next;
          array[counter] = (char *)current->key.p;
          ++counter;
        }
    }
  sort_keys(array, ht->num_entries);
  return array;
}

/// hjälpfunktion för att få fram vilken merch som tillhör vilket nummer
static merch_t *check_list(webstore_backend_t *wsbe, int merch_number)
{
  while (merch_number > wsbe->ht_merch->num_entries || merch_number < 1)
    {
      merch_number = ask_question_int("There is no merch connected to that number! Try again");
    }
  char **array_of_merch = create_array_of_keys(wsbe->ht_merch);
  char *our_merch_name = array_of_merch[merch_number-1];
  merch_t *our_merch = ioopm_hash_table_lookup(wsbe->ht_merch, (elem_t)(void *)our_merch_name).value.p;
  free(array_of_merch);
  return our_merch;
}

/// hjälpfunktion som printar alla carts man kan välja bland
static bool print_valid_carts(webstore_backend_t *wsbe)
{
  if (wsbe->ht_carts->num_entries == 0)
    {
      puts("There are no carts to work with");
      return false;
    }
  int all_carts[wsbe->ht_carts->num_entries];
  int counter = 0;
  for (int i = 0; i < wsbe->ht_carts->num_buckets; ++i)
    {
      entry_t *current = &wsbe->ht_carts->buckets[i];
      while (current->next != NULL)
        {
          current = current->next;
          int cart_num = current->key.i;
          all_carts[counter] = cart_num;
          ++counter;
        }
    }
  printf("==========================\n"
         "valid cart numbers are: [");
  for (int i = 0; i < wsbe->ht_carts->num_entries; ++i)
    {
      printf("%d, ", all_carts[i]);
    }
  printf("]\n");
  return true;
}

static bool list_merch_choice(char *command, char *input)
{
  char *yes_or_no = ask_question_menu(command, input);
  if (yes_or_no[0] == 'Y' || yes_or_no[0] == 'y' || yes_or_no[0] == 'M' || yes_or_no[0] == 'm')
    {
      free(yes_or_no);
      return true;
    }
  if (yes_or_no[0] == 'N' || yes_or_no[0] == 'n' || yes_or_no[0] == 'E' || yes_or_no[0] == 'e' ||
      yes_or_no[0] == 'R' || yes_or_no[0] == 'r' || yes_or_no[0] == 'S' || yes_or_no[0] == 's' ||
      yes_or_no[0] == 'P' || yes_or_no[0] == 'p' || yes_or_no[0] == 'D' || yes_or_no[0] == 'd')
    {
      free(yes_or_no);
      return false;
    }
  free(yes_or_no);
  return true;
}

void interface_add_merch(webstore_backend_t *wsbe)
{
  bool loop_number;
  do
    {
      char *new_name = ask_question_string("What is the name for your new merchendise?");
      char *new_desc = ask_question_string("What is the description for your new merchendise?");
      int new_price = ask_question_int("What is the price for your new merchendise?");
      if (ioopm_add_merchendise(wsbe, new_name, new_desc, new_price) == false)
        {
          free(new_name);
          free(new_desc);
          printf("That merch already exists!");
          loop_number = false;
        }
      else
        {
          loop_number = true;
        }
    }
  while (loop_number == false);
}

static bool list_merch_input(webstore_backend_t *unused_wsbe)
{
  return list_merch_choice("Would you like to view more [Y]es or [N]o", "YyNn");
}

void interface_list_merch(webstore_backend_t *wsbe)
{
  ioopm_list_merchendise(wsbe, list_merch_input);
}


static bool remove_merch_input(webstore_backend_t *wsbe)
{
  if (list_merch_choice("Would you like to view [m]ore or [r]emove merch", "MmRr"))
    {
      return true;
    }
  int merch_number = ask_question_int("What merch would you like to delete?");
  merch_t *our_merch = check_list(wsbe, merch_number);
  ioopm_remove_merchendise(wsbe, our_merch);
  return false;
}

void interface_remove_merch(webstore_backend_t *wsbe)
{
  ioopm_list_merchendise(wsbe, remove_merch_input);
}

static bool edit_merch_input(webstore_backend_t *wsbe)
{
  if (list_merch_choice("Would you like to view [m]ore or [e]dit merch", "MmEe"))
    {
      return true;
    }
  bool result;
  int merch_number = ask_question_int("What merch would you like to edit?");
  do
    {
      merch_t *our_merch = check_list(wsbe, merch_number);
      char *new_name = ask_question_string("What is the new name for that merch?");
      char *new_desc = ask_question_string("What is the new description for that merch?");
      int new_price = ask_question_int("What is the new price for that merch?");
      result = ioopm_edit_merchendise(wsbe, our_merch, new_name, new_desc, new_price);
      if (result == false)
        {    
          printf("%s already exists as another merchendise! Try another name \n", new_name);
        }
    }
  while (result == false);
  return false;
}

void interface_edit_merch(webstore_backend_t *wsbe)
{
  ioopm_list_merchendise(wsbe, edit_merch_input);
}

static bool replenish_merch_input(webstore_backend_t *wsbe)
{
  if (list_merch_choice("Would you like to view [m]ore or re[p]lenish merch", "MmPp"))
    {
      return true;
    }
  bool result;
  int merch_number = ask_question_int("What merch would you like to replenish?");
  do
    {
      merch_t *our_merch = check_list(wsbe, merch_number);
      char *merch_shelf = ask_question_item("At what shelf would you like to replenish your merch");
      int amount_to_rep = ask_question_int("How many would you like to add?");
      result = ioopm_replenish(wsbe, our_merch, merch_shelf, amount_to_rep);
    }
  while (result == false);
  return false;

}

void interface_replenish_merch(webstore_backend_t *wsbe)
{
  ioopm_list_merchendise(wsbe, replenish_merch_input);
}

bool show_stock_input(webstore_backend_t *wsbe)
{
  if (list_merch_choice("Would you like to view [m]ore or [s]how stock", "MmSs"))
    {
      return true;
    }
  int merch_stock_number = ask_question_int("What merch would you like to see the stock on");
  merch_t *our_merch = check_list(wsbe, merch_stock_number);

  link_t *current_link = our_merch->list->first; 
  if (current_link->next == NULL)
    {
      printf("There are no shelves connected to %s, its stock is: 0\n", our_merch->name);
      return false;
    }
  puts("==========Shelves=========");
  while (current_link->next != NULL)
    {
      current_link = current_link->next;
      shelf_t *shelf = current_link->element.p;
      printf("%s: %d\n", shelf->shelf_num, shelf->amount);
    }
  int total = ioopm_calculate_stock(our_merch);
  printf("Total: %d\n", total);
  return false;
}

void interface_show_stock(webstore_backend_t *wsbe)
{
  ioopm_list_merchendise(wsbe, show_stock_input);
}

void interface_create_cart(webstore_backend_t *wsbe)
{
  int cart = ioopm_create_cart(wsbe);
  printf("Cart number \"%d\" has been created\n", cart);
}


void interface_remove_cart(webstore_backend_t *wsbe)
{
  if (!print_valid_carts(wsbe))
    {
      return;
    }
  int cart_remove = ask_question_int("Which cart would you like to remove?");
  while (Unsuccessful(ioopm_hash_table_lookup(wsbe->ht_carts, (elem_t)cart_remove)))
    {
      cart_remove = ask_question_int("No cart with that number, try again:");
    }
  ioopm_remove_cart(wsbe, cart_remove);
}

static int add_to_cart_input(webstore_backend_t *wsbe)
{
  /// Det här printar och frågar vilken cart du vill ha till du skriver in en som finns.
  print_valid_carts(wsbe);
  int cart = ask_question_int("What cart would you like to add the merch to?");
  while (Unsuccessful(ioopm_hash_table_lookup(wsbe->ht_carts, (elem_t)cart)))
    {
      cart = ask_question_int("No cart with that number, try again:");
    }
  return cart;
}

static bool add_to_cart_merch_input(webstore_backend_t *wsbe)
{
  /// Det här frågar vilket merch du vill ha och om merchen du skrev in finns i systemet tills du skriver in rätt.
  if (list_merch_choice("Would you like to view [m]ore or a[d]d to cart", "MmDd"))
    {
      return true;
    }
  int merch_num = ask_question_int("What merch would you like to add to your cart?");
  merch_t *merch = check_list(wsbe, merch_num);
  if (ioopm_calculate_stock(merch) == 0)
    {
      puts("There is no stock for that merch, add some with \"p\" and try again");
      return false;
    }
  bool result;
  int cart_num = add_to_cart_input(wsbe);
  do
    {
      printf("How many %s\'s would you like to add to your cart?\n", merch->name);
      int amount = ask_question_int("(more than 1 and less than the stock of the merch)");
      result = ioopm_add_to_cart(wsbe, cart_num, merch->name, amount);
    }
  while(!result);
  return false;
}

void interface_add_to_cart(webstore_backend_t *wsbe)
{
  if (wsbe->ht_carts->num_entries == 0)
    {
      puts("There are no carts to work with");
      return;
    }
  ioopm_list_merchendise(wsbe, add_to_cart_merch_input);
}

void interface_calculate_cost_for_cart(webstore_backend_t *wsbe)
{
  if (!print_valid_carts(wsbe))
    {
      return;
    }
  int cart_num = ask_question_int("Which cart do you want to check the price for?");
  while (Unsuccessful(ioopm_hash_table_lookup(wsbe->ht_carts, (elem_t)cart_num)))
    {
      cart_num = ask_question_int("No cart with that number, try again:");
    }
  int cost = ioopm_calculate_cost(wsbe, cart_num);
  printf("Cart number %d costs %dkr\n", cart_num, cost);
  return;
}

void interface_checkout_cart(webstore_backend_t *wsbe)
{
  if (!print_valid_carts(wsbe))
    {
      return;
    }
  int cart_num = ask_question_int("Which cart would you like to check out?");
  while (Unsuccessful(ioopm_hash_table_lookup(wsbe->ht_carts, (elem_t)cart_num)))
    {
      cart_num = ask_question_int("No cart with that number, try again:");
    }
  int cost = ioopm_calculate_cost(wsbe, cart_num);
  ioopm_checkout_cart(wsbe, cart_num);
  printf("Cart number %d has been checked out! That'll be %dkr please\n", cart_num, cost);
}
