#include "comet_message.h"

json_t  *init_execute_data(char *cmd, HashMaps *data, char *from, char *fromRes, char *owner, char *resource)
{
  json_t *root = json_object();
  json_t *dataR = json_object();
  bool isOkay = false;

  if(data->value != NULL)
    while (data)
      {
	if(strcmp(data->type, "string") == 0)
	  json_object_set(dataR, data->key, json_string((char*)data->value));
	else if (strcmp(data->type, "integer") == 0)
	  json_object_set(dataR, data->key, json_integer((int)data->value));
	data = data->next;
	isOkay = true;
      }
  json_object_set(root, "cmd", json_string(cmd));
  json_object_set(root, "from", json_string(from));
  json_object_set(root, "fromResource", json_string(fromRes));
  json_object_set(root, "owner", json_string(owner));
  if (isOkay){
    printf("isokay = %d\n", isOkay);
    json_object_set(root, "data", dataR);
  }
  json_object_set(root, "resource", json_string(resource));

  return (root);
}

json_t  *init_ping_data(char *action)
{
  json_t *root = json_object();

  json_object_set(root, "action", json_string(action));
  return (root);
}

json_t  *init_pong_data(char *action, bool avaible, char *owner, char *resource, char *uid, char *user)
{
  json_t *root = json_object();

  json_object_set(root, "action", json_string(action));
  json_object_set(root, "avaible", json_boolean(avaible));
  json_object_set(root, "owner", json_string(owner));
  json_object_set(root, "resource", json_string(resource));
  json_object_set(root, "uid", json_string(uid));
  json_object_set(root, "user", json_string(user));

  return root;
}

json_t  *init_game_data()
{
  json_t *root = json_object();

  json_object_set(root, "next_move", json_string("request"));
  return (root);
}

json_t  *init_macro_call_data(char *name, HashMaps *params)
{
  json_t *root = json_object();
  json_t *para = json_object();

  while (params)
    {
      if(strcmp(params->type, "string") == 0)
	json_object_set(para, params->key, json_string((char*)params->value));
      else if (strcmp(params->type, "integer") == 0)
	json_object_set(para, params->key, json_integer((int)params->value));
      params = params->next;
    }
  json_object_set(root, "name", json_string(name));
  json_object_set(root, "parameters", para);
  return (root);
}

json_t  *init_groups_mygroups_data(const char *owner)
{
  json_t *root = json_object();

  json_object_set(root, "owner", json_string(owner));
  return (root);
}

json_t  *init_del_users_data(char *group, char *groupName, char *owner, HashMaps *user)
{
  json_t *root = json_object();
  json_t *users = json_array();

    while (user)
    {
      json_array_append(users, json_string((char*)user->value));
      user = user->next;
    }
    json_object_set(root, "group", json_string(group));
    json_object_set(root, "groupName", json_string(groupName));
    json_object_set(root, "owner", json_string(owner));
    json_object_set(root, "users", users);

    return (root);
}

json_t  *init_del_user_data(char *group, char *owner, char *user)
{
    json_t *root = json_object();

    json_object_set(root, "group", json_string(group));
    json_object_set(root, "owner", json_string(owner));
    json_object_set(root, "user", json_string(user));

    return (root);
}

json_t  *init_list_grant_data(HashMaps *data, char *group, const char *owner, char *ressource)
{
  json_t *root = json_object();
  json_t *data2 = json_array();
  bool    isokay = false;

  while (data)
    {
      json_array_append(data2, json_string((char*)data->value));
      data = data->next;
    }
  json_object_set(root, "group", json_string(group));
  json_object_set(root, "owner", json_string(owner));
  json_object_set(root, "resource", json_string(ressource));
  json_object_set(root, "actions", data2);

  return (root); 
}

json_t  *init_revoke_data(char *action, char *group, const char *owner, char *ressource)
{
  json_t *root = json_object();
  bool    isokay = false;

  json_object_set(root, "action", json_string(action));
  json_object_set(root, "group", json_string(group));
  json_object_set(root, "owner", json_string(owner));
  json_object_set(root, "resource", json_string(ressource));

  return (root);
  
}

