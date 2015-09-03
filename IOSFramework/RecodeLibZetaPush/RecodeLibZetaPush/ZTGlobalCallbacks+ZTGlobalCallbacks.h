//
//  ZTGlobalCallbacks+ZTGlobalCallbacks.h
//  RecodeLibZetaPush
//
//  Created by Hakim on 12/08/15.
//  Copyright (c) 2015 Hakim. All rights reserved.
//

#import "ZTCometClient.h"
#import "ZTCometMessage.h"

@interface ZTGlobalCallbacks :ZTCometClient
{
@public
    ZTCometClient *client;
}

// ------ General purpose variables ------ //
@property (nonatomic, readwrite)bool processed;
@property (nonatomic, readwrite) SEL selector;
@property (nonatomic, readwrite) SEL messageSelector;
@property (nonatomic, retain)ZTCometClient *client;
@property (nonatomic, retain) NSString *channel;
@property (nonatomic, readwrite)bool noErase;

// ------ General purpose Methods ------ //

-(id)setClientDuringProcess:(ZTCometClient *)context;
-(id)initWithClientContext:(ZTCometClient *)context Selector:(SEL)selector FiringSelector:(SEL) ignitionSel;

// ************************************** //




// ***------- Query purpose variables ------*** //

// --- For ls service --- //

@property (nonatomic, retain) NSString *folder;
-(NSMutableDictionary *)constructZpfsJson;

// ************************************** //



// --- For Group Services --- //

// ----> Variables to use in Json construction
@property (nonatomic, retain)NSString *cmd;
@property (nonatomic, retain)NSMutableDictionary *GroupData;
@property (nonatomic, retain)NSString *from;
@property (nonatomic, retain)NSString *fromResource;
@property (nonatomic, retain)NSString *owner;
@property (nonatomic, retain)NSString *resource;
@property (nonatomic, retain)NSString *answerRes;
@property (nonatomic, retain)NSString *askRes;
@property (nonatomic, retain)NSArray *capabilities;
@property (nonatomic, retain)NSArray *users;
@property (nonatomic, retain)NSString *action;
@property (nonatomic, retain)NSArray *actions;
@property BOOL avaible;
@property (nonatomic, retain)NSString *uid;
@property (nonatomic, retain)NSString *user;
@property (nonatomic, retain)NSString *group;
@property (nonatomic, retain)NSString *groupName;

// ----> Methods To Construct Json Dictionnary
-(NSMutableDictionary *)constructGroupAddListener;
-(NSMutableDictionary *)constructGroupCapabilities;
-(NSMutableDictionary *)constructGroupExecute;
-(NSMutableDictionary *)constructGroupNotify;
-(NSMutableDictionary *)constructGroupPing;
-(NSMutableDictionary *)constructGroupPong;
-(NSMutableDictionary *)constructGroupRemoveListener;
-(NSMutableDictionary *)constructGroupAddMe;
-(NSMutableDictionary *)constructGroupAddUser;
-(NSMutableDictionary *)constructGroupAddUsers;
-(NSMutableDictionary *)constructGroupAllGroups;
-(NSMutableDictionary *)constructGroupCreateGroup;
-(NSMutableDictionary *)constructGroupDelGroup;
-(NSMutableDictionary *)constructGroupDelUser;
-(NSMutableDictionary *)constructGroupDelUsers;
-(NSMutableDictionary *)constructGroupGrant;
-(NSMutableDictionary *)constructGroupUsers;
-(NSMutableDictionary *)constructGroupGroups;
-(NSMutableDictionary *)constructGroupListGrants;
-(NSMutableDictionary *)constructGroupMemberOf;
-(NSMutableDictionary *)constructGroupMGrant;
-(NSMutableDictionary *)constructGroupMRevoke;
-(NSMutableDictionary *)constructGroupMyGroups;
-(NSMutableDictionary *)constructGroupRevoke;

// ************************************** //



// --- For GDA (Generic Data Service) Services --- //

// ----> Variables to use in Json construction
@property  BOOL *map;
@property (nonatomic, retain) NSString *name;
@property (nonatomic, retain) NSString *GdaDataType;
@property (nonatomic, retain) NSString *type;
@property (nonatomic, retain) NSString *column;
@property (nonatomic, retain) NSArray  *columns;//
@property (nonatomic, retain) NSString *key;
@property (nonatomic, retain) NSArray *multipleKey2;
@property (nonatomic, retain) NSString *key2;
@property (nonatomic, retain) NSString *table;
@property (nonatomic, retain) id         data;
@property (nonatomic, retain) NSMutableDictionary  *page;
@property (nonatomic, retain) NSString *start;
@property (nonatomic, retain) NSString *stop;

// ----> Methods To Construct Json Dictionnary
-(NSMutableDictionary *)constructGdaGet;
-(NSMutableDictionary *)constructGdaCells;
-(NSMutableDictionary *)constructGdaInc;
-(NSMutableDictionary *)constructGdaList;
-(NSMutableDictionary *)constructGdaPut;
-(NSMutableDictionary *)constructGdaPuts;
-(NSMutableDictionary *)constructGdaRange;
-(NSMutableDictionary *)constructGdaRemoveCell;
-(NSMutableDictionary *)constructGdaRemoveColumn;
-(NSMutableDictionary *)constructGdaRemoveRow;


// ************************************** //


// --- For GDA (Generic Data Service) Services --- //

// ----> Variables to use in Json construction
@property (nonatomic, retain) NSString *oldPath;
@property (nonatomic, retain) NSString *path;
@property (nonatomic, retain) NSString *contentType;
@property BOOL parents;
@property (nonatomic, retain) NSString *guid;
@property (nonatomic, retain) NSArray  *tags;
@property (nonatomic, retain) NSMutableDictionary *metadata;
@property (nonatomic, retain) NSMutableDictionary *metadataFiles;


// ----> Methods To Construct Json Dictionnary

-(NSMutableDictionary *)constructS3Cp;
-(NSMutableDictionary *)constructS3Du;
-(NSMutableDictionary *)constructS3FreeUploadUrl;
-(NSMutableDictionary *)constructS3Link;
-(NSMutableDictionary *)constructS3Ls;
-(NSMutableDictionary *)constructS3MkDir;
-(NSMutableDictionary *)constructS3Mv;
-(NSMutableDictionary *)constructS3NewFile;
-(NSMutableDictionary *)constructS3NewUploadUrl;
-(NSMutableDictionary *)constructS3Rm;
-(NSMutableDictionary *)constructS3UpdateMeta;

// ************************************** //

@end
