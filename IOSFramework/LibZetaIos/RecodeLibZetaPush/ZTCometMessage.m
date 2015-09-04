#import "ZTCometMessage.h"
#import "ZTGlobalCallbacks+ZTGlobalCallbacks.h"

@interface NSDate (ISO8601)

+ (NSDate *)dateWithISO8601String:(NSString *)string;
- (NSString *)ISO8601Representation;

@end

@implementation NSDate (ISO8601)

+ (NSDate *)dateWithISO8601String:(NSString *)string
{
	NSDateFormatter *fmt = [[[NSDateFormatter alloc] init] autorelease];
	[fmt setDateFormat:@"yyyy-MM-ZT'T'HH:mm:ss"];
	return [fmt dateFromString:string];
}

- (NSString *)ISO8601Representation
{
	NSDateFormatter *fmt = [[[NSDateFormatter alloc] init] autorelease];
	[fmt setDateFormat:@"yyyy-MM-ZT'T'HH:mm:ss"];
	return [fmt stringFromDate:self];
}

@end

@interface NSError (Bayeux)

+ (NSError *)errorWithBayeuxFormat:(NSString *)string;
- (NSString *)bayeuxFormat;

@end

@implementation NSError (Bayeux)

+ (NSError *)errorWithBayeuxFormat:(NSString *)string
{
	NSArray *components = [string componentsSeparatedByString:@":"];
	NSInteger code = [[components objectAtIndex:0] integerValue];
	NSDictionary *userInfo = [NSDictionary dictionaryWithObjectsAndKeys:[components objectAtIndex:2], NSLocalizedDescriptionKey, nil];
    
	return [[[NSError alloc] initWithDomain:@"" code:code userInfo:userInfo] autorelease];
}

- (NSString *)bayeuxFormat
{
	NSString *args = @"";
	NSArray *components = [NSArray arrayWithObjects:[NSString stringWithFormat:@"%ld", (long)[self code]], args, [self localizedDescription], nil];
	return [components componentsJoinedByString:@":"];
}

@end

@implementation ZTCometMessage

@synthesize channel = m_channel,
    JsonBruteData = m_JsonData,
    reco = m_reco,
    cback = m_cback,
    userID  = m_userID,
	version = m_version,
    token = m_token,
    login = m_login,
    resource = m_resource,
    password = m_password,
    action = m_action,
    type = m_type,
	minimumVersion = m_minimumVersion,
	supportedConnectionTypes = m_supportedConnectionTypes,
	clientID = m_clientID,
	advice = m_advice,
	connectionType = m_connectionType,
	ID = m_ID,
	timestamp = m_timestamp,
	data = m_data,
	successful = m_successful,
	subscription = m_subscription,
	error = m_error,
	ext = m_ext;

- (void)dealloc
{
    [m_cback release];
    [m_channel release];
    [m_userID release];
    [m_login release];
    [m_password release];
    [m_resource release];
    [m_action release];
    [m_JsonData release];
    [m_type release];
	[m_version release];
	[m_minimumVersion release];
	[m_supportedConnectionTypes release];
	[m_clientID release];
	[m_advice release];
    [m_connectionType release];
	[m_ID release];
	[m_timestamp release];
	[m_data release];
	[m_successful release];
    [m_cback release];
	[m_subscription release];
	[m_error release];
	[m_ext release];
    [super dealloc];
}

- (id)initWithCallback:(ZTGlobalCallbacks *)cb
{
    if ((self = [super init]))
    {
        self.cback = cb;
    }
    
    return self;
}

+ (ZTCometMessage *)messageWithChannel:(NSString *)channel
{
	ZTCometMessage *message = [[[ZTCometMessage alloc] init] autorelease];
	message.channel = channel;
	return message;
}

@end

@implementation ZTCometMessage (JSON)

