#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define CFG_BIND_LOCAL_PORT
//#define CFG_INCREMENT_PORT
#define CFG_MAX_PORT 47999
#define CFG_INITIAL_PORT 47809

/* Creates a client that transmits and receives to/from a server using a pair of specific ports */

int main() {
	const char* server_name = "169.254.78.246";	// This is the server address (remote)
	int server_port = 47809;			// This is the server port (remote)
	int local_port = CFG_INITIAL_PORT;				// This is the client port (local)

	struct sockaddr_in server_address;		// Address structure for setting server address/port
	struct sockaddr_in client_address;		// Address structure for setting the local port

	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;

	// creates binary representation of server name
	// and stores it as sin_addr
	// http://beej.us/guide/bgnet/output/html/multipage/inet_ntopman.html
	inet_pton(AF_INET, server_name, &server_address.sin_addr);

	// htons: port in network order format
	server_address.sin_port = htons(server_port);

	// open socket for tx
	int sock;
	if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("could not create socket\n");
		return 1;
	}

#ifdef CFG_BIND_LOCAL_PORT
	// Binds socket for reception
	memset(&client_address, 0, sizeof(client_address));
	client_address.sin_family = AF_INET;
	client_address.sin_addr.s_addr = htonl(INADDR_ANY);
	client_address.sin_port = htons(local_port);
	if (bind(sock, (struct sockaddr *) &client_address, sizeof(client_address)) < 0) {
		perror("binding error");
		return 1;
	}
#endif
	// data that will be sent to the server
	char data_to_send[1024];
	unsigned long i = 0L;
	char buffer[1024];

	for (i=0L;i<10000000;i++){
		sprintf(data_to_send,"Message %ld",i);
	
		// send data
		int len =
		    sendto(sock, data_to_send, strlen(data_to_send), 0,
			   (struct sockaddr*)&server_address, sizeof(server_address));

		// received echoed data back
		len = recvfrom(sock, buffer, sizeof(buffer), 0, NULL, NULL);

		buffer[len] = '\0';
		printf("received: '%s'\n", buffer);
#ifdef CFG_INCREMENT_PORT
		if (local_port < CFG_MAX_PORT){
			local_port++;
		} else {
			local_port = CFG_INITIAL_PORT;
		}
		close(sock);
		// htons: port in network order format
		server_address.sin_port = htons(server_port);

		// open socket for tx
		if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
			printf("could not create socket\n");
			return 1;
		}
#ifdef CFG_BIND_LOCAL_PORT
		// Binds socket for reception
		client_address.sin_port = htons(local_port);
		if (bind(sock, (struct sockaddr *) &client_address, sizeof(client_address)) < 0) {
			perror("binding error");
			return 1;
		}
#endif
#endif
	}
	printf("Finished!");
	// close the socket
	close(sock);
	return 0;
}
