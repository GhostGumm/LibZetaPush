ZetaIos is a IOS CometD client Library that aims at making possible for ios users to easly communicate with ZetaPush CometD Servers.

The library have been coded in Objective-c under the 6.3.1 Xcode Version.
The Project is linked with the "SystemConfiguration".Framework as well as the "Foundation".Framework. The few other external libraries are already implemented in the project in order to keep linking easy for any user who doesn't want to spend extra time fetching and scratching his head on versioning and linking.

INSTALLATION :

In order to install the library on your brand new Ios project, we give you two options :

- 1 : You can drag and drop the whole project into yours wich will give you access to all the ressources avaible in the library. A demo is also avaible and ready to run after the drag and drop. All you have to do is click on the Run button.

- 2: You can Drag and Drop the static library into your project and import the ZTCometClient class like so :

'  #import "ZTCometClient.h"  '

This will give you the access on the main Class wich connects all the other ones.

Using The Library

Using the ZetaIos Library is pretty straight.
It sums up to 3 steps :

#----------------- FIRST STEP ----------------- :

In order to start the Client you have to initialize it with some basic informations about your account to let it procceed a complete and successfull handshake. To do so you only got to declare this method from the ZTCometClient class :

 m_client = [[ZTCometClient alloc] initWithAllInfo:[NSURL URLWithString:@"http://m.zpush.ovh:8080/str/strd"] BusinessId:@"GmY-HuzW" DeployementId:@"KZyH" Login:@"test2" PassWord:@"password"];


****************************************
************* m_client  ****************

m_client is class variable wich i declared into my ViewController.h like so

@interface ViewController : UIViewController
{
@private
    ZTCometClient *m_client;
}

To put it private or public is up to you.
****************************************



But lets cut this method into little pieces to reach a better understanding of it.

- First Parameter : [NSURL URLWithString:@"******"]

The first parameter is the Target Server. Basically the Target Server is the one you want to query with the services requests.
the [NSURL URLWithString:@"******"] method only format the string to make it understandable for the client. It is advised to allways pass the Target Server adress like so.


- Second Parameter : BusinessId:@"GmY-HuzW"

The second parameter is the Business Id. This one is provided by ZetaPush at your subscription. Put it there as a string in order for us to process it.


- Third Parameter : DeployementId:@"KZyH"

The Third parameter is the Deployment Id. This one is also provided by ZetaPush at your subscription to a defined service. Put it there as a string.

- The Fourth and Fifth Parameters :

These ones are your login and password. Put them there as strings as well.

Pretty easy isn't it :). Okay now let's start the Client.

#----------------- SECOND STEP ----------------- :

Now that you have provided enought informations to the client in order for it to proceed to handshake lets see how to start it up.

For this step all you have to do is declaring this method from the ZetaCometClient class. :

[m_client handshake];

That's it, the client is now able to query the server, configure itself automaticaly, as well as starting the Outgoing Messages Thread and Queue to send messages from the client.

But if you can send messages you will probably want to receive an answer, so lets start the Second Thread and get it over with configuration.


#----------------- THIRD STEP ----------------- :

The Third Step is as easy as the second one.

To trigger the Incoming Messages Thread and Queue safely, all you have to do is declaring the following method from the ZetaCometClient class :

[m_client scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];

The RunLoop will start a silent Thread that will keep fetching for new messages coming from the server and hand them to you as soon as possible.


We are now done for the configuration part. You can find an example given in the ViewController.m file under the viewDidLoad method. The file can be found in the TestAllRecode Project File.

Now that we are all set up, lets see how to call a service and use it.

************************
******* WARNING *********

Do Not Call Any Service in the configuration steps this is a load phase no query should be triggerd ! (they will be stop if you do so, and server will kick you out for a good moment)
EarlyBird messages are not supported anymore by the library, as they only lead to memory leaks and asynchonous problems.

***********************

_______________________________________________

