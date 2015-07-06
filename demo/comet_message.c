#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "zetaclient.h"
#include "cometd_internal.h"

static int sid;

int cometd_json_dump_callback_file(const char *buffer, size_t size, void *data) {
  return 0;  //fwrite(buffer, size, 1, (FILE*)data) < 0; // TODO limit length
}
int cometd_json_dump_callback_buffer(const char *buffer, size_t size, void *data) {
	return cometd_buffer_append((cometd_buffer_t*) data, buffer, size);
}

int cometd_dump_message(void * output, cometd_message * message, json_dump_callback_t callback) {
	json_t * root = json_object();
	json_object_set_new(root, CHANNEL_FIELD, json_string_nocheck(message->channel));
	if (message->id)
		json_object_set_new(root, ID_FIELD, json_string_nocheck(message->id));
	if (message->clientId)
		json_object_set_new(root, CLIENT_ID_FIELD, json_string_nocheck(message->clientId));
	json_object_set_new(root, SUCCESSFUL_FIELD, message->successful ? json_true() : json_false());
	json_object_set_new(root, VERSION_FIELD, json_string_nocheck(message->version));
	if (message->supportedConnectionTypes)
		json_object_set(root, SUPPORTED_CONNECTION_TYPES_FIELD, message->supportedConnectionTypes);
	if (message->connectionType)
	  json_object_set_new(root, CONNECTION_TYPE_FIELD, json_string_nocheck(message->connectionType));
	if (message->data)
	  json_object_set(root, DATA_FIELD, message->data);
	if (message->ext)
	  json_object_set(root, EXT_FIELD, message->ext);
	if (message->advice.present) {
	  if (message->advice.sourceAdvice)
	    json_object_set_new(root, ADVICE_FIELD, message->advice.sourceAdvice);
	  else {
	    json_t * advice = json_object();
	    if (message->advice.reconnect)
	      json_object_set_new(advice, RECONNECT_FIELD, json_string_nocheck(message->advice.reconnect));
	    json_object_set_new(advice, INTERVAL_FIELD, json_integer(message->advice.interval));
	    json_object_set(root, DATA_FIELD, message->data);
	    json_object_set_new(root, ADVICE_FIELD, advice);
	  }
	}
	int result = json_dump_callback(root, callback, output, 0);
	json_decref(root);
	return result;
}
int cometd_dump_message_file(FILE * f, cometd_message * message) {
	return cometd_dump_message(f, message, &cometd_json_dump_callback_file);
}
int cometd_dump_message_buffer(cometd_buffer_t* buffer, cometd_message * message) {
	// cometd 3.x always expects arrays of messages
	int errCode = 0;
	errCode |= cometd_json_dump_callback_buffer("[", 1, buffer);
	errCode |= cometd_dump_message(buffer, message, &cometd_json_dump_callback_buffer);
	errCode |= cometd_json_dump_callback_buffer("]", 1, buffer);
	return errCode;
}

int cometd_dump_messages(void * output, cometd_array_t * messageList, json_dump_callback_t callback) {
	CMTD_TRACE_IN
	int first = 1, errCode = 0;
	if (messageList) {
		cometd_message ** array = (cometd_message **)messageList->array;
		errCode |= callback("[", 1, output);
		while(*array && !errCode) {
			if (!first)
				errCode |= callback(",", 1, output);
			errCode |= cometd_dump_message(output, *array++, callback);
			first = 0;
		}
		errCode |= callback("]", 1, output);
	}
	CMTD_RETURN(errCode);
}
int cometd_dump_messages_file(FILE * f, cometd_array_t * messageList) {
	return cometd_dump_messages(f, messageList, &cometd_json_dump_callback_file);
}
int cometd_dump_messages_buffer(cometd_buffer_t* buffer, cometd_array_t * messageList) {
	return cometd_dump_messages(buffer, messageList, &cometd_json_dump_callback_buffer);
}

