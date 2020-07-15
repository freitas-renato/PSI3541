/**
 *  @file: utils.h
 *  @brief: Biblioteca de funções auxiliares
 * 
 *  @author: Isabella Bologna - 9267161
 *  @author: Renato Freitas   - 9837708
*/


char* getcurrentdir(char *path, int pathsize);

void append(char *dest, int buffersize, char *src);

void lista_diretorio(char *path, char *buffer, int buffersize);

int transferfile(char *path, int output_fd);

int composepath(char *oldpath,char *path,char *newpath);
