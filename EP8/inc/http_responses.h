/**
 *  @file: http_responses.h
 *  @brief: Biblioteca que cria respostas HTTP
 * 
 *  @author: Isabella Bologna - 9267161
 *  @author: Renato Freitas   - 9837708
*/

void ok(char* response, char* type);
void bad_request(char* response);
void not_found(char* response);
void not_supported(char* response);
int send_response(char* img, int sd);
