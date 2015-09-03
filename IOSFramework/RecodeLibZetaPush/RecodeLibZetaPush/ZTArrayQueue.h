
#import <Foundation/Foundation.h>
#import "ZTQueue.h"


@interface ZTArrayQueue : NSObject <ZTQueue>
{
@private
	NSMutableArray *m_array;
	id<ZTQueueDelegate> m_delegate;
}

@end
