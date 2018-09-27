# Encrypted Server-to-Client and Client-to-Client Communication
CSCE 3600 Principles of Systems Programming - Major 2
Group 21: Eunice Salinas, Dheepthi Somangili Babulal, Megdelawit Woldeselassi

Summary: Design and implement socket programming to establish server-to-client and client-to-client encrypted TCP connections using the RSA algorithm. 

Organization: The problem was divided into three main section. These were server code, client to server communication, and client to client communication. Each team member was assigned a section and a due date. Google drive was used to share any files and code. Several version of the code were provided for other team members to review and implement.

Programmer and Responsabilities
  1.Eunice Salinas: Server code and encrypted communication on server side, client to client communication.
  2.Dheepthi Somangili Babulal: Client to server communication and encryption on client side.
  3.Megdelawit Woldesselassi: Testing and re-design
  
Programm Overview
  Server:
          1. Server will receive 2 integer values from client (p and q) and checks their validity(i.e., if they are prime and their                  product is greater than 128)
          2. If invalid sends “invalid” message back to client requesting for a different pair of numbers.	
          3. Preforms RSA algorithm on the obtained numbers and will generate private and public keys for the given pair of numbers.
	        4. The server will then send the private key for encryption purpose to one of the 
             client (i.e.’ the client that initially provided p and q values) and will send the public 
             key to other client for decryption.
          5. Then the server will get another pair of p and q value for generating encryption and decryption keys using RSA algorithm                to send a reply message to other client.     
          6. If a client send 0 as input value for p or q, then the client will disconnect form the server and the server will wait for              another client to be connected. 
          7. If two clients are already connected with the server and another client requests for 
             connection, then the server will deny the connection for the incoming client. 
  Client: 
          1. Client connects with server.
          2. Client requests for two integer values p and q from the user and send that value to the server.
          3. If the client receives “invalid”  as a reply from the server then the client must enter another  
             pair of integers for p and q.
          4. The client that send the p and  q value will receive the private key from the server i.e., it will     
             receive ‘e’ and ‘n’. The other client that did not send  and q will receive the public key used for 
             decryption. 
          5. The client then establishes a connection with another client.
          6. The client then requests for a text message form the user and uses the private key to encrypt   
             the message one character at a time.
          7. After encrypting each character the client sends the encrypted cipher text of the 
             character to the other client.
          8. The client that received the public key i.e., ‘d’ and ‘n’ receives the cipher text and decrypts it     
             using the public key and prints the decrypted text message.
          9. The second client requests for a pair of integer from the user to generate private and public   
             key and then request for a reply message form the user and use the newly generated private    
             key to encrypt that message one character at a time and send the encrypted cipher. The 
             other client would use the newly generated public key to decrypt the received cipher.

Bugs: The program must terminate when the user enter the p or q value as 0, but the program terminates only when both p and q are 0 else the program hangs.

   
