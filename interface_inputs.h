#ifndef __INTERFACE_INPUTS__
#define __INTERFACE_INPUTS__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "utils.h"
#include "buisness_logic.h"

/**
 * @file interface_inputs.h
 * @author Alexander Lahti och Joachim Forsberg
 * @date 23 Oct 2019
 * @brief Interface functions to a webstore
 */

/// @brief asks for input and from input create a merch
/// @param wsbe the webstore we're working in 
void interface_add_merch(webstore_backend_t *wsbe);

/// @brief list first 20 merches in webstore. 
/// @param wsbe the webstore we're working in
void interface_list_merch(webstore_backend_t *wsbe);

/// @brief first list merch, use input to remove a merch from list 
/// @param wsbe the webstore we're working in
void interface_remove_merch(webstore_backend_t *wsbe);

/// @brief first list merch, edit selected merch from list with new input
/// @param wsbe the webstore we're working in
void interface_edit_merch(webstore_backend_t *wsbe);

/// @brief first list merch, use input to select merch then replenish that merch
/// @param wsbe the webstore we're working in
void interface_replenish_merch(webstore_backend_t *wsbe);

/// @brief first list merch, use input to select merch then show stock of that merch
/// @param wsbe the webstore we're working in
void interface_show_stock(webstore_backend_t *wsbe);

/// @brief creates a new cart with a specific cart number
/// @param wsbe the webstore we're working in
void interface_create_cart(webstore_backend_t *wsbe);

/// @brief first list all carts, use input to select cart then remove that cart
/// @param wsbe the webstore we're working in 
void interface_remove_cart(webstore_backend_t *wsbe);

/// @brief first list all carts, use input to select cart then add merch to that cart
/// @param wsbe the webstore we're working in
void interface_add_to_cart(webstore_backend_t *wsbe);

/// @brief first list all carts, use input to select cart then show total cost of all merch in that cart
/// @param wsbe the webstore we're working in
void interface_calculate_cost_for_cart(webstore_backend_t *wsbe);

/// @brief first list all carts, use input to select cart then checkout that cart
/// @param wsbe the webstore we're working in
void interface_checkout_cart(webstore_backend_t *wsbe);

#endif
