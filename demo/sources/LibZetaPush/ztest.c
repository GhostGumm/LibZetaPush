#include <jansson.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>

//#include <ncurses.h>
//#include <curses.h>
//#include <term.h>
#include "glut.h"
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

/*#PING*/

#define DELAY 50

zeta_handshake_manager_t * hr;
zeta_client_t * cr;


static struct termios old, new;

/* Initialize new terminal i/o settings */
void initTermios(int echo)
{
  tcgetattr(0, &old); /* grab old terminal i/o settings */
  new = old; /* make new settings same as old settings */
  new.c_lflag &= ~ICANON; /* disable buffered i/o */
  new.c_lflag &= echo ? ECHO : ~ECHO; /* set echo mode */
  tcsetattr(0, TCSANOW, &new); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void)
{
  tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo)
{
  char ch;
  initTermios(echo);
  ch = getchar();
  resetTermios();
  return ch;
}

/* Read 1 character without echo */
char getch(void)
{
  return getch_(0);
}

/* Read 1 character with echo */
char getche(void)
{
  return getch_(1);
}



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

const char *  parse_game(json_t *source)
{
  return (json_string_value(json_object_get(source, "next_move")));
}

bool execute_callback(cometd_client_t *client, cometd_message *message)
{
  printf("execute callback\n\n\n");
  
  return (false);
}

bool comand_callback(cometd_client_t *client, cometd_message *message)
{
  printf("command callback\n");
  
  exit (0);
}

bool ping_callback(cometd_client_t *client, cometd_message *message)
{
  printf("Ping callback lets pong : \n\n");
  GdaParsingInfo(message);

  cometd_pong_request(cr->cometClient, hr, "2v_u", "Game", true, "ShDmD1_ZFwNnmmjLxJCroA", "resource1", "uid", "ShDmD1_ZFwNnmmjLxJCroA");  
  return false;
}

bool pong_callback(cometd_client_t *client, cometd_message *message)
{
  printf("It ponged well let's execute : \n\n");
  
  GdaParsingInfo(message);

  return false;
}


int main(int argc, char** argv) {
  
  int argn = 1;
  char c;
  char *toto = malloc(sizeof(char *) + 4);
  
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
  const char * game = "";
  char  buff[4096];
  HashMaps *data = HashMapInit();
  
	
	printf("Starting test program for user %s and resource %s\n", login, resource);
	if (cometd_init()) {
		printf("cometd_init failed\n");
	} else {
	  zeta_handshake_manager_t * hm = zeta_create_mem_handshake_manager(businessId, deploymentId, login, password);
	  hr = hm;
	  zeta_client_t * client = zeta_create_client(server, cometd_create_long_polling_transport(), businessId, hm, "resource0");
	  cr = client;
	  if (client) {
	    printf("client created\n");
		  cometd_channel_subscribe(client->cometClient, execute_callback, "/service/GmY-HuzW/2v_u/execute");
		  cometd_channel_subscribe(client->cometClient, pong_callback, "/service/GmY-HuzW/2v_u/pong");
	      
		  //cometd_channel_subscribe(client->cometClient, comand_callback, "/service/GmY-HuzW/2v_u/command");
		  if (!zeta_handshake(client)) {
		    printf("cometd_handshake OK\n");
		    cometd_client_impl* cli = (cometd_client_impl*)client->cometClient;
		    int prev = 0;
		    while (!cometd_main_loop(client->cometClient)) {
		      puts("---------Echo request---------");
		      c = getche();
			
		      printf("character = %c\n", c);
		      toto[0] = c;
		      cometd_execute_request(cr->cometClient, hm, "2v_u" , toto, fillMap(), "ShDmD1_ZFwNnmmjLxJCroA", "resource0","ShDmD1_ZFwNnmmjLxJCroA", "resource1");
			  
			
		      //cometd_execute_request(cr->cometClient, hm, "2v_u" , toto, fillMap(), "ShDmD1_ZFwNnmmjLxJCroA", "resource0","ShDmD1_ZFwNnmmjLxJCroA", "resource1");

		      puts("---------Echo Request---------");
		      //printf("cometd_main_loop OK\n");
		      //if (!(i++ % 10))
		      //cometd_send_chat_message(client, "this is a chat message", buddy);
		      //if (i == 25)
		      //cometd_disconnect(client->cometClient);
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
