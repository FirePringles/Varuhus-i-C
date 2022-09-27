#ifndef __ITERATOR_H__
#define __ITERATOR_H__

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "common.h"

typedef struct iter ioopm_list_iterator_t;

/// @brief Create an iterator for a given list, where the list has more than 0 links
/// @param the list to be iterated over
/// @return an iteration positioned at the start of list 
ioopm_list_iterator_t *ioopm_list_iterator(ioopm_list_t *list);

/// @brief Checks if there are more elements to iterate over
/// @param iter the iterator
/// @return true if there is a link after the link that the iterator is curretly on
bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter);

/// @brief Step the iterator forward one step unless there is no step, then it stays at the current step.
/// @param iter the iterator
/// @return the next element unless there was no next element, in that case it returns the current element.
elem_t ioopm_iterator_next(ioopm_list_iterator_t *iter);

/// @brief Remove the current element from the underlying list and step the iterator forward
/// If the element the iterator is on is the only one in the list nothing will be removed.
/// If the element the iterator removed was the last one in the list the iterator will be placed at the start of the list.
/// @param iter the iterator
/// @return the removed element if one was removed, otherwise returns the element the iterator is on.
elem_t ioopm_iterator_remove(ioopm_list_iterator_t *iter);

/// @brief Insert a new element into the underlying list making the current element it's next
/// @param iter the iterator
/// @param element the element to be inserted
void ioopm_iterator_insert(ioopm_list_iterator_t *iter, elem_t element);

/// @brief Reposition the iterator at the start of the underlying list 
/// @param iter the iterator
void ioopm_iterator_reset(ioopm_list_iterator_t *iter);

/// @brief Return the current element from the underlying list
/// @param iter the iterator
/// @return the current element
elem_t ioopm_iterator_current(ioopm_list_iterator_t *iter);

/// @brief Destroy the iterator and return its resources
/// @param iter the iterator
void ioopm_iterator_destroy(ioopm_list_iterator_t *iter);


#endif
