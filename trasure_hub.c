#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>

pid_t monitor_pid;
int monitor_running = 0;
int monitor_stopping = 0;
void list_treasures(){}
void list_hunts(){
    printf("List of hunts:\n");
}
void view_treasure(){

}
void stop_monitor(){

}
void monitor_process()
{
    printf("Monitor process started\n");
    struct sigaction monitor_actions;
    memset(&monitor_actions, 0x00, sizeof(struct sigaction));
    monitor_actions.sa_handler = list_hunts;
    if(sigaction(SIGUSR1, &monitor_actions, NULL) < 0)
    {
        perror("Process SIGUSR1 failed");
        exit(-1);
    }
    monitor_actions.sa_handler = list_treasures;
    if(sigaction(SIGUSR2, &monitor_actions, NULL) < 0)
    {
        perror("Process SIGUSR2 failed");
        exit(-1);
    }
    monitor_actions.sa_handler = view_treasure;
    if(sigaction(SIGINT, &monitor_actions, NULL) < 0)
    {
        perror("Process SIGINT failed");
        exit(-1);
    }
    monitor_actions.sa_handler = stop_monitor;
    if(sigaction(SIGTERM, &monitor_actions, NULL) < 0)
    {
        perror("Process SIGTERM failed");
        exit(-1);
    }
    while(1)
    {
        pause();
    }
}
int main(int argc, char *argv[])
{
    char command[256];
    int fd, mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    while(1)
    {
        printf("Commands: start_monitor, list_hunts, list_treasure, view_treasure, stop_monitor, exit\n");
        printf("Enter command: ");
        scanf("%s", command);
        if (strcmp(command, "start_monitor") == 0)
        {
            if(monitor_running == 1)
            {
                printf("Monitor is already running\n");
                return 1;
            }
            else{
                monitor_running = 1;
                monitor_stopping = 0;
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
                monitor_pid = fork();
                if (monitor_pid < 0)
                {
                    perror("Fork failed");
                    exit(-1);
                }
                else if (monitor_pid == 0)
                {
                    monitor_process();
                    exit(0);
                }
                else 
                {
                    sleep(1);
                    printf("\n");
                    continue;
                }
            }
        }
        else if(strcmp(command, "list_hunts") == 0)
        {   
            
            
            if(monitor_running == 0)
            {
                printf("Monitor is not running\n");
                return 1;
            }
            char huntId[256];
            printf("Enter hunt ID: ");
            scanf("%s", huntId);
            if(kill(monitor_pid, SIGUSR1) == -1)
            {
                perror("Failed to send SIGUSR1");
                return 1;
            }
            sleep(1);
        }
        else if(strcmp(command, "list_treasures") == 0)
        {
            if(monitor_running == 0)
            {
                printf("Monitor is not running\n");
                return 1;
            }
            if(kill(monitor_pid, SIGUSR2) == -1)
            {
                perror("Failed to send SIGUSR2");
                return 1;
            }
            sleep(1);
        }
        else if (strcmp(command, "view_treasure") == 0)
        {
            if(monitor_running == 0)
            {
                printf("Monitor is not running\n");
                return 1;
            }
            if(kill(monitor_pid, SIGINT) == -1)
            {
                perror("Failed to send SIGINT");
                return 1;
            }
            sleep(1);
        }
        else if(strcmp(command, "stop_monitor") == 0)
        {
            if(monitor_running == 0)
            {
                printf("Monitor is not running\n");
                return 1;
            }
            if(kill(monitor_pid, SIGTERM) == -1)
            {
                perror("Failed to send SIGTERM");
                return 1;
            }
            monitor_running = 0;
        }
        else if(strcmp(command, "exit") == 0)
        {
            if(monitor_running == 1)
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
            exit(1);
        }
    }
}