+ (ZTCometMessage *)messageWithJson:(NSDictionary *)jsonData
{
    
	ZTCometMessage *message = [[[ZTCometMessage alloc] init] autorelease];
    message.JsonBruteData = jsonData;
    
	for (NSString *key in [jsonData keyEnumerator])
	{
		id object = [jsonData objectForKey:key];
        if ([key isEqualToString:@"channel"])
        {
            NSLog(@"message channel = %@", object);
            message.channel = object;
        }
        else if([key isEqualToString:@"action"])
            message.action = object;
        else if([key isEqualToString:@"ext"])
        {
            id auth = [object objectForKey:@"authentication"];
            message.token = [auth objectForKey:@"token"];
            message.userID = [auth objectForKey:@"userId"];
        }
        else if ([key isEqualToString:@"type"])
            message.type = object;
        else if ([key isEqualToString:@"resource"])
            message.resource = object;
		else if ([key isEqualToString:@"version"])
			message.version = object;
		else if ([key isEqualToString:@"minimumVersion"])
			message.minimumVersion = object;
		else if ([key isEqualToString:@"supportedConnectionTypes"])
			message.connectionType = object;
		else if ([key isEqualToString:@"clientId"])
        {
			message.clientID = object;
        }
		else if ([key isEqualToString:@"advice"])
        {
            id reco = [object objectForKey:@"reconnect"];
            message.advice = object;
            if (reco != nil)
                message.reco = reco;
            
        }
		else if ([key isEqualToString:@"connectionType"])
			message.connectionType = object;
		else if ([key isEqualToString:@"id"])
			message.ID = object;
		else if ([key isEqualToString:@"timestamp"])
			message.timestamp = [NSDate dateWithISO8601String:object];
		else if ([key isEqualToString:@"data"])
			message.data = object;
		else if ([key isEqualToString:@"successful"])
        {
            NSNumber *number = [[NSNumber alloc] initWithBool:true];
                message.successful = number;
        }
		else if ([key isEqualToString:@"subscription"])
            message.subscription = object;
		else if ([key isEqualToString:@"error"])
			message.error = [NSError errorWithBayeuxFormat:object];
        if ([key isEqualToString:@"ext"])
			message.ext = object;
	}
    
	return message;
}

- (NSDictionary *)proxyForJson
{
        if ([m_channel isEqualToString:@"/meta/connect"]){
        NSMutableDictionary *connect = [NSMutableDictionary dictionary];
            if (!m_clientID || !m_channel){
                NSLog(@"Crucial Data not yet updated from server, please wait for Connection");
                return nil;
            }
        [connect setObject:@"long-polling" forKey:@"connectionType"];
        [connect setObject:m_channel forKey:@"channel"];
        [connect setObject:m_clientID forKey:@"clientId"];
        [connect setObject:@"false" forKey:@"successful"];
        [connect setObject:@"1.0" forKey:@"version"];
        
        return connect;
    }
    else if([m_channel isEqualToString:@"/meta/handshake"]){
        NSMutableDictionary *Data = [NSMutableDictionary dictionary];
        [Data setObject:m_login forKey:@"login"];
        [Data setObject:m_password forKey:@"password"];
    
        NSMutableDictionary *Auth = [NSMutableDictionary dictionary];
        [Auth setObject:m_action forKey:@"action"];
        [Auth setObject:m_type forKey:@"type"];
        [Auth setObject:m_resource forKey:@"resource"];
        [Auth setObject:m_version forKey:@"version"];
        [Auth setObject:Data forKey:@"data"];
        NSMutableDictionary *Ext =  [NSMutableDictionary dictionary];
        [Ext setObject:Auth  forKey:@"authentication"];
        NSMutableDictionary *handshake = [NSMutableDictionary dictionary];
        [handshake setObject:m_channel forKey:@"channel"];
        [handshake setObject:m_successful forKey:@"successful"];
        [handshake setObject:m_supportedConnectionTypes forKey:@"supportedConnectionTypes"];
        [handshake setObject:Ext forKey:@"ext"];
        
	return handshake;
    }
    else
    {
        if (!m_cback.messageSelector){
            NSLog(@"Selector value = nil. No callback will be called");
            return nil;
        }
        return [m_cback performSelector:m_cback.messageSelector];
    }
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"%@ %@", [super description], [self proxyForJson]];
}

@end
