#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

//------------------------------------------------------------------------------------------------------------------

ssize_t writen(int fd, const void *vptr, size_t n)				//function given from the lessons
 {
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;
    ptr = vptr;
	nleft = n;
	while (nleft > 0) {

		if  ((nwritten = write(fd, ptr, nleft)) <= 0 ){ 
			if (errno == EINTR) 
				nwritten = 0;
          	else 
          		return -1; /* error */ 
		} 
		nleft -= nwritten;
        ptr += nwritten;
	} 
	return n;
}
//------------------------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	//Define the struct and the variables
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *serv;
    int i, er;
    char buffer[1024];

//-------------------------------------------------------------------------------------------------------------------
    char str1[15];	 		
    char str2[15]; 
    char val1[15]; 			
    
//--------------------------------------------------------------------------------------------------------------------
    portno = atoi(argv[2]);
  	serv = gethostbyname(argv[1]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 					// Create a socket 
    bzero((char *) &serv_addr, sizeof(serv_addr)); 				//take the serv_addr and put the right protocol
    serv_addr.sin_family = AF_INET; 							// Address family = Internet 
    bcopy((char *)serv->h_addr,(char *)&serv_addr.sin_addr.s_addr,serv->h_length);
    serv_addr.sin_port = htons(portno); 						// Set port number
    serv_addr.sin_addr = *((struct in_addr *)serv->h_addr);
    connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr));

    //input example : ./client diogenis.ceid.upatras.gr 1234 put city patra get city 	
 	
    char prot;
    for( i = 3; i < argc; i++ ) 								//Here we send the right input arguments
    {
		strcpy(str1,"get");
   		strcpy(str2,"put");
    	if ( strcmp(argv[i],"get" ) == 0) 		 				//if argv[i]=get then send g
		{
			prot = 103;
    		writen(sockfd,&prot,1);			    				//send g
					
			if (argv[i+1]==NULL) break;							//if there isn't key, then stop the connection		
			writen(sockfd,argv[i+1],strlen(argv[i+1])+1);		//else, write it to the socket, send it to the server
			recv(sockfd, buffer, 1, 0);							//take the message from the server	
			
			if (buffer[0] == 'n'){								 //if the answer from server is n->110 ascii (didn't found)
				printf("\n");
				i +=1;
				continue;										 //keep searching untill you find it
			} else if (buffer[0] == 'f'){						  //if we found  it, then we recieve f->102 ascii
					while((er = read(sockfd, &buffer, 1))>0 && buffer[0]!='\0') //and we take it from the server
					printf("%c", buffer[0]);
			}
				
			printf("\n");
			i++;
			continue;
    	} 
    	
    	if ( strcmp(argv[i],"put" ) == 0) { 					//if argv[i]=put
    		prot = 112;
    		writen(sockfd,&prot,1);	 							//send p
			
			if (argv[i+1]==NULL) break;							//if we haven't found key or value stop the connection
			if (argv[i+2]==NULL) break;
			
			writen(sockfd,argv[i+1],strlen(argv[i+1])+1);		//else, take both of them
			writen(sockfd,argv[i+2],strlen(argv[i+2])+1);
			i+=2;
			continue;
    	}  
  	}

	shutdown(sockfd, SHUT_WR);
    close(sockfd); 											// close the socket
    return 0;	
}

//-----------------------------------------------------------------------------------------------------------------------------