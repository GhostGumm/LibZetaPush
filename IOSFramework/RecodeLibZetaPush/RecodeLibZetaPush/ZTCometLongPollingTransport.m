#import "ZTCometLongPollingTransport.h"
#import "ZTCometClient.h"
#import "ZTCometMessage.h"
#import "ZTQueue.h"
#import "SBJson4.h"
#import "ZTGlobalCallbacks+ZTGlobalCallbacks.h"
#import "Reachability.h"

@interface ZTCometLongPollingTransport ()

- (NSURLConnection *)sendMessages:(NSArray *)messages;
- (NSArray *)outgoingMessages;
- (NSURLRequest *)requestWithMessages:(NSArray *)messages;
- (id)keyWithConnection:(NSURLConnection *)connection;

@end

@implementation ZTCometLongPollingTransport

- (id)initWithClient:(ZTCometClient *)client
{
	if ((self = [super init]))
	{
		m_client = [client retain];
        
		m_responseDatas = [[NSMutableDictionary alloc] initWithCapacity:2];
	}
	return self;
}

- (void)dealloc
{
	[m_responseDatas release];
	[m_client release];
	[super dealloc];
}

- (void)start
{
	[self performSelectorInBackground:@selector(main) withObject:nil];
}

- (void)cancel
{
	m_shouldCancel = YES;
}

#pragma mark -

- (void)main
{
    NSArray *messages = [[NSArray alloc] init];
    NSURLConnection *connection = [[NSURLConnection alloc] init];
    
    NSLog(@"using long-polling transport");

     do
    {
        BOOL isPolling = '\0';
        BOOL isConnectTime = '\0';
        
        messages = [self outgoingMessages];

        if ([messages count] == 0)
        {
            isPolling = YES;
            if (m_client.state == ZTCometStateConnected)
                    isConnectTime = true;
            else
                [NSThread sleepForTimeInterval:0.01];
        }
        if ([messages count] != 0){
            connection = [self sendMessages:messages];
        }
        else if ([messages count] == 0 && isConnectTime == YES){
            isPolling = YES;
            NSArray *noConn = [[NSArray alloc] autorelease];
            noConn = [NSArray arrayWithObject:m_client.sendConnectMessage];
            connection = [self sendMessages:noConn];
            
            [noConn release];
            noConn = nil;
        }
        
        if (connection)
        {
            NSRunLoop *runLoop = [NSRunLoop currentRunLoop];
            while ([runLoop runMode:NSDefaultRunLoopMode beforeDate:[NSDate dateWithTimeIntervalSinceNow:0.3]])
            {
                if (isPolling)
                {
                    if (m_shouldCancel)
                    {
                        m_shouldCancel = NO;
                        [connection cancel];
                    }
                    else
                    {
                        isPolling = YES;
                        
                        messages = [self outgoingMessages];
                        if ([messages count] != 0 && m_client.state == ZTCometStateConnected){
                                connection =  [self sendMessages:messages];
                                [messages release];
                                messages = nil;
                            }
                            else{
                                [messages release];
                                messages = nil;
                        }
                    }
                }
            }
        }
        
    } while (m_client.state != ZTCometStateDisconnected);
}

- (NSURLConnection *)sendMessages:(NSArray *)messages
{
    //acctually send the message
	NSURLConnection *connection = [[NSURLConnection alloc] autorelease];
    
	if ([messages count] != 0)
	{
		NSURLRequest *request = [self requestWithMessages:messages];
    
        [[NSURLCache sharedURLCache] removeCachedResponseForRequest:request];
        
		connection = [NSURLConnection connectionWithRequest:request delegate:self];
		if (connection)
		{
			NSRunLoop *runLoop = [NSRunLoop currentRunLoop];
			[connection scheduleInRunLoop:runLoop forMode:[runLoop currentMode]];
			[connection start];
		}
    }
	return connection;
}

- (NSArray *)outgoingMessages
{
	NSMutableArray *messages = [NSMutableArray array];
	ZTCometMessage *message;
	id<ZTQueue> outgoingQueue = [m_client outgoingQueue];
	while ((message = [outgoingQueue removeObject]))
		[messages addObject:message];
	return messages;
    
}

- (NSURLRequest *)requestWithMessages:(NSArray *)messages
{
	NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:m_client.endpointURL];
	
	SBJson4Writer *jsonWriter = [[SBJson4Writer alloc] init];
    
    NSData *jbody = [jsonWriter dataWithObject:messages];
    [jsonWriter release];
    
    
	[request setHTTPMethod:@"POST"];
	[request setValue:@"application/json;charset=UTF-8" forHTTPHeaderField:@"Content-Type"];
    [request setHTTPBody:jbody];

    
	NSNumber *timeout = [m_client.advice objectForKey:@"timeout"];
	if (timeout)
		[request setTimeoutInterval:([timeout floatValue] /1000)];
	
	return request;
}

- (id)keyWithConnection:(NSURLConnection *)connection
{
	return [NSNumber numberWithUnsignedInteger:[connection hash]];
}

#pragma mark - NSURLConnectionDelegate

- (NSCachedURLResponse *)connection:(NSURLConnection *)connection willCacheResponse:(NSCachedURLResponse *)cachedResponse {
    return nil;
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
	NSMutableData *responseData = [m_responseDatas objectForKey:[self keyWithConnection:connection]];
	[responseData appendData:data];
    
    [[NSURLCache sharedURLCache] removeAllCachedResponses];
    [[NSURLCache sharedURLCache] setMemoryCapacity:0];
    [[NSURLCache sharedURLCache] setDiskCapacity:0];
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
    [m_responseDatas setObject:[NSMutableData data] forKey:[self keyWithConnection:connection]];
    [[NSURLCache sharedURLCache] removeAllCachedResponses];
    [[NSURLCache sharedURLCache] setMemoryCapacity:0];
    [[NSURLCache sharedURLCache] setDiskCapacity:0];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection
{
	NSData *responseData = [[m_responseDatas objectForKey:[self keyWithConnection:connection]] retain];
   
	[m_responseDatas removeObjectForKey:[self keyWithConnection:connection]];
    
    NSError *error= nil;
    NSArray *responses = [NSJSONSerialization JSONObjectWithData:responseData options:kNilOptions error:&error];
    if (responses != nil){
        NSLog(@"Actuall real response!!!!!! ==  %@", responses);
        
	[responseData release];
	responseData = nil;
	
	id<ZTQueue> incomingQueue = [m_client incomingQueue];
	
        for (NSDictionary *messageData in responses)
        {
            ZTCometMessage *message = [ZTCometMessage messageWithJson:messageData];
            [incomingQueue addObject:message];
        }
        [connection release];
    }
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
	[m_responseDatas removeObjectForKey:[self keyWithConnection:connection]];
}

@end
