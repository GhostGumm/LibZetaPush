#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <jansson.h>

#include "comet_message.h"
#include "CommandParsingInfo.h"

const char *command;

void		print_json_indent(int indent);

void	command_json_null(json_t *element, int indent) {
  (void)element;
  //print_json_indent(indent);
  //printf("JSON Null\n");
}

void	command_json_object(json_t *element, int indent) {
  size_t size;
  const char *key;
  json_t *value;

  //print_json_indent(indent);
  size = json_object_size(element);

  //printf("JSON Object of %ld pair:\n", size);
  json_object_foreach(element, key, value) {
    //print_json_indent(indent + 2);
    //printf("JSON Key: \"%s\"\n", key);
    command_type(value, indent + 2,  key);
  }
}

void	command_json_array(json_t *element, int indent, const char *key) {
  size_t i;
  size_t size = json_array_size(element);

  //print_json_indent(indent);
  //printf("JSON Array of %ld elements", size);
  for (i = 0; i < size; i++) {
    command_type(json_array_get(element, i), indent + 2,  key);
  }
}

void	command_json_string(json_t *element, int indent, const char *key) {
  //print_json_indent(indent);
  CommandStringValue(element, key);
  //printf("JSON String: \"%s\"\n", json_string_value(element));
}

void  CommandStringValue(json_t *element, const char *key)
{
  if (strcmp(key, "cmd") == 0)
    {
      command = malloc(sizeof(char *) * strlen(json_string_value(element)));
      command = json_string_value(element);
    }
}


void	command_type(json_t* element, int indent, const char *key)
{
  switch (json_typeof(element)) {
  case JSON_OBJECT:
    command_json_object(element, indent);
    break;
  case JSON_ARRAY:
    command_json_array(element, indent,  key);
    break;
  case JSON_STRING:
    command_json_string(element, indent,  key);
    break;
  case JSON_NULL:
    command_json_null(element, indent);
    break;
  default:
    fprintf(stderr, "unrecognized JSON type %d\n", json_typeof(element));
  }
  
}

char   *CommandParsinginfo(cometd_message *message)
{
  const char *key;
  json_t* source = message->source;

  command_type(source, 1, key);
  return(command);
}
