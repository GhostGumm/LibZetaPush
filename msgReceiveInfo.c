#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "HashMaps.h"
#include "messageEntryInfo.h"

void	check_msg_type(json_t* element, int indent, MsgInfo **gda, const char *key);



MsgInfo*     InitMsg(MsgInfo *init)
{
  init = malloc(sizeof(MsgInfo));
  init->target = NULL;
  init->source = NULL;

  init->data = malloc(sizeof(HashMaps));
  init->data->key = NULL;
  init->data->value = NULL;
  init->data->type = NULL;

  return (init);
}

void	msg_json_null(json_t *element, int indent, MsgInfo **gda) {
  (void)element;
  print_json_indent(indent);
  //printf("JSON Null\n");
}

void	msg_json_object(json_t *element, int indent, MsgInfo **gda) {
  size_t size;
  const char *key;
  json_t *value;

  print_json_indent(indent);
  size = json_object_size(element);

  //printf("JSON Object of %ld pairs\n", size);
  json_object_foreach(element, key, value) {
    print_json_indent(indent + 2);
    //printf("JSON Key: \"%s\"\n", key);
    check_msg_type(value, indent + 2, gda, key);
  }

}

void	msg_json_array(json_t *element, int indent, MsgInfo **gda, const char *key) {
  size_t i;
  size_t size = json_array_size(element);

  print_json_indent(indent);
  //printf("JSON Array of %ld element\n", size);
  for (i = 0; i < size; i++) {
    check_msg_type(json_array_get(element, i), indent + 2, gda, key);
  }
}

void  AssignMsgStringValue(json_t *element, const char *key, MsgInfo **gda)
{
  if (strcmp(key, "target") == 0) {
    (*gda)->target = json_string_value(element);return;}
  if (strcmp(key, "source") == 0){
    (*gda)->source = json_string_value(element);return;}
  else
    {
      (*gda)->data->key = key;
      (*gda)->data->type = "string";
      (*gda)->data->value = (void*)json_string_value(element);
      (*gda)->data = AddKeyValue((*gda)->data);
      return;
    }
}

void msg_json_integer(json_t *element, int indent, MsgInfo **gda, const char *key) {
  print_json_indent(indent);
  //printf("JSON Integer: \"%" JSON_INTEGER_FORMAT "\"\n", json_integer_value(element));
}

void	msg_json_string(json_t *element, int indent, MsgInfo **gda, const char *key) {
  print_json_indent(indent);
  AssignMsgStringValue(element, key, gda);
  //printf("JSON String: \"%s\"\n", json_string_value(element));
}

void    msg_json_real(json_t *element, int indent, MsgInfo **gda, const char *key) {
  print_json_indent(indent);
  //printf("JSON Real: %f\n", json_real_value(element));
}

void    msg_json_true(json_t *element, int indent, MsgInfo **gda, const char *key) {
  (void)element;
  print_json_indent(indent);
  //printf("JSON True\n");
}

void    msg_json_false(json_t *element, int indent, MsgInfo **gda, const char *key) {
  (void)element;
  print_json_indent(indent);
  //printf("JSON False\n");
}

void	check_msg_type(json_t* element, int indent, MsgInfo **gda, const char *key)
{
  switch (json_typeof(element)) {
  case JSON_OBJECT:
    msg_json_object(element, indent, gda);
    break;
  case JSON_ARRAY:
    msg_json_array(element, indent, gda, key);
    break;
  case JSON_STRING:
    msg_json_string(element, indent, gda, key);
    break;
  case JSON_INTEGER:
    msg_json_integer(element, indent, gda, key);
    break;
  case JSON_REAL:
    msg_json_real(element, indent, gda, key);
    break;
  case JSON_TRUE:
    msg_json_true(element, indent, gda, key);
    break;
  case JSON_FALSE:
    msg_json_false(element, indent, gda, key);
    break;
  case JSON_NULL:
    msg_json_null(element, indent, gda);
    break;
  default:
    fprintf(stderr, "unrecognized JSON type %d\n", json_typeof(element));
  }
  
}

void  printMsgData(MsgInfo **gda)
{
  if ((*gda)->data->prev != NULL)
    (*gda)->data = (*gda)->data->prev;
  while ((*gda)->data)
    {
      if ((*gda)->data->type != NULL){
	if (strcmp((*gda)->data->type, "string") == 0){
	  void *carrier = (*gda)->data->value;
	  printf("key = %s\n", (*gda)->data->key);
	  printf("string = %s\n", (char *)carrier);
	}
      }
      (*gda)->data = (*gda)->data->prev;
    }
}

MsgInfo   *MsgInfoParsingInfo(cometd_message *message)
{
  char *key;
  json_t* source = message->source;
  MsgInfo *gda = InitMsg(gda);
  MsgInfo **Gda = &gda;

  check_msg_type(source, 1, Gda, key);

  return ((*Gda));
}
