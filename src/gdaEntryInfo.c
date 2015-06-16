#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "listingGdaInfo.h"
#include "listingEntryInfo.h"

const char *json_plural_size(int count) {return count == 1 ? "" : "s";}

GdaColumns* GdaColumnsAdd(GdaColumns *head)
{
  GdaColumns *bound = malloc(sizeof(GdaColumns));
  bound->firstName = NULL;
  bound->lastName = NULL;
  bound->key = NULL;
  static int i;
  
  bound->index = i++;
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

GdaList*     InitGda(GdaList *init)
{
  init = malloc(sizeof(GdaList));
  init->owner = NULL;
  init->table = NULL;
  
  init->columns = malloc(sizeof(GdaColumns));
  init->columns->key = NULL;
  init->columns->firstName = NULL;
  init->columns->lastName = NULL;
  init->columns->next = NULL;
  init->columns->prev = NULL;

  init->page = malloc(sizeof(Pageable));
  init->page->size = 0;
  init->page->totalElements = 0;
  
  return (init);
}

void	handle_json_null(json_t *element, int indent, GdaList **gda) {
  (void)element;
  print_json_indent(indent);
  printf("JSON Null\n");
}

void	handle_json_object(json_t *element, int indent, GdaList **gda) {
  size_t size;
  const char *key;
  json_t *value;

  print_json_indent(indent);
  size = json_object_size(element);

  printf("JSON Object of %ld pair%s:\n", size, json_plural_size(size));
  json_object_foreach(element, key, value) {
    print_json_indent(indent + 2);
    printf("JSON Key: \"%s\"\n", key);
    check_light_type(value, indent + 2, gda, key);
  }

}

void	handle_json_array(json_t *element, int indent, GdaList **gda, const char *key) {
  size_t i;
  size_t size = json_array_size(element);

  print_json_indent(indent);
  printf("JSON Array of %ld element%s:\n", size, json_plural_size(size));
  for (i = 0; i < size; i++) {
    (*gda)->columns = GdaColumnsAdd((*gda)->columns);
    check_light_type(json_array_get(element, i), indent + 2, gda, key);
  }
}

void  AssignStringValue(json_t *element, const char *key, GdaList **gda)
{
  if (strcmp(key, "owner") == 0)
    (*gda)->owner = json_string_value(element);
  else if (strcmp(key, "table") == 0)
    (*gda)->table = json_string_value(element);
  else if (strcmp(key, "fistName") == 0)
    (*gda)->columns->firstName = json_string_value(element);
  else if (strcmp(key, "lastName") == 0)
    (*gda)->columns->lastName = json_string_value(element);
  else if (strcmp(key, "__key") == 0)
    (*gda)->columns->key = json_string_value(element);
}

void  AssignIntegerValue(json_t *element, const char *key, GdaList **gda)
{
  if (strcmp(key, "totalElements") == 0)
    (*gda)->page->totalElements = json_integer_value(element);
  else if (strcmp(key, "size") == 0)
    (*gda)->page->size = json_integer_value(element);
}

void handle_json_integer(json_t *element, int indent, GdaList **gda, const char *key) {
  print_json_indent(indent);
  AssignIntegerValue(element, key, gda);
  printf("JSON Integer: \"%" JSON_INTEGER_FORMAT "\"\n", json_integer_value(element));
}

void	handle_json_string(json_t *element, int indent, GdaList **gda, const char *key) {
  print_json_indent(indent);
  AssignStringValue(element, key, gda);
  printf("JSON String: \"%s\"\n", json_string_value(element));
}

void    handle_json_real(json_t *element, int indent, GdaList **gda, const char *key) {
  print_json_indent(indent);
  printf("JSON Real: %f\n", json_real_value(element));
}

void    handle_json_true(json_t *element, int indent, GdaList **gda, const char *key) {
  (void)element;
  print_json_indent(indent);
  printf("JSON True\n");
}

void    handle_json_false(json_t *element, int indent, GdaList **gda, const char *key) {
  (void)element;
  print_json_indent(indent);
  printf("JSON False\n");
}

void	check_light_type(json_t* element, int indent, GdaList **gda, const char *key)
{
  switch (json_typeof(element)) {
  case JSON_OBJECT:
    handle_json_object(element, indent, gda);
    break;
  case JSON_ARRAY:
    handle_json_array(element, indent, gda, key);
    break;
  case JSON_STRING:
    handle_json_string(element, indent, gda, key);
    break;
  case JSON_INTEGER:
    handle_json_integer(element, indent, gda, key);
    break;
  case JSON_REAL:
    handle_json_real(element, indent, gda, key);
    break;
  case JSON_TRUE:
    handle_json_true(element, indent, gda, key);
    break;
  case JSON_FALSE:
    handle_json_false(element, indent, gda, key);
    break;
  case JSON_NULL:
    handle_json_null(element, indent, gda);
    break;
  default:
    fprintf(stderr, "unrecognized JSON type %d\n", json_typeof(element));
  }
  
}

void  printAllGda(GdaList **gda)
{
  printf("owner = %s\n", (*gda)->owner);
  printf("table = %s\n", (*gda)->table);

  while ((*gda)->columns->prev != NULL)
    {
      printf("columns->index = %d\n", (*gda)->columns->index);
      printf("columns->firstName = %s\n", (*gda)->columns->firstName);
      printf("columns->lastName = %s\n", (*gda)->columns->lastName);
      printf("columns->Key = %s\n", (*gda)->columns->key);
      (*gda)->columns = (*gda)->columns->prev;
    }
  printf("init->page->size = %d\n", (*gda)->page->size);
  printf("init->page->totalElements = %zd\n", (*gda)->page->totalElements);
}

GdaList	*GdaParsingInfo(cometd_message *message)
{
  const char *key;
  json_t* source = message->source;
  GdaList *gda = InitGda(gda);
  GdaList **Gda = &gda;

  check_light_type(source, 1, Gda, key);

  return ((*Gda));
}
