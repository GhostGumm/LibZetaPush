#include "HashMaps.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct _test {char *column; char *test;}test;


void GoBack(HashMaps **head)
{
  while ((*head)->prev != NULL)
    (*head) = (*head)->prev;
}

void GoFront(HashMaps **head)
{
  while ((*head)->next != NULL)
    (*head) = (*head)->next;
}

int  GetTotalElements(HashMaps *head)
{
  GoFront(&head);
  
  return (head->index);
}

HashMaps  *AddKeyValue(HashMaps *head)
{
  HashMaps *bound = malloc(sizeof(HashMaps));
  bound->key = NULL;
  bound->value = NULL;
  bound->type = NULL;
  bound->value = NULL;

  static int i;
  
  bound->index = ++i;
    
  if (head->prev == NULL)
    {
      head->next = bound;
      head->next->prev = head;
      bound->next = NULL;
      head = bound;
    }
  else
    {
      head->next = bound;
      head->next->prev = head;
      bound->next = NULL;
       head = bound;
     }
  
  return (head);
}

HashMaps    *GetValueFromKey(char *key, HashMaps *head)
{
  void *value;

  while (head->prev != NULL)
    head = head->prev;
  while(head->next != NULL)
    {
      if (strcmp(key, head->key) == 0)
	return head;
      head = head->next;
    }
  return (NULL);
}

bool    IfKeyExist(char *key, HashMaps *head)
{
  GoBack(&head);
  
  while(head->next != NULL)
    {
      if (strcmp(key, head->key) == 0)
	{
	  GoFront(&head);
	  return true;
	}
      head = head->next;
    }
  return (false);
  
}

HashMaps  *HashMapInit()
{
  HashMaps *init;
  init = malloc(sizeof(HashMaps));
  init->key = NULL;
  init->value = NULL;
  init->type = NULL;
  init->index = 0;
  
  init->next = NULL;
  init->prev = NULL;
  
  return (init);  
}

void testM(HashMaps **head)
{
  GoBack(head);
  while ((*head)->next != NULL)
    {
      void *test = (*head)->value;
      struct _test *toto = (struct _test *)test;
      printf("index = %d\n column = %s\n  test = %s\n", (*head)->index, toto->column, toto->test);
      
      (*head) = (*head)->next;
    }
}
