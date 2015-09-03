//
//  ZetaInitConnexion+ZetaInitConnexion.h
//  RecodeLibZetaPush
//
//  Created by Hakim on 06/08/15.
//  Copyright (c) 2015 Hakim. All rights reserved.
//


#import <Foundation/Foundation.h>

@interface ZetaInitConnexion: NSObject{

@private NSString *businessId;
@private NSString *deployementId;
@private NSString *authType;
@private NSString *authVersion;
    
}

-(NSString *) Zeta_Handshake_Fields:(ZetaInitConnexion *)toto;

@end

@interface ZetaCometdArray{

    NSArray *array;
    size_t  *capacity;
}

-(void)Cometd_Array_Add:(void *)element Array:(NSArray *)Array;


@end

