#include "groupUsersInfo.h"

void	users_json_object(json_t *element, int indent, GroupUsers **ls) {
  size_t size;
  const char *key;
  json_t *value;
  print_json_indent(indent);
  size = json_object_size(element);
  
  printf("JSON Object of %ld pairs\n", size);
  json_object_foreach(element, key, value) {
    print_json_indent(indent + 2);
    printf("JSON Key: \"%s\"\n", key);
    check_users_type(value, indent + 2, ls, key);
  }
}

void	users_json_array(json_t *element, int indent, GroupUsers **ls, const char *key) {
  size_t i;
  size_t size = json_array_size(element);

  print_json_indent(indent);
  printf("JSON Array of %ld element\n", size);
  for (i = 0; i < size; i++) {
    if (strcmp(key, "tags") != 0) {
      check_users_type(json_array_get(element, i), indent + 2, ls, key);
      (*ls)->userList = AddKeyValue((*ls)->userList);
    }
  }
}

void  AssignStringValueToUsers(json_t *element, const char *key, GroupUsers **ls)
{
  
  if (strcmp(key, "group") == 0 && (*ls)->group == NULL) {
    (*ls)->group = json_string_value(element);return;}
  if (strcmp(key, "groupName") == 0 && (*ls)->groupName == NULL){
    (*ls)->groupName = json_string_value(element);return;}
  if (strcmp(key, "owner") == 0 && (*ls)->owner == NULL){
    (*ls)->owner = json_string_value(element);return;}
  else if ((*ls)->userList->value == NULL)
    {
      (*ls)->userList->key = key;
      (*ls)->userList->type = "string";
      (*ls)->userList->value = (void*)json_string_value(element);
      return;
    }
}

void	users_json_string(json_t *element, int indent, GroupUsers **ls, const char *key) {
  print_json_indent(indent);
  AssignStringValueToUsers(element, key, ls);
  printf("JSON String : ");
  puts(json_string_value(element));
}

void	check_users_type(json_t* element, int indent, GroupUsers **ls, const char *key)
{
  switch (json_typeof(element)) {
  case JSON_OBJECT:
    users_json_object(element, indent, ls);
    break;
  case JSON_ARRAY:
    users_json_array(element, indent, ls, key);
    break;
  case JSON_STRING:
    users_json_string(element, indent, ls, key);
    break;
  default:
    fprintf(stderr, "unrecognized JSON type %d\n", json_typeof(element));
  }
  
}

GroupUsers   *UsersInit(GroupUsers *ls)
{
  ls = malloc(sizeof(GroupUsers));
  ls->userList = HashMapInit();
  ls->group = NULL;
  ls->groupName = NULL;
  ls->owner = NULL;

  return (ls);
}
  
/*void  printUserss(GroupUsers **ls)
{
  while ((*ls)->listeners->prev)
    (*ls)->listeners = (*ls)->listeners->prev;
  while ((*ls)->listeners->next)
    {
      printf("key = %s\n value = %s\n", (*ls)->listeners->key, (char *)(*ls)->listeners->value);
      (*ls)->listeners = (*ls)->listeners->next;
    }
  
    }*/


GroupUsers   *UsersParsingInfo(cometd_message *message)
{
  char *key;
  json_t* source = message->source;
  GroupUsers *ls = UsersInit(ls);
  GroupUsers **Ls = &ls;
  
  check_users_type(source, 1, Ls, key);
  //printListeners(Ls);
  return ((*Ls));
}
