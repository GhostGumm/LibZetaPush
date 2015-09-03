//
//  ViewController.m
//  TestAllRecode
//
//  Created by Hakim on 06/08/15.
//  Copyright (c) 2015 Hakim. All rights reserved.
//

#import "ViewController.h"
#import "ZTCometClient.h"
#import "ZTCometMessage.h"
#import "ZTCometLongPollingTransport.h"
#import "ZTCometMessage.h"
#import "ZTCometSubscription.h"
#import "ZTConcurrentQueue.h"
#import "ZTQueueProcessor.h"
#import <ZTGlobalCallbacks+ZTGlobalCallbacks.h>

//launching threads
//You can ask for an early service if you want it delivered as soon as the connect answer comes from server
//You can now ask for you services you will be notified with the proper answer in the callback you have setted up.
//Don't mess with service parameters, read the documentation and proceed accordingly. Errors are not corrected  thus your request will be sent with a nil parameter wich can make your code crash because of miss handling in your callback setup.


#define SELEC @selector(callbackme:)

@interface ViewController ()

@end


ZTCometClient *client;

@implementation ViewController

@synthesize textView = m_textView,
textField = m_textField,
AnotherClassText = V_ACT;

- (void)dealloc
{
    [m_client release];
    [_Button release];
    [_Text release];
    [V_ACT release];
    [super dealloc];
}


- (void)viewDidLoad
{
    [super viewDidLoad];
    [self.Button setTitle:@"Click to Test Threading" forState:UIControlStateNormal];
    if (m_client == nil)
    {
#pragma GCC diagnostic ignored "-Wundeclared-selector"
        
        m_client = [[ZTCometClient alloc] initWithAllInfo:[NSURL URLWithString:@"http://m.zpush.ovh:8080/str/strd"] BusinessId:@"GmY-HuzW" DeployementId:@"KZyH" Login:@"test2" PassWord:@"password"];
        
        [m_client handshake];
        
        
        [m_client scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    }
}

- (void)viewWillAppear:(BOOL)animated
{
    [m_textField becomeFirstResponder];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    NSLog(@"toto");
    return interfaceOrientation == UIInterfaceOrientationPortrait;
}

- (IBAction)ButtonHandler:(id)sender {
    
    [m_client invokeS3Ls:@"/" owner:nil page:nil SDepID:@"2vC3" callBackSelector:@selector(DoingShit:) noErase:FALSE];
    self.Text.text = @"It's working well, watch the console";
}


#pragma mark -

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
  /*  NSDictionary *data = [NSDictionary dictionaryWithObjectsAndKeys:m_textField.text, @"chat", @"iPhone user", @"user", nil];
    [m_client publishData:data toChannel:@"/service/GmY-HuzW/cKXj/list"];
    
    m_textField.text = @"";*/
    return YES;
}

- (void)appendText:(NSString *)text
{
    m_textView.text = [m_textView.text stringByAppendingFormat:@"%@\n", text];
}

#pragma mark -

- (void)cometClientHandshakeDidSucceed:(ZTCometClient *)client
{
  /*  NSLog(@"Handshake succeeded");
    
    [self appendText:@"[connected]"];
    
    [client subscribeToChannel:@"/service/GmY-HuzW/cKXj/list" target:self selector:@selector(chatMessageReceived:)];
    [m_client scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];//[client subscribeToChannel:@"/service/GmY-HuzW/2v_u/ping" target:self selector:@selector(membershipMessageReceived:)];
    
    //NSDictionary *data = [NSDictionary dictionaryWithObjectsAndKeys:@"/chat/demo", @"room", @"iPhone user", @"user", nil];
    //[m_client publishData:data toChannel:@"/service/GmY-HuzW/cKXj/list"];*/
}

- (void)cometClient:(ZTCometClient *)client handshakeDidFailWithError:(NSError *)error
{
    NSLog(@"Handshake failed");
}

- (void)cometClientConnectDidSucceed:(ZTCometClient *)client
{
    NSLog(@"Connect succeeded");
}

- (void)cometClient:(ZTCometClient *)client connectDidFailWithError:(NSError *)error
{
    NSLog(@"Connect failed");
}

- (void)cometClient:(ZTCometClient *)client subscriptionDidSucceed:(ZTCometSubscription *)subscription
{
    NSLog(@"Subsription succeeded");
}

- (void)cometClient:(ZTCometClient *)client subscription:(ZTCometSubscription *)subscription didFailWithError:(NSError *)error
{
    NSLog(@"Subsription failed");
}

- (void)chatMessageReceived:(ZTCometMessage *)message
{
   /* NSLog(@"Message-received");
    if (message.successful == nil)
        [self appendText:[NSString stringWithFormat:@"%@: %@", [message.data objectForKey:@"user"], [message.data objectForKey:@"chat"]]];
    else if (![message.successful boolValue])
        [self appendText:@"Unable to send message"];*/
}

- (void)membershipMessageReceived:(ZTCometMessage *)message
{
    /*if ([message.data isKindOfClass:[NSDictionary class]])
        [self appendText:[NSString stringWithFormat:@"[%@ are in the chat]", [message.data objectForKey:@"user"]]];
    if ([message.data isKindOfClass:[NSArray class]])
        [self appendText:[NSString stringWithFormat:@"[%@ are in the chat]", [message.data componentsJoinedByString:@", "]]];*/
}

@end
