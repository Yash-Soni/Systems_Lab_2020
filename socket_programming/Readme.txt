==================================================================================================================
		A client server communication system based on TCP and UDP 
=================================================================================================================

INPUT FORMAT OF THE APPLICATION:
	1. input given to the both file from command line arguments.

	INPUT FORMAT FOR client.cpp FILE:
		1. Two argument pass from command line argument
			
			sample input:
				==><executable code> <Server IP Address> <Server Port number>
				==> ./client 172.0.0.1 45556
	
	INPUT FORMAT FOR server.cpp FILE:
		1. one argument pass from command line argument
			
			sample input:
				==><executable code> <Server Port number>
				==> ./serve 45556
	
----------------------------------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------------------------------
OUTPUT FORMAT OF THE APPLICATION:

	OUTPUT FORMAT FOR client.cpp FILE:
		-->output format is easily understood 
		sample output format:
			[*] IP Address    PORT No.    Protocol     Message  
			-----------------------------------------------------------------
			[+] 127.0.0.1 -----46565   :    TCP	   Socket created
			[+] 127.0.0.1 -----46565   :    TCP	   Connection Esatablished
			[+] 127.0.0.1 -----46565   :    TCP	   Type 1 Message send(UDP Port no. req).
			[+] 127.0.0.1 -----46565   :    TCP	   Type 2 Message Received 
			Message :- 2 10 42563
			[+] 127.0.0.1 -----46565   :    TCP	   Connection Closed
			[+] 127.0.0.1 -----42563   :    UDP	   Socket created
			[+] 127.0.0.1 -----42563   :    UDP	   Type 3 Message send.
			[+] 127.0.0.1 -----42563   :    UDP	   Type 4 Message Received.
			Message :- 4 10 UDP message received successfully

			[+] Action : Do You want to continue to send data type y/n : 


	OUTPUT FORMAT FOR server.cpp FILE:
		-->output format is easily understood 
		sample output format:
			[*] IP Address    PORT No.    Protocol     Message  
			-----------------------------------------------------------------
			[+] 127.0.0.1 -----46565   :    TCP	   Socket created.
			[+] 127.0.0.1 -----46565   :    TCP	   binding completed.
			[+] 127.0.0.1 -----46565   :    TCP	   Listening....
			[+] 127.0.0.1 -----46565   :    TCP	   Connected to new client.
			[+] 127.0.0.1 -----46565   :    TCP	   Message Received 
			Message :- 1 13 req udp
			[+] 127.0.0.1 -----46565   :    TCP	   Type 2 Message Send To Client.
			[+] 127.0.0.1 -----46565   :    TCP	   Connection Closed.
			[+] 127.0.0.1 -----42563   :    UDP	   Socket Created.
			[+] 127.0.0.1 -----42563   :    UDP	   Binding Completed.
			[+] 127.0.0.1 -----42563   :    UDP	   Type 3 Message Received.
			Message :- 3 26 message on udp connection
			[+] 127.0.0.1 -----42563   :    UDP	   Type 4 Message Send To Client.
 
	

----------------------------------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------------------------------

EXECUTION STEPS OF THE APPLICATION:
	1. compile both file with following command
		--> g++ client.cpp -o client
		--> g++ server.cpp -o server
	2. Run the code by following commands
		--> ./client 172.0.0.1 45556
		--> ./server 45556


******************************************************************EOF******************************************************************************
