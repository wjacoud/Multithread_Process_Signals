#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

void handle_USR1(int signo, siginfo_t *sinfo, void *context)
{
    int s = sinfo->si_signo;
    int p = sinfo->si_pid;
    int i1 = 0; int i2 = 0;
    char sig[5], gis[5], proc[12], corp[12];
    while(s > 0 || i1 == 0) {
        sig[i1] = '0'+ s%10;    // sig[i2] fica com o digito das unidades
        s = s/10;               // Descarta digito das unidades, "empurra" digitos para a direita
        ++i1;
    }
    for (int j = 0; j < i1 ; j++) { // Inversao de correcao do array
        gis[j] = sig[i1 - 1 - j];
    }
    while(p > 0 || i2 == 0) {
        proc[i2] = '0' + p % 10;    // proc[i2] fica com o digito das unidades
        p = p / 10;               // Descarta digito das unidades, "empurra" digitos para a direita
        ++i2;
    }
    for (int j = 0; j < i2 ; j++) { // Inversao de correcao do array
        corp[j] = proc[i2 - 1 - j];
    }
    corp[i2] = '\n';
    i2++;
    write(STDOUT_FILENO, "O sinal ", 8);
    write(STDOUT_FILENO, gis, i1);
    write(STDOUT_FILENO, " foi capturado. Foi enviado pelo processo ", 42);
    write(STDOUT_FILENO, corp, i2);
}

int main(int argc, char *argv[])
{
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    sigemptyset(&act.sa_mask); /* No signals blocked */
    act.sa_sigaction = handle_USR1;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);
    /* SIGUSR1 will now be captured and handled (POSIX). */
    /* (...) */

        for(;;){
        printf("I Love SCOMP! \n");
        sleep(5);
    }

    return 0;
}

//a. Which is most suitable to handle signals? Justify your choice.
//(signal vs sigaction functions)

//sigaction funciona em POSIX, enquanto que signal funciona em ANSI C
//A função singal não bloqueia a receção de outros sinais enquanto um handler
//está em execução. Sigaction pode bloquear outros sinais até que o handler termine de executar

//b)

//int sigaction (int sig,
//               const struct sigaction * act,
//               struct sigaction * oact);

//sig - inteiro correspondente ao sinal recebido (ver kill -l)
//const struct sigaction * act - associa detalhes de uma ação ao sinal sig
//struct sigaction * oact - é usado para guardar os detalhes de uma "antiga" ação dada ao sinal

//d) Não é possível usar variáveis globais ou estáticas de maneira segura na função,
//pois não é possível assegurar que elas não serão alteradas por componentes externos durante a execução.

