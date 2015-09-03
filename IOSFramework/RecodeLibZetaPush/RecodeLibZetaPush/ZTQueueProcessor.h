
#import <Foundation/Foundation.h>
#import "ZTQueue.h"


@interface ZTQueueProcessor : NSObject <ZTQueueDelegate>
{
@private
	id m_target;
	SEL m_selector;
	CFRunLoopSourceRef m_source;
	NSRunLoop *m_runLoop;
	NSString *m_mode;
}

+ (ZTQueueProcessor *)queueProcessorWithQueue:(id<ZTQueue>)queue
									   target:(id)target
									 selector:(SEL)selector;
- (id)initWithTarget:(id)target selector:(SEL)selector;
- (void)scheduleInRunLoop:(NSRunLoop *)runLoop forMode:(NSString *)mode;

@end
