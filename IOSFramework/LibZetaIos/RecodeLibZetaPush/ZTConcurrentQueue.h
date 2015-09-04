
#import <Foundation/Foundation.h>
#import "ZTQueue.h"


@class ZTConcurrentQueueNode;

/**
 * Lock-free queue based on Doug Lea's ConcurrentLinkedQueue, based on
 * http://www.cs.rochester.edu/u/michael/PODC96.html by Maged M. Michael and Michael L. Scott.
 */

@interface ZTConcurrentQueue : NSObject <ZTQueue>
{
@private
	ZTConcurrentQueueNode * volatile m_head;
	ZTConcurrentQueueNode * volatile m_tail;
	id<ZTQueueDelegate> m_delegate;
}

@end