//e)
//abort(3)               Added in POSIX.1-2003
//accept(2)
//access(2)
//aio_error(3)
//aio_return(3)
//aio_suspend(3)         See notes below
//alarm(2)
//bind(2)
//cfgetispeed(3)
//cfgetospeed(3)
//cfsetispeed(3)
//cfsetospeed(3)
//chdir(2)
//chmod(2)
//chown(2)
//clock_gettime(2)
//close(2)
//
//connect(2)
//creat(2)
//dup(2)
//dup2(2)
//execl(3)               Added in POSIX.1-2008; see notes below
//execle(3)              See notes below
//execv(3)               Added in POSIX.1-2008
//execve(2)
//_exit(2)
//_Exit(2)
//faccessat(2)           Added in POSIX.1-2008
//fchdir(2)              Added in POSIX.1-2013
//fchmod(2)
//fchmodat(2)            Added in POSIX.1-2008
//fchown(2)
//fchownat(2)            Added in POSIX.1-2008
//fcntl(2)
//fdatasync(2)
//fexecve(3)             Added in POSIX.1-2008
//ffs(3)                 Added in POSIX.1-2016
//fork(2)                See notes below
//fstat(2)
//fstatat(2)             Added in POSIX.1-2008
//fsync(2)
//ftruncate(2)
//futimens(3)            Added in POSIX.1-2008
//getegid(2)
//geteuid(2)
//getgid(2)
//getgroups(2)
//getpeername(2)
//getpgrp(2)
//getpid(2)
//getppid(2)
//getsockname(2)
//getsockopt(2)
//getuid(2)
//htonl(3)               Added in POSIX.1-2016
//htons(3)               Added in POSIX.1-2016
//kill(2)
//link(2)
//linkat(2)              Added in POSIX.1-2008
//listen(2)
//longjmp(3)             Added in POSIX.1-2016; see notes below
//lseek(2)
//lstat(2)
//memccpy(3)             Added in POSIX.1-2016
//memchr(3)              Added in POSIX.1-2016
//memcmp(3)              Added in POSIX.1-2016
//memcpy(3)              Added in POSIX.1-2016
//memmove(3)             Added in POSIX.1-2016
//memset(3)              Added in POSIX.1-2016
//mkdir(2)
//mkdirat(2)             Added in POSIX.1-2008
//mkfifo(3)
//mkfifoat(3)            Added in POSIX.1-2008
//mknod(2)               Added in POSIX.1-2008
//mknodat(2)             Added in POSIX.1-2008
//ntohl(3)               Added in POSIX.1-2016
//ntohs(3)               Added in POSIX.1-2016
//open(2)
//openat(2)              Added in POSIX.1-2008
//pause(2)
//pipe(2)
//poll(2)
//
//posix_trace_event(3)
//pselect(2)
//pthread_kill(3)        Added in POSIX.1-2013
//pthread_self(3)        Added in POSIX.1-2013
//pthread_sigmask(3)     Added in POSIX.1-2013
//raise(3)
//read(2)
//readlink(2)
//readlinkat(2)          Added in POSIX.1-2008
//recv(2)
//recvfrom(2)
//recvmsg(2)
//rename(2)
//renameat(2)            Added in POSIX.1-2008
//rmdir(2)
//select(2)
//sem_post(3)
//send(2)
//sendmsg(2)
//sendto(2)
//setgid(2)
//setpgid(2)
//setsid(2)
//setsockopt(2)
//setuid(2)
//shutdown(2)
//sigaction(2)
//sigaddset(3)
//sigdelset(3)
//sigemptyset(3)
//sigfillset(3)
//sigismember(3)
//siglongjmp(3)          Added in POSIX.1-2016; see notes below
//signal(2)
//sigpause(3)
//sigpending(2)
//sigprocmask(2)
//sigqueue(2)
//sigset(3)
//sigsuspend(2)
//sleep(3)
//sockatmark(3)          Added in POSIX.1-2004
//socket(2)
//socketpair(2)
//stat(2)
//stpcpy(3)              Added in POSIX.1-2016
//stpncpy(3)             Added in POSIX.1-2016
//strcat(3)              Added in POSIX.1-2016
//strchr(3)              Added in POSIX.1-2016
//strcmp(3)              Added in POSIX.1-2016
//strcpy(3)              Added in POSIX.1-2016
//strcspn(3)             Added in POSIX.1-2016
//strlen(3)              Added in POSIX.1-2016
//strncat(3)             Added in POSIX.1-2016
//strncmp(3)             Added in POSIX.1-2016
//strncpy(3)             Added in POSIX.1-2016
//strnlen(3)             Added in POSIX.1-2016
//strpbrk(3)             Added in POSIX.1-2016
//strrchr(3)             Added in POSIX.1-2016
//strspn(3)              Added in POSIX.1-2016
//strstr(3)              Added in POSIX.1-2016
//strtok_r(3)            Added in POSIX.1-2016
//symlink(2)
//symlinkat(2)           Added in POSIX.1-2008
//tcdrain(3)
//
//tcflow(3)
//tcflush(3)
//tcgetattr(3)
//tcgetpgrp(3)
//tcsendbreak(3)
//tcsetattr(3)
//tcsetpgrp(3)
//time(2)
//timer_getoverrun(2)
//timer_gettime(2)
//timer_settime(2)
//times(2)
//umask(2)
//uname(2)
//unlink(2)
//unlinkat(2)            Added in POSIX.1-2008
//utime(2)
//utimensat(2)           Added in POSIX.1-2008
//utimes(2)              Added in POSIX.1-2008
//wait(2)
//waitpid(2)
//wcpcpy(3)              Added in POSIX.1-2016
//wcpncpy(3)             Added in POSIX.1-2016
//wcscat(3)              Added in POSIX.1-2016
//wcschr(3)              Added in POSIX.1-2016
//wcscmp(3)              Added in POSIX.1-2016
//wcscpy(3)              Added in POSIX.1-2016
//wcscspn(3)             Added in POSIX.1-2016
//wcslen(3)              Added in POSIX.1-2016
//wcsncat(3)             Added in POSIX.1-2016
//wcsncmp(3)             Added in POSIX.1-2016
//wcsncpy(3)             Added in POSIX.1-2016
//wcsnlen(3)             Added in POSIX.1-2016
//wcspbrk(3)             Added in POSIX.1-2016
//wcsrchr(3)             Added in POSIX.1-2016
//wcsspn(3)              Added in POSIX.1-2016
//wcsstr(3)              Added in POSIX.1-2016
//wcstok(3)              Added in POSIX.1-2016
//wmemchr(3)             Added in POSIX.1-2016
//wmemcmp(3)             Added in POSIX.1-2016
//wmemcpy(3)             Added in POSIX.1-2016
//wmemmove(3)            Added in POSIX.1-2016
//wmemset(3)             Added in POSIX.1-2016
//write(2)

