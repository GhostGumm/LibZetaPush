#ifndef _HANDLER_H__
#define _HANDLER_H__











bool		ls_redirection_handler(cometd_client_t* client, cometd_message* message);
bool		echo_redirection_handler(cometd_client_t *client, cometd_message *message);
bool		gda_list_handler(cometd_client_t *client, cometd_message *message);
bool		gda_put_handler(cometd_client_t *client, cometd_message *message);
bool		gda_get_handler(cometd_client_t *client, cometd_message *message);
bool		messaging_handler(cometd_client_t *client, cometd_message *message);
bool		receive_handler(cometd_client_t *client, cometd_message *message);
bool		stack_list_handler(cometd_client_t *client, cometd_message *message);
bool		stack_remove_handler(cometd_client_t *client, cometd_message *message);
bool		stack_set_listeners_handler(cometd_client_t *client, cometd_message *message);
bool		stack_get_listeners_handler(cometd_client_t *client, cometd_message *message);
bool		queue_register_handler(cometd_client_t *client, cometd_message *message);
bool		queue_submit_handler(cometd_client_t *client, cometd_message *message);
bool		queue_done_handler(cometd_client_t *client, cometd_message *message);
bool		group_create_handler(cometd_client_t *client, cometd_message *message);
bool		add_me_handler(cometd_client_t *client, cometd_message *message);
bool		group_users_handler(cometd_client_t *client, cometd_message *message);
bool		group_add_users_handler(cometd_client_t *client, cometd_message *message);
bool		group_revoke_handler(cometd_client_t *client, cometd_message *message);
bool		group_grant_handler(cometd_client_t *client, cometd_message *message);
bool		group_error_handler(cometd_client_t *client, cometd_message *message);
bool		group_list_grant_handler(cometd_client_t *client, cometd_message *message);
bool		my_groups_handler(cometd_client_t *client, cometd_message *message);
bool		groups_handler(cometd_client_t *client, cometd_message *message);
bool		group_del_users_handler(cometd_client_t *client, cometd_message *message);
bool		group_del_user_handler(cometd_client_t *client, cometd_message *message);
bool		macro_call_handler(cometd_client_t *client, cometd_message *message);


#endif
