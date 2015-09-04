#import "ZTCometClient.h"
#import <libkern/OSAtomic.h>
#import "ZTCometLongPollingTransport.h"
#import "ZTCometMessage.h"
#import "ZTCometSubscription.h"
#import "ZTConcurrentQueue.h"
#import "ZTQueueProcessor.h"
#import "ZTGlobalCallbacks+ZTGlobalCallbacks.h"
#import "ViewController.h"
#import "Reachability.h"

@interface ZTCometClient ()

- (NSString *)nextMessageID;
- (void)sendMessage:(ZTCometMessage *)message;
- (void)handleMessage:(ZTCometMessage *)message;

@end


@implementation ZTCometClient

@synthesize clientID = m_clientID,
    pendingInvocation = m_pInvoke,
    type = m_type,
    businessID = m_businessID,
    deployementID = m_deployementID,
    login = m_login,
    password = m_password ,
    userID = m_userID,
	endpointURL = m_endpointURL,
	state = m_state,
	advice = m_advice,
	delegate = m_delegate;

- (id)initWithURL:(NSURL *)endpointURL
{
	if ((self = [super init]))
	{
		m_endpointURL = [endpointURL retain];
		m_pendingSubscriptions = [[NSMutableDictionary alloc] init];
		m_subscriptions = [[NSMutableArray alloc] init];
		m_outgoingQueue = [[ZTConcurrentQueue alloc] init];
        m_incomingQueue = [[ZTConcurrentQueue alloc] init];
	}
	return self;
}

-(id)initWithAllInfo:(NSURL *)endpointURL BusinessId:(NSString *)bID DeployementId:(NSString *)depID Login:(NSString *)login PassWord:(NSString *)pass
{
    
    if (self = [super init])
    {
        m_endpointURL = [endpointURL retain];
        m_pendingSubscriptions = [[NSMutableDictionary alloc] init];
        m_subscriptions = [[NSMutableArray alloc] init];
        m_outgoingQueue = [[ZTConcurrentQueue alloc] init];
        m_incomingQueue = [[ZTConcurrentQueue alloc] init];
        m_businessID = [bID retain];
        m_deployementID = [depID retain];
        m_login = [login retain];
        m_password = [pass retain];
        m_pInvoke = [[NSMutableArray alloc] init];
        NSString *myType = [NSString stringWithFormat:@"%@.%@.%@",bID, depID, @"simple"];
        m_type = myType;
    }
    return self;
}

- (void)dealloc
{
	[m_transport release];
	[m_incomingQueue release];
	[m_outgoingQueue release];
	[m_subscriptions release];
	[m_pendingSubscriptions release];
	[m_endpointURL release];
	[m_clientID release];
	[m_incomingProcessor release];
	[m_advice release];
	[super dealloc];
}

- (void)scheduleInRunLoop:(NSRunLoop *)runLoop forMode:(NSString *)mode
{    
	m_incomingProcessor = [[ZTQueueProcessor alloc] initWithTarget:self selector:@selector(processIncomingMessages)];
    [m_incomingQueue setDelegate:m_incomingProcessor];
    
    [m_incomingProcessor scheduleInRunLoop:runLoop forMode:mode];
}

- (ZTCometMessage *)sendConnectMessage
{
    ZTCometMessage *message = [[ZTCometMessage alloc] autorelease];
    message.channel = @"/meta/connect";
    message.clientID = m_clientID;
    message.successful = [[NSNumber alloc] initWithBool:false];
    message.version = @"1.0";
    message.supportedConnectionTypes = [NSArray arrayWithObject:@"long-polling"];
    
    return message;
}

- (ZTCometMessage *)sendHandshakeMessage
{
    ZTCometMessage *message = [[ZTCometMessage alloc] autorelease];
    
    message.channel = @"/meta/handshake";
    message.login = m_login;
    message.password = m_password;
    message.action = @"authenticate";
    message.type = m_type;
    message.resource = @"resource0";
    message.version = @"none";
    message.successful = [[NSNumber alloc] initWithBool:false];
    message.supportedConnectionTypes = [NSArray arrayWithObject:@"long-polling"];
    
    return message;
}


- (ZTCometMessage *)handshake
{
	m_state = ZTCometStateConnecting;
    
	ZTCometMessage *message = [[ZTCometMessage alloc] init];
    message = [self sendHandshakeMessage];
	[self sendMessage:message];
    [message release];
	return message;
}

