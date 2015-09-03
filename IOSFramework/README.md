ZetaIos is a IOS CometD client Library that aims at making possible for ios users to easly communicate with ZetaPush CometD Servers.

The library have been coded in Objective-c under the 6.3.1 Xcode Version.
The Project is linked with the "SystemConfiguration".Framework as well as the "Foundation".Framework. The few other external libraries are already implemented in the project in order to keep linking easy for any user who doesn't want to spend extra time fetching and scratching his head on versioning and linking.

INSTALLATION :

In order to install the library on your brand new Ios project, we give you two options :

- 1 : You can drag and drop the whole project into yours wich will give you access to all the ressources avaible in the library. A demo is also avaible and ready to run after the drag and drop. All you have to do is click on the Run button.

- 2: You can Drag and Drop the static library into your project and import the ZTCometClient class like so :

#import "ZTCometClient.h"

This will give you the access on the main Class wich connects all the other ones.

Using The Library

Using the ZetaIos Library is pretty straight.
It sums up to 3 steps :

----------------- FIRST STEP ----------------- :

In order to start the Client you have to initialize it with some basic informations about your account to let it procceed a complete and successfull handshake. To do so you only got to declare this method from the ZTCometClient class :

 m_client = [[ZTCometClient alloc] initWithAllInfo:[NSURL URLWithString:@"http://m.zpush.ovh:8080/str/strd"] BusinessId:@"GmY-HuzW" DeployementId:@"KZyH" Login:@"test2" PassWord:@"password"];

************* m_client  ****************

m_client is class variable wich i declared into my ViewController.h like so

@interface ViewController : UIViewController
{
@private
    ZTCometClient *m_client;
}

To put it private or public is up to you.
****************************************

Lets cut this method into little pieces to reach a better understanding of it.

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

----------------- SECOND STEP ----------------- :

Now that you have provided enought informations to the client in order for it to proceed to handshake lets see how to start it up.

For this step all you have to do is declaring this method from the ZetaCometClient class. :

[m_client handshake];

That's it, the client will query the server, configure it self automaticaly as well as starting a Thread to send messages from the client.

But if you send messages you will probably want to receive an answer, so lets start the Second Thread and get it over with configuration.


----------------- THIRD STEP ----------------- :

