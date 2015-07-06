#include <stdio.h>
#include <stdlib.h>

#include "stackListenerInfo.h"

void	listener_json_object(json_t *element, int indent, StackListeners **ls) {
  size_t size;
  const char *key;
  json_t *value;
  print_json_indent(indent);
  size = json_object_size(element);
  
  printf("JSON Object of %ld pairs\n", size);
  json_object_foreach(element, key, value) {
    print_json_indent(indent + 2);
    printf("JSON Key: \"%s\"\n", key);
    check_listener_type(value, indent + 2, ls, key);
  }
}

void	listener_json_array(json_t *element, int indent, StackListeners **ls, const char *key) {
  size_t i;
  size_t size = json_array_size(element);

  print_json_indent(indent);
  printf("JSON Array of %ld element\n", size);
  for (i = 0; i < size; i++) {
    if (strcmp(key, "tags") != 0) {
      check_listener_type(json_array_get(element, i), indent + 2, ls, key);
    (*ls)->listeners = AddKeyValue((*ls)->listeners);
    }
  }
}

void  AssignStringValueToSLI(json_t *element, const char *key, StackListeners **ls)
{
  
  if (strcmp(key, "owner") == 0 && (*ls)->owner == NULL) {
    (*ls)->owner = json_string_value(element);return;}
  if (strcmp(key, "stack") == 0 && (*ls)->stack == NULL){
    (*ls)->stack = json_string_value(element);return;}
  else if ((*ls)->listeners->value == NULL)
    {
      (*ls)->listeners->key = key;
      (*ls)->listeners->type = "string";
      (*ls)->listeners->value = (void*)json_string_value(element);
      return;
    }
}

void listener_json_integer(json_t *element, int indent, StackListeners **ls, const char *key) {
  print_json_indent(indent);
  printf("JSON Integer: \"%" JSON_INTEGER_FORMAT "\"\n", json_integer_value(element));
}

void	listener_json_string(json_t *element, int indent, StackListeners **ls, const char *key) {
  print_json_indent(indent);
  AssignStringValueToSLI(element, key, ls);
  printf("JSON String : ");
  puts(json_string_value(element));
  //printf("JSON String: \"%s\"\n", carrier);
}

void	check_listener_type(json_t* element, int indent, StackListeners **ls, const char *key)
{
  switch (json_typeof(element)) {
  case JSON_OBJECT:
    listener_json_object(element, indent, ls);
    break;
  case JSON_ARRAY:
    listener_json_array(element, indent, ls, key);
    break;
  case JSON_STRING:
    listener_json_string(element, indent, ls, key);
    break;
  case JSON_INTEGER:
    listener_json_integer(element, indent, ls, key);
    break;
  default:
    fprintf(stderr, "unrecognized JSON type %d\n", json_typeof(element));
  }
  
}

StackListeners   *ListenerInit(StackListeners *ls)
{
  ls = malloc(sizeof(StackListeners));
  ls->listeners = HashMapInit();
  ls->index = 0;
  ls->stack = NULL;
  ls->owner = NULL;

  return (ls);
}
  
void  printListeners(StackListeners **ls)
{
  while ((*ls)->listeners->prev)
    (*ls)->listeners = (*ls)->listeners->prev;
  while ((*ls)->listeners->next)
    {
      printf("key = %s\n value = %s\n", (*ls)->listeners->key, (char *)(*ls)->listeners->value);
      (*ls)->listeners = (*ls)->listeners->next;
    }
  
}


StackListeners   *ListenerParsingInfo(cometd_message *message)
{
  const char *key = NULL;
  json_t* source = message->source;
  StackListeners *ls = NULL;
  ls = ListenerInit(ls);
  StackListeners **Ls = &ls;
  
  check_listener_type(source, 1, Ls, key);
  //printListeners(Ls);
  return ((*Ls));
}