cometd_message * cometd_parse_message(json_t * source) {
	CMTD_TRACE_IN
	CALLOC(cometd_message, message);
	message->source = source;
	message->channel = json_string_value(json_object_get(source, CHANNEL_FIELD));
	message->clientId = json_string_value(json_object_get(source, CLIENT_ID_FIELD));
	message->id = json_string_value(json_object_get(source, ID_FIELD));
	message->version = json_string_value(json_object_get(source, VERSION_FIELD));
	message->connectionType = json_string_value(json_object_get(source, CONNECTION_TYPE_FIELD));
	message->data = json_object_get(source, DATA_FIELD);
	message->ext = json_object_get(source, EXT_FIELD);
	message->userId = json_string_value(json_object_get(json_object_get(json_object_get(source, EXT_FIELD), "authentication"), "userId"));
	message->supportedConnectionTypes = json_object_get(source, SUPPORTED_CONNECTION_TYPES_FIELD);
	message->successful = json_is_true(json_object_get(source, SUCCESSFUL_FIELD));
	json_t * advice = json_object_get(source, ADVICE_FIELD);
	if (advice) {
		message->advice.present = true;
		message->advice.reconnect = json_string_value(json_object_get(advice, CONNECTION_TYPE_FIELD));
		message->advice.interval = json_integer_value(json_object_get(advice, INTERVAL_FIELD));
	}
	CMTD_RETURN(message);
}
cometd_array_t * cometd_transport_default_message_parser(const char *buffer, size_t size) {
	CMTD_TRACE_IN
	json_error_t errorCode;
	json_t * parsed = json_loadb(buffer, size, 0, &errorCode);
	if (!parsed) {
		printf("Failed to parse '%.*s' : '%s'\n", (int)size, buffer, errorCode.text);
		CMTD_RETURN(0);
	} else {
		cometd_array_t * list = 0;
		size_t size = json_array_size(parsed);
		if (json_is_array(parsed))
			for (size_t i = 0; i < size; i++)
				cometd_array_add(cometd_parse_message(json_array_get(parsed, i)), &list);
		else
			cometd_array_add(cometd_parse_message(parsed), &list);
		CMTD_RETURN(list);
	}
}
bool cometd_string_startsWith(const char * string, const char * prefix) {
	return strstr(string, prefix) == string;
}
bool cometd_isMeta(const cometd_message * message) {
	return message && cometd_string_startsWith(message->channel, META_PREFIX);
}

cometd_message * cometd_message_create(void) {
	CALLOC(cometd_message, message);
	return message;
}
void cometd_message_free(cometd_message * message) {
	if (message) {
		json_decref(message->source);
		cometd_free_fn(message);
	}
}
/*static void millisleep(int millis) {
	timespec req;
	req.tv_sec = millis / 1000;
	req.tv_nsec = (millis - (req.tv_sec * 1000))*1000;
	nanosleep(&req, 0);
}
void millisleep(unsigned int ms) 
{ 
	struct timeval tm;
	tm.tv_sec  = ms/1000L;
	tm.tv_usec = 1000L*(ms%1000L);
	return select(0,NULL,NULL,NULL,&tm); 
}*/

void millisleep(unsigned int ms)
{
	sleep((ms+999) / 1000);
}

char*    concate_ids2channel(const char* bId, const char* SDepId, char* option)
{
  char* channel = malloc(sizeof(char *) * (strlen(bId) + strlen(SDepId) + strlen(option)) + 10);
  sprintf(channel,"/service/%s/%s/%s", bId, SDepId, option);
  return (channel);
}

bool    cometd_execute_request(cometd_client_t *client, zeta_handshake_manager_t *hm, char *SDepId, char *cmd, HashMaps *data, char *from, char *fromRes, char *owner, char *resource)
{  
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);

  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "execute");
  message->clientId = cli->clientId;
  message->data = init_execute_data(cmd, data, from, fromRes, owner, resource);
  client->transport->sender(client->transport, message, client, true);
  CMTD_RETURN(false);  
}

