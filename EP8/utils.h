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

inline float map(float former_value, float former_min, float former_max, float new_min, float new_max) {
    double new_value;

    new_value = (double) (former_value - former_min) * (new_max - new_min);
    new_value /= (former_max - former_min);
    new_value += new_min;

    return (float) new_value;
}

