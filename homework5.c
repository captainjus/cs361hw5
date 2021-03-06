#include <fnmatch.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <dirent.h>		// from directory_reading_example

#define BACKLOG (10)

void serve_request(int);

char * request_str = "HTTP/1.0 200 OK\r\n"
        "Content-type: text/html; charset=UTF-8\r\n\r\n";


char * index_hdr = "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\"><html>"
        "<title>Directory listing for %s</title>"
"<body>"
"<h2>Directory listing for %s</h2><hr><ul>";

// snprintf(output_buffer,4096,index_hdr,filename,filename);


char * index_body = "<li><a href=\"%s\">%s</a>";

char * index_ftr = "</ul><hr></body></html>";

char* get_directory_contents(char* directory_path)
{
  char* directory_listing = NULL;
  
  // open directory path up 
  DIR* path = opendir(directory_path);

  // check to see if opening up directory was successful

      directory_listing = (char*)malloc(sizeof(char)*1013);
	  char* body_section = NULL;
      directory_listing[0] = '\0';

      // stores underlying info of files and sub_directories of directory_path
      struct dirent* underlying_file = NULL;

	  sprintf(body_section, "<html><head><title>%s</title></head>", directory_path);
	  sprintf(body_section, "<body>");
	  
      // iterate through all of the  underlying files of directory_path
      while((underlying_file = readdir(path)) != NULL)
      {
		  if (strstr(underlying_file->d_name, ".html")){
			  sprintf(body_section, "<a href=\"%s\">%s</a>", underlying_file->d_name, underlying_file->d_name);
		  }
		  else if (strstr(underlying_file->d_name, ".gif")){
			  sprintf(body_section, "<a href=\"%s\">%s</a>", underlying_file->d_name, underlying_file->d_name);
		  }
		  else if (strstr(underlying_file->d_name, ".png")){
			  sprintf(body_section, "<a href=\"%s\">%s</a>", underlying_file->d_name, underlying_file->d_name);
		  }
		  else if (strstr(underlying_file->d_name, ".jpeg")){
			  sprintf(body_section, "<a href=\"%s\">%s</a>", underlying_file->d_name, underlying_file->d_name);
		  }
		  else if (strstr(underlying_file->d_name, ".pdf")){
			  sprintf(body_section, "<a href=\"%s\">%s</a>", underlying_file->d_name, underlying_file->d_name);
		  }
		  else if (strstr(underlying_file->d_name, ".ico")){
			  sprintf(body_section, "<a href=\"%s\">%s</a>", underlying_file->d_name, underlying_file->d_name);
		  }
		  else{
			  sprintf(body_section, "<a href=\"%s\">%s</a>", underlying_file->d_name, underlying_file->d_name);
		  }
		  
      }
	  
	  sprintf(body_section, "</body></html>");
	  
      closedir(path);
	  return body_section;

}

/* char* parseRequest(char* request)
 * Args: HTTP request of the form "GET /path/to/resource HTTP/1.X" 
 *
 * Return: the resource requested "/path/to/resource"
 *         0 if the request is not a valid HTTP request 
 * 
 * Does not modify the given request string. 
 * The returned resource should be free'd by the caller function. 
 */
char* parseRequest(char* request) {
  //assume file paths are no more than 256 bytes + 1 for null. 
  char *buffer = malloc(sizeof(char)*257);
  memset(buffer, 0, 257);
  
  if(fnmatch("GET * HTTP/1.*",  request, 0)) return 0; 

  sscanf(request, "GET %s HTTP/1.", buffer);
  return buffer; 
}