- (ZTCometMessage *)disconnect
{
	m_state = ZTCometStateDisconnecting;
	
	ZTCometMessage *message = [ZTCometMessage messageWithChannel:@"/meta/disconnect"];
	[self sendMessage:message];
	return message;
}

- (ZTCometMessage *)subscribeToChannel:(NSString *)channel target:(id)target selector:(SEL)selector
{
    //For Further Evo
    ZTCometMessage *message = [ZTCometMessage messageWithChannel:@"/service/GmY-HuzW/2vC3/ls"];
	message.ID = [self nextMessageID];
	message.subscription = channel;
	ZTCometSubscription *subscription = [[[ZTCometSubscription alloc] initWithChannel:channel target:target selector:selector] autorelease];
	@synchronized(m_pendingSubscriptions)
	{
		[m_pendingSubscriptions setObject:subscription forKey:message.ID];
	}
	[self sendMessage:message];
	return message;
}

- (ZTCometMessage *)unsubsubscribeFromChannel:(NSString *)channel target:(id)target selector:(SEL)selector
{
    //For Further Evo
	ZTCometMessage *message = [ZTCometMessage messageWithChannel:@"/meta/unsubscribe"];
	message.ID = [self nextMessageID];
	message.subscription = channel;
	@synchronized(m_subscriptions)
	{
		NSMutableIndexSet *indexes = [NSMutableIndexSet indexSet];
		NSUInteger count = [m_subscriptions count];
		for (NSUInteger i = 0; i < count; i++)
		{
			ZTCometSubscription *subscription = [m_subscriptions objectAtIndex:i];
			if ([subscription.channel isEqualToString:channel] && subscription.target == target && subscription.selector == selector)
			{
				[indexes addIndex:i]; 
			}
		}
		[m_subscriptions removeObjectsAtIndexes:indexes];
	}
	return message;
}

- (ZTCometMessage *)publishData:(id)data toChannel:(NSString *)channel
{
    //For Further Evo
	ZTCometMessage *message = [ZTCometMessage messageWithChannel:channel];
	message.data = data;
	[self sendMessage:message];
	return message;
}

#pragma mark -

- (id<ZTQueue>)outgoingQueue
{
	return m_outgoingQueue;
}

- (id<ZTQueue>)incomingQueue
{
	return m_incomingQueue;
}

#pragma mark -

- (NSString *)nextMessageID
{
	return [NSString stringWithFormat:@"%d", OSAtomicIncrement32Barrier(&m_messageCounter)];
}

- (void)sendMessage:(ZTCometMessage *)message
{
	
	if (!message.ID)
		message.ID = [self nextMessageID];
    NSLog(@"Sending message : %@", message);
    
	[m_outgoingQueue addObject:message];
	
	if (m_transport == nil)
	{
		m_transport = [[ZTCometLongPollingTransport alloc] initWithClient:self];
		[m_transport start];
	}
}

- (void)handleMessage:(ZTCometMessage *)message
{
	NSString *channel = message.channel;
	if ([channel hasPrefix:@"/meta/"])
	{
		if ([channel isEqualToString:@"/meta/handshake"])
		{
			if ([message.successful boolValue])
			{
                m_clientID = [message.clientID retain];
                m_userID = [message.userID retain];
                
                ZTCometMessage *connectMessage = [self sendConnectMessage];
				[self sendMessage:connectMessage];
              
                [m_delegate cometClientHandshakeDidSucceed:self];
			}
			else
			{
				m_state = ZTCometStateDisconnected;
				if (m_delegate && [m_delegate respondsToSelector:@selector(cometClient:handshakeDidFailWithError:)])
					[m_delegate cometClient:self handshakeDidFailWithError:message.error];
			}
		}
		else if ([channel isEqualToString:@"/meta/connect"])
        {
            NSLog(@"Connected and connecting");
            if ([message.reco isEqualToString:@"handshake"])
            {
                NSLog(@"Handshaking Because of unscheduled disconnection");
                [self.outgoingQueue addObject:[self sendHandshakeMessage]];
            }
			if (message.advice)
			{
				[m_advice release];
				m_advice = [message.advice retain];
			}
			if (![message.successful boolValue])
			{
               
				m_state = ZTCometStateDisconnected;
				if (m_delegate && [m_delegate respondsToSelector:@selector(cometClient:connectDidFailWithError:)])
					[m_delegate cometClient:self connectDidFailWithError:message.error];
			}
			else if (m_state == ZTCometStateConnecting)
			{
				m_state = ZTCometStateConnected;
                [m_delegate cometClientConnectDidSucceed:self];

            }
		}
    }
}

