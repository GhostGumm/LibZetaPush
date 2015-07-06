#include <jansson.h>

#include "zetaclient.h"
#include "listingEntryInfo.h"
#include "ListingGdaInfo.h"
#include "comet_message.h"
#include "putGdaInfo.h"
#include "getGdaInfo.h"
#include "HashMaps.h"
#include "messageEntryInfo.h"
#include "stackListenerInfo.h"
#include "groupUsersInfo.h"
#include "stackListingInfo.h"

bool            macro_call_handler(cometd_client_t *client, cometd_message *message)
{
  printf("macro is handled\n");
  return (true);
}

bool            group_del_user_handler(cometd_client_t *client, cometd_message *message)
{
  printf("User deleted successfuly\n");
  return (true);
}

bool            group_del_users_handler(cometd_client_t *client, cometd_message *message)
{
  printf("Users deleted successfuly\n");  
  return (true);
}

bool            groups_handler(cometd_client_t *client, cometd_message *message)
{
  printf("Groups handled successfully\n");
  return (true);
}

bool            my_groups_handler(cometd_client_t *client, cometd_message *message)
{
  printf("My Groups handled successfully \n");
  return (true);
}

bool group_list_grant_handler(cometd_client_t *client, cometd_message *message)
{
  printf("ready to parse grant accesss\n");
  return (true);
}
bool group_error_handler(cometd_client_t *client, cometd_message *message)
{
  printf("An error occured while trying to query the server\n");

  return(true);
}

bool group_grant_handler(cometd_client_t *client, cometd_message *message)
{
  printf("Access rights granted\n");
  return(true);
}

bool group_revoke_handler(cometd_client_t *client, cometd_message *message)
{
  printf("revokation successfully done\n");

  return (true);
}

bool group_add_users_handler(cometd_client_t *client, cometd_message *message)
{
  printf("user successfully added\n");
  return(true);
}

bool group_users_handler(cometd_client_t *client, cometd_message *message)
{
  printf("all users ready to parse\n");
  UsersParsingInfo(message);
  return(true);
}

bool add_me_handler(cometd_client_t *client, cometd_message *message)
{
  printf("add me handled\n");
  return(true);
}

bool all_groups_handler(cometd_client_t *client, cometd_message *message)
{
  printf("AllGroup list done\n");
  return(true);
}

bool group_create_handler(cometd_client_t *client, cometd_message *message)
{
  printf("Groupe Created successfully\n");
  return (true);
}


bool queue_done_handler(cometd_client_t *client, cometd_message *message)
{
  printf("Done is handled \n");
  return (true);
}

bool queue_submit_handler(cometd_client_t *client, cometd_message *message)
{
  printf("Queue submit handled\n");
  return (true);
}

bool queue_register_handler(cometd_client_t *client, cometd_message *message)
{
  printf("Queue register handled\n");
  return (true);
}

bool stack_get_listeners_handler(cometd_client_t *client, cometd_message *message)
{
  printf("ready to parse the get listeners\n");
  ListenerParsingInfo(message);
  return (true);
}

bool   stack_set_listeners_handler(cometd_client_t *client, cometd_message *message)
{
  printf("Data listener(s) set successfully\n");  
  return (true);
}

bool    stack_remove_handler(cometd_client_t *client, cometd_message *message)
{
  printf("Data Successfully Removed\n");
  return(true);
}

bool    stack_list_handler(cometd_client_t *client, cometd_message *message)
{
  printf("Stack Parsing\n\n\n");
  StackListParsingInfo(message);
  return(true);
}

bool    receive_handler(cometd_client_t *client, cometd_message *message)
{
  printf("\n\n received \n\n");
  printf("chat message :\n");
  MsgInfo *result = MsgInfoParsingInfo(message);
  HashMaps *mesg = GetValueFromKey("text", result->data);
  if (mesg != NULL)
    printf("Message = %s\n", (char *)mesg->value);
  else
    printf("No Text\n");
  return true;
}

bool    messaging_handler(cometd_client_t *client, cometd_message *message)
{
  printf ("Messaging sent\n");
  return (true);
}

bool    gda_get_handler(cometd_client_t *client, cometd_message *message)
{
  printf("Gda get handled\n");
  GdaGet *result;
  result = GdaGetParsingInfo(message);

  /*You can do your function callings here, read the data or use it. 
    Create a Global to gather the data...Whatever you like.
    After your work is done this callback will return true as a validation for the Cometd Engine.
    False if you want to keep trying to connect to server after action.
  */  
  return (true);
}

bool    gda_put_handler(cometd_client_t *client, cometd_message *message)
{
  puts("Gda put handled");
  GdaPut *result = GdaPutParsingInfo(message);
  /*You can do your function callings here, read the data or use it. 
    Create a Global to gather the data...Whatever you like.
    After your work is done this callback will return the validation to the Cometd Engine.
  */
  return (true);
}

bool    gda_list_handler(cometd_client_t *client, cometd_message *message)
{
  puts("Gda list handled");
  GdaList *result = GdaParsingInfo(message);

  /*You can do your function callings here, read the data or use it. 
    Create a Global structure to gather the data...Whatever you like.
    After your work is done this callback will return the validation to the Cometd Engine.
  */
  return (true);
}

bool    echo_redirection_handler(cometd_client_t *client, cometd_message *message)
{
  puts("echo handled");
  
  return (true);
}

bool    ls_redirection_handler(cometd_client_t* client, cometd_message* message)
{
  puts("Ls handled");
  FolderListing *result =   FolderParsingInfo(message);
  
  /*You can do your function callings here, read the data or use it. 
    Create a Global to gather the data...Whatever you like.
    After your work is done this callback will return the validation to the Cometd Engine.
  */
  
  return (true);
}