void serve_request(int client_fd){
  int read_fd;
  int bytes_read;
  int file_offset = 0; // Amount of bytes
  int retval;
  char client_buf[4096];
  char send_buf[4096];
  char filename[4096];
  char * requested_file;
  memset(client_buf,0,4096);
  memset(filename,0,4096);
  
  while(1){
    file_offset += recv(client_fd,&client_buf[file_offset],4096,0);
    if(strstr(client_buf,"\r\n\r\n"))
      break;
  }
  
  requested_file = parseRequest(client_buf);
  printf("%s\n", requested_file);
  
  char * request_str = NULL;
		
  printf("Current working directory: %s\n", getcwd(request_str, 600));
  struct stat file_stat;
  if (stat(&requested_file[1], &file_stat) != 0) { // File doesn't exist
	  request_str = "HTTP/1.0 404 Not found\r\n"
		"Content-type: text/html; charset=UTF-8\r\n\r\n";
	  retval = send(client_fd,request_str,strlen(request_str),0);
	  
	  char* send_buf = "<html><head><title>404</title></head>"
			"<body>"
			"<h1>404 File Not Found!</h1>"
			"</body></html>";

	  retval = send(client_fd,send_buf,strlen(send_buf),0);
	   
	  close(client_fd);
	  return;
  }
  else if (S_ISDIR(file_stat.st_mode)){ // before looking for files we check dir
	  //chdir(&requested_file[1]);
	  char* index_check = strcat(&requested_file[1], "/index.html");
	  if(stat(index_check, &file_stat) != 0) { // index.html not found
		  printf("index.html not found. %s\n", index_check);
		  
		  exit(0);
	  }
	  else{ // index.html is found, run and send that
		  printf("index.html found. %s\n", index_check);
		  	  request_str = "HTTP/1.0 200 OK\r\n"
				"Content-type: text/html; charset=UTF-8\r\n\r\n";
	  }
	  
  }
  else if (strstr(requested_file, ".html")){
	  request_str = "HTTP/1.0 200 OK\r\n"
        "Content-type: text/html; charset=UTF-8\r\n\r\n";
  }
  else if (strstr(requested_file, ".gif")){
	  request_str = "HTTP/1.0 200 OK\r\n"
        "Content-type: image/gif; charset=UTF-8\r\n\r\n";
  }
  else if (strstr(requested_file, ".png")){
	  request_str = "HTTP/1.0 200 OK\r\n"
        "Content-type: image/png; charset=UTF-8\r\n\r\n";
  }
  else if (strstr(requested_file, ".jpeg")){
	  request_str = "HTTP/1.0 200 OK\r\n"
        "Content-type: image/jpeg; charset=UTF-8\r\n\r\n";
  }
  else if (strstr(requested_file, ".pdf")){
	  request_str = "HTTP/1.0 200 OK\r\n"
        "Content-type: application/pdf; charset=UTF-8\r\n\r\n";
  }
  else if (strstr(requested_file, ".ico")){
	  request_str = "HTTP/1.0 200 OK\r\n"
        "Content-type: image/x-icon; charset=UTF-8\r\n\r\n";
  }
  else{
	  request_str = "HTTP/1.0 200 OK\r\n"
        "Content-type: text/plain; charset=UTF-8\r\n\r\n";
  }
  
  retval = send(client_fd,request_str,strlen(request_str),0);
  //printf("%d\n", retval);
  // take requested_file, add a . to beginning, open that file
  filename[0] = '.';
  strncpy(&filename[1],requested_file,4095);
  read_fd = open(filename,0,0);
  
  while(1){
    bytes_read = read(read_fd,send_buf,4096);
    if(bytes_read == 0)
      break;

    retval = send(client_fd,send_buf,bytes_read,0);
  }
  
  close(read_fd);
  close(client_fd);
  return;
}

/* Your program should take two arguments:
 * 1) The port number on which to bind and listen for connections, and
 * 2) The directory out of which to serve files.
 */
