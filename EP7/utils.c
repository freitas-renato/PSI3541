/**
 *  @file: utils.h
 *  @brief: Biblioteca de funções auxiliares
 * 
 *  @author: Isabella Bologna - 9267161
 *  @author: Renato Freitas   - 9837708
*/


#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#define min(x,y)   (((x)<(y))?(x):(y))

#define  PATHSIZE 200
#define BUFFERSIZE 15000

// getcurrentdir()
//      Retorna o diretorio corrente atual
//      Entrada: buffer para string e tamanho deste buffer
//      Saida: ponteiro para string

char* getcurrentdir(char *path, int pathsize)  {
    char *p;

    p = getcwd(path,pathsize);

    if ( (p = getcwd(path,pathsize)) == NULL) {
      perror("Erro na chamada getcwd");
    }

    return p;
}


void append(char *dest, int buffersize, char *src) {
    int d;
    int i;

    d = strlen(dest);
    for (i = 0; i < min(strlen(src),buffersize-1-d); i++) {
       dest[d+i] = src[i];
    }
    dest[d+i] = '\0';
}

// Lista diretorio
//    path: diretorio a ser listado
//    buffer: buffer que contera' a string com a sequencia ASCII
//            resultado da listagem do diretorio (finalizada por '\0'
//    bufffersize: tamanho do buffer

void lista_diretorio(char *path, char *buffer, int buffersize) {
    DIR           * dirp;
    struct dirent * direntry;
    char            linha[80];
    int             i = 0;

    dirp = opendir(path);
    if (dirp ==NULL) {
       perror("ERRO: chamada opendir(): Erro na abertura do diretorio: ");
       snprintf(buffer,buffersize,"Erro na listagem diretorio!\n");
       return;
    }
    
    buffer[0]='\0';
    while (dirp) {
        // Enquanto nao chegar ao fim do diretorio, leia cada entrada
        direntry = readdir(dirp);
        if (direntry == NULL) {
           // chegou ao fim
           break;
    
        } else {
           // ler entrada
           append(buffer,buffersize,direntry->d_name);
           append(buffer,buffersize,"\n");
        }
    }
    
    closedir(dirp);
}

int transferfile(char *path, int output_fd) {
    int          input_fd;     // input file descriptor
    int          status;
    int          n;
    char         buffer[BUFFERSIZE];
    char         str[10];
    struct stat  statp;
   
    input_fd = open(path,O_RDONLY, 0666);
    if (input_fd < 0) {
         perror("ERRO chamada open(): Erro na abertura do arquivo: ");
        return(-1);
    }

    // obtem tamanho do arquivo
    status = fstat(input_fd,&statp);
    if (status != 0) {
        perror("ERRO chamada stat(): Erro no acesso ao arquivo: ");
        status = close(input_fd);
        return(-1);
    }
    // sprintf(str,"SIZE=%d\n",statp.st_size);
    // write(output_fd,str,strlen(str));

    // le arquivo , por partes 
    do {
        n = read(input_fd,buffer,BUFFERSIZE);
        if (n < 0) {
            perror("ERRO: chamada read(): Erro na leitura do arquivo: ");
            status = close(input_fd);
            return(-1);
        }
        write(output_fd,buffer,n);
        // printf("%s", buffer);
    } while(n > 0);
    // write(output_fd, "\n", 1);

    status = close(input_fd);
   
    if (status == -1)  {
            perror("ERRO: chamada close(): Erro no fechamento do arquivo: " );
            return(-1);
        }

    bzero(buffer, BUFFERSIZE);

    return(0);
}

// Compose Path: altera PATH 
//    oldpath: path original (ex: /home/jose/test
//    path   : path para ser alterado (ex: "..", "/tmp", "prog1" )
//             (imagine na forma: "cd ..", "cd /tmp", "cd prog1")
//    newpath: path resultante da composicao de oldpath e path
//             (ex: /home/jose, /tmp, /home/jose/test/prog1)

int composepath(char *oldpath,char *path,char *newpath) {
    char * olddir;
    char * oldbase;
    char * oldpath1;
    char * oldpath2;
    char * dir;
    char * base;
    char * path1;
    char * path2;
    int    status;

    oldpath1 = strdup(oldpath);
    oldpath2 = strdup(oldpath);
    olddir   = dirname(oldpath1);
    oldbase  = basename(oldpath2);

    path1 = strdup(path);
    path2 = strdup(path);
    dir   = dirname (path1);
    base  = basename(path2);

    strcpy(newpath,"/");

    if (path[0]=='/') {
        strcpy(newpath,path);
        status=0;
    }
    else if ((strcmp(dir,".")==0) && (strcmp(base,".")==0)) {
        strcpy(newpath,oldpath);
        status=0;
    }
    else if ((strcmp(dir,".")==0) && (strcmp(base,"..")==0)) {
        strcpy(newpath,olddir);
        status=0;
    }
    else if ((strcmp(dir,".")==0) && (strcmp(oldbase,"/")==0)) {
        strcpy(newpath,oldpath);
        strcat(newpath,path);
        status=0;
    }
    else if (strcmp(dir,".")==0) {
        strcpy(newpath,oldpath);
        strcat(newpath,"/");
        strcat(newpath,path);
        status=0;
    }
    else {
        strcpy(newpath,oldpath);
        status=-1;
    }

    free(oldpath1);
    free(oldpath2);
    free(path1);
    free(path2);

    return(status);
} 