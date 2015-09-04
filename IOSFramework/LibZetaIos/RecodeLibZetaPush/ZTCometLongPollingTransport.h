
#import <Foundation/Foundation.h>


@class ZTCometClient;

@interface ZTCometLongPollingTransport : NSObject
{
@private
	ZTCometClient *m_client;
	volatile BOOL m_shouldCancel;
	NSMutableDictionary *m_responseDatas;
@public NSMutableData *RData;
}

- (id)initWithClient:(ZTCometClient *)client;
- (void)start;
- (void)cancel;

@end
