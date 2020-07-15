#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>


#define HOSTNAMESIZE  30
#define MSGMAXSIZE    96    // Tam. maximo mensagem do protocolo = 5(cmd)+10(user)+79(msg)+1(delimitador)
#define CHATMSGSIZE (79+1)  // Tam. maximo de um texto chat

pthread_t thread_timer;
pthread_t thread_rx;
pthread_t thread_tx;

char  terminalname[80];
FILE *terminalrx;
int   sd;                 // socket descriptor de comunicação com o servidor chat
int   ack_pending;        // contador de respostas OKOK de teste não recebidas na sequencia
int   encerrando = 0;       // 1 --> sinalização para encerrar o programa
int   stop_TEST;
char  autoanswer = 1; 


int Send_OKOK()
    {
    int    status;

    status =  send(sd,"OKOK:",5+1, 0);
    if (status < 0)
         {
         perror("Erro na chamada send() na função Send_OKOK");
         return(-1);
         }
    return(0);
    }

int Send_TEST()
    {
    int    status;

    status =  send(sd,"TEST:",5+1, 0);
    if (status < 0)
         {
         perror("Erro na chamada send() na função Send_TEST");
         return(-1);
         }
    return(0);
    }

int Send_USER(char *username)
    {
    int    status;
    char   buffer[16];   // 4 (cmd) + 1 (:) + 10 (msg) + 1 (delimitador) 

    if (strlen(username) == 0)
        {
        printf("Erro: username vazio\n");
        return(-1);
        }
    else if (strlen(username) > 10)
        {
        printf("Erro: tamanho de username maior que o esperado\n");
        return(-1);
        }
    // troca eventual \n por \0
    if (username[strlen(username)-1]=='\n')
         username[strlen(username)-1]='\0';
    sprintf(buffer,"USER:%s",username); 

    status =  send(sd,buffer,strlen(buffer)+1, 0);
    if (status < 0)
         {
         perror("Erro na chamada send() na função Send_USER");
         return(-1);
         }
    return(0);
    }

int Send_UP(char * msg)
    {
    int    status;
    char   buffer[85];   // 4 (cmd) + 1 (:) + 79 (msg) + 1 (delimitador) 

    if (strlen(msg) == 0)
        {
        printf("Erro: mensagem vazia.\n");
        return(-1);
        }
    else if (strlen(msg) > 79)
        {
        printf("Erro: mensagem excede o tamanho permitido.\n");
        return(-1);
        }
    // troca eventual \n por \0
    if (msg[strlen(msg)-1]=='\n')
        msg[strlen(msg)-1]='\0';
    sprintf(buffer,"UP  :%s",msg); 
    status =  send(sd,buffer,strlen(buffer)+1, 0);
    if (status < 0)
         {
         perror("Erro na chamada send() na função Send_UP");
         return(-1);
         }
    return(0);
    }

int Send_EXIT()
    {
    int    status;

    status = send(sd,"EXIT:",5+1, 0);
    if (status < 0)
         {
         perror("Erro na chamada send() na função Send_EXIT");
         return(-1);
         }
    return(0);
    }

void encerrar_chat(void)
    {
    int status;
    char txbuffer[10];

    printf(            "Enviado pedido de desconexão ao servidor (comando EXIT).\n");
    fprintf(terminalrx,"Enviado pedido de desconexão ao servidor (comando EXIT).\n");
    encerrando = 1; 
    Send_EXIT();
    printf(            "Aguardando comando BYE para encerrar. \n\n");
    fprintf(terminalrx,"Aguardando comando BYE para encerrar. \n\n");
    }

void timer()
    {
    int    status;
    char   timerbuffer[10];    // buffer para mensagem "TEST:"

    while (1)
        {
        sleep(30);
        if (encerrando == 1)
            {
            printf(            "Pedido de encerramento (EXIT) sem respost BYE.\n");
            fprintf(terminalrx,"Pedido de encerramento (EXIT) sem respost BYE.\n");
            printf(            "Cliente chat encerrado! \n\n");
            fprintf(terminalrx,"Cliente chat encerrado! \n\n");
            exit(1);
            }
        else if (ack_pending >= 3)
           {
           printf("\nPerda de conexão com o servidor (3 mensagens TEST sem resposta)\n");
           fprintf(terminalrx,"\nPerda de conexão com o servidor (3 mensagens TEST sem resposta)\n");
           encerrar_chat();
           }
        Send_TEST();
        ack_pending++;
        }
    }

