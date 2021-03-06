//
//  ViewController.h
//  TestAllRecode
//
//  Created by Hakim on 06/08/15.
//  Copyright (c) 2015 Hakim. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "ZTCometClient.h"

@interface ViewController : UIViewController
{
@private
    ZTCometClient *m_client;
    UITextView *m_textView;
    UITextField *m_textField;
}

@property (retain, nonatomic) IBOutlet UIButton *Button;
@property (retain, nonatomic) IBOutlet UITextField *Text;
@property (retain, nonatomic) IBOutlet UITextField *AnotherClassText;

@property (nonatomic, assign) IBOutlet UITextView *textView;
@property (nonatomic, assign) IBOutlet UITextField *textField;

@end



