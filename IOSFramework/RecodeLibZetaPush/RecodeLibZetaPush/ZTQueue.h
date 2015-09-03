
#import <Foundation/Foundation.h>


@protocol ZTQueueDelegate;

@protocol ZTQueue <NSObject>

- (void)addObject:(id)object;
- (id)removeObject;

@optional
- (void)setDelegate:(id<ZTQueueDelegate>)delegate;

@end

@protocol ZTQueueDelegate <NSObject>

- (void)queueDidAddObject:(id<ZTQueue>)queue;

@end