- (void)processIncomingMessages
{
	ZTCometMessage *message;
    
           while ((message = [m_incomingQueue removeObject])){
            NSLog(@"Processing message =  %@", message.channel);
            if ([message.channel isEqualToString:@"/meta/connect"] || [message.channel isEqualToString:@"/meta/handshake"])
                [self handleMessage:message];
            else if ([m_pInvoke count] > 0)
            {
                NSArray *copiteur = [[m_pInvoke copy] retain];
                id object;
                for(object in copiteur)
                {
                    ZTGlobalCallbacks *callback = (ZTGlobalCallbacks *)object;
                    if([callback.channel isEqualToString:message.channel])
                    {
                        [callback performSelector:callback.selector withObject:message.JsonBruteData];
                        if (callback.noErase == false)
                            [m_pInvoke removeObject:object];
                        [callback release];
                        callback = nil;
                        break;
                    }
                }
                [copiteur release];
            }
        }
    [message release];
    message = nil;
}

-(BOOL)CheckForConnexion
{
    Reachability *networkReachability = [Reachability reachabilityForInternetConnection];
    NetworkStatus networkStatus = [networkReachability currentReachabilityStatus];
    
    if ((networkStatus = [networkReachability currentReachabilityStatus]) == 0)
    {
        [networkReachability release];
        return false;
    }
    else
    {
        [networkReachability release];
        return true;
    }
}


#pragma mark Services Igniters

-(void)callbackToDeliver:(NSDictionary *)Data
{
    NSLog(@"Going throught callback in the client");
}


//-- ZPFS --------------------------|

-(void)invokeZpfsList:(NSString *)folder SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase String:(NSString *)string
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"ls"];
    
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructZpfsJson)];
    
    callback.channel = mychannel; callback.noErase = noErase;
    callback.folder = folder;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

//--GROUP --------------------------|

-(void)invokeGroupAddListener:(NSString *)cmd data:(id)data from:(NSString *)from fromResource:(NSString *)fromResource owner:(NSString *)owner resource:(NSString *)resource SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"addListener"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGroupAddListener)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.cmd = cmd;
    callback.data = data;
    callback.from = from;
    callback.fromResource = fromResource;
    callback.owner = owner;
    callback.resource = resource;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGroupCapabilities:(NSString *)answer askingResources:(NSString *)askingResources capabilities:(NSArray *)capabilities SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"capabilities"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGroupCapabilities)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.answerRes = answer;
    callback.askRes = askingResources;
    callback.capabilities = capabilities;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGroupExecute:(NSString *)cmd data:(id)data owner:(NSString *)owner resource:(NSString *)resource SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"execute"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGroupExecute)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.cmd = cmd;
    callback.data = data;
    callback.owner = owner;
    callback.resource = resource;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGroupNotify:(NSString *)cmd data:(id)data from:(NSString *)from fromResource:(NSString *)fromResource owner:(NSString *)owner resource:(NSString *)resource SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"notify"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGroupNotify)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.cmd = cmd;
    callback.data = data;
    callback.from = from;
    callback.fromResource = fromResource;
    callback.owner = owner;
    callback.resource = resource;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGroupPing:(NSString *)action SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"ping"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGroupPing)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.action = action;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGroupPong:(NSString *)action avaible:(BOOL)avaible owner:(NSString *)owner resource:(NSString *)resource uid:(NSString *)uid user:(NSString *)user SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"pong"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGroupPong)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.action = action;
    callback.avaible = avaible;
    callback.owner = owner;
    callback.resource = resource;
    callback.uid = uid;
    callback.user = user;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGroupRemoveListener:(NSString *)cmd data:(id)data from:(NSString *)from fromResource:(NSString *)fromResource owner:(NSString *)owner resource:(NSString *)resource SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"removeListener"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGroupRemoveListener)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.cmd = cmd;
    callback.data = data;
    callback.from = from;
    callback.fromResource = fromResource;
    callback.owner = owner;
    callback.resource = resource;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGroupAddMe:(NSString *)group owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"addMe"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGroupAddMe)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.group = group;
    callback.owner = owner;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGroupAddUser:(NSString *)group owner:(NSString *)owner user:(NSString *)user SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"addUser"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGroupAddUser)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.group = group;
    callback.owner = owner;
    callback.user = user;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGroupAddUsers:(NSString *)group groupName:(NSString *)groupName owner:(NSString *)owner users:(NSArray *)users SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"addUsers"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGroupAddUsers)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.group = group;
    callback.groupName = groupName;
    callback.owner = owner;
    callback.users = users;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGroupAllGroups:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"allGroups"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGroupAllGroups)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.owner = owner;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGroupCreateGroup:(NSString *)group groupName:(NSString *)groupName owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"createGroup"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGroupCreateGroup)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.group = group;
    callback.groupName = groupName;
    callback.owner = owner;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGroupDelGroup:(NSString *)group owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"delGroup"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGroupDelGroup)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.group = group;
    callback.owner = owner;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGroupDelUser:(NSString *)group owner:(NSString *)owner user:(NSString *)user SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"delUser"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGroupDelUser)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.group = group;
    callback.owner = owner;
    callback.user = user;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGroupDelUsers:(NSString *)group groupName:(NSString *)groupName owner:(NSString *)owner users:(NSArray *)users SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"delUsers"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGroupDelUsers)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.group = group;
    callback.owner = owner;
    callback.users = users;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGroupGrant:(NSString *)group action:(NSString *)action owner:(NSString *)owner resource:(NSString *)resource SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"grant"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGroupGrant)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.group = group;
    callback.owner = owner;
    callback.action = action;
    callback.resource = resource;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGroupUsers:(NSString *)group owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"groupUsers"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGroupUsers)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.group = group;
    callback.owner = owner;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGroupGroups:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"groups"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGroupGroups)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.owner = owner;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGroupListGrants:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"listGrants"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGroupListGrants)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.owner = owner;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGroupMemberOf:(NSString *)group owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"memberOf"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGroupMemberOf)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.owner = owner;
    callback.group = group;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGroupMGrant:(NSArray *)actions group:(NSString *)group owner:(NSString *)owner ressource:(NSString *)resource SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"mgrant"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGroupMGrant)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.owner = owner;
    callback.group = group;
    callback.actions = actions;
    callback.resource = resource;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGroupMRevoke:(NSArray *)actions group:(NSString *)group owner:(NSString *)owner ressource:(NSString *)resource SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"mrevoke"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGroupMRevoke)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.owner = owner;
    callback.group = group;
    callback.actions = actions;
    callback.resource = resource;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGroupMyGroups:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"myGroups"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGroupMyGroups)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.owner = owner;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

