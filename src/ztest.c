#include <jansson.h>

#include "zetaclient.h"
#include "listingEntryInfo.h"
#include "ListingGdaInfo.h"
#include "comet_message.h"
#include "putGdaInfo.h"
#include "getGdaInfo.h"
#include "HashMaps.h"
#include "messageEntryInfo.h"
#include "stackListingInfo.h"

bool		cometd_request_echo(cometd_client_t* client, zeta_handshake_manager_t *hm, const char *request, const char *SDepId);

bool		cometd_request_ls(cometd_client_t* client, zeta_handshake_manager_t *hm, const char *folder, const char* SDepId);

bool		cometd_request_gda(cometd_client_t* client, zeta_handshake_manager_t *hm, const char *request, const char *SDepId, const char *tor);

bool		stack_list_handler(cometd_client_t *client, cometd_message *message);

bool		stack_remove_handler(cometd_client_t *client, cometd_message *message);

bool		stack_set_listeners_handler(cometd_client_t *client, cometd_message *message);

bool		stack_get_listeners_handler(struct cometd_client *client, cometd_message *message);

/* !!!!!!! Important read this and you will find the light at the end of the tunnel. Don't do cross reading you little monkey :3 !!!!!!!!
   
  These callbacks below can have any given name as long as you have declared it in the :
  ** cometd_channel_subscribe(client->cometClient, Here ----> ls_redirection_handler <---- Here, "/service/GmY-HuzW/2vC3/ls");
  or other example  : 
  ** cometd_channel_subscribe(client->cometClient, Here ----> iLoveCookies <---- Here, "/service/GmY-HuzW/2vC3/ls");
  It will be called by the engine no problem.
  It is better then to put the scope as a Forward Declaration to not have : "implicit declaration" Warnings.
  Your scope will allways take as parameters a : 
  -> cometd_client_t *client
  and a :
  -> cometd_message *message
  also returning a : -> bool
  For example : 
  ** Forward declaration : 
    bool iloveCookies(cometd_client_t *client, cometd_message *message);
  ** Actual callBack definition to execute code :
    bool iloveCookies(cometd_client_t *client, cometd_message *message)
    {
      Code here
    }

   Depending on what request you had sent, you want to call to according JsonParsing function.
   For Example i want to perform a ls command on my Zpfs server on the folder "/" were all my cookies pics are stored :
   
   - I set the callback name : 
   ** cometd_channel_subscribe(client->cometClient, iLoveCookies, "/service/GmY-HuzW/2vC3/ls");
   - I set the ForwardDeclaration :
   ** bool iloveCookies(cometd_client_t *client, cometd_message *message);
   - I open my function and call the right ls parsing function :
   **     bool iloveCookies(cometd_client_t *client, cometd_message *message)
          {
	   FolderListing *result = FolderParsingInfo(message); <-- Parsing Function	  
	   -> You code here <-
	   return (true); <-- to ensure the engine that the parsing went flowless, if it doesn't you will crash (and it will be your fault, shame on you !!) so no need to if/else, but it very rarely do so.
	  }
   - Then you will find the result of the parsing in *result. Easy isn't it :).
   You are all set up now :) all the examples are shown below. Refer to the documentation for further information.
   
   Thank you for reading this and enjoy.
   Kind Cookies, Hakimu jungulu.
*/

HashMaps   *fillMap()
{
  HashMaps *list = HashMapInit();
  list->type = "string";
  list->key = "test1";
  list->value = "listener1";
  list = AddKeyValue(list);
  list->key = "test2";
  list->type = "string";
  list->value = "listener2";
  list = AddKeyValue(list);
  list->key = "test3";
  list->type = "string";
  list->value = "listener3";
  while (list->prev != NULL)
    list = list->prev;
  return (list);
}