bool    cometd_pong_request(cometd_client_t *client, zeta_handshake_manager_t *hm, char *SDepId, char *action, bool avaible, char *owner, char *ressource, char *uid, char *user)
{
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);
  
  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "pong");
  message->clientId = cli->clientId;
  message->data = init_pong_data(action, avaible, owner, ressource, uid, user);
  client->transport->sender(client->transport, message, client, true);
  CMTD_RETURN(false);  
}

bool    cometd_ping_request(cometd_client_t *client, zeta_handshake_manager_t *hm, char *SDepId, char *action)
{
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);

  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "ping");
  message->clientId = cli->clientId;
  message->data = init_ping_data(action);
  client->transport->sender(client->transport, message, client, true);
  CMTD_RETURN(false);
}

bool    cometd_game_request(cometd_client_t *client, zeta_handshake_manager_t *hm, char *SDepId)
{
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);

  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "game");
  message->clientId = cli->clientId;
  message->data = init_game_data();
  client->transport->sender(client->transport, message, client, true);
  CMTD_RETURN(false);
}

bool    cometd_macro_call_request(cometd_client_t *client, zeta_handshake_manager_t *hm, const char *SDepId, char *name, HashMaps *param)
{
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);
  
  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "call");
  message->clientId = cli->clientId;
  message->data = init_macro_call_data(name, param);
  client->transport->sender(client->transport, message, client, true);
  CMTD_RETURN(false);
}  

bool    cometd_my_groups_request(cometd_client_t *client, zeta_handshake_manager_t *hm, const char *SDepId, const char *owner)
{
    CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);
  
  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "myGroups");
  message->clientId = cli->clientId;
  message->data = init_groups_mygroups_data(owner);
  client->transport->sender(client->transport, message, client, true);
  CMTD_RETURN(false);
}

bool    cometd_groups_request(cometd_client_t *client, zeta_handshake_manager_t *hm, const char *SDepId, char *owner)
{
    CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);
  
  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "groups");
  message->clientId = cli->clientId;
  message->data = init_groups_mygroups_data(owner);
  client->transport->sender(client->transport, message, client, true);
  CMTD_RETURN(false);
}

bool    cometd_group_del_users_request(cometd_client_t *client, zeta_handshake_manager_t *hm, const char *SDepId, char *group, char *groupName, char *owner, HashMaps *users)
{
    CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);
  
  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "delUsers");
  message->clientId = cli->clientId;
  message->data = init_del_users_data(group, groupName, owner, users);
  client->transport->sender(client->transport, message, client, true);
  CMTD_RETURN(false);
} 

bool    cometd_group_del_user_request(cometd_client_t *client, zeta_handshake_manager_t *hm, const char *SDepId, char *group, char *owner, char *user)
{
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);
  
  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "delUser");
  message->clientId = cli->clientId;
  message->data = init_del_user_data(group, owner, user);
  client->transport->sender(client->transport, message, client, true);
  CMTD_RETURN(false);
}

bool    cometd_group_list_grant_request(cometd_client_t *client, zeta_handshake_manager_t *hm, const char *SDepId, HashMaps *action, char *group, char *owner, char *ressource, bool rorg)
{
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);

  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "listGrants");
  message->clientId = cli->clientId;
  message->data = init_list_grant_data(action, group, owner, ressource);
  client->transport->sender(client->transport, message, client, true);
  CMTD_RETURN(false);
}

bool    cometd_group_revoke_and_grant_request(cometd_client_t *client, zeta_handshake_manager_t *hm, const char *SDepId, char *action, char *group, char *owner, char *ressource, bool rorg)
{
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);
  
  message->id = str;
  if (rorg)
    message->channel = concate_ids2channel(hm->businessId, SDepId, "revoke");
  else
    message->channel = concate_ids2channel(hm->businessId, SDepId, "grant");
  message->clientId = cli->clientId;
  message->data = init_revoke_data(action, group, owner, ressource);
  client->transport->sender(client->transport, message, client, true);
  CMTD_RETURN(false);
}

