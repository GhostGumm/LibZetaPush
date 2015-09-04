//
//  ZTGlobalCallbacks+ZTGlobalCallbacks.m
//  RecodeLibZetaPush
//
//  Created by Hakim on 12/08/15.
//  Copyright (c) 2015 Hakim. All rights reserved.
//

#import "ZTGlobalCallbacks+ZTGlobalCallbacks.h"

@implementation ZTGlobalCallbacks 

@synthesize
selector = p_selector,
messageSelector = f_selector,
oldPath = f_oldPath,
path = f_path,
contentType = f_contentType,
parents = f_parents,
tags = f_tags,
guid = f_guid,
metadataFiles = f_metadataFiles,
metadata = f_metadata,
uid = f_uid,
group = f_group,
groupName = f_groupName,
user = f_user,
cmd = f_cmd,
users = f_users,
avaible = f_avaible,
action = f_action,
actions = f_actions,
askRes = f_askRes,
answerRes = f_answRes,
capabilities = f_capa,
GroupData = f_GData,
from = f_from,
fromResource = f_FRes,
start = f_start,
stop = f_stop,
data = f_data,
page = f_page,
key = f_key,
key2 = f_key2,
multipleKey2 = f_MKey2,
table = f_table,
client = f_client,
folder = f_folder,
owner = f_owner,
resource = f_resource,
name = f_name,
map = f_map,
type = f_type,
column = f_column,
columns = f_columns,
channel = f_channel;

- (void)dealloc
{
    [client release];
    [self.channel release];
    [self.folder release];
    [super dealloc];
}

-(id)initWithClientContext:(ZTCometClient *)context Selector:(SEL)selector FiringSelector:(SEL)ignitionSel
{
    if((self = [super init]))
    {
        if ([self respondsToSelector:(ignitionSel)] && [self respondsToSelector:(selector)])
        {
            f_client = context;
            p_selector = selector;
            f_selector = ignitionSel;
        }
    }
    
    return self;
}

-(id)setClientDuringProcess:(ZTCometClient *)context
{
    f_client = context;
    
    return self;
}

-(void)callbackme:(NSDictionary *)data
{
    NSLog(@"throught callback");
}


// --- ZPFS SERVICES --- //

-(NSMutableDictionary *)constructZpfsJson
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    (f_folder != nil) ? [Data setObject:f_folder forKey:@"folder"] : [Data setObject:@"nil" forKey:@"folder"];
    //[Data setObject:f_client.userID forKey:@"owner"];
    
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    
    return list;
}

// --- GROUP SERVICES --- //

-(NSMutableDictionary *)constructGroupAddListener
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    (f_cmd != nil) ? [Data setObject:f_cmd forKey:@"cmd"] : [Data setObject:@"nil" forKey:@"cmd"];
    if (f_data != nil)
        [Data setObject:f_data forKey:@"data"];
    if (f_from != nil)
        [Data setObject:f_from forKey:@"from"];
    if (f_FRes != nil)
        [Data setObject:f_FRes forKey:@"fromResource"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    if (f_resource != nil)
        [Data setObject:f_owner forKey:@"owner"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];

    return list;
}


-(NSMutableDictionary *)constructGroupCapabilities
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];

    if (f_answRes != nil)
        [Data setObject:f_answRes forKey:@"answeringResource"];
    if (f_askRes != nil)
        [Data setObject:f_askRes forKey:@"askingResource"];
    if (f_capa != nil)
        [Data setObject:f_capa forKey:@"capabilities"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    
    return list;
}

-(NSMutableDictionary *)constructGroupExecute
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    (f_cmd != nil) ? [Data setObject:f_cmd forKey:@"cmd"] : [Data setObject:@"nil" forKey:@"cmd"];
    if (f_resource != nil)
        [Data setObject:f_resource forKey:@"resource"];
    if (f_data != nil)
        [Data setObject:f_data forKey:@"data"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    
    return list;
}