int main(int argc, char** argv) {
  
	int argn = 1;
	const char * server = "http://m.zpush.ovh:8080/str/strd";
	const char * login = "test2";
	const char * resource = "zetaTuto";
	const char * businessId = "GmY-HuzW";
	const char * deploymentId = "KZyH";
	const char * Zpfs = "p8qU";
	const char * gda  = "IFa0";
	const char * echo = "w3FQ";
	const char * Zpfs2 = "9LDy";
	const char * Zpfs3 = "2vC3";
	const char * password = "password";
	const char * msging = "RSVu";
	const char * stackId  = "cKXj";
	const char * remoteId = "";
	
	printf("Starting test program for user %s and resource %s\n", login, resource);
	if (cometd_init()) {
		printf("cometd_init failed\n");
	} else {
		zeta_handshake_manager_t * hm = zeta_create_mem_handshake_manager(businessId, deploymentId, login, password);
		zeta_client_t * client = zeta_create_client(server, cometd_create_long_polling_transport(), businessId, hm);		
		if (client) {
			printf("client created\n");
			//cometd_channel_subscribe(client->cometClient, stack_get_listeners_handler, "/service/GmY-HuzW/cKXj/getListeners");
			int i = 0;
			if (!zeta_handshake(client)) {
			  printf("cometd_handshake OK\n");
			  while (!cometd_main_loop(client->cometClient)) {
			    puts("---------Echo request---------");
			    //cometd_stack_push_request(client->cometClient, hm, guid, stack, fillMap(), owner);
			    puts("---------Echo Request---------");
			    //printf("cometd_main_loop OK\n");
			    //if (!(i++ % 10))
			    //cometd_send_chat_message(client, "this is a chat message", buddy);
			    if (i == 25)
				    cometd_disconnect(client->cometClient);
				}	
			}else
				printf("cometd_handshake failed\n");
		}
	}
	return 0;
}

/*All of the requests possible*/
/*

//cometd_stack_get_listeners_request(client->cometClient, hm, stackId, "stack", "toto");
//cometd_stack_set_listeners_request(client->cometClient, hm, stackId, fillMap(), "stack", "toto");
//cometd_channel_subscribe(client->cometClient, stack_set_listeners_handler, "/service/GmY-HuzW/cKXj/setListeners");
//cometd_channel_subscribe(client->cometClient, stack_list_handler, "/service/GmY-HuzW/cKXj/list");
//cometd_stack_push_request(client->cometClient, hm, stackId, "///+sheCphERWQ/JVSEhSg==", "stack", fillMap(), "toto");
//cometd_stack_listing_request(client->cometClient, hm, stackId, "stack", "null", "toto");
//cometd_stack_remove_request(client->cometClient, hm, stackId, fillMap(), "stack",  "toto");
//cometd_stack_add_listner_request(client->cometClient, hm, stackId, fillMap(), "stack", "toto");
//cometd_request_messaging(client->cometClient, hm, msging, "ShDmD1_ZFwNnmmjLxJCroA", "coucou");
//cometd_request_ls(client->cometClient, hm, "/", Zpfs3);
//cometd_stack_listing_request(client->cometClient, hm, stackId, "stack", "null", "toto");
//cometd_request_list_gda(client->cometClient, hm, gda, "table", "userList", "0", "10");
//cometd_request_put_gda(client->cometClient, hm, gda, "userList", "list", "${str:rnd64(6)}",NULL , "Mikael", "Morvan");
//cometd_request_get_gda(client->cometClient, hm, gda, "userList", "list", "mikaelmorvan11", NULL);
//cometd_request_list_gda(client->cometClient, hm, gda, "table", "userList", "0", "10");
//cometd_channel_subscribe(client->cometClient, receive_handler, "/service/GmY-HuzW/RSVu/reply");
//cometd_channel_subscribe(client->cometClient, ls_list_handler, "/service/GmY-HuzW/p8qU/ls");
//cometd_channel_subscribe(client->cometClient, gda_put_handler, "/service/GmY-HuzW/IFa0/put");

*/
