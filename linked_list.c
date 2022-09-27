#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "linked_list.h"


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

ioopm_list_t *ioopm_linked_list_create(ioopm_eq_function eq_func)
{
  ioopm_list_t *result = calloc(1, sizeof(ioopm_list_t));
  link_t *dummy = link_create((elem_t)0, NULL);
  result->first = dummy;
  result->last = dummy;
  result->eq_func = eq_func;
  return result;
}

void ioopm_linked_list_destroy(ioopm_list_t *list)
{
  ioopm_linked_list_clear(list);
  free(list->first);
  free(list);
}

void ioopm_linked_list_append(ioopm_list_t *list, elem_t value)
{
  link_t *new = link_create(value, NULL);
  list->last->next = new;
  list->last = new;
}

void ioopm_linked_list_prepend(ioopm_list_t *list, elem_t value)
{
  link_t *new_link = link_create(value, list->first->next);
  list->first->next = new_link;
  if (new_link->next == NULL)
    {
      list->last = new_link;
    }
}

void ioopm_linked_list_insert(ioopm_list_t *list, int index, elem_t value)
{
  link_t *current_link = list->first;
  for (int i = 0; i < index; ++i)
    {
      current_link = current_link->next;
    }
  link_t *new_link = link_create(value, current_link->next);
  current_link->next = new_link;
  if (new_link->next == NULL)
    {
      list->last = new_link; 
    }
}

elem_t ioopm_linked_list_remove(ioopm_list_t *list, int index)
{
  link_t *prev_link = list->first;
  link_t *link_to_be_removed = prev_link->next;
  for (int i = 0; i < index; ++i)
    {
      prev_link = prev_link->next;
      link_to_be_removed = link_to_be_removed->next;
    }
  elem_t value_of_removed_link = link_to_be_removed->element;
  prev_link->next = link_to_be_removed->next;
  if(prev_link->next == NULL)
    {
      list->last = prev_link;
    }
  free(link_to_be_removed);
  return value_of_removed_link;
}

elem_t ioopm_linked_list_get(ioopm_list_t *list, int index)
{
  link_t *current_link = list->first->next;
  for (int i = 0; i < index; ++i)
    {
      current_link = current_link->next;
    }
    return current_link->element;
}

bool ioopm_linked_list_contains(ioopm_list_t *list, elem_t element)
{
  link_t *current_link = list->first;
  while (current_link->next != NULL && !list->eq_func(current_link->element, element))
    {
      current_link = current_link->next;
    }
  if (list->eq_func(current_link->element, element))
    {
      return true;
    }
  return false;
}

size_t ioopm_linked_list_size(ioopm_list_t *list)
{
  link_t *current_link = list->first;
  size_t counter = 0;
  while (current_link->next != NULL)
    {
      current_link = current_link->next;
      ++counter;
    }
  return counter;
}

bool ioopm_linked_list_is_empty(ioopm_list_t *list)
{
  if (list->first->next == NULL)
    {
      return true;
    }
  return false;
}


void ioopm_linked_list_clear(ioopm_list_t *list)
{
  link_t *current_link = list->first;
  if (current_link->next != NULL)
    {  
      current_link = current_link->next;
      while(current_link->next != NULL)
        {
          link_t *remove_link = current_link;
          current_link = current_link->next;
          free(remove_link);
        }
      list->first->next = NULL;
      free(current_link);
    }
  list->last = list->first;
}

bool ioopm_linked_list_all(ioopm_list_t *list, ioopm_predicate prop, elem_t extra)
{
  if(ioopm_linked_list_is_empty(list))
    {
      return false;
    }
  for(link_t *curr_link = list->first->next; curr_link != NULL; curr_link = curr_link->next)
    {
      if(!prop((elem_t)0, curr_link->element, extra, list->eq_func)) // first element to prop is unused
        {
          return false;
        }
    }
  return true;
}

bool ioopm_linked_list_any(ioopm_list_t *list, ioopm_predicate prop, elem_t extra)
{
  if(ioopm_linked_list_is_empty(list))
    {
      return false;
    }
  for(link_t *curr_link = list->first->next; curr_link != NULL; curr_link = curr_link->next)
    {
      if (prop((elem_t)0, curr_link->element, extra, list->eq_func))
        {
          return prop((elem_t)0,curr_link->element, extra, list->eq_func);
        }
    }
  return false;
}


void ioopm_linked_apply_to_all(ioopm_list_t *list, ioopm_apply_function fun, elem_t extra)
{
  link_t *current = list->first;
  if (current->next != NULL)
    {
      do
        {
          current = current->next;
          fun((elem_t)0, &current->element, extra);
        }
      while (current->next != NULL);
    }
}

//=======================================================
// iterator functions below

ioopm_list_iterator_t *ioopm_list_iterator(ioopm_list_t *list)
{
  ioopm_list_iterator_t *result = calloc(1, sizeof(struct iter));
  result->current = list->first;
  result->list = list;
  return result;  
}

bool ioopm_iterator_has_next(ioopm_list_iterator_t *iter)
{
  return iter->current->next->next != NULL;
}

elem_t ioopm_iterator_next(ioopm_list_iterator_t *iter)
{
  if (ioopm_iterator_has_next(iter))
    {
      iter->current = iter->current->next;
    }
  return iter->current->next->element;
}

elem_t ioopm_iterator_remove(ioopm_list_iterator_t *iter)
{
  if (iter->list->first->next == iter->list->last)
    {
      return iter->current->next->element;
    }
  if (ioopm_iterator_has_next(iter))
    {
      link_t *to_remove = iter->current->next;
      iter->current->next = to_remove->next;
      
      elem_t to_return = to_remove->element;
      free(to_remove);
      return to_return;
    }
  elem_t to_return = iter->current->next->element;
  free(iter->current->next);
  
  iter->current->next = NULL;
  iter->list->last = iter->current;
  iter->current = iter->list->first;
  
  return to_return;
}

void ioopm_iterator_insert(ioopm_list_iterator_t *iter, elem_t element)
{
  link_t *new_link = link_create(element, iter->current->next);
  iter->current->next = new_link;
  iter->current = new_link;
}

void ioopm_iterator_reset(ioopm_list_iterator_t *iter)
{
  iter->current = iter->list->first;
}

elem_t ioopm_iterator_current(ioopm_list_iterator_t *iter)
{
  return iter->current->next->element;
}

void ioopm_iterator_destroy(ioopm_list_iterator_t *iter)
{
  free(iter);
}