void sig_intr_handler(int sig)
    {
    char opcao;
    char buffer[MSGMAXSIZE];

    printf("\n\nChegou sinal #%d,\n",sig);
    if (sig == 2)
          encerrar_chat();
    }

void rx()
    {
    int    status;
    char   rxbuffer[MSGMAXSIZE+10];    // buffer de recepcao
    
    fprintf(terminalrx,"\n\n");
    fprintf(terminalrx,"***********************\n");
    fprintf(terminalrx,"* CLIENTE CHAT        *\n");
    fprintf(terminalrx,"**********************\n");
    fprintf(terminalrx,"Terminal de recepção.\n");
    fprintf(terminalrx,"\n");

    while (1)
        {
        // Aguarda mensagem 
        status =  recv(sd, rxbuffer, MSGMAXSIZE+10, 0);
        if (status < 0)
              {
              perror("ERRO na chamada recv() na função rx()");
              //exit(1); 
              }
         rxbuffer[status+1]='\0';
         //fprintf(terminalrx,">> %s \n",rxbuffer);
         //fflush(terminalrx);

         if (status >= MSGMAXSIZE)
             fprintf(terminalrx,"Erro: recebida mensagem acima do tamanho esperado.\n");

         if (strncmp("TEST:",rxbuffer,5) == 0)
             {
             fprintf(terminalrx,">>> Mensagem TEST\n");
             if (autoanswer == 1)
                Send_OKOK();
             }
         else if (strncmp("OKOK:",rxbuffer,5) == 0)
             {
             fprintf(terminalrx,">>> Mensagem OKOK\n");
             ack_pending = 0;
             }
         else if (strncmp("DOWN:",rxbuffer,5) == 0)
             {
             ack_pending = 0;
             rxbuffer[MSGMAXSIZE-1] = '\0';    // adicionado por segurança
             fprintf(terminalrx,"%s",&rxbuffer[5]);
             }
         else if (strncmp("BYE :",rxbuffer,5) == 0)
             {
             fprintf(terminalrx,">>> Mensagem BYE\n");
             if (encerrando == 1)
                {
                printf("Recebida mensagem BYE- Programa cliente chat encerrado. \n\n");
                fprintf(terminalrx,"Recebida mensagem BYE- Programa cliente chat encerrado. \n\n");
                exit(1);
                }
             else
                {
                fprintf(terminalrx,"Aviso: Comando BYE recebido sem encaminhar EXIT. \n");
                fprintf(terminalrx,"Cliente Chat não será encerrado ...\n");
                }
             }
         else if (strncmp("EXIT:",rxbuffer,5) == 0)
             fprintf(terminalrx,"Chegada de mensagem EXIT não esperada. \n");
         else
             fprintf(terminalrx,"Chegada de mensagem desconhecida: >>>%s<<< \n",rxbuffer);
         }
    }

void tx()
    {
    int    status;
    char   getout = 0;
    char   message[CHATMSGSIZE];    // mensagem para transmitir 

    printf("\n");
    printf("Terminal de escrita de mensagens:\n"); 
    printf("    Comandos especiais:\n");
    printf("      exit    - Terminar sessão do chat. \n");
    printf("      disable - Desabilitar resposta OKOK ao comando TEST (debug).\n"); 
    printf("      enable  - Habilitar resposta OKOK ao comando TEST (debug).\n"); 
    printf("\n"); 

    fgets(message,CHATMSGSIZE,stdin);
    while (getout==0)
        {
        printf("> ");
        fgets(message,CHATMSGSIZE,stdin);

        if (strncmp("exit",message,4) == 0)
            {
            printf("Terminando sessão chat ... \n");
            encerrar_chat(); 
            getout = 1;
            }
        else if (strncmp("disable",message,7) == 0)
            {
            printf("Desabilitando resposta automática...\n");
            autoanswer = 0;
            }
        else if (strncmp("enable",message,6) == 0)
            {
            printf("Habilitando resposta automática...\n");
            autoanswer = 0;
            }
        else
           status = Send_UP(message);
        }
    printf("\n");
    }


