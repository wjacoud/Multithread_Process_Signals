#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#define MAX_INPUT 100

void handle_ALARM(int signo, siginfo_t *sinfo, void *context) {
    if (signo == 30 | signo == 14){
        write(STDOUT_FILENO, "To slow, that is why the program will end!\n", 43);
        exit(EXIT_FAILURE);
    }
    sleep(20);
    printf("SUCCESSEFULL EXECUTION!\n");
    exit(EXIT_FAILURE);
}
// a)
//int main() {
//    struct sigaction act;
//    memset(&act, 0, sizeof(struct sigaction));
//    sigfillset(&act.sa_mask); /* All signals blocked */
//    act.sa_sigaction = handle_ALARM;
//    act.sa_flags = SA_SIGINFO;
//    sigaction(SIGALRM, &act, NULL);
//    alarm(10);
//    /* You have 10 seconds before interrupting the following code. */
//    char input[MAX_INPUT];
//    printf("Insert a String:\n");
//    scanf("%s", input);
//    int n = strlen(input);
//    alarm(0);
//    printf("%d caracteres\n", n);
//    sleep(20);
//    printf("SUCCESSEFULL EXECUTION!\n");
//    return 0;
//}
//b)
int main() {
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    sigfillset(&act.sa_mask); /* All signals blocked */
    act.sa_sigaction = handle_ALARM;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);
    pid_t p = fork();
    if(p == 0){
        char input[MAX_INPUT];
        printf("Insert a String:\n");
        scanf("%s", input);
        int n = strlen(input);
        printf("%d caracteres\n", n);
        raise(SIGUSR2);
        exit(0);
    } else {
        sleep(10);
        kill(p,SIGUSR1);
    }
    wait(NULL);
    return 0;
}