#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//------------------------------------------------------------------------------------------------------------------
struct store
{
  int next;                         //for the next empty cell
  char keyvalue[1000][2][50];       // 2D with two values in a cell
};

struct store netlab;                

//------------------------------------------------------------------------------------------------------------------
ssize_t writen(int fd, const void *vptr, size_t n)                    //function from the lessons-> writen
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

//---------------------------------------------------------------------------------------------------------------------------
char *get(char *key)
{ 
  /** Returns the value associated with the key, or NULL
  * if the key is not in the store. **/
  
  int i=0;        //counter
  int flag=0;     //if not exists the key 
  char p[50];     // value whitch returns if we didn't find the key 
  int pos=0;      //variable to store the position


   for( i = 0; i<=netlab.next ; i++)     //while we have positions to check, we continue 
   {
      if ((strcmp(netlab.keyvalue[i][0], key)) == 0)   //if the comparations return 0 then we have found the key 
      {
           return netlab.keyvalue[i][1];              //return the value of the key, the second value from the cell
      }
   }

   return NULL;   //return nothing
}

//----------------------------------------------------------------------------------------------------------------------------

void put(char *key, char *value)
{
  /* Puts the <key,value> pair into the store. If the key already existed, its previous 
  * value is discarded and replaced by the new one. */

  int x;
  int flag=0;

      for (x=0;x<netlab.next ;x++) // check all the positions
      {   
          if (strcmp(netlab.keyvalue[x][0], key)== 0) //if you find the key update
          {
    
               strcpy(netlab.keyvalue[x][1], value);
               strcpy(netlab.keyvalue[x][0], key);
               flag=1;
               return;
          } 
      }

     if (flag == 0) //if the value doesn't exist, then we store it in the array
     {
          netlab.next++;   //put it in the next empty cell
          strcpy(netlab.keyvalue[netlab.next][1], value);
          strcpy(netlab.keyvalue[netlab.next][0], key);
          return;
    }       
}
//----------------------------------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{

    int sockfd, newsockfd, portno;
    int status,socket_fd, client_fd,num;
    socklen_t size;
    char  n_found = 110, found = 102;
    char ch;
    struct sockaddr_in serv_addr, cli_addr;
    int n, n1;
    int k,i;
    char val[50];
    char *ptr;
    int on; //new
    char * bfr = malloc(20*sizeof(char  )); //new
    char * bfr2 = malloc(20*sizeof(char  )); //new
    char * my_buffer;

    netlab.next=0; 

    //Here i create the socket from server's side
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)); //new
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)); 
     listen(sockfd,5);
     int len;

     while(1) { 
        len= sizeof(struct sockaddr_in);  
        newsockfd = accept(sockfd, (struct sockaddr *)&serv_addr, &len);
        
       while(1) {
          recv(newsockfd, bfr, 1,0);
     
        if (bfr[0] == 'g')      //if the inptu is then
        {
            i=0; 
            while( recv(newsockfd, &ch, 1,0)>0 && (ch!='\0')){ //while we recieve arg from client
                bfr[i++] = ch;                //store them
            } 
            bfr[i] = '\0';                    //store 0 before the arg
        
          bzero(my_buffer, 0); 
          my_buffer = get(bfr);               //store the result from get function
        
          if ( my_buffer == NULL )            //if we took null
            writen(newsockfd, &n_found, 1);   //return n
          else 
          {
            writen(newsockfd, &found, 1);     //else f for found
            writen(newsockfd, my_buffer, strlen(my_buffer)+1); //return the result from get
          }
          
     //---------------------------------------------------------------------------------------------------   
      } else if (bfr[0] == 'p')           //if the inptu is then
      {      
         i=0; 
         while( recv(newsockfd, &ch, 1,0)>0 && (ch!='\0')){   //while we recieve arg from client
            bfr[i++] = ch;        //store them
         }
        bfr[i] = '\0';            //store 0 before the arg
        
        i=0; 
        while( recv(newsockfd, &ch, 1,0)>0 && (ch!='\0')){  //while we recieve arg from client
          bfr2[i++] = ch;         //store them
        }
        bfr2[i] = '\0';           //store 0 before the arg
        put(bfr, bfr2);           //send key,value to put function
        
     } else 
       break;                     //if we recieved no p or g then stop the connection
  }
 }
  
  close(newsockfd);  
  close(sockfd);  
  return 0;
}