bool    cometd_group_del_request(cometd_client_t *client, zeta_handshake_manager_t *hm, const char *SDepId, char *groupe, char *owner)
{
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);
  
  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "delGroup");
  message->clientId = cli->clientId;
  message->data = init_add_me_data(groupe, owner, NULL);
  client->transport->sender(client->transport, message, client, true);
  CMTD_RETURN(false);
}

bool    cometd_group_add_users_request(cometd_client_t *client, zeta_handshake_manager_t *hm, const char *SDepId, char *group, char *groupName, char *owner, HashMaps *users)
{
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);
  
  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "addUsers");
  message->clientId = cli->clientId;
  message->data = init_add_users_data(group, groupName, owner, users);
  client->transport->sender(client->transport, message, client, true);
  CMTD_RETURN(false);
}


bool    cometd_group_users_request(cometd_client_t *client, zeta_handshake_manager_t *hm, const char *SDepId, char *group, char *owner)
{  
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);

  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "groupUsers");
  message->clientId = cli->clientId;
  message->data = init_add_me_data(group, owner, NULL);
  client->transport->sender(client->transport, message, client, true);
  CMTD_RETURN(false);
}

bool    cometd_add_me_request(cometd_client_t *client, zeta_handshake_manager_t *hm, const char *SDepId, char *group, char *owner, char * user)
{
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);

  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "addMe");
  message->clientId = cli->clientId;
  message->data = init_add_me_data(group, owner, user);
  client->transport->sender(client->transport, message, client, true);
  CMTD_RETURN(false);
}

bool    cometd_all_groups_request(cometd_client_t *client, zeta_handshake_manager_t *hm, const char *SDepId, char *owner)
{
    CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);

  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "allGroups");
  message->clientId = cli->clientId;
  message->data = init_group_create_data(NULL, NULL, owner);
  client->transport->sender(client->transport, message, client, true);
  CMTD_RETURN(false);
}

bool    cometd_group_create_request(cometd_client_t *client, zeta_handshake_manager_t *hm, const char *SDepId, char *group, char *groupName, char *owner)
{
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);
  
  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "createGroup");
  message->clientId = cli->clientId;
  message->data = init_group_create_data(group, groupName, owner);
  client->transport->sender(client->transport, message, client, true);
  CMTD_RETURN(false);
}

bool     cometd_queue_done_request(cometd_client_t *client, zeta_handshake_manager_t *hm, const char *SDepId, HashMaps *result, bool success, char *taskId)
{
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);

  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "done");
  message->clientId = cli->clientId;
  message->data = init_queue_done_data(result, success, taskId);
  client->transport->sender(client->transport, message, client, true);
  CMTD_RETURN(false);
}
			       

bool     cometd_queue_submit_request(cometd_client_t *client, zeta_handshake_manager_t *hm, const char *SDepId, OwnerRessource *origin, char *desc, char *origBusy, char *origDep, HashMaps *data)
{
  /*  originator" : "OwnerResource", "description" : "String", "originBusinessId" : "String", "originDeploymentId" : "String", "data" : "Map[String,Object]"*/
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);
  
  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "submit");
  message->clientId = cli->clientId;
  message->data = init_queue_submit_data(origin, desc, origBusy, origDep, data);
  client->transport->sender(client->transport, message, client, true);
  CMTD_RETURN(false);
}

bool     cometd_queue_register_request(cometd_client_t *client, zeta_handshake_manager_t *hm, const char *SDepId, int qid)
{
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);

  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "register");
  message->clientId = cli->clientId;
  message->data = init_queue_register_data(qid);
  client->transport->sender(client->transport, message, client, true);
  CMTD_RETURN(false);
}

bool     cometd_stack_get_listeners_request(cometd_client_t *client, zeta_handshake_manager_t *hm, const char *SDepId, char *stack, char *owner)
{
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);

  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "getListeners");
  message->clientId = cli->clientId;
  message->data = init_stack_get_listeners_data(stack, owner);
  client->transport->sender(client->transport, message, client, true);
  CMTD_RETURN(false);
}

