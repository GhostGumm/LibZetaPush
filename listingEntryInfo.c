#include <stdio.h>
#include <stdlib.h>
#include <jansson.h>
#include "ListingEntryInfo.h"

void print_json_indent(int indent) {
  int i;
  for (i = 0; i < indent; i++) { putchar(' '); }
}

const char *json_plural(int count) {
  return count == 1 ? "" : "s";
}

void	check_list_type(json_t* element, int indent, FolderListing **ls, const char *key);

FolderListing  *FolderListingAdd(FolderListing *head)
{
  FolderListing *bound = FolderInit(bound);
  
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

FolderListing   *FolderInit(FolderListing *init)
{
  /*  ------------- Access Struct  -------------  */
  init = malloc(sizeof(FolderListing));

  init->index = 0;
  init->name = NULL;
  init->fileType = NULL;
  init->url = NULL;

  init->metadata = HashMapInit();
  init->tag = NULL;
  init->exists = false;

  init->next = NULL;
  init->prev = NULL;

  return (init);
}

void	list_json_null(json_t *element, int indent, FolderListing **ls) {
  (void)element;
  print_json_indent(indent);
  printf("JSON Null\n");
}

void	list_json_object(json_t *element, int indent, FolderListing **ls) {
  size_t size;
  const char *key;
  json_t *value;
  print_json_indent(indent);
  size = json_object_size(element);
  
  printf("JSON Object of %ld pairs\n", size);
  json_object_foreach(element, key, value) {
    print_json_indent(indent + 2);
    printf("JSON Key: \"%s\"\n", key);
    check_list_type(value, indent + 2, ls, key);
  }
}

void	list_json_array(json_t *element, int indent, FolderListing **ls, const char *key) {
  size_t i;
  size_t size = json_array_size(element);

  print_json_indent(indent);
  printf("JSON Array of %ld element\n", size);
  for (i = 0; i < size; i++) {
    if (strcmp(key, "tags") != 0)
      (*ls) = FolderListingAdd((*ls));
    check_list_type(json_array_get(element, i), indent + 2, ls, key);
  }
}

void  AssignListStringValue(json_t *element, const char *key, FolderListing **ls)
{
  if (strcmp(key, "name") == 0 && (*ls)->name == NULL) {
    (*ls)->name = json_string_value(element);return;}
  if (strcmp(key, "type") == 0 && (*ls)->fileType == NULL){
    (*ls)->fileType = json_string_value(element);return;}
  if (strcmp(key, "url") == 0 && (*ls)->url == NULL){
    (*ls)->url = json_string_value(element);return;}
  if (strcmp(key, "tags") == 0 && (*ls)->tag == NULL){
    (*ls)->tag = json_string_value(element);return;}
  else
    {
      (*ls)->metadata->key = key;
      (*ls)->metadata->type = "string";
      (*ls)->metadata->value = (void*)json_string_value(element);
      (*ls)->metadata = AddKeyValue((*ls)->metadata);
      return;
    }
}

void  AssignListIntegerValue(json_t *element, const char *key, FolderListing **ls)
{
  (*ls)->metadata->key = key;
  (*ls)->metadata->type = "integer";
  (*ls)->metadata->value = (void*)json_integer_value(element);
  (*ls)->metadata = AddKeyValue((*ls)->metadata);
  return;
}


void list_json_integer(json_t *element, int indent, FolderListing **ls, const char *key) {
  print_json_indent(indent);
  AssignListIntegerValue(element, key, ls);
  printf("JSON Integer: \"%" JSON_INTEGER_FORMAT "\"\n", json_integer_value(element));
}

void	list_json_string(json_t *element, int indent, FolderListing **ls, const char *key) {
  print_json_indent(indent);
  AssignListStringValue(element, key, ls);
  printf("JSON String : ");
  puts(json_string_value(element));
  //printf("JSON String: \"%s\"\n", carrier);
}

void    list_json_real(json_t *element, int indent) {
  print_json_indent(indent);
  printf("JSON Real: %f\n", json_real_value(element));
}

void    list_json_true(json_t *element, int indent) {
  (void)element;
  print_json_indent(indent);
  printf("JSON True\n");
}

void    list_json_false(json_t *element, int indent) {
  (void)element;
  print_json_indent(indent);
  printf("JSON False\n");
}

void	check_list_type(json_t* element, int indent, FolderListing **ls, const char *key)
{
  switch (json_typeof(element)) {
  case JSON_OBJECT:
    list_json_object(element, indent, ls);
    break;
  case JSON_ARRAY:
    list_json_array(element, indent, ls, key);
    break;
  case JSON_STRING:
    list_json_string(element, indent, ls, key);
    break;
  case JSON_INTEGER:
    list_json_integer(element, indent, ls, key);
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
    list_json_null(element, indent, ls);
    break;
  default:
    fprintf(stderr, "unrecognized JSON type %d\n", json_typeof(element));
  }
  
}

void  printListLs(FolderListing **ls)
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
      printf("\n name : %s\n fileType : %s\n url : %s\n", (*ls)->name, (*ls)->fileType, (*ls)->url);
      (*ls) = (*ls)->next;
    }
}

FolderListing   *FolderParsingInfo(cometd_message *message)
{
  char *key;
  json_t* source = message->source;
  FolderListing *ls = FolderInit(ls);
  FolderListing **Ls = &ls;

  check_list_type(source, 1, Ls, key);

  return ((*Ls));
}
