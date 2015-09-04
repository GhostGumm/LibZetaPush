#include <stdlib.h>
#include <string.h>

#include "HashMaps.h"
#include "putGdaInfo.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void	check_put_type(json_t* element, int indent, GdaPut **gda, const char *key);



GdaPut  *AddGdaPut(GdaPut *head)
{
  GdaPut *bound = NULL;
  bound = InitPutGda(bound);
  bound->key = head->key;
  bound->column = head->column;

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

GdaPut*     InitPutGda(GdaPut *init)
{
  init = malloc(sizeof(GdaPut));
  init->column = NULL;
  init->key = NULL;
  init->key2 = NULL;

  init->object = malloc(sizeof(HashMaps));
  init->object->key = NULL;
  init->object->value = NULL;
  init->object->type = NULL;

  return (init);
}

void	put_json_null(json_t *element, int indent, GdaPut **gda) {
  (void)element;
  print_json_indent(indent);
  printf("JSON Null\n");
}

void	put_json_object(json_t *element, int indent, GdaPut **gda) {
  size_t size;
  const char *key;
  json_t *value;

  print_json_indent(indent);
  size = json_object_size(element);

   printf("JSON Object of %ld pairs\n", size);
  json_object_foreach(element, key, value) {
    print_json_indent(indent + 2);
    printf("JSON Key: \"%s\"\n", key);
    check_put_type(value, indent + 2, gda, key);
  }

}

void	put_json_array(json_t *element, int indent, GdaPut **gda, const char *key) {
  size_t i;
  size_t size = json_array_size(element);

  print_json_indent(indent);
  printf("JSON Array of %ld element\n", size);
  for (i = 0; i < size; i++) {
    (*gda) = AddGdaPut((*gda));
    check_put_type(json_array_get(element, i), indent + 2, gda, key);
  }
}

void  AssignPutStringValue(json_t *element, const char *key, GdaPut **gda)
{
  if (strcmp(key, "column") == 0) {
    (*gda)->column = json_string_value(element);return;}
  if (strcmp(key, "key") == 0){
    (*gda)->key = json_string_value(element);return;}
  if (strcmp(key, "key2") == 0){
    (*gda)->key2 = json_string_value(element);return;}
  else
    {
      (*gda)->object->key = key;
      (*gda)->object->type = "string";
      (*gda)->object->value = (void*)json_string_value(element);
      (*gda)->object = AddKeyValue((*gda)->object);
      return;
    }
}

void  AssignPutIntegerValue(json_t *element, const char *key, GdaPut **gda)
{
  (*gda)->object->key = key;
  (*gda)->object->type = "integer";
  (*gda)->object->value = (void *)json_integer_value(element);
  (*gda)->object = AddKeyValue((*gda)->object);      
}

void put_json_integer(json_t *element, int indent, GdaPut **gda, const char *key) {
  print_json_indent(indent);
  AssignPutIntegerValue(element, key, gda);
  printf("JSON Integer: \"%" JSON_INTEGER_FORMAT "\"\n", json_integer_value(element));
}

void	put_json_string(json_t *element, int indent, GdaPut **gda, const char *key) {
  print_json_indent(indent);
  AssignPutStringValue(element, key, gda);
  printf("JSON String: \"%s\"\n", json_string_value(element));
}

void    put_json_real(json_t *element, int indent, GdaPut **gda, const char *key) {
  print_json_indent(indent);
  printf("JSON Real: %f\n", json_real_value(element));
}

void    put_json_true(json_t *element, int indent, GdaPut **gda, const char *key) {
  (void)element;
  print_json_indent(indent);
  printf("JSON True\n");
}

void    put_json_false(json_t *element, int indent, GdaPut **gda, const char *key) {
  (void)element;
  print_json_indent(indent);
  printf("JSON False\n");
}

void	check_put_type(json_t* element, int indent, GdaPut **gda, const char *key)
{
  switch (json_typeof(element)) {
  case JSON_OBJECT:
    put_json_object(element, indent, gda);
    break;
  case JSON_ARRAY:
    put_json_array(element, indent, gda, key);
    break;
  case JSON_STRING:
    put_json_string(element, indent, gda, key);
    break;
  case JSON_INTEGER:
    put_json_integer(element, indent, gda, key);
    break;
  case JSON_REAL:
    put_json_real(element, indent, gda, key);
    break;
  case JSON_TRUE:
    put_json_true(element, indent, gda, key);
    break;
  case JSON_FALSE:
    put_json_false(element, indent, gda, key);
    break;
  case JSON_NULL:
    put_json_null(element, indent, gda);
    break;
  default:
    fprintf(stderr, "unrecognized JSON type %d\n", json_typeof(element));
  }
  
}

void  printPutGda(GdaPut **gda)
{

  while ((*gda)->object->prev)
    {
      (*gda)->object = (*gda)->object->prev;
    }
  
  while ((*gda)->object)
    {
      if ((*gda)->object->type != NULL){
	if (strcmp((*gda)->object->type, "string") == 0){
	  void *carrier = (*gda)->object->value;
	  printf("key = %s\n", (*gda)->object->key);
	  printf("string = %s\n", (char *)carrier);
	}
	else
	  {
	    void *carrier = (*gda)->object->value;
	    printf("key = %s\n", (*gda)->object->key);
	    printf("string = %d\n", (int)carrier);
	  }
      }
      (*gda)->object = (*gda)->object->next;
    }

}

void    testGVFK(GdaPut **gda)
{
  HashMaps *carrier;
  puts("test GVFK");
  (*gda)->object = (*gda)->object->prev;
  carrier = GetValueFromKey("firstName", (*gda)->object);
  if (carrier != NULL)
    printf("type = %s\n value = %s\n", carrier->type, (char *)carrier->value);
  else puts("failed to print value,  Gda is nill");
}

void   testIKE(GdaPut **gda)
{
  printf("ifKeyexist = %d\n", IfKeyExist("lastName", (*gda)->object));
  return;
}

void testGTE(GdaPut **gda)
{
  printf("total Elements in object %d\n", GetTotalElements((*gda)->object));
}

GdaPut   *GdaPutParsingInfo(cometd_message *message)
{
  const char *key = NULL;
  json_t* source = message->source;
  GdaPut *gda = NULL;
  gda = InitPutGda(gda);
  GdaPut **Gda = &gda;
  
  check_put_type(source, 1, Gda, key);
  
  return ((*Gda));
}
