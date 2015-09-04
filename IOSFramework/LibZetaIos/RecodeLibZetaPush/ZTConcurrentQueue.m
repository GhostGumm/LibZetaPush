#import "ZTConcurrentQueue.h"
#import <libkern/OSAtomic.h>


@interface ZTConcurrentQueueNode : NSObject
{
@private
    id m_object;
	ZTConcurrentQueueNode * volatile m_next;
}

@property (nonatomic, retain) id object;
@property (nonatomic, readonly) ZTConcurrentQueueNode *next;

- (BOOL)compareNext:(ZTConcurrentQueueNode *)old andSet:(ZTConcurrentQueueNode *)new;

@end

@implementation ZTConcurrentQueueNode

@synthesize object = m_object, next = m_next;

- (id)initWithObject:(id)object
{
	if ((self = [super init]))
	{
		m_object = [object retain];
	}
	return self;
}

- (void)dealloc
{
	[m_object release];
	[m_next release];
	[super dealloc];
}

- (BOOL)compareNext:(ZTConcurrentQueueNode *)old andSet:(ZTConcurrentQueueNode *)new
{
	return OSAtomicCompareAndSwapPtrBarrier(old, new, (void * volatile *)&m_next);
}

@end

@interface ZTConcurrentQueue ()

- (BOOL)compareHead:(ZTConcurrentQueueNode *)old andSet:(ZTConcurrentQueueNode *)new;
- (BOOL)compareTail:(ZTConcurrentQueueNode *)old andSet:(ZTConcurrentQueueNode *)new;

@end

@implementation ZTConcurrentQueue

- (id)init
{
	if ((self = [super init]))
	{
		ZTConcurrentQueueNode *node = [[ZTConcurrentQueueNode alloc] init];
		m_head = node;
		m_tail = node;
	}
	return self;
}

- (void)addObject:(id)object
{
	ZTConcurrentQueueNode *node = [[ZTConcurrentQueueNode alloc] initWithObject:object];
	while (YES)
	{
		ZTConcurrentQueueNode *tail = m_tail;
		ZTConcurrentQueueNode *next = tail.next;
		if (tail == m_tail)
		{
			if (next == nil)
			{
				if ([tail compareNext:next andSet:node])
				{
					[self compareTail:tail andSet:node];
					break;
				}
			}
			else
			{
				[self compareTail:tail andSet:node];
			}
		}
	}
	if (m_delegate)
		[m_delegate queueDidAddObject:self];
}

- (id)removeObject
{
	while (YES)
    {
		ZTConcurrentQueueNode *head = m_head;
		ZTConcurrentQueueNode *tail = m_tail;
		ZTConcurrentQueueNode *first = head.next;
		if (head == m_head)
		{
			if (head == tail)
			{
				if (first == nil)
					return nil;
				else
					[self compareTail:tail andSet:first];
			}
			else if ([self compareHead:head andSet:first])
			{
				id object = [[first.object retain] autorelease];
				if (object != nil)
				{
					first.object = nil;
					return object;
				}
                
				// else skip over deleted item, continue loop
			}
		}
    }
}

- (void)setDelegate:(id<ZTQueueDelegate>)delegate
{
	m_delegate = delegate;
}

- (BOOL)compareHead:(ZTConcurrentQueueNode *)old andSet:(ZTConcurrentQueueNode *)new
{
	return OSAtomicCompareAndSwapPtrBarrier(old, new, (void * volatile *)&m_head);
}

- (BOOL)compareTail:(ZTConcurrentQueueNode *)old andSet:(ZTConcurrentQueueNode *)new
{
	return OSAtomicCompareAndSwapPtrBarrier(old, new, (void * volatile *)&m_tail);
}

@end