int main()
    {
    char   hostnamep[HOSTNAMESIZE]; // nome do servidor
    void * hostaddress;             // ponteiro p/ endereco IP do servido, formato netwok (4 byes) 
    int    serverport;              // porta UDP do servidor
    int    status;                  // resultado da chamada
    struct hostent   * hostentryp;  // estrutura para traducao de nomes
    struct sockaddr_in serversockaddr;  // socket address do servidor
    char   username[11];                // max 10 caracteres + \0
    char   rxbuffer[CHATMSGSIZE];       // buffer de recepcao
    char   opcao;

    printf("Entre com um nome ou um endereço IP: ");
    scanf("%s",hostnamep);

    printf("Porta: ");
    scanf("%d",&serverport);

    // SOCKET
    sd = socket(PF_INET, SOCK_DGRAM, 0); 
    if (sd < 0)
        {
        perror("ERRO na chamada socket()");
        exit(1); 
        }

    //************************************
    //* Converte nome de host ou endereco IP para network byte order 
    //************************************                          
    
    hostentryp = gethostbyname(hostnamep);
    if (hostentryp != NULL)
        {
        // conseguiu converter nome em endereco IP: copia os 4 bytes do endereço IP
        hostaddress = hostentryp->h_addr;
        bcopy(hostaddress, (char *)&serversockaddr.sin_addr, hostentryp->h_length);
        }
    else
        {
        // Nao conseguiu converter nome para endereco IP
        // Verifica se hostname informado é um endereço IP
    
        status = inet_pton(AF_INET,hostnamep,&serversockaddr.sin_addr);
        if (status <=0)
            {
            perror("ERRO: inet_pton()");
            printf("Nome de host ou endereço IP inválido \n");
            return(-1);
            }
        }
        
    //********************************
    // CONNECT
    //********************************

    // define endereco do parceiro 
    serversockaddr.sin_family = AF_INET;
    serversockaddr.sin_port   = htons(serverport);
           
    // ativa connect()
    status = connect(sd, (struct sockaddr *) &serversockaddr, sizeof(serversockaddr));
    if (status != 0)
       {
       perror("ERRO na chamada connect()");
       exit(1); 
       }
   
    inet_ntop(AF_INET,&serversockaddr.sin_addr,hostnamep,HOSTNAMESIZE)  ;
    printf("Cliente UDP ativo e associado ao servidor %s na porta %d.\n\n", 
            hostnamep, ntohs(serversockaddr.sin_port));

    //********************************
    // Abertura do terminal auxiliar
    //********************************
        
    // Informar o terminal:
    printf("Abra um outro terminal e obtenha o nome deste terminal digitando o comando tty. \n");
    printf("Entre com o nome do terminal (ex:/dev/pts/2): ");
    scanf("%s",terminalname);

    terminalrx = fopen(terminalname,"a+");
    if (terminalrx == NULL)
        {
        perror("Abertura do terminal auxiliar");
        exit (1);
        }

    printf("\n\n\n");
    printf("***************************\n");
    printf("* CLIENTE CHAT UDP        *\n");
    printf("***************************\n");
    printf("Terminal de comandos.\n");
    printf("\n");

    while (1)
       {
       printf("Nome do usuário: ");
       scanf("%10s",username);
       Send_USER(username); 
        
       // Aguarda 
       status =  recv(sd, rxbuffer, sizeof(rxbuffer)-1, 0);
       if (status < 0)
           {
           perror("ERRO na chamada recv() na função rx()");
           //exit(1); 
           }
       rxbuffer[status+1]='\0';
       fprintf(terminalrx,">> %s \n",rxbuffer);

       if (strncmp("BUSY:",rxbuffer,5) == 0)
           {
           printf("Recebida mensagem BUSY\n");
           continue;
           }
       else if (strncmp("OKOK:",rxbuffer,5) == 0)
           {
           printf("Recebida mensagem OKOK\n");
           break;
           }
       else 
           {
           fprintf(terminalrx,"Mensagem desconhecida.\n");
           exit(0);
           }
       }

    // Trata sinal INTR (^C)
    signal(2, sig_intr_handler);

    pthread_create(&thread_timer, NULL, (void*) timer, NULL);
    pthread_create(&thread_rx, NULL, (void*) rx, NULL);
    pthread_create(&thread_tx, NULL, (void*) tx, NULL);

    pthread_join(thread_rx,NULL);
    pthread_join(thread_tx,NULL);
    }