-(NSMutableDictionary *)constructGroupNotify
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    (f_cmd != nil) ? [Data setObject:f_cmd forKey:@"cmd"] : [Data setObject:@"nil" forKey:@"cmd"];
    if (f_resource != nil)
        [Data setObject:f_resource forKey:@"resource"];
    if (f_data != nil)
        [Data setObject:f_data forKey:@"data"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    
    return list;
}

-(NSMutableDictionary *)constructGroupPing
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    (f_action != nil) ? [Data setObject:f_action forKey:@"action"] : [Data setObject:@"nil" forKey:@"action"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    
    return list;
}

-(NSMutableDictionary *)constructGroupPong
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    if (f_action != nil)
        [Data setObject:f_action forKey:@"action"];
    //if (f_avaible != nil)
        //[Data setObject:f_avaible forKey:@"avaible"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    if (f_resource != nil)
        [Data setObject:f_resource forKey:@"resource"];
    if (f_uid != nil)
        [Data setObject:f_uid forKey:@"uid"];
    if (f_user != nil)
        [Data setObject:f_user forKey:@"user"];
    
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    
    return list;
}

-(NSMutableDictionary *)constructGroupRemoveListener
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    (f_cmd != nil) ? [Data setObject:f_cmd forKey:@"cmd"] : [Data setObject:@"nil" forKey:@"cmd"];

    if (f_data != nil)
        [Data setObject:f_data forKey:@"data"];
    if (f_from != nil)
        [Data setObject:f_from forKey:@"from"];
    if (f_FRes != nil)
        [Data setObject:f_FRes forKey:@"fromResource"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    if (f_resource != nil)
        [Data setObject:f_resource forKey:@"resource"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    
    return list;
}

-(NSMutableDictionary *)constructGroupAddMe
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    (f_group != nil) ? [Data setObject:f_group forKey:@"group"] : [Data setObject:@"nil" forKey:@"group"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    
    return list;
}

-(NSMutableDictionary *)constructGroupAddUser
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    (f_group != nil) ? [Data setObject:f_group forKey:@"group"] : [Data setObject:@"nil" forKey:@"group"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    if (f_user != nil)
        [Data setObject:f_user forKey:@"user"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    
    return list;
}

-(NSMutableDictionary *)constructGroupAddUsers
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    (f_group != nil) ? [Data setObject:f_group forKey:@"group"] : [Data setObject:@"nil" forKey:@"group"];
    if (f_groupName != nil)
        [Data setObject:f_groupName forKey:@"groupName"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    if (f_users != nil)
        [Data setObject:f_users forKey:@"users"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    
    return list;
}

-(NSMutableDictionary *)constructGroupAllGroups
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    
    return list;
}


-(NSMutableDictionary *)constructGroupCreateGroup
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    (f_group != nil) ? [Data setObject:f_group forKey:@"group"] : [Data setObject:@"nil" forKey:@"group"];
    if (f_groupName != nil)
        [Data setObject:f_groupName forKey:@"groupName"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    
    return list;
}

-(NSMutableDictionary *)constructGroupDelGroup
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    (f_group != nil) ? [Data setObject:f_group forKey:@"group"] : [Data setObject:@"nil" forKey:@"group"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    
    return list;
}

-(NSMutableDictionary *)constructGroupDelUser
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    (f_group != nil) ? [Data setObject:f_group forKey:@"group"] : [Data setObject:@"nil" forKey:@"group"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    if (f_user != nil)
        [Data setObject:f_user forKey:@"user"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    
    return list;
}

-(NSMutableDictionary *)constructGroupDelUsers
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    (f_group != nil) ? [Data setObject:f_group forKey:@"group"] : [Data setObject:@"nil" forKey:@"group"];
    
    if (f_groupName != nil)
        [Data setObject:f_groupName forKey:@"groupName"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    if (f_users != nil)
        [Data setObject:f_users forKey:@"users"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    
    return list;
}

-(NSMutableDictionary *)constructGroupGrant
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    
    (f_action != nil) ? [Data setObject:f_action forKey:@"action"] : [Data setObject:@"nil" forKey:@"action"];
    (f_group != nil) ? [Data setObject:f_group forKey:@"group"] : [Data setObject:@"nil" forKey:@"group"];
    
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    (f_resource != nil) ? [Data setObject:f_resource forKey:@"resource"] : [Data setObject:@"nil" forKey:@"resource"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    
    return list;
}

-(NSMutableDictionary *)constructGroupUsers
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
   
    (f_group != nil) ? [Data setObject:f_group forKey:@"group"] : [Data setObject:@"nil" forKey:@"group"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    
    return list;
}

-(NSMutableDictionary *)constructGroupGroups
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    
    return list;
}

-(NSMutableDictionary *)constructGroupListGrants
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    
    (f_group != nil) ? [Data setObject:f_group forKey:@"group"] : [Data setObject:@"nil" forKey:@"group"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    
    return list;
}

-(NSMutableDictionary *)constructGroupMemberOf
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    
    (f_group != nil) ? [Data setObject:f_group forKey:@"group"] : [Data setObject:@"nil" forKey:@"group"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    
    return list;
}

-(NSMutableDictionary *)constructGroupMGrant
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    
    (f_actions != nil) ? [Data setObject:f_actions forKey:@"actions"] : [Data setObject:@"nil" forKey:@"actions"];
    (f_group != nil) ? [Data setObject:f_group forKey:@"group"] : [Data setObject:@"nil" forKey:@"group"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    (f_resource != nil) ? [Data setObject:f_resource forKey:@"resource"] : [Data setObject:@"nil" forKey:@"resource"];
    
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    
    return list;
}

-(NSMutableDictionary *)constructGroupMRevoke
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    
    (f_actions != nil) ? [Data setObject:f_actions forKey:@"actions"] : [Data setObject:@"nil" forKey:@"actions"];
    (f_group != nil) ? [Data setObject:f_group forKey:@"group"] : [Data setObject:@"nil" forKey:@"group"];
    
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    (f_resource != nil) ? [Data setObject:f_resource forKey:@"resource"] : [Data setObject:@"nil" forKey:@"resource"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    
    return list;
}

-(NSMutableDictionary *)constructGroupMyGroups
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    
    return list;
}

-(NSMutableDictionary *)constructGroupRevoke
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    
    (f_action != nil) ? [Data setObject:f_action forKey:@"action"] : [Data setObject:@"nil" forKey:@"action"];
    (f_group != nil) ? [Data setObject:f_group forKey:@"group"] : [Data setObject:@"nil" forKey:@"group"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    (f_resource != nil) ? [Data setObject:f_resource forKey:@"resource"] : [Data setObject:@"nil" forKey:@"resource"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    
    return list;
}



// --- GDA SERVICES --- //

-(NSMutableDictionary *)constructGdaGet
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    
    (f_key != nil) ? [Data setObject:f_key forKey:@"key"] : [Data setObject:@"nil" forKey:@"key"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    (f_table != nil) ? [Data setObject:f_table forKey:@"table"] : [Data setObject:@"nil" forKey:@"table"];
    
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    return list;
}

-(NSMutableDictionary *)constructGdaCells
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    
    (f_column != nil) ? [Data setObject:f_column forKey:@"column"] : [Data setObject:@"nil" forKey:@"column"];
    (f_key != nil) ? [Data setObject:f_key forKey:@"key"] : [Data setObject:@"nil" forKey:@"key"];
    (f_MKey2 != nil) ? [Data setObject:f_MKey2 forKey:@"key2"] : [Data setObject:@"nil" forKey:@"key2"];
    
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    (f_table != nil) ? [Data setObject:f_table forKey:@"table"] : [Data setObject:@"nil" forKey:@"table"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    return list;
}

-(NSMutableDictionary *)constructGdaInc
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
 
    (f_table != nil) ? [Data setObject:f_table forKey:@"table"] : [Data setObject:@"nil" forKey:@"table"];
    (f_data != nil) ? [Data setObject:f_data forKey:@"data"] : [Data setObject:@"nil" forKey:@"data"];
    (f_column != nil) ? [Data setObject:f_column forKey:@"column"] : [Data setObject:@"nil" forKey:@"column"];
    (f_key != nil) ? [Data setObject:f_key forKey:@"key"] : [Data setObject:@"nil" forKey:@"key"];
    
    if (f_key2 != nil)
        [Data setObject:f_key2 forKey:@"key2"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    return list;
}

-(NSMutableDictionary *)constructGdaList
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    if (f_columns != nil)
        [Data setObject:f_columns forKey:@"columns"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    if (f_page != nil)
        [Data setObject:f_page forKey:@"page"];
    (f_table != nil) ? [Data setObject:f_table forKey:@"table"] : [Data setObject:@"nil" forKey:@"table"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    return list;
}

-(NSMutableDictionary *)constructGdaPut
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    
    (f_table != nil) ? [Data setObject:f_table forKey:@"table"] : [Data setObject:@"nil" forKey:@"table"];
    (f_data != nil) ? [Data setObject:f_data forKey:@"data"] : [Data setObject:@"nil" forKey:@"data"];
    (f_column != nil) ? [Data setObject:f_column forKey:@"column"] : [Data setObject:@"nil" forKey:@"column"];
    (f_key != nil) ? [Data setObject:f_key forKey:@"key"] : [Data setObject:@"nil" forKey:@"key"];
    
    if (f_key2 != nil)
        [Data setObject:f_key2 forKey:@"key2"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    return list;
    
}

-(NSMutableDictionary *)constructGdaPuts
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
   
    (f_table != nil) ? [Data setObject:f_table forKey:@"table"] : [Data setObject:@"nil" forKey:@"table"];
    (f_MKey2 != nil) ? [Data setObject:f_MKey2 forKey:@"rows"] : [Data setObject:@"nil" forKey:@"rows"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    return list;
    
}

-(NSMutableDictionary *)constructGdaRange
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    
    if (f_columns != nil)
        [Data setObject:f_columns forKey:@"columns"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    if (f_page != nil)
        [Data setObject:f_page forKey:@"page"];
    
    (f_start != nil) ? [Data setObject:f_start forKey:@"start"] : [Data setObject:@"nil" forKey:@"start"];
    (f_stop != nil) ? [Data setObject:f_stop forKey:@"stop"] : [Data setObject:@"nil" forKey:@"stop"];
    (f_table != nil) ? [Data setObject:f_table forKey:@"table"] : [Data setObject:@"nil" forKey:@"table"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    return list;
}


-(NSMutableDictionary *)constructGdaRemoveCell
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    
    (f_column != nil) ? [Data setObject:f_column forKey:@"column"] : [Data setObject:@"nil" forKey:@"column"];
    (f_key != nil) ? [Data setObject:f_key forKey:@"key"] : [Data setObject:@"nil" forKey:@"key"];
    (f_key2 != nil) ? [Data setObject:f_key2 forKey:@"key2"] : [Data setObject:@"nil" forKey:@"key2"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    (f_table != nil) ? [Data setObject:f_table forKey:@"table"] : [Data setObject:@"nil" forKey:@"table"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    return list;
}

-(NSMutableDictionary *)constructGdaRemoveColumn
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    
    (f_column != nil) ? [Data setObject:f_column forKey:@"column"] : [Data setObject:@"nil" forKey:@"column"];
    (f_key != nil) ? [Data setObject:f_key forKey:@"key"] : [Data setObject:@"nil" forKey:@"key"];

    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    (f_table != nil) ? [Data setObject:f_table forKey:@"table"] : [Data setObject:@"nil" forKey:@"table"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    return list;
}

-(NSMutableDictionary *)constructGdaRemoveRow
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    (f_key != nil) ? [Data setObject:f_key forKey:@"key"] : [Data setObject:@"nil" forKey:@"key"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    (f_table != nil) ? [Data setObject:f_table forKey:@"table"] : [Data setObject:@"nil" forKey:@"table"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    return list;
}

// ------------------- //

// --- S3 SERVICES --- //


-(NSMutableDictionary *)constructS3Cp
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    
    if (f_oldPath != nil)
        [Data setObject:f_oldPath forKey:@"oldPath"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    if (f_path != nil)
        [Data setObject:f_path forKey:@"path"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    return list;
}

-(NSMutableDictionary *)constructS3Du
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    if (f_path != nil)
        [Data setObject:f_path forKey:@"path"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    return list;
}

-(NSMutableDictionary *)constructS3FreeUploadUrl
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    
    if (f_contentType != nil)
        [Data setObject:f_contentType forKey:@"contentType"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    if (f_path != nil)
        [Data setObject:f_path forKey:@"path"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    return list;
}

-(NSMutableDictionary *)constructS3Link
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    
    if (f_oldPath != nil)
        [Data setObject:f_oldPath forKey:@"oldPath"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    if (f_path != nil)
        [Data setObject:f_path forKey:@"path"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    return list;
}

-(NSMutableDictionary *)constructS3Ls
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    
    if (!f_channel || !f_client.clientID){
        NSLog(@"Too early for calling a service. Retrieving Handshake crutial data first.");
        return nil;
    }
    
    (f_folder != nil) ? [Data setObject:f_folder forKey:@"folder"] : [Data setObject:@"" forKey:@"folder"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    if (f_page != nil)
        [Data setObject:f_page forKey:@"page"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    return list;
}

-(NSMutableDictionary *)constructS3MkDir
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    
    (f_folder != nil) ? [Data setObject:f_folder forKey:@"folder"] : [Data setObject:@"" forKey:@"folder"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    if ([NSNumber numberWithBool:f_parents] != nil)
        [Data setObject:[NSNumber numberWithBool:f_parents] forKey:@"parents"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    return list;
}

-(NSMutableDictionary *)constructS3Mv
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    
    if (f_oldPath != nil)
        [Data setObject:f_oldPath forKey:@"oldPath"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    if (f_path != nil)
        [Data setObject:f_path forKey:@"path"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    return list;
}

-(NSMutableDictionary *)constructS3NewFile
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    
    (f_guid != nil) ? [Data setObject:f_guid forKey:@"guid"] : [Data setObject:@"" forKey:@"guid"];
    if (f_metadata != nil)
        [Data setObject:f_metadata forKey:@"metadata"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    if (f_tags != nil)
        [Data setObject:f_path forKey:@"tags"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    return list;
}

-(NSMutableDictionary *)constructS3NewUploadUrl
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    
    if (f_contentType != nil)
        [Data setObject:f_contentType forKey:@"contentType"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    if (f_path != nil)
        [Data setObject:f_path forKey:@"path"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    return list;
}

-(NSMutableDictionary *)constructS3Rm
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    if (f_path != nil)
        [Data setObject:f_path forKey:@"path"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    return list;
}

-(NSMutableDictionary *)constructS3UpdateMeta
{
    NSMutableDictionary *Data = [NSMutableDictionary dictionary];
    
    if (f_metadata != nil)
        [Data setObject:f_metadata forKey:@"metadata"];
    if (f_metadataFiles != nil)
        [Data setObject:f_metadataFiles forKey:@"metadataFiles"];
    if (f_owner != nil)
        [Data setObject:f_owner forKey:@"owner"];
    if (f_path != nil)
        [Data setObject:f_path forKey:@"path"];
    NSMutableDictionary *list = [NSMutableDictionary dictionary];
    [list setObject:f_channel forKey:@"channel"];
    [list setObject:f_client.clientID forKey:@"clientId"];
    [list setObject:@"false" forKey:@"successful"];
    [list setObject:[NSArray arrayWithObject:@"long-polling"] forKey:@"connectionType"];
    [list setObject:Data forKey:@"data"];
    return list;
}


// ------------------- //

@end
