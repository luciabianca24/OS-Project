#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
pid_t monitor_pid;
int monitor_running = 0;
int mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

void exec_command(char **argv)
{
    int exec_pid;
    if ((exec_pid = fork()) < 0)
    {
        perror("Error creating fork-exec\n");
        exit(1);
    }
    if (exec_pid == 0)
    {
        execvp(argv[0], argv);
    }
    else
    {
        int status;
        if(waitpid(exec_pid, &status, 0) == -1)
        {
            perror("Error waitpid");
            exit(-1);
        }
    }
}
void list_treasures(char hunt_id[]) {
    char *argv[] = {"./treasure_manager", "--list", hunt_id, NULL};
    exec_command(argv);
}
void list_hunts()
{
    char *argv[] = {"./treasure_manager", "--list_hunts", NULL};
    exec_command(argv);
}
void view_treasure(char hunt_id[], char treasure_id[])
{
    char *argv[] = {"./treasure_manager", "--view", hunt_id, treasure_id, NULL};
    exec_command(argv);
}
void read_from_file()
{
    char hunt_id[100] = "";
    char treasure_id[100] = "";
    int fd;
    if ((fd = open("./commands.txt", O_RDONLY)) < 0)
    {
        perror("Error creating commands file");
        exit(-1);
    }
    char command[100] = "";
    if (read(fd, &command, 100) < 0)
    {
        perror("Error reading in file");
        exit(-1);
    }
    if (strcmp(command, "list_hunts") == 0)
    {
        list_hunts();
    }
    else if (strcmp(command, "list_treasure") == 0)
    {
        if (read(fd, hunt_id, 100) < 0)
        {
            perror("Error reading in file");
            exit(-1);
        }
        list_treasures(hunt_id);
    }
    else if (strcmp(command, "view_treasure") == 0)
    {
        if (read(fd, hunt_id, 100) < 0)
        {
            perror("Error reading in file");
            exit(-1);
        }
        if (read(fd, treasure_id, 100) < 0)
        {
            perror("Error reading in file");
            exit(-1);
        }
        view_treasure(hunt_id, treasure_id);
    }
    if (close(fd) == -1)
    {
        perror("Error closing commands file");
        exit(-1);
    }
}
void read_from_pipe(int pfd)
{
    char buff[512];
    while(1)
    {
        int read_bytes = read(pfd, buff, sizeof(buff) - 1);
        if (read_bytes > 0)
        {
            buff[read_bytes] = '\0';
            printf("%s\n", buff);
        }
        else if (read_bytes == 0)
        {
            break;
        }
        else if (read_bytes == -1 && errno == EAGAIN)
        {
            break;
        }
        else
        {
            perror("Error reading from pipe");
            exit(-1);
        }
    }
}
void calculate_score()
{
    struct dirent *dp;
    DIR *d;
    int pfd[2];
    int pid;
    if ((d = opendir("./hunts")) == NULL)
    {
        perror("Error opening hunts directory");
        exit(-1);
    }
    int compile_status = system("gcc -o calculate_score calculate_score.c");
    if (compile_status != 0)
    {
        perror("Error compiling calculate_score.c");
        return;
    }
    while((dp = readdir(d)) != NULL)
    {
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            continue;
        if(pipe(pfd) == -1)
        {
            perror("Error creating pipe");
            exit(-1);
        }
        if(pid = fork() == -1)
        {
            perror("Error creating fork");
            exit(-1);
        }
        else if(pid == 0)
        {
            if(close(pfd[0]) == -1)
            {
                perror("Error closing pipe");
                exit(-1);
            }
            dup2(pfd[1], STDOUT_FILENO);
            char *argv[] = {"./calculate_score", dp->d_name, NULL};
            exec_command(argv);
            exit(-1);
        }
        else
        {
            if(close(pfd[1]) == -1)
            {
                perror("Error closing pipe");
                exit(-1);
            }
            int status;
            if (waitpid(pid, &status, 0) == -1)
            {
                perror("Error waitpid");
                exit(-1);
            }
            read_from_pipe(pfd[0]);
        }
    }
    if (closedir(d) == -1)
    {
        perror("Error closing dir");
        exit(-1);
    }
}
void monitor_process(int pfd[2])
{
    printf("Monitor process started\n");
    dup2(pfd[1], STDOUT_FILENO);
    struct sigaction monitor_actions;
    memset(&monitor_actions, 0x00, sizeof(struct sigaction));
    monitor_actions.sa_handler = read_from_file;
    if (sigaction(SIGUSR1, &monitor_actions, NULL) < 0)
    {
        perror("Process SIGUSR1 failed");
        exit(-1);
    }
    if (close(pfd[1]) == -1)
    {
        perror("Error closing pipe");
        exit(-1);
    }
    while (1)
    {
        pause();
    }
}
int main()
{
    char command[256];
    int fd;
    int pfd[2];
    while (1)
    {
        printf("Commands: start_monitor, list_hunts, list_treasure, view_treasure, calculate_score, stop_monitor, exit\n");
        printf("Enter command: ");
        scanf("%s", command);
        if (strcmp(command, "start_monitor") == 0)
        {
            if (monitor_running == 1)
            {
                printf("Monitor is already running\n");
            }
            else
            {
                monitor_running = 1;
                if ((fd = open("./commands.txt", O_CREAT, mode)) < 0)
                {
                    perror("Error creating commands file");
                    exit(-1);
                }
                if (close(fd) == -1)
                {
                    perror("Error closing commands file");
                    exit(-1);
                }
                if (pipe(pfd) == -1)
                {
                    perror("Error creating pipe");
                    exit(-1);
                }
                monitor_pid = fork();
                if (monitor_pid < 0)
                {
                    perror("Fork failed");
                    exit(-1);
                }
                else if (monitor_pid == 0)
                {
                    if(close(pfd[0]) == -1)
                    {
                        perror("Error closing pipe");
                        exit(-1);
                    }
                    monitor_process(pfd);
                    exit(0);
                }
                else
                {
                    if(close(pfd[1]) == -1)
                    {
                        perror("Error closing pipe");
                        exit(-1);
                    }
                    if(fcntl(pfd[0], F_SETFL, O_NONBLOCK) == -1)
                    {
                        perror("Error setting non-blocking mode");
                        exit(-1);
                    }
                    sleep(1);
                    printf("\n");
                    continue;
                }
            }
        }
        else if (strcmp(command, "list_hunts") == 0)
        {
            if (monitor_running == 0)
            {
                printf("Monitor is not running\n");
            }
            else
            {
                if ((fd = open("./commands.txt", O_WRONLY, mode)) < 0)
                {
                    perror("Error creating commands file");
                    exit(-1);
                }
                if (write(fd, "list_hunts", 100) < 0)
                {
                    perror("Error writing in file");
                    exit(-1);
                }
                if (close(fd) == -1)
                {
                    perror("Error closing commands file");
                    exit(-1);
                }
                if (kill(monitor_pid, SIGUSR1) == -1)
                {
                    perror("Failed to send SIGUSR1");
                    return 1;
                }
                sleep(1);
                read_from_pipe(pfd[0]);
                sleep(1);
            }
        }
        else if (strcmp(command, "list_treasure") == 0)
        {
            if (monitor_running == 0)
            {
                printf("Monitor is not running\n");
            }
            else
            {
                char huntId[100];
                printf("Enter hunt ID: ");
                scanf("%s", huntId);
                if ((fd = open("./commands.txt", O_WRONLY, mode)) < 0)
                {
                    perror("Error creating commands file");
                    exit(-1);
                }
                if(write(fd, "list_treasure", 100) < 0)
                {
                    perror("Error writing in file");
                    exit(-1);
                }
                if(write(fd,huntId,100)<0)
                {
                    perror("Error writing in file");
                    exit(-1);
                }
                if (close(fd) == -1)
                {
                    perror("Error closing commands file");
                    exit(-1);
                }
                if (kill(monitor_pid, SIGUSR1) == -1)
                {
                    perror("Failed to send SIGUSR2");
                    return 1;
                }
                sleep(1);
                read_from_pipe(pfd[0]);
                sleep(1);
            }
        }
        else if (strcmp(command, "view_treasure") == 0)
        {
            if (monitor_running == 0)
            {
                printf("Monitor is not running\n");
            }
            else
            {
                char hunt_id[100] = "";
                char treasure_id[100] = "";
                printf("Enter hunt ID: ");
                scanf("%s", hunt_id);
                printf("Enter treasure ID: ");
                scanf("%s", treasure_id);
                printf("\n");
                if ((fd = open("./commands.txt", O_WRONLY)) < 0)
                {
                    perror("Error creating commands file");
                    exit(-1);
                }
                if (write(fd, "view_treasure", 100) < 0)
                {
                    perror("Error writing in file");
                    exit(-1);
                }
                if (write(fd, hunt_id, 100) < 0)
                {
                    perror("Error writing in file");
                    exit(-1);
                }
                if (write(fd, treasure_id, 100) < 0)
                {
                    perror("Error writing in file");
                    exit(-1);
                }
                if (close(fd) == -1)
                {
                    perror("Error closing commands file");
                    exit(-1);
                }
                if (kill(monitor_pid, SIGUSR1) == -1)
                {
                    perror("Failed to send SIGINT");
                    return 1;
                }
                sleep(1);
                read_from_pipe(pfd[0]);
                sleep(1);
            }
        }
        else if (strcmp(command, "stop_monitor") == 0)
        {
            if (monitor_running == 0)
            {
                printf("Monitor is not running\n");
            }
            else
            {

                if (kill(monitor_pid, SIGTERM) == -1)
                {
                    perror("Failed to send SIGTERM");
                    return 1;
                }
                int status;
                waitpid(monitor_pid, &status, WNOHANG);
                printf("Monior stopped with code:%d\n", WEXITSTATUS(status));
                monitor_running = 0;
                monitor_pid = 0;
            }
        }
        else if(strcmp(command, "calculate_score") == 0)
        {
            calculate_score();
        }
        else if (strcmp(command, "exit") == 0)
        {
            if (monitor_running == 1)
            {
                printf("Monitor is still running. Please stop the monitor\n");
            }
            else
            {
                exit(0);
            }
        }
        else
        {
            printf("Invalid command\n");
        }
    }
}