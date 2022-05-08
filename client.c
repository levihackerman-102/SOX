#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define TRUE 1
#define FALSE 0

struct SMTP_AUTH_CREDS {
  char email[50];
  char password[50];
};

struct USER {
  char name[50];
  char email[50];
  char password[50];
};

struct STATUS_MSG {
  int status_code;
  char status_msg[50];
};

struct MAIL {
  char from[50];
  char to[50];
  char subject[50];
  char body[100];
};

struct INBOX {
  int count;
  struct MAIL mails[100];
};

void options(int *option);
void get_creds(struct SMTP_AUTH_CREDS *);
void send_creds_to_server(int sockfd, struct SMTP_AUTH_CREDS *);
void get_user_info(struct USER *);
void send_user_info(int sockfd, struct USER *);
int  status(int sockfd);
void show_main_menu(int *option);
void compose_mail(int sockfd, char *from);
void show_inbox(int sockfd);

int main(int argc, char *argv[])
{
  struct  sockaddr_in server_addr;
  struct  SMTP_AUTH_CREDS auth_creds;
  struct  USER user;
  int     sockfd, port, option, is_logged_in, is_socket_connected = FALSE;

  if(argc != 3) 
  {
    fprintf(stderr, "Invalid arguments\n");
    fprintf(stderr, "./server -p <port number>\n");
    return -1;
  }

  port = atoi(argv[2]); // assign port number

  // create a socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) 
  {
    perror("socket");
    return -1;
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);

  if ( connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1 )
  {
    perror("connect");
    return -1;
  }
  else
  {
    is_socket_connected = TRUE;
  }
  
  
  while(is_socket_connected) 
  {
    options(&option);
    send(sockfd, &option, sizeof(option), 0);
    switch (option)
    {
      case 1:
        get_creds(&auth_creds);
        send_cred_to_server(sockfd, &auth_creds);
        is_logged_in = status(sockfd);
        break;
    
      case 2:
        get_user_info(&user);
        send_user_info(sockfd, &user);
        is_logged_in = status(sockfd);
        strcpy(auth_creds.email, user.email);
        strcpy(auth_creds.password, user.password);
        break;

      default:
        printf("Please choose an option to proceed\n");
    }

    while (is_logged_in == TRUE)
    {
      show_main_menu(&option);

      // send option to server
      send(sockfd, &option, sizeof(option), 0);

      switch (option)
      {
        case 1:
          show_inbox(sockfd);
          break;
      
        case 2:
          compose_mail(sockfd, auth_creds.email);
          break;

        case 3:
          is_logged_in = 0;
          is_socket_connected = FALSE;
          break;

        default:
          printf("Please choose an option to proceed!\n");
      }
    } 
  }

  close(sockfd);    
  return 1;
}

void login_or_create(int *option)
{
  printf("\n\n_______________________START MENU_____________________\n\n");
  printf("1. LOGIN \n");
  printf("2. CREATE ACCOUNT \n");
  printf("Choose an option => ");
  scanf("%d", option);
}

void get_creds(struct SMTP_AUTH_CREDS *auth_creds)
{
  printf("EMAIL => ");
  scanf("%s", auth_creds->email);
  printf("PASSWORD => ");
  scanf("%s", auth_creds->password);
}

void send_creds_to_server(int sockfd, struct SMTP_AUTH_CREDS *auth_creds)
{
  send(sockfd, auth_creds, sizeof(*auth_creds), 0);
}

void get_user_info(struct USER *user)
{
  printf("ENTER USER NAME => ");
  scanf("%s", user->name);
  printf("ENTER EMAIL ADDRESS => ");
  scanf("%s", user->email);
  printf("ENTER PASSWORD => ");
  scanf("%s", user->password);
}

void send_user_info(int sockfd, struct USER *user)
{
  send(sockfd, user, sizeof(*user), 0);
}

int status(int sockfd)
{
  struct STATUS_MSG status;
  recv(sockfd, &status, sizeof(status), 0);

  printf("%s\n", status.status_msg);

  if(status.status_code == 500 || status.status_code == 403) {
     return FALSE;
  }  
   
  return TRUE;
}

void show_main_menu(int *option)
{
  printf("\n\n____________________MAIN MENU_________________\n\n");
  printf("1. INBOX\n");
  printf("2. COMPOSE\n");
  printf("3. LOGOUT\n");
  printf("CHOOSE OPTION => ");
  scanf("%d", option);
}

void compose_mail(int sockfd, char *from)
{
  struct MAIL mail;
  strcpy(mail.from, from);

  printf("\n\n____________________COMPOSE MAIL_________________\n\n");
  printf("FROM : %s\n", from);
  printf("TO => ");
  scanf("%s", mail.to);

  int c;
  while ((c = getchar()) != '\n' && c != EOF);
  
  printf("SUBJECT => ");
  fgets(mail.subject, sizeof(mail.subject), stdin);

  printf("BODY => ");
  fgets(mail.body, sizeof(mail.body), stdin);

  // send mail to server
  send(sockfd, &mail, sizeof(mail), 0);

  // waiting for server to respond with a status
  status(sockfd);

}


void show_inbox(int sockfd)
{
  struct INBOX inbox;
  int    i = 0;

  recv(sockfd, &inbox, sizeof(struct INBOX), 0);

  for(i = 0; i < inbox.count; i++) {
    printf("\n___________________________________________________________\n\
n");
    printf("From : %s\n", inbox.mails[i].from);
    printf("To : %s\n", inbox.mails[i].to);
    printf("Subject : %s\n", inbox.mails[i].subject);
    printf("%s\n", inbox.mails[i].body);
    printf("\n___________________________________________________________\n");
  }

}
