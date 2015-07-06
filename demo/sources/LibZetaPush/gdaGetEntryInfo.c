#include <stdlib.h>
#include <string.h>

#include "getGdaInfo.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void	check_get_type(json_t* element, int indent, GdaGet **gda, const char *key);

GdaGet  *AddGdaGet(GdaGet *head)
{
  GdaGet *bound = InitGetGda(bound);
  bound->Row = head->Row;

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

GdaGet*     InitGetGda(GdaGet *init)
{
  init = malloc(sizeof(GdaGet));

  init->Row = InitPutGda(init->Row);
  init->data = HashMapInit();

  return (init);
}

void	get_json_null(json_t *element, int indent, GdaGet **gda) {
  (void)element;
  print_json_indent(indent);
  printf("JSON Null\n");
}

void	get_json_object(json_t *element, int indent, GdaGet **gda) {
  size_t size;
  const char *key;
  json_t *value;

  print_json_indent(indent);
  size = json_object_size(element);
  
  printf("JSON Object of %ld pairs\n", size);
  json_object_foreach(element, key, value) {
    print_json_indent(indent + 2);
    printf("JSON Key: \"%s\"\n", key);
    check_get_type(value, indent + 2, gda, key);
  }

}

void	get_json_array(json_t *element, int indent, GdaGet **gda, const char *key) {
  size_t i;
  size_t size = json_array_size(element);

  print_json_indent(indent);
  printf("JSON Array of %ld elements\n", size);
  for (i = 0; i < size; i++) {
    (*gda) = AddGdaGet((*gda));
    check_get_type(json_array_get(element, i), indent + 2, gda, key);
  }
}

void  AssignGetStringValue(json_t *element, const char *key, GdaGet **gda)
{
  if (strcmp(key, "column") == 0) {
    (*gda)->Row->column = json_string_value(element);return;}
  if (strcmp(key, "key") == 0){
    (*gda)->Row->key = json_string_value(element);return;}
  if (strcmp(key, "key2") == 0){
     (*gda)->Row->key2 = json_string_value(element);return;}
  else
    {
      (*gda)->data->key = key;
      (*gda)->data->type = "string";
      (*gda)->data->value = (void*)json_string_value(element);
      (*gda)->data = AddKeyValue((*gda)->data);
      return;
    }
}

void  AssignGetIntegerValue(json_t *element, const char *key, GdaGet **gda)
{
  (*gda)->Row->key = key;
  (*gda)->data->type = "integer";
  (*gda)->data->value = (void *)json_integer_value(element);
  (*gda)->data = AddKeyValue((*gda)->data);      
}

void get_json_integer(json_t *element, int indent, GdaGet **gda, const char *key) {
  print_json_indent(indent);
  AssignGetIntegerValue(element, key, gda);
  printf("JSON Integer: \"%" JSON_INTEGER_FORMAT "\"\n", json_integer_value(element));
}

void	get_json_string(json_t *element, int indent, GdaGet **gda, const char *key) {
  print_json_indent(indent);
  AssignGetStringValue(element, key, gda);
  printf("JSON String: \"%s\"\n", json_string_value(element));
}

void    get_json_real(json_t *element, int indent, GdaGet **gda, const char *key) {
  print_json_indent(indent);
  printf("JSON Real: %f\n", json_real_value(element));
}

void    get_json_true(json_t *element, int indent, GdaGet **gda, const char *key) {
  (void)element;
  print_json_indent(indent);
  printf("JSON True\n");
}

void    get_json_false(json_t *element, int indent, GdaGet **gda, const char *key) {
  (void)element;
  print_json_indent(indent);
  printf("JSON False\n");
}

void	check_get_type(json_t* element, int indent, GdaGet **gda, const char *key)
{
  switch (json_typeof(element)) {
  case JSON_OBJECT:
    get_json_object(element, indent, gda);
    break;
  case JSON_ARRAY:
    get_json_array(element, indent, gda, key);
    break;
  case JSON_STRING:
    get_json_string(element, indent, gda, key);
    break;
  case JSON_INTEGER:
    get_json_integer(element, indent, gda, key);
    break;
  case JSON_REAL:
    get_json_real(element, indent, gda, key);
    break;
  case JSON_TRUE:
    get_json_true(element, indent, gda, key);
    break;
  case JSON_FALSE:
    get_json_false(element, indent, gda, key);
    break;
  case JSON_NULL:
    get_json_null(element, indent, gda);
    break;
  default:
    fprintf(stderr, "unrecognized JSON type %d\n", json_typeof(element));
  }
  
}

void  printGetGda(GdaGet **gda)
{
  while ((*gda)->prev)
    (*gda) = (*gda)->prev;

  while ((*gda)->data)
    {
      if ((*gda)->data->type != NULL){
	if (strcmp((*gda)->data->type, "string") == 0){
	  void *carrier = (*gda)->data->value;
	  printf("key = %s\n", (*gda)->data->key);
	  printf("string = %s\n", (char *)carrier);
	}
	else
	  {
	    void *carrier = (*gda)->data->value;
	    printf("key = %s\n", (*gda)->data->key);
	    printf("string = %d\n", (int)carrier);
	  }
      }
      (*gda)->data = (*gda)->data->next;
    }
}

/*
void    testGVFK(GdaGet **gda)
{
  HashMaps *carrier;
  gets("test GVFK");
  (*gda)->object = (*gda)->object->prev;
  carrier = GetValueFromKey("firstName", (*gda)->object);
  if (carrier != NULL)
    printf("type = %s\n value = %s\n", carrier->type, (char *)carrier->value);
  else gets("failed to print value,  Gda is nill");
}

void   testIKE(GdaGet **gda)
{
  printf("ifKeyexist = %d\n", IfKeyExist("lastName", (*gda)->object));
  return;
}

void testGTE(GdaGet **gda)
{
  printf("total Elements in object %d\n", GetTotalElements((*gda)->object));
  }*/

void fixDataForDumbProgrammers(GdaGet **gda)
{
  while ((*gda))
    {
      while ((*gda)->data->prev)
	(*gda)->data = (*gda)->data->prev;
      if ((*gda)->prev != NULL)
	(*gda) = (*gda)->prev;
      else
	return;
    }

}

GdaGet   *GdaGetParsingInfo(cometd_message *message)
{
  char *key;
  json_t* source = message->source;
  GdaGet *gda = InitGetGda(gda);
  GdaGet **Gda = &gda;

  check_get_type(source, 1, Gda, key);

  fixDataForDumbProgrammers(Gda);

  return ((*Gda));
}
