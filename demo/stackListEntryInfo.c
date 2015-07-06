#include <stdio.h>
#include <stdlib.h>

#include "stackListingInfo.h"
#include "ListingEntryInfo.h"



StackListing  *StackListingAdd(StackListing *head)
{
  StackListing *bound = StackInit(bound);
  
  static int i;
  
  bound->index = ++i;
  bound->owner = head->owner;
  bound->stack = head->stack;
  bound->total = head->total;
  
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

StackListing   *StackInit(StackListing *init)
{
  /*  ------------- Access Struct  -------------  */
  init = malloc(sizeof(StackListing));
  init->index = 0;
  init->total = 0;
  init->metadata = HashMapInit();

  init->next = NULL;
  init->prev = NULL;

  return (init);
}

void	stack_json_null(json_t *element, int indent, StackListing **ls) {
  (void)element;
  print_json_indent(indent);
  printf("JSON Null\n");
}

void	stack_json_object(json_t *element, int indent, StackListing **ls) {
  size_t size;
  const char *key;
  json_t *value;
  print_json_indent(indent);
  size = json_object_size(element);
  
  printf("JSON Object of %ld pairs\n", size);
  json_object_foreach(element, key, value) {
    print_json_indent(indent + 2);
    printf("JSON Key: \"%s\"\n", key);
    check_stack_type(value, indent + 2, ls, key);
  }
}

void	stack_json_array(json_t *element, int indent, StackListing **ls, const char *key) {
  size_t i;
  size_t size = json_array_size(element);

  print_json_indent(indent);
  printf("JSON Array of %ld element\n", size);
  for (i = 0; i < size; i++) {
    if (strcmp(key, "tags") != 0)
      (*ls) = StackListingAdd((*ls));
    check_stack_type(json_array_get(element, i), indent + 2, ls, key);
  }
}

void  AssignStackStringValue(json_t *element, const char *key, StackListing **ls)
{
  if (strcmp(key, "owner") == 0 && (*ls)->owner == NULL) {
    (*ls)->owner = json_string_value(element);return;}
  if (strcmp(key, "stack") == 0 && (*ls)->stack == NULL){
    (*ls)->stack = json_string_value(element);return;}
  else
    {
      (*ls)->metadata->key = key;
      (*ls)->metadata->type = "string";
      (*ls)->metadata->value = (void*)json_string_value(element);
      (*ls)->metadata = AddKeyValue((*ls)->metadata);
      return;
    }
}

void  AssignStackIntegerValue(json_t *element, const char *key, StackListing **ls)
{
  if (strcmp(key, "totalElements") == 0 && (*ls)->total == 0) {
    (*ls)->total = json_number_value(element);return;}
  if (strcmp(key, "pageSize") == 0 && (*ls)->total == 0) {
    (*ls)->pageSize = json_number_value(element);return;}
  if (strcmp(key, "pageNumber") == 0 && (*ls)->total == 0) {
    (*ls)->pageNumber = json_number_value(element);return;}
  if (strcmp(key, "offset") == 0 && (*ls)->total == 0) {
    (*ls)->offset = json_number_value(element);return;}

  (*ls)->metadata->key = key;
  (*ls)->metadata->type = "integer";
  (*ls)->metadata->value = (void*)json_integer_value(element);
  (*ls)->metadata = AddKeyValue((*ls)->metadata);
  return;
  }
  

void stack_json_integer(json_t *element, int indent, StackListing **ls, const char *key) {
  print_json_indent(indent);
  AssignStackIntegerValue(element, key, ls);
  printf("JSON Integer: \"%" JSON_INTEGER_FORMAT "\"\n", json_integer_value(element));
}

void	stack_json_string(json_t *element, int indent, StackListing **ls, const char *key) {
  print_json_indent(indent);
  AssignStackStringValue(element, key, ls);
  printf("JSON String : ");
  puts(json_string_value(element));
  //printf("JSON String: \"%s\"\n", carrier);
}

void	check_stack_type(json_t* element, int indent, StackListing **ls, const char *key)
{
  switch (json_typeof(element)) {
  case JSON_OBJECT:
    stack_json_object(element, indent, ls);
    break;
  case JSON_ARRAY:
    stack_json_array(element, indent, ls, key);
    break;
  case JSON_STRING:
    stack_json_string(element, indent, ls, key);
    break;
  case JSON_INTEGER:
    stack_json_integer(element, indent, ls, key);
    break;
  case JSON_REAL:
    list_json_real(element, indent);
    break;
  case JSON_TRUE:
    list_json_true(element, indent);
    break;
  case JSON_FALSE:
    list_json_false(element, indent);
    break;
  case JSON_NULL:
    stack_json_null(element, indent, ls);
    break;
  default:
    fprintf(stderr, "unrecognized JSON type %d\n", json_typeof(element));
  }
  
}

void  printStack(StackListing **ls)
{
  int i = 0;
  while ((*ls)->prev)
    (*ls) = (*ls)->prev;
  
  while ((*ls))
    {
      while ((*ls)->metadata)
	{
	  if ((*ls)->metadata->type != NULL){
	    if (strcmp((*ls)->metadata->type, "string") == 0){
	      printf("-----> :");
	      void *carrier = (*ls)->metadata->value;
	      printf("key = %s\n", (*ls)->metadata->key);
	      printf("string = %s\n", (char *)carrier);
	    }
	    else
	      {
		printf("-----> :");
		void *carrier = (*ls)->metadata->value;
		printf("key = %s\n", (*ls)->metadata->key);
		printf("string = %d\n", (int)carrier);
	      }
	  }
	  (*ls)->metadata = (*ls)->metadata->prev;
	}
      printf("\n\n --------- new Object -------- ");
      printf("\n stack : %s\n Owner : %s\n", (*ls)->owner, (*ls)->stack);
      if ((*ls)->total != 0)
	printf("\n total : %d\n", (*ls)->total);
      (*ls) = (*ls)->next;
    }
}

StackListing   *StackListParsingInfo(cometd_message *message)
{
  char *key;
  json_t* source = message->source;
  StackListing *ls = StackInit(ls);
  StackListing **Ls = &ls;

  check_stack_type(source, 1, Ls, key);
  //printStack(Ls);

  return ((*Ls));
}
