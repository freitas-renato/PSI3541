# N sei o titulo


## POSIX Socket API

Interface de programação mais usual para utilizar os serviços de TCP/IP.

Permite uma aplicação estabelecer sessão de comunicação UDP, conexão TCP (transporte), sessão de comunicação IP (rede).

### Socket Address

* Socket = extremo da comunicação entre duas aplicações

* Socket address = endereço que permite identificar um soquete de comunicação. No IPv4, contém o endereço IP e a porta do protocolo de transporte.
    
    * Socket addr = IP + Porta(transporte)


```c
// Estrutura genérica, representa o endereço de um socket
struct sockaddr {
    uint16_t sa_family;   // Tipo de endereço utilizado: AF_INET ou AF_INET6
    char    sa_data[14];  // Endereço
};
```

* Cada protocolo de comunicação possui uma forma própria de representar o endereço de um socket:

```c
// Formato específico para IPv4
struct sockaddr_in {
    sa_family_t     sin_family;  // addr family: AF_INET
    in_port_t       sin_port;    // porta em network byte order
    struct in_addr  sin_addr;    // endereço IP
};

struct in_addr {
    uint32_t s_addr;  // endereço IP em network byte order
};
```

### Utilização de sockets na pilha TCP/IP

* Ao chamar `socket()`, o SO aloca uma estrutura socket contendo inicialmente a família do protocolo, tipo de serviço e o protocolo de interação (IP, UDP, TCP)

    * Protocol family = PF_INET
    * Address family = AF_INET
    * Protocol = TCP, UDP, STCP (transporte); IP (rede)
    * Type = SOCK_STREAM, SOCK_DGRAM ou SOCK_RAW

----

## Cliente UDP

