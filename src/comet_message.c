#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "zetaclient.h"
#include "cometd_internal.h"

static int sid;

int cometd_json_dump_callback_file(const char *buffer, size_t size, void *data) {
	return fwrite(buffer, size, 1, (FILE*)data) < 0; // TODO limit length
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

bool     cometd_stack_get_listeners_request(cometd_client_t *client, zeta_handshake_manager_t *hm, char *SDepId, char *stack, char *owner)
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

bool     cometd_stack_set_listeners_request(cometd_client_t *client, zeta_handshake_manager_t *hm, char *SDepId, HashMaps *setString, char *stack, char *owner)
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

bool      cometd_stack_remove_request(cometd_client_t *client, zeta_handshake_manager_t *hm, char *SDepId, HashMaps *guidList, char *stack, char *owner)
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

bool       cometd_stack_push_request(cometd_client_t *client, zeta_handshake_manager_t *hm, char *SDepId, char *guid, char *stack, HashMaps *data, char *owner)
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


bool       cometd_stack_listing_request(cometd_client_t *client, zeta_handshake_manager_t *hm, char *SDepId, char *stack, char *page, char *owner)
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

bool       cometd_request_messaging(cometd_client_t *client, zeta_handshake_manager_t *hm, char *SDepId, char *Target, char *Message)
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

bool       cometd_request_get_gda(cometd_client_t* client, zeta_handshake_manager_t *hm, char *SDepId, char *table,  char *column , char *key, char *key2)
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

bool       cometd_request_put_gda(cometd_client_t* client, zeta_handshake_manager_t *hm, char *SDepId, char *table,  char *column , char *key, char *key2, HashMaps *YourData)
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

bool       cometd_request_list_gda(cometd_client_t* client, zeta_handshake_manager_t *hm, char *SDepId, char *table, char *column, char *pNum, char *pSize)
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

bool       cometd_request_echo(cometd_client_t* client, zeta_handshake_manager_t *hm, char *request, char *SDepId)
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