json_t  *init_add_users_data(char *Group, const char *GroupName, const char *Owner, HashMaps *users)
{
  json_t  *root = json_object();
  bool    isokay = false;
  json_t  *data = json_array();
  if (Group){
    json_t  *group = json_string(Group);
    json_object_set(root, "group", group);
  }
  if (GroupName){
    json_t  *groupName = json_string(GroupName);
    json_object_set(root, "groupName", groupName);
  }
  if (Owner){
    json_t  *owner = json_string(Owner);
    json_object_set(root, "owner", owner);
  }
  
  
  while (users)
    {
      if (strcmp(users->type, "string") == 0)
	json_array_append(data, json_string((char*)users->value));
      isokay = true;
      users = users->next;      
    }
  if (isokay)
    json_object_set(root, "users", data);
  
  return(root);
}

json_t  *init_add_me_data(char *group, const char *owner, const char *user)
{
  json_t * root = json_object();

  json_object_set(root, "group", json_string(group));
  json_object_set(root, "owner", json_string(owner));
  if (user)
    json_object_set(root, "user", json_string(user));

  return (root);
}

json_t *init_group_create_data(char *group, char *groupName, const char *owner)
{
  json_t  *root = json_object();

  if (group){
    json_t  *grp = json_string(group);
    json_object_set(root, "group", grp);}
  if (groupName){
    json_t  *grpn = json_string(groupName);
    json_object_set(root, "groupName", grpn);}

  if (owner){
    json_t * own = json_string(owner);
    json_object_set(root, "owner", own);}

  return (root);
}

json_t  *init_queue_done_data(HashMaps *data, bool success, char *taskId)
{
  json_t   *root = json_object();
  json_t   *data2 = json_object();
  json_t   *boole  = json_boolean(success);
  json_t   *task = json_string(taskId);
  bool     isokay = false;
  
  
  while (data)
    {
      if (strcmp(data->type, "string") == 0)
	json_object_set(data2, data->key, json_string((char*)data->value));
      else if (strcmp(data->type, "integer") == 0)
	json_object_set(data2, data->key, json_integer((int)data->value));
      data = data->next;
      isokay = true;
    }

  json_object_set(root, "success", boole);
  json_object_set(root, "taskId", task);
  if (isokay)
    json_object_set(root, "result", data2);
  
  return(root);
    
}

json_t  *init_queue_submit_data(OwnerRessource *origin, char *desc, char *origBusy, char *origDep, HashMaps *data)
{
  /*"originator" : "OwnerResource",
"description" : "String",
"originBusinessId" : "String",
"originDeploymentId" : "String",
"data" : "Map[String,Object]"
}*/
  json_t   *root = json_object();
  json_t   *data2 = json_object();
  json_t   *originator = json_object();
  bool      isokay = false;
  

  json_t *description = json_string(desc);
  json_t *originBusinessId = json_string(origBusy);
  json_t *originDeploymentId = json_string(origDep);

  while (data)
    {
      if (strcmp(data->type, "string") == 0)
	json_object_set(data2, data->key, json_string((char*)data->value));
      else if (strcmp(data->type, "integer") == 0)
	json_object_set(data2, data->key, json_integer((int)data->value));
      data = data->next;
      isokay = true;
    }

  if (origin) {
    json_object_set(originator, "owner", json_string(origin->owner));
    json_object_set(originator, "resource", json_string(origin->ressource));
  }
  if (isokay)
    json_object_set(root, "data", data2);
  json_object_set(root, "originator", originator);
  json_object_set(root, "description", description);
  json_object_set(root, "originBusinessId", originBusinessId);
  json_object_set(root, "originDeploymentId", originDeploymentId);
  
  return (root);
}

json_t  *init_queue_register_data(int Capacity)
{
  json_t *root = json_object();
  json_t *post = json_integer(Capacity);
  
  json_object_set(root, "capacity", post);

  return (root);
}

