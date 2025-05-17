#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#define DIM_MAX 100
typedef struct
{
    float latitude;
    float longitude;
} GPS;
typedef struct
{
    char treasure_id[DIM_MAX];
    char user_name[DIM_MAX];
    GPS gps;
    char clue_text[DIM_MAX];
    int value;
} Treasure;
int main(int argc, char **argv)
{
    Treasure t;
    int fd;
    char user_name[DIM_MAX][DIM_MAX];
    int user_count = 0;
    int i;
    char path[DIM_MAX] = "";
    if (argc != 2)
    {
        printf("The number of arguments is incorrect\n");
        exit(-1);
    }
    if (sprintf(path, "./hunts/%s/%s_treasures.dat", argv[1], argv[1]) < 0)
    {
        perror("Error creating path\n");
        exit(-1);
    }
    if ((fd = open(path, O_RDONLY)) < 0)
    {
        perror("Error opening treasure file\n");
        exit(-1);
    }
    int res;
    while ((res = read(fd, &t, sizeof(Treasure))) > 0)
    {
        for(i = 0; i < user_count; i++)
        {
            if(strcmp(user_name[i], t.user_name) == 0)
            {
                break;
            }
        }
        if (i == user_count)
        {
            strcpy(user_name[user_count], t.user_name);
            user_count++;
        }
    }
    if(res == -1)
    {
        perror("Error reading in file");
        exit(-1);
    }
    printf("Hunt ID: %s\n", argv[1]);
    for(i = 0; i < user_count; i++)
    {
        int user_value = 0;
        if (lseek(fd, 0, SEEK_SET) == -1)
        {
            perror("Error seeking in file");
            exit(-1);
        }
            while (res = (read(fd, &t, sizeof(Treasure))) > 0)
            {
                if (strcmp(user_name[i], t.user_name) == 0)
                {
                    user_value += t.value;
                }
            }
        if(res == -1)
        {
            perror("Error reading in file");
            exit(-1);
        }
        printf("User: %s, Score: %d\n", user_name[i], user_value);
        
    }
    if(close(fd) == -1)
    {
        perror("Error closing file");
        exit(-1);
    }
}