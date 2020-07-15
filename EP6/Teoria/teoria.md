# TCP

## Cliente

### Resumo das chamadas

*mesma coisa do UDP, ver em "EP5/Teoria.md"*


## Servidor

### Observações

- endereço IP do socked address do servidor pode ser:
    - todas as interfaces locais (0.0.0.0) = usar a macro INADDR_ANY
    - associado a uma interface do servidor = ex: localhost (127.0.0.1)

- `listen()`
    - abre o socked address no qual o servidor irá aguardar pedidos de conexão TCP
    - uma conexão estabelecida poderá ser conhecida somente após a ativação de `accept()`
    - `listen(int sd, int queuelenght)`
    - queuelenght = quantidade max de conexões pendentes a serem aceitas

- `accept()`
    - extrai a primeira conexão da fila e gera um novo sd para essa conexão
    - o sd original não é afetado
    - `sd2 = accept(int sd, struct sockaddr *addr, socklen_t *addrlen)`
    - addr = contém o endereço do parceiro de comunicação
    - addrlen = tamanho da struct socketaddr
     - ```c
        // Exemplo
        struct sockaddr_in client_addr;
        size = sizeof(client_addr);
        client_sd = accept(sd, (struct sockaddr*) &client_addr, (socklen_t*) &size);
     ```

- `read(int sd, void *buf, int buffersize)`

- `write(int sd, void *buf, int count)`

- `close(int sd)`    


### Servidor Concorrente

- Capacidade de processar mais que uma requisição simultaneamente
- implementação: 
    - chamadas async
    - servidor multithreaded

    