bool     cometd_stack_set_listeners_request(cometd_client_t *client, zeta_handshake_manager_t *hm, const char *SDepId, HashMaps *setString, char *stack, char *owner)
{
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);

  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "setListeners");
  message->clientId = cli->clientId;
  message->data = init_stack_remove_data(setString, stack, owner, 1);
  client->transport->sender(client->transport, message, client, true);	
  CMTD_RETURN(false);

}

bool      cometd_stack_remove_request(cometd_client_t *client, zeta_handshake_manager_t *hm, const char *SDepId, HashMaps *guidList, char *stack, char *owner)
{
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);
  
  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "remove");
  message->clientId = cli->clientId;
  message->data = init_stack_remove_data(guidList, stack, owner, 0);
  client->transport->sender(client->transport, message, client, true);	
  CMTD_RETURN(false);
}

bool       cometd_stack_push_request(cometd_client_t *client, zeta_handshake_manager_t *hm, const char *SDepId, char *guid, char *stack, HashMaps *data, char *owner)
{
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);

  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "push");
  message->clientId = cli->clientId;
  message->data = init_stack_push_data(guid, stack, data, owner);
  client->transport->sender(client->transport, message, client, true);	
  CMTD_RETURN(false);
}


bool       cometd_stack_listing_request(cometd_client_t *client, zeta_handshake_manager_t *hm, const char *SDepId, char *stack, char *page, char *owner)
{
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);

  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "list");
  message->clientId = cli->clientId;
  message->data = init_stack_data(stack, page, owner);
  client->transport->sender(client->transport, message, client, true);	
  CMTD_RETURN(false);
}

bool       cometd_request_messaging(cometd_client_t *client, zeta_handshake_manager_t *hm, const char *SDepId, char *Target, char *Message)
{/*  {target=toto, source=null, data={text=coucou}} */
  
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);
  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "send");
  message->data = init_messaging_data(Message, Target);
  message->clientId = cli->clientId;
  client->transport->sender(client->transport, message, client, true);
  CMTD_RETURN(false);
}

bool       cometd_request_get_gda(cometd_client_t* client, zeta_handshake_manager_t *hm, const char *SDepId, char *table,  char *column , char *key, char *key2)
{/*[{"id":"7","channel":"/service/GmY-HuzW/IFa0/get","data":{"table":"userList","column":"list","key":"mikaelmorvan11"},"clientId":"3g1it0k2t9afst01c9qc5tzknjjq"}]*/

/*[{"data": {"table":"userList", "key":"${str:rnd64(6)}", "column":"list"}, "channel":"/service/GmY-HuzW/IFa0/get", "successful":false, "id":"5", "clientId": "70pt7hyaty6efqt7q7ap7uhd3q"}]*/
  
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);
  HashMaps  *empty = NULL;
  
  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "get");
  message->data = init_put_data(table, column, key, key2, empty);
  message->clientId = cli->clientId;
  client->transport->sender(client->transport, message, client, true);
  CMTD_RETURN(false);
}

bool       cometd_request_put_gda(cometd_client_t* client, zeta_handshake_manager_t *hm, const char *SDepId, char *table,  char *column , char *key, char *key2, HashMaps *YourData)
{  /* [{"id":"12","channel":"/service/GmY-HuzW/IFa0/put","data":{"table":"userList","column":"list","key":{"__ref":"${str:rnd64(6)}"},"data":{"fistName":"Mikael","lastName":"Morvan"}},"clientId":"3c1svz3e26v4k8lo8stg0mfj59k"}] */
  /* [{"channel": "/service/GmY-HuzW/IFa0/put", "id": "5", "clientId": "861w4r3127e8i5p1aa9l2n9p03ud", "data": {"column": "list", "table": "userList", "key": "${str:rnd64(6)}", "data": {"firstName": "Mikael", "lastName": "Morvan"}}, "successful": false}]*/
  
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);
  
  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "put");
  message->data = init_put_data(table, column, key, key2, YourData);
  message->clientId = cli->clientId;
  client->transport->sender(client->transport, message, client, true);	
  CMTD_RETURN(false);
}