int main(int argc, char** argv) {
    /* For checking return values. */
    int retval;

    /* Read the port number from the first command line argument. */
    int port = atoi(argv[1]);

    /* Create a socket to which clients will connect. */
    int server_sock = socket(AF_INET6, SOCK_STREAM, 0);
    if(server_sock < 0) {
        perror("Creating socket failed");
        exit(1);
    }

    /* A server socket is bound to a port, which it will listen on for incoming
     * connections.  By default, when a bound socket is closed, the OS waits a
     * couple of minutes before allowing the port to be re-used.  This is
     * inconvenient when you're developing an application, since it means that
     * you have to wait a minute or two after you run to try things again, so
     * we can disable the wait time by setting a socket option called
     * SO_REUSEADDR, which tells the OS that we want to be able to immediately
     * re-bind to that same port. See the socket(7) man page ("man 7 socket")
     * and setsockopt(2) pages for more details about socket options. */
    int reuse_true = 1;
    retval = setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &reuse_true,
                        sizeof(reuse_true));
    if (retval < 0) {
        perror("Setting socket option failed");
        exit(1);
    }

    /* Create an address structure.  This is very similar to what we saw on the
     * client side, only this time, we're not telling the OS where to connect,
     * we're telling it to bind to a particular address and port to receive
     * incoming connections.  Like the client side, we must use htons() to put
     * the port number in network byte order.  When specifying the IP address,
     * we use a special constant, INADDR_ANY, which tells the OS to bind to all
     * of the system's addresses.  If your machine has multiple network
     * interfaces, and you only wanted to accept connections from one of them,
     * you could supply the address of the interface you wanted to use here. */
    
   
    struct sockaddr_in6 addr;   // internet socket address data structure
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(port); // byte order is significant
    addr.sin6_addr = in6addr_any; // listen to all interfaces

    
    /* As its name implies, this system call asks the OS to bind the socket to
     * address and port specified above. */
    retval = bind(server_sock, (struct sockaddr*)&addr, sizeof(addr));
    if(retval < 0) {
        perror("Error binding to port");
        exit(1);
    }

	// BORROWED FROM EXAMPLE // ****** Initial check if Directory exists ************
	//check if file exists
	struct stat file_stat;
	if (stat(argv[2], &file_stat) != 0) {
		printf("%s does not exist!  Program exiting\n", argv[2]);
		exit(-1);
	}


	//check if file is a directory
	if (S_ISDIR(file_stat.st_mode)) {
		printf("%s is a directory\n", argv[2]);
		chdir(argv[2]);
	} else {
		printf("%s is NOT a directory!  Program exiting\n", argv[2]);
		exit(-1);
	}
	// BORROWED FROM EXAMPLE //
	
    /* Now that we've bound to an address and port, we tell the OS that we're
     * ready to start listening for client connections.  This effectively
     * activates the server socket.  BACKLOG (#defined above) tells the OS how
     * much space to reserve for incoming connections that have not yet been
     * accepted. */
    retval = listen(server_sock, BACKLOG);
    if(retval < 0) {
        perror("Error listening for connections");
        exit(1);
    }
	
	struct thread_arg{
		int socket_num;
	};
	
	struct thread_arg *arguments = NULL;
	pthread_t *threads = NULL;
	int num_threads = 0;
	
    while(1) {
        /* Declare a socket for the client connection. */
        int sock;
        char buffer[256];

        /* Another address structure.  This time, the system will automatically
         * fill it in, when we accept a connection, to tell us where the
         * connection came from. */
        struct sockaddr_in remote_addr;
        unsigned int socklen = sizeof(remote_addr); 

        /* Accept the first waiting connection from the server socket and
         * populate the address information.  The result (sock) is a socket
         * descriptor for the conversation with the newly connected client.  If
         * there are no pending connections in the back log, this function will
         * block indefinitely while waiting for a client connection to be made.
         * */
        sock = accept(server_sock, (struct sockaddr*) &remote_addr, &socklen);
        if(sock < 0) {
            perror("Error accepting connection");
            exit(1);
        }

		/* Allocate some memory for the thread structures that the pthreads library
		* uses to store thread state information. */
		threads = realloc(threads, (num_threads+1) * sizeof(pthread_t));
		if (threads == NULL) {
			printf("malloc() failed\n");
			exit(1);
		}
		
		/* Allocate some memory for the arguments that we're going to pass to our
		* threads when we create them. */
		arguments = realloc(arguments, (num_threads+1) * sizeof(struct thread_arg));
		if (threads == NULL) {
			printf("malloc() failed\n");
			exit(1);
		}
		
		arguments[num_threads].socket_num = sock;
	
		
        /* At this point, you have a connected socket (named sock) that you can
         * use to send() and recv(). */

		 
		// WRAPPER FUNCTION FOR SERVE_REQUEST AND CLOSE //
		void *serve_close_wrapper(void *argument){
			struct thread_arg *my_arg = (struct thread_arg *) argument;
			serve_request(my_arg->socket_num);
			close(my_arg->socket_num);
			return NULL;
		}
		
		
		int retval = pthread_create(&threads[num_threads], NULL,
                                    serve_close_wrapper, &arguments[num_threads]);
        if (retval) {
            printf("pthread_create() failed\n");
            exit(1);
        }
		
		num_threads = num_threads + 1;
        /* ALWAYS check the return value of send().  Also, don't hardcode
         * values.  This is just an example.  Do as I say, not as I do, etc. */
        //serve_request(sock);

        /* Tell the OS to clean up the resources associated with that client
         * connection, now that we're done with it. */
        //close(sock);
		
		
    }
	
    close(server_sock);
}