//--GDA --------------------------|

-(void)invokeGdaGet:(NSString *)key TableName:(NSString *)tableName owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"get"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGdaGet)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.table = tableName;
    callback.key = key;
    callback.owner = owner;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGdaCells:(NSString *)column key:(NSString *)key key2:(NSMutableArray *)key2 owner:(NSString *)owner tableName:(NSString *)tableName SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"getCells"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGdaCells)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.table = tableName;
    callback.key = key;
    callback.multipleKey2 = key2;
    callback.column = column;
    callback.owner = owner;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];

}


-(void)invokeGdaInk:(NSString *)table data:(id)data key:(NSString *)key key2:(NSString *)key2 owner:(NSString *)owner column:(NSString *)column SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"inc"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGdaInc)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.table = table;
    callback.data = data;
    callback.key = key;
    callback.key2 = key2;
    callback.owner = owner;
    callback.column = column;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
    
}

-(void)invokeGdaList:(NSString *)table owner:(NSString *)owner page:(NSMutableDictionary *)page columns:(NSArray *)columns SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"list"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGdaList)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.table = table;
    callback.owner = owner;
    callback.columns = columns;
    callback.page = page;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
    
}


-(void)invokeGdaPut:(NSString *)table data:(id)data key:(NSString *)key key2:(NSString *)key2 owner:(NSString *)owner column:(NSString *)column SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"put"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGdaPut)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.table = table;
    callback.data = data;
    callback.key = key;
    callback.key2 = key2;
    callback.owner = owner;
    callback.column = column;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
    
}

-(void)invokeGdaPuts:(NSString *)table rows:(NSArray *)rows owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"puts"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGdaPuts)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.table = table;
    callback.owner = owner;
    callback.multipleKey2 = rows;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
    
}

