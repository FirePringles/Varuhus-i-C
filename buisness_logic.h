#ifndef __BUISNESS_LOGIC__
#define __BUISNESS_LOGIC__

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "hash_table.h"

typedef bool(*input_func)(webstore_backend_t *wsbe);

/**
 * @file buisness_logic.h
 * @author Alexander Lahti and Joachim Forsberg
 * @date 10 Oct 2019 - 24 Oct 2019
 * @brief backend functions to a webstore
 */

/// @brief create a new webstore backend
/// @return a new empy webstore backend
webstore_backend_t *ioopm_create_webstore_backend();

/// @brief frees all of the memory associated with the webstore backend
/// @param wsbe the webstore backend we want to destroy
void ioopm_destroy_webstore_backend(webstore_backend_t *wsbe);

/// @brief adds new merchendise to the webstore, only what the merchendise is, does not add stock
/// @param wsbe the webstore we're working with
/// @param name the name of the merchendise we want to add
/// @param desc a description of the merch
/// @param price how much we want our merch to cost
/// @return returns true if name was a didn't already exist and false if it did(if false it doesnt add the merch we wanted to add)
bool ioopm_add_merchendise(webstore_backend_t *wsbe, char *name, char *desc, int price);

/// @brief lists all the merch in the store in intervalls of 20 and applies some function 
/// @param wsbe the webstore we're working with
/// @param func a function which either works on one of the listed items or "flips" to the next page
/// @throw "there is no merchendie, add some with A" if there is no merch in the store
void ioopm_list_merchendise(webstore_backend_t *wsbe, input_func func);

/// @brief removes a merchendise and all of it's stock and stock locations in the webstore 
/// @param wsbe the webstore we're working with
/// @param merch the merch we want to remove from the webstore
/// @return true if the merch existed in the webstore and false if it didn't (in which case nothing is removed)
bool ioopm_remove_merchendise(webstore_backend_t *wsbe, merch_t *merch);

/// @brief edits a merchendise with a new name, desc and price
/// @param wsbe the webstore we're working with
/// @param old_merch the merch we want to edit
/// @param new_name the new name we want to give our merch , may be the same as the last name but not the same as another name of a merch in the webstore
/// @param new_desc the new description of our merch
/// @param new_price the new price of our merch
/// @return returns false if our new name is used by another merch in the webstore (and then it does nothing) otherwise it's true
bool ioopm_edit_merchendise(webstore_backend_t *wsbe, merch_t *old_merch, char *new_name, char *new_desc, int new_price);

/// @brief calculates the total stock on all shelves that have the same merch on them
/// @param merch the merch we want to calculate the stock of
/// @return the total stock of our merch
int ioopm_calculate_stock(merch_t *merch);

/// @brief creates new/more stock for a merchendise on a new or existing shelf
/// @param wsbe the webstore we're working with
/// @param merch the merch we want to add stock for
/// @param shelf_num the shelf number we want to add the stock to (e.g A01 or B34, on letter followed by two digits)
/// @param amount how much stock we want to add to the shelf
/// @return false if we try to add 0 stock or if we try to add stock to a shelf with another merchendise on it, true otherwise
/// @throw "you have to replenish with more than 0" if amount is 0
/// @throw "there is another merchendise on that shelf right now, try another shelf" if we try to add  stock to a shelf with another merch on it
bool ioopm_replenish(webstore_backend_t *wsbe, merch_t *merch, char *shelf_num, size_t amount);

/// @brief creates a shopping cart which we can use to "purchase" merch with
/// @param wsbe the webstore we're working with
/// @return the number our shopping cart got (e.g 1,32,5434) this increases with every new cart created
int ioopm_create_cart(webstore_backend_t *wsbe);

/// @brief removes a shopping cart
/// @param wsbe the webstore we're working with
/// @param cart_num the number of the cart we want to remove
void ioopm_remove_cart(webstore_backend_t *wsbe, int cart_num);

/// @brief adds a certain amount of  merch to a shopping cart
/// @param wsbe the webstore we're working with
/// @param cart_num the number of the cart we want to add the merch to
/// @param merch_name the name of the merch we want to add
/// @param amount the amount of merch we want to add
/// @return false if you try to add 0 of a merch or if you try to add more than what's stored on shelves-in + other carts, true otherwise
/// @throw "you can't add 0 to your cart" if amount is 0 
/// @throw "you can only add a maximum of %d %s s to your cart, try again" if amount is more than what's stored on shelves-in + other carts
bool ioopm_add_to_cart(webstore_backend_t *wsbe, int cart_num, char *merch_name, size_t amount);

/// @brief calculates the cost of a shopping cart
/// @param wsbe the webstore we're working with
/// @param cart_num the number of the cart we want to calculate the cost of
/// @return the cost of all of the merch in the shopping cart
int ioopm_calculate_cost(webstore_backend_t *wsbe, int cart_num);

/// @brief purchases all of the merch in a shopping cart, removes the merch from the webstore and removes the shopping cart
/// @param wsbe the webstore we're working with
/// @param cart_num the number of the cart we want to check out
void ioopm_checkout_cart(webstore_backend_t *wsbe, int cart_num);

#endif