bool       cometd_request_list_gda(cometd_client_t* client, zeta_handshake_manager_t *hm, const char *SDepId, char *table, char *column, char *pNum, char *pSize)
{/*[{"id":"9","channel":"/service/GmY-HuzW/IFa0/list","data":{"table":"userList"},"clientId":"3c1svz3e26v4k8lo8stg0mfj59k"}]*/
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
    static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);

  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "list");
  message->clientId = cli->clientId;
  message->data = init_request_data(column, table, pNum, pSize);
  client->transport->sender(client->transport, message, client, true);	
  CMTD_RETURN(false);
}

bool       cometd_request_echo(cometd_client_t* client, zeta_handshake_manager_t *hm, char *request, const char *SDepId)
{ /*{"id":"5","channel":"/service/GmY-HuzW/w3FQ/echo","data":{"message":"Hello Echo Service"},"clientId":"2pcstwmu0tegq82qvyvep25dri"}]*/
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "echo");
  message->clientId = cli->clientId;
  message->data = init_request_data(request, "message", NULL, NULL);
  client->transport->sender(client->transport, message, client, true);	
  CMTD_RETURN(false);
}

bool      cometd_request_ls(cometd_client_t* client, zeta_handshake_manager_t *hm, char *folder, char* SDepId)
{/*Well formated request */
  /* {"id":6,"channel":"/service/GmY-HuzW/6sd0/ls","connectionType":"long-polling","data":{"folder":"/"},"clientId":"5895iwwuqlmbwg1qv1d5c7r41v8"}*/
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
    static int id;
  char *str = malloc(sizeof(str));
  sprintf(str, "%d", id++);

  message->id = str;
  message->channel = concate_ids2channel(hm->businessId, SDepId, "ls");
  message->connectionType = LONG_POLLING;
  message->clientId = cli->clientId;
  message->data = init_request_data(folder, "folder", NULL, NULL);
  client->transport->sender(client->transport, message, client, true);	
  CMTD_RETURN(false);
}

bool cometd_handshake(cometd_client_t * client, json_t * handshakeFields) {
  CMTD_TRACE_IN
    CALLOC(cometd_message, message);
  cometd_client_impl* cli = (cometd_client_impl*)client;
  cli->handshakeFields = handshakeFields;
  message->channel = META_HANDSHAKE;
  message->version = DEFAULT_VERSION;
  message->supportedConnectionTypes = json_array();
  json_array_append_new(message->supportedConnectionTypes, json_string_nocheck(LONG_POLLING));
  // TODO manage returned reference for ext
  json_t * ext = json_object_get(handshakeFields, EXT_FIELD);
  message->ext = ext;
  client->transport->sender(client->transport, message, client, true);
  CMTD_RETURN(false);
}

bool cometd_handshake_message(cometd_client_t * client, json_t * handshakeFields) {
	CMTD_TRACE_IN
	cometd_handshake(client, handshakeFields);
	bool result = client->transport->main(client->transport, client);
	CMTD_RETURN(result);
}

void cometd_publish(cometd_client_t * client, const char * channel, json_t * data) {
	CMTD_TRACE_IN
	CALLOC(cometd_message, message);
	cometd_client_impl* cli = (cometd_client_impl*)client;
	message->channel = channel;
	message->version = DEFAULT_VERSION;
	message->clientId = cli->clientId;
	message->data = data;
	client->transport->sender(client->transport, message, client, false);
	CMTD_TRACE_OUT
}
bool cometd_connect(cometd_client_t * client, cometd_message * msg) {
	CMTD_TRACE_IN
	CALLOC(cometd_message, message);
	cometd_client_impl* cli = (cometd_client_impl*)client;
	message->channel = META_CONNECT;
	message->version = DEFAULT_VERSION;
	message->clientId = cli->clientId;
	message->connectionType = LONG_POLLING;
	if (msg && msg->advice.interval)
		millisleep(msg->advice.interval);
	client->transport->sender(client->transport, message, client, false);
	CMTD_RETURN(false);
}
void cometd_disconnect(cometd_client_t * client) {
	cometd_publish(client, META_DISCONNECT, 0);
}
