#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "interface_inputs.h"


void ioopm_print_menu()
{
  printf("==========================\n"
         "[A]dd some merchendise\n"
         "[R]emove some merchendise\n"
         "[E]dit some merchendise\n"
         "[L]ist merchendise\n"
         "[S]how the stock\n"
         "Re[p]lenish\n"
         "[C]reate cart\n"
         "Re[M]ove cart\n"
         "A[D]d to cart\n"
         "C[H]eck price for cart\n"
         "Chec[K]out for cart\n"
         "[Q]uit the program\n"
         "=========================\n");
}


bool choice_consequence(webstore_backend_t *wsbe, char choice)
{
  if (choice == 'A' || choice == 'a')
    {
      interface_add_merch(wsbe);
      return true;
    }
  if (choice == 'R' || choice == 'r')
    {
      interface_remove_merch(wsbe);
      return true;
    }
  if (choice == 'L' || choice == 'l')
    {
      interface_list_merch(wsbe);
      return true;
    }
  if (choice == 'E' || choice == 'e')
    {
      interface_edit_merch(wsbe);
      return true;
    }
  if (choice == 'S' || choice == 's')
    {
      interface_show_stock(wsbe);
      return true;
    }
  if (choice == 'P' || choice == 'p')
    {
      interface_replenish_merch(wsbe);
      return true;
    }
  if (choice == 'C' || choice == 'c')
    {
      interface_create_cart(wsbe);
      return true;
    }
  if (choice == 'M' || choice == 'm')
    {
      interface_remove_cart(wsbe);
      return true;
    }
  if (choice == 'D' || choice == 'd')
    {
      interface_add_to_cart(wsbe);
      return true;
    }
  if (choice == 'H' || choice == 'h')
    {
      interface_calculate_cost_for_cart(wsbe);
      return true;
    }
  if (choice == 'K' || choice == 'k')
    {
      interface_checkout_cart(wsbe);
      return true;
    }
  if (choice == 'Q' || choice == 'q')
    {
      return false;
    }
  return false; //DODGE
}

void event_loop(webstore_backend_t *wsbe)
{
  bool in_loop = true;
  
  do
    {
      ioopm_print_menu();
      char *choice = ask_question_menu("What do you want to do?", "AaRrEeHhKkQqSsDdMmCcLlPp");
      in_loop = choice_consequence(wsbe, choice[0]);
      free(choice);
    }
  while (in_loop == true);
}

int main(void)
{
  webstore_backend_t *wsbe = ioopm_create_webstore_backend();
  printf("~~~~~~~~~~~~~~~~~~~~~~\n"
         "Welcome to the webstore!\n");
  event_loop(wsbe);
  ioopm_destroy_webstore_backend(wsbe);
  return 0;
}
