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

bool stack_get_listeners_handler(struct cometd_client *client, cometd_message *message)
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

