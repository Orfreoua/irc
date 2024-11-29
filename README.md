# IRC

A simple IRC (Internet Relay Chat) server application written in C++. This project implements the server-side logic of the IRC protocol, allowing clients to connect, join channels, and send messages in real-time. It's designed for developers or enthusiasts interested in the IRC protocol or creating their own IRC server applications.

## Getting Started

To set up and run the project locally, follow these steps:

1. **Clone the repository**

   `git clone https://github.com/Orfreoua/irc.git`

2. **Build the project**

	`cd irc`

	`make`

3. **Run the project**

Once the project is compiled, you can run the **IRC server**. The server will listen for incoming connections
from IRC clients (such as **Netcat** or a real IRC client) on a specified port, by default, port `6667` with a password,
use the following command:

	`./bin/irc 6667 yourpassword`

_____________________________________________________________________________________________________________________________
## Using Netcat locally

1. **To connect to your server running on localhost, use the following commands**

	`nc localhost 6667`

2.	**Send the password using the PASS command**

	`PASS yourpassword`

3.	**Set the nickname using the NICK command**

	`NICK yournickname`

4.	**Set the username using the USER command (this should be followed by your username, hostname, and real name)**

	`USER yourusername 0 * :Your Real Name`

5. **Join a Channel (or create it if it doesn’t exist)**

	`JOIN #channelname`

6. **Send Messages to the Channel**

	`PRIVMSG #channelname :Your message`

7. **Use Additional Commands**

	`INVITE nickname #mychannel`

	`KICK #mychannel nickname`

	`TOPIC #mychannel :New topic text`

	`MODE #mychannel +i  # Make the channel invite-only`

	`MODE #mychannel -i  # Allow anyone to join the channel`

	`MODE #mychannel +t  # Make topic-only by operators`

	`MODE #mychannel -t  # Allow everyone to change the topic`

	`MODE #mychannel +k password123  # Set a password for the channel`

	`MODE #mychannel -k  # Remove the password requirement`

	`MODE #mychannel +o nickname  # Grant operator status to a user`

	`MODE #mychannel -o nickname  # Revoke operator status`

	`MODE #mychannel +l 50  # Limit the channel to 50 users`

	`MODE #mychannel -l  # Remove the user limit`

_____________________________________________________________________________________________________________________________

## Further Documentation
	https://modern.ircdocs.horse

_____________________________________________________________________________________________________________________________
**Important Note for Some Environments**

	In some environments, such as Netcat, you may need to manually add both a carriage return (\r) and a newline (\n)
	 at the end of each command for the server to correctly interpret it. Some terminals, like Netcat, do not automatically
	  add these characters, so the server might not process the command as expected.

	To do this:

	After typing a command, press Ctrl + V, then Ctrl + M to insert a carriage return (\r).
	Then, press Ctrl + V, then Ctrl + J to insert a newline (\n).
	For example:

	Correct command (with \r\n):

	`NICK yournickname`
	`(press Ctrl + V, Ctrl + M then Ctrl + V, Ctrl + J)`

	Incorrect command (without \r\n):

	`NICK yournickname`
	`(this will not be processed correctly)`

_____________________________________________________________________________________________________________________________

Enjoy and Contribute!
Thank you for checking out this IRC server implementation! I hope it helps you understand how IRC servers work or serves as a 
foundation for building your own custom IRC server. If you have any questions, suggestions, or contributions,
feel free to open an issue or submit a pull request.

Happy coding and have fun chatting on IRC! 😊