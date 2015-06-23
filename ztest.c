#include "cometd_internal.h"
#include "zetaclient.h"
#include "listingEntryInfo.h"
#include "ListingGdaInfo.h"
#include "comet_message.h"
#include "putGdaInfo.h"
#include "getGdaInfo.h"
#include "HashMaps.h"
#include "messageEntryInfo.h"
#include "stackListingInfo.h"
#include "queueInfo.h"
#include "handler.h"

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

OwnerRessource*  fillRessource()
{
  OwnerRessource *res = malloc(sizeof(OwnerRessource));
  res->owner = "toto";
  res->ressource = "stack";

  return res;
}

HashMaps   *fillUsers()
{
  HashMaps *list = HashMapInit();
  list->type = "string";
  list->key = "test3";
  list->value = "OvWudbZchXdOa-uoMGZNfA";
  list = AddKeyValue(list);
  list->key = "test4";
  list->type = "string";
  list->value = "tfXrOlDf9CYWF88akl73Lg";
  while (list->prev != NULL)
    list = list->prev;
  return (list);
}

HashMaps   *fillActions()
{
  HashMaps *list = HashMapInit();
  list->type = "string";
  list->key = "action";
  list->value = "grant";
  list = AddKeyValue(list);
  list->type = "string";
  list->key = "action";
  list->value = "revoke";
  list = AddKeyValue(list);
  list->type = "string";
  list->key = "action";
  list->value = "grant";
  while (list->prev != NULL)
    list = list->prev;
  return (list);
}

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
	const char * queueId = "aHqx";
	const char * groupe = "2v_u";
	const char * macro = "57C3";
	
	printf("Starting test program for user %s and resource %s\n", login, resource);
	if (cometd_init()) {
		printf("cometd_init failed\n");
	} else {
		zeta_handshake_manager_t * hm = zeta_create_mem_handshake_manager(businessId, deploymentId, login, password);
		zeta_client_t * client = zeta_create_client(server, cometd_create_long_polling_transport(), businessId, hm);		
		if (client) {
		  printf("client created\n");
		  cometd_channel_subscribe(client->cometClient, macro_call_handler, "/service/GmY-HuzW/57C3/call");
		  cometd_channel_subscribe(client->cometClient, group_del_user_handler, "/service/GmY-HuzW/2v_u/delUser");
		  cometd_channel_subscribe(client->cometClient, my_groups_handler, "/service/GmY-HuzW/2v_u/myGroups");
		  //cometd_channel_subscribe(client->cometClient, group_list_grant_handler, "/service/GmY-HuzW/2v_u/listGrants");
		  //cometd_channel_subscribe(client->cometClient, group_grant_handler,  "/service/GmY-HuzW/2v_u/grant");
		  //cometd_channel_subscribe(client->cometClient, group_del_user_handler, "/service/GmY-HuzW/2v_u/delGroup");
		  cometd_channel_subscribe(client->cometClient, group_add_users_handler, "/service/GmY-HuzW/2v_u/addUser");
		  cometd_channel_subscribe(client->cometClient, group_users_handler, "/service/GmY-HuzW/2v_u/groupUsers");
		  //cometd_channel_subscribe(client->cometClient, group_create_handler, "/service/GmY-HuzW/2v_u/createGroup");
		  //cometd_channel_subscribe(client->cometClient, all_groups_handler, "/service/GmY-HuzW/2v_u/allGroups");
		  //cometd_channel_subscribe(client->cometClient, add_me_handler, "/service/GmY-HuzW/2v_u/error");
		  //cometd_channel_subscribe(client->cometClient, add_me_handler, "/service/GmY-HuzW/2v_u/addMe");
			int i = 0;
			if (!zeta_handshake(client)) {
			  printf("cometd_handshake OK\n");
			  cometd_client_impl* cli = (cometd_client_impl*)client->cometClient;
			  while (!cometd_main_loop(client->cometClient)) {
			    puts("---------Echo request---------");
			    cometd_macro_call_request(client->cometClient, hm, macro, "add", fillMap());
			    cometd_my_groups_request(client->cometClient, hm, groupe, cli->userId);
			    //cometd_group_del_users_request(client->cometClient, hm, groupe, "firstgroup", "firstgroup", cli->userId, fillUsers());
			    //cometd_group_del_user_request(client->cometClient, hm, groupe, "firstgroup", cli->userId, "tfXrOlDf9CYWF88akl73Lg");
			    //cometd_group_revoke_and_grant_request(client->cometClient, hm, groupe, "revoke", "zetagroup", "", "none", true);
			    //cometd_group_list_grant_request(client->cometClient, hm, groupe, fillActions(), "zetagroup", "", "none", true);
			    //cometd_group_revoke_and_grant_request(client->cometClient, hm, groupe, "revoke", "zetagroup", "", "none", true);
			    //cometd_group_revoke_and_grant_request(client->cometClient, hm, groupe, "grant", "zetagroup", "", "none", false);
			    //cometd_group_del_request(client->cometClient, hm, groupe, "zetagroup", "");
			    //cometd_group_add_users_request(client->cometClient, hm, groupe, "firstgroup", "firstgroup", cli->userId, fillUsers());			    
			    //cometd_group_users_request(client->cometClient, hm, groupe, "firstgroup");
			    //cometd_group_create_request(client->cometClient, hm, groupe, "firstgroup", "firstgroup");
			    //cometd_all_groups_request(client->cometClient, hm, groupe, "mikael");
			    //cometd_add_me_request(client->cometClient, hm, groupe, "Groupe", "mikael", "toto");
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
