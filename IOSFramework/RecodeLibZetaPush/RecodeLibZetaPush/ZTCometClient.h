
/*
 Copyright (C) 2015 ZetaPush. All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 * Neither the name of the author (Hakim El bakkali) nor the names of its contributors (Pablo Abreu, Mikael Morvan) may be used
 to endorse or promote products derived from this software without specific
 prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#import <Foundation/Foundation.h>


@class ZTCometLongPollingTransport;
@class ZTCometMessage;
@class ZTCometSubscription;
@class ZTQueueProcessor;
@class ZTGlobalCallbacks;
@protocol ZTCometClientDelegate;
@protocol ZTQueue;

typedef enum
{
	ZTCometStateDisconnected,
	ZTCometStateConnecting,
	ZTCometStateConnected,
	ZTCometStateDisconnecting
} ZTCometState;

@interface ZTCometClient : NSObject
{
@private
	NSURL *m_endpointURL;
	volatile int32_t m_messageCounter;
	NSMutableDictionary *m_pendingSubscriptions; // by id
	NSMutableArray *m_subscriptions;
	ZTCometState m_state;
	NSDictionary *m_advice;
	id<ZTQueue> m_outgoingQueue;
	id<ZTQueue> m_incomingQueue;
	ZTCometLongPollingTransport *m_transport;
	ZTQueueProcessor *m_incomingProcessor;
}



@property (nonatomic, readonly) NSString *businessID;
@property (nonatomic, readonly) NSString *deployementID;
@property (nonatomic, readonly) NSString *login;
@property (nonatomic, readonly) NSString *password;
@property (nonatomic, readonly) NSString *type;
@property (nonatomic, assign) NSMutableArray *pendingInvocation;

@property BOOL post;

@property (nonatomic, assign) NSString *userID;
@property (nonatomic, assign) NSString *clientID;
@property (nonatomic, readonly) NSURL *endpointURL;
@property (nonatomic, readwrite) ZTCometState state;
@property (nonatomic, readonly) NSDictionary *advice;
@property (nonatomic, assign) id<ZTCometClientDelegate> delegate;

// --- Services Invocation --- //

//--- ZPFS SERVICE ---//

-(void)invokeZpfsList:(NSString *)folder SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase String:(NSString *)string;

//--- GROUP SERVICE ---//

-(void)invokeGroupAddListener:(NSString *)cmd data:(id)data from:(NSString *)from fromResource:(NSString *)fromResource owner:(NSString *)owner resource:(NSString *)resource SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGroupCapabilities:(NSString *)answer askingResources:(NSString *)askingResources capabilities:(NSArray *)capabilities SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGroupExecute:(NSString *)cmd data:(id)data owner:(NSString *)owner resource:(NSString *)resource SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGroupNotify:(NSString *)cmd data:(id)data from:(NSString *)from fromResource:(NSString *)fromResource owner:(NSString *)owner resource:(NSString *)resource SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGroupPing:(NSString *)action SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGroupPong:(NSString *)action avaible:(BOOL)avaible owner:(NSString *)owner resource:(NSString *)resource uid:(NSString *)uid user:(NSString *)user SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGroupRemoveListener:(NSString *)cmd data:(id)data from:(NSString *)from fromResource:(NSString *)fromResource owner:(NSString *)owner resource:(NSString *)resource SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGroupAddMe:(NSString *)group owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGroupAddUser:(NSString *)group owner:(NSString *)owner user:(NSString *)user SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGroupAddUsers:(NSString *)group groupName:(NSString *)groupName owner:(NSString *)owner users:(NSArray *)users SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGroupAllGroups:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGroupCreateGroup:(NSString *)group groupName:(NSString *)groupName owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGroupDelGroup:(NSString *)group owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGroupDelUser:(NSString *)group owner:(NSString *)owner user:(NSString *)user SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGroupDelUsers:(NSString *)group groupName:(NSString *)groupName owner:(NSString *)owner users:(NSArray *)users SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGroupGrant:(NSString *)group action:(NSString *)action owner:(NSString *)owner resource:(NSString *)resource SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGroupUsers:(NSString *)group owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGroupGroups:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGroupListGrants:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGroupMemberOf:(NSString *)group owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGroupMGrant:(NSArray *)actions group:(NSString *)group owner:(NSString *)owner ressource:(NSString *)resource SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGroupMRevoke:(NSArray *)actions group:(NSString *)group owner:(NSString *)owner ressource:(NSString *)resource SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGroupMyGroups:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;


// ----------------------------------------------------------------------------- //

//--- GDA SERVICE---//

-(void)invokeGdaGet:(NSString *)key TableName:(NSString *)tableName owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGdaCells:(NSString *)column key:(NSString *)key key2:(NSString *)key2 owner:(NSString *)owner tableName:(NSString *)tableName SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGdaInk:(NSString *)table data:(id)data key:(NSString *)key key2:(NSString *)key2 owner:(NSString *)owner column:(NSString *)column SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGdaList:(NSString *)table owner:(NSString *)owner page:(NSMutableDictionary *)page columns:(NSArray *)columns SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGdaPut:(NSString *)table data:(id)data key:(NSString *)key key2:(NSString *)key2 owner:(NSString *)owner column:(NSString *)column SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGdaPuts:(NSString *)table rows:(NSArray *)rows owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGdaRange:(NSString *)table columns:(NSArray *)columns owner:(NSString *)owner page:(NSMutableDictionary *)page start:(NSString *)start stop:(NSString *)stop SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGdaRemoveCell:(NSString *)table column:(NSString *)column key:(NSString *)key key2:(NSString *)key2 owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGdaRemoveColumn:(NSString *)table column:(NSString *)column key:(NSString *)key owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeGdaRemoveRow:(NSString *)table key:(NSString *)key owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

// ----------------------------------------------------------------------------- //


//--- S3 SERVICE---//

-(void)invokeS3Cp:(NSString *)oldPath path:(NSString *)path owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeS3Du:(NSString *)path owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeS3FreeUploadUrl:(NSString *)contentType path:(NSString *)path owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeS3Link:(NSString *)oldPath owner:(NSString *)owner path:(NSString *)path SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-()invokeS3Ls:(NSString *)folder owner:(NSString *)owner page:(NSMutableDictionary *)page SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeS3MkDir:(NSString *)folder owner:(NSString *)owner parents:(BOOL)parents SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeS3Mv:(NSString *)oldPath owner:(NSString *)owner path:(NSString *)path SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeS3NewFile:(NSString *)guid metadata:(NSMutableDictionary *)metadata owner:(NSString *)owner tags:(NSArray *)tags SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeS3NewUploadUrl:(NSString *)contentType owner:(NSString *)owner path:(NSString *)path SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeS3Rm:(NSString *)owner path:(NSString *)path SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

-(void)invokeS3UpdateMeta:(NSMutableDictionary *)metadata metadataFiles:(NSMutableDictionary *)metadataFiles owner:(NSString *)owner path:(NSString *)path SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase ;

// ----------------------------------------------------------------------------- //

//--- CLASS COMPONENTS---//

-(id)initWithAllInfo:(NSURL *)endpointURL BusinessId:(NSString *)bID DeployementId:(NSString *)depID Login:(NSString *)login PassWord:(NSString *)pass;
- (id)initWithURL:(NSURL *)endpointURL;
- (void)scheduleInRunLoop:(NSRunLoop *)runLoop forMode:(NSString *)mode;
- (ZTCometMessage *)sendConnectMessage;
- (ZTCometMessage *)sendHandshakeMessage;

- (ZTCometMessage *)handshake;
- (ZTCometMessage *)disconnect;
- (ZTCometMessage *)subscribeToChannel:(NSString *)channel target:(id)target selector:(SEL)selector;
- (ZTCometMessage *)unsubsubscribeFromChannel:(NSString *)channel target:(id)target selector:(SEL)selector;
- (ZTCometMessage *)publishData:(id)data toChannel:(NSString *)channel;
- (BOOL)CheckForConnexion;

@end

@interface ZTCometClient (Internal)

- (id<ZTQueue>)outgoingQueue;
- (id<ZTQueue>)incomingQueue;

@end

@protocol ZTCometClientDelegate <NSObject>
@optional
- (void)cometClientHandshakeDidSucceed:(ZTCometClient *)client;
- (void)cometClient:(ZTCometClient *)client handshakeDidFailWithError:(NSError *)error;
- (void)cometClientConnectDidSucceed:(ZTCometClient *)client;
- (void)cometClient:(ZTCometClient *)client connectDidFailWithError:(NSError *)error;
- (void)cometClient:(ZTCometClient *)client subscriptionDidSucceed:(ZTCometSubscription *)subscription;
- (void)cometClient:(ZTCometClient *)client subscription:(ZTCometSubscription *)subscription didFailWithError:(NSError *)error;
@end