json_t  *init_messaging_data(char *Message, char *Target)
{/* {target=toto, source=null, data={text=coucou}} */
  json_t *root  =  json_object();
  json_t *target  =  json_string(Target);
  json_t *source = json_string("null");
  json_t *data  =  json_object();
  json_t *text  =  json_string(Message);

  json_object_set(data, "text", text);
  json_object_set(root, "target", target);
  json_object_set(root, "source", source);
  json_object_set(root, "data", data);
  return(root);

}

json_t*  init_request_data(char *request, char *service, char *pNum, char *pSize)
{
  json_t *data = json_object();
  json_t* value = json_string(request);
  if (pNum != NULL || pSize != NULL) {
    json_t* page = json_object();
    json_t* num = json_string(pNum);
    json_t* size = json_string(pSize);
    json_object_set(page, "pageNumber", num);
    json_object_set(page, "pageSize", size);
    json_object_set(data, "page", page);
  }   
  json_object_set(data, service, value);
  
  return (data);
}



json_t* init_put_data(char *table, char *column, char *key, char *key2, HashMaps *YourData)
{
  json_t *data = json_object();
  json_t* data2 = json_object();
  json_t *Table = json_string(table);
  json_t *Column = json_string(column);
  json_t *Key = json_object();
  json_t *Kkey = json_string(key);
  bool isokay = false;
  HashMaps *free;
  
    while (YourData)
      {
	if (strcmp(YourData->type, "string") == 0)
	  json_object_set(data2, YourData->key, json_string((char*)YourData->value));
	else if (strcmp(YourData->type, "integer") == 0)
	  json_object_set(data2, YourData->key, json_integer((int)YourData->value));
	isokay = true;
	YourData = YourData->next;
      }
    
  json_object_set(Key, "__ref", Kkey);
  
  json_object_set(data,"table", Table);
  json_object_set(data, "column", Column);
  json_object_set(data, "key", Key);
  if (key2 != NULL){
    json_t *Key2 = json_string(key2);
    json_object_set(data, "key2", Key2);
  }
  if (isokay == true)
    json_object_set(data, "data", data2);
  
  return (data);
}

json_t     *init_stack_data(char *stack, char *page, char *owner)
{
  json_t *Stack = json_string(stack);
  json_t *Page = json_string(page);
  json_t *Owner = json_string(owner);
  json_t *root = json_object();

  json_object_set(root, "stack", Stack);
  //json_object_set(root, "page", Page);
  json_object_set(root, "owner", Owner);
  return (root);
}

json_t   *init_stack_push_data(char *Guid, char *Stack, HashMaps *Data, char *Owner)
{
  json_t *root = json_object();
  json_t *data = json_object();
  json_t *stack = json_string(Stack);
  json_t *owner = json_string(Owner);
  json_t *guid = json_string(Guid);
  bool isokay = false;
  while (Data)
    {
      if (strcmp(Data->type, "string") == 0)
	json_object_set(data, Data->key, json_string((char*)Data->value));
      else if (strcmp(Data->type, "integer") == 0)
	json_object_set(data, Data->key, json_integer((int)Data->value));
      isokay = true;
      Data = Data->next;
    }

  json_object_set(root, "guid", guid);
  json_object_set(root, "stack", stack);
  json_object_set(root, "owner", owner);
  if (isokay == true)
    json_object_set(root, "data", data);
  return (root);
}

json_t*     init_stack_remove_data(HashMaps *Data, char *Stack, char *Owner, int type){
  json_t *root = json_object();
  json_t *data = json_array();
  json_t *owner = json_string(Owner);
  json_t *stack = json_string(Stack);
  size_t i = 0;
  while (Data)
    {
      if (strcmp(Data->type, "string") == 0)
	json_array_append(data, json_string((char*)Data->value));
      Data = Data->next;
    }
  json_object_set(root, "stack", stack);
  json_object_set(root, "owner", owner);
  if (type == 1)
    json_object_set(root, "listeners", data);
  else
    json_object_set(root, "guids", data);

  return (root);
  
}

json_t*  init_stack_get_listeners_data(char *Stack, char *Owner)
{
  json_t *root = json_object();
  json_t *owner = json_string(Owner);
  json_t *stack = json_string(Stack);

  json_object_set(root, "stack", stack);
  json_object_set(root, "owner", owner);

  return (root);
}

