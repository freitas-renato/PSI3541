# Resolução de nomes

* nome de host <-> IP 
* nome de serviço <-> numero da porta/protocolo de transporte (ex: http <-> 80/TCP)
* nome do protocolo <-> numero do protocolo (ex: UDP <-> 17)


## Resolução de nome de host

- `hostent gethostbyname(name)`: a partir de um nome, retorna o IP
- `hostent gethostbyaddr(addr, len, type)`: a partir de um IP, retorna o nome

> type = tipo de endereço, AF_INET representa IPv4

```C

struct hostent {
  char *h_name;     /* Official name of host.  */
  char **h_aliases; /* Alias list.  */
  int h_addrtype;   /* Host address type.  */
  int h_length;     /* Length of address.  */
  char **h_addr_list;   /* List of addresses from name server.  */
  #define h_addr h_addr_list[0]
};

```

## Resolução de nome de serviço

- `servent getservbyname(name, proto)`: a partir de um nome de serviço, descobre a porta do protocolo de transporte
- `servent getservbyport(port, proto)`: a partir de uma porta (protocolo de transporte), permite descobrir o nome do serviço associado

```C
struct servent {
  char *s_name;			/* Official service name.  */
  char **s_aliases;		/* Alias list.  */
  int s_port;			/* Port number. (network byte order) */
  char *s_proto;		/* Protocol to use. (TCP/UDP) */
};
```

> número da porta é representado por int16_t

> utilizar `htohs()` para traduzir um int16_t na representação *host bye order* para *network byte order*


## Resolução de nomes de protocolo

- `protent getprotobyname(name)`: a partir do nome do protocolo, descobre seu número
- `protent getprotobynumber(proto)`: a partir do numero do protocolo, descobre seu nome

```C
struct protoent {
  char *p_name;			/* Official protocol name.  */
  char **p_aliases;		/* Alias list.  */
  int p_proto;			/* Protocol number.  (host byte order) */
};
```