#include "../globals/headers.h"
#include "../globals/global_variables.h"
/* Modify this file as needed*/
int remaining_time;
int *shm_ptr;

int resumeTime;
void SIGUSR2_handler(int sig)
{
    resumeTime = getClk();
    signal(SIGUSR2, SIGUSR2_handler);
}
void SIGUSR1_handler(int sig)
{
    remaining_time -= (getClk() - resumeTime) > 0 ? (getClk() - resumeTime) : 0;
    signal(SIGUSR1, SIGUSR1_handler);
    *shm_ptr = remaining_time;
    raise(SIGSTOP);
}
void SIGBUS_handler(int sig)
{
    remaining_time -= (getClk() - resumeTime) > 0 ? (getClk() - resumeTime) : 0;
    resumeTime = getClk();
    *shm_ptr = remaining_time;
    signal(SIGBUS, SIGBUS_handler);
}
int main(int argc, char *argv[])
{
    initClk();
    if (argc < 3)
        perror("Usage: ./process <remaining time> <scheduler_id> ");
    FILE *f = fopen("test_file.txt", "w");
    remaining_time = atoi(argv[1]);
    int scheduler_id = atoi(argv[2]);
    int shm_id = shmget(shm_key, sizeof(int), 0666 | IPC_CREAT);
    shm_ptr = (int *)shmat(shm_id, NULL, 0);
    signal(SIGUSR1, SIGUSR1_handler);
    signal(SIGUSR2, SIGUSR2_handler);
    signal(SIGBUS, SIGBUS_handler);
    *shm_ptr = getpid();
    int clk = getClk();
    resumeTime = getClk();
    printf("process with pid : %d started at time %d\n", getpid(), getClk());
    while (remaining_time > 0)
    {
        if (remaining_time <= (getClk() - resumeTime))
            break;
    }
    printf("process with pid : %d finished at time %d\n", getpid(), getClk());
    kill(scheduler_id, SIGUSR1);
    return 0;
}