-(void)invokeGdaRange:(NSString *)table columns:(NSArray *)columns owner:(NSString *)owner page:(NSMutableDictionary *)page start:(NSString *)start stop:(NSString *)stop SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"range"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGdaRange)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.table = table;
    callback.owner = owner;
    callback.columns = columns;
    callback.page = page;
    callback.start = start;
    callback.stop = stop;

    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGdaRemoveCell:(NSString *)table column:(NSString *)column key:(NSString *)key key2:(NSString *)key2 owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"removeCell"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGdaRemoveCell)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.table = table;
    callback.owner = owner;
    callback.key = key;
    callback.key2 = key2;
    callback.column = column;

    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGdaRemoveColumn:(NSString *)table column:(NSString *)column key:(NSString *)key owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"removeColumn"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGdaRemoveColumn)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.table = table;
    callback.owner = owner;
    callback.key = key;
    callback.column = column;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
    [callback release];
}

-(void)invokeGdaRemoveRow:(NSString *)table key:(NSString *)key owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"removeRow"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructGdaRemoveRow)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.table = table;
    callback.owner = owner;
    callback.key = key;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
}

//--UploadS3 --------------------------|

-(void)invokeS3Cp:(NSString *)oldPath path:(NSString *)path owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"cp"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructS3Cp)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.oldPath = oldPath;
    callback.path = path;
    callback.owner = owner;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
}

-(void)invokeS3Du:(NSString *)path owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"removeRow"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructS3Du)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.path = path;
    callback.owner = owner;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
}

-(void)invokeS3FreeUploadUrl:(NSString *)contentType path:(NSString *)path owner:(NSString *)owner SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"freeUploadUrl"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructS3FreeUploadUrl)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.path = path;
    callback.owner = owner;
    callback.contentType = contentType;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
}

-(void)invokeS3Link:(NSString *)oldPath owner:(NSString *)owner path:(NSString *)path SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"link"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructS3Link)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.oldPath = oldPath;
    callback.path = path;
    callback.owner = owner;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
}


-(ZTGlobalCallbacks *)invokeS3Ls:(NSString *)folder owner:(NSString *)owner page:(NSMutableDictionary *)page SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"ls"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructS3Ls)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.folder = folder;
    callback.page = page;
    callback.owner = owner;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
   
    [self.outgoingQueue addObject:message];
    [m_pInvoke addObject:callback];
   
    return callback;
}

-(void)invokeS3MkDir:(NSString *)folder owner:(NSString *)owner parents:(BOOL)parents SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"mkdir"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructS3MkDir)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.folder = folder;
    callback.parents = parents;
    callback.owner = owner;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
}

-(void)invokeS3Mv:(NSString *)oldPath owner:(NSString *)owner path:(NSString *)path SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"mv"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructS3Mv)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.oldPath = oldPath;
    callback.path = path;
    callback.owner = owner;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
}

-(void)invokeS3NewFile:(NSString *)guid metadata:(NSMutableDictionary *)metadata owner:(NSString *)owner tags:(NSArray *)tags SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"newFile"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructS3NewFile)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.guid = guid;
    callback.metadata = metadata;
    callback.tags = tags;
    callback.owner = owner;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
}

-(void)invokeS3NewUploadUrl:(NSString *)contentType owner:(NSString *)owner path:(NSString *)path SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"newUploadUrl"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructS3NewUploadUrl)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.contentType = contentType;
    callback.owner = owner;
    callback.path = path;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
}

-(void)invokeS3Rm:(NSString *)owner path:(NSString *)path SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"rm"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructS3Rm)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.owner = owner;
    callback.path = path;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
}

-(void)invokeS3UpdateMeta:(NSMutableDictionary *)metadata metadataFiles:(NSMutableDictionary *)metadataFiles owner:(NSString *)owner path:(NSString *)path SDepID:(NSString *)SDepID callBackSelector:(SEL)callbackSel noErase:(BOOL)noErase 
{
    NSString *mychannel = [NSString stringWithFormat:@"%@/%@/%@/%@", @"/service",m_businessID, SDepID, @"updateMeta"];
    ZTGlobalCallbacks *callback = [[ZTGlobalCallbacks alloc] initWithClientContext:self Selector:callbackSel FiringSelector:@selector(constructS3UpdateMeta)];
    callback.channel = mychannel; callback.noErase = noErase;
    callback.metadata = metadata;
    callback.metadataFiles = metadataFiles;
    callback.owner = owner;
    callback.path = path;
    
    ZTCometMessage *message = [[ZTCometMessage alloc] initWithCallback:callback];
    message.channel = callback.channel;
    
    [self.outgoingQueue addObject:message];
    
    [m_pInvoke addObject:callback];
}

//--Game Engine --------------------------|





@end
