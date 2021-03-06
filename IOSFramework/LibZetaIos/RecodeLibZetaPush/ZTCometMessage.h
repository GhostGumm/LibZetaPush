
#import <Foundation/Foundation.h>

@class ZTGlobalCallbacks;

@interface ZTCometMessage : NSObject

- (id)initWithCallback:(ZTGlobalCallbacks *)cb;

@property (nonatomic, retain)ZTGlobalCallbacks *cback;
@property (nonatomic, retain) NSString *channel;
@property (nonatomic, retain) NSString *version;
@property (nonatomic, retain) NSString *token;
@property (nonatomic, retain) NSString *minimumVersion;
@property (nonatomic, retain) NSString *userID;
@property (nonatomic, retain) NSString *action;
@property (nonatomic, retain) NSString *password;
@property (nonatomic, retain) NSString *login;
@property (nonatomic, retain) NSString *type;
@property (nonatomic, retain) NSString *resource;
@property (nonatomic, retain) NSArray *supportedConnectionTypes;
@property (nonatomic, retain) NSString *clientID;
@property (nonatomic, retain) NSDictionary *advice;
@property (nonatomic, retain) NSString *connectionType;
@property (nonatomic, retain) NSString *ID;
@property (nonatomic, retain) NSDate *timestamp;
@property (nonatomic, retain) id data;
@property (nonatomic, retain) NSNumber *successful;
@property (nonatomic, retain) NSString *subscription;
@property (nonatomic, retain) NSError *error;
@property (nonatomic, retain) NSDictionary *JsonBruteData;
@property (nonatomic, retain) id ext;
@property (nonatomic, retain) NSString *reco;


+ (ZTCometMessage *)messageWithChannel:(NSString *)channel;

@end

@interface ZTCometMessage (JSON)

+ (ZTCometMessage *)messageWithJson:(NSDictionary *)jsonData;
- (NSDictionary *)proxyForJson;

@end
