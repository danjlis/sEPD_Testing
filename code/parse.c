#include <stdio.h>
#include <string.h>

int parse()
{
    FILE *fp = fopen("parse_test.txt", "r");
    const char s[3] = ": ";
    char *token;
    int i;
    int b = 0;
    char *buf = new char[10];
    char *here = new char[10];
    char *there = new char[10];
    if(fp != NULL)
    {
        char line[20];
        while(fgets(line, sizeof line, fp) != NULL)
        {
            token = strtok(line, s);
            for(i=0;i<2;i++)
            {
                if(i==0)
                {
                    sprintf(buf, "%s",token);
                    token = strtok(NULL,s);
                    if (strncmp(buf,"Here", 4) == 0) b = 1;
                    else if (strncmp(buf,"There", 5) == 0) b = 2;;

                } else {
                    if (b == 1){
                      sprintf(here, "%s", token);
                      b = 0;
                    }
                    if (b == 2){
                      sprintf(there, "%s", token);
                      b = 0;
                    }
                }
            }
        }
        fclose(fp);
    } else {
        perror("user.dat");
    }
    printf("Here: %s \n", here);
    printf("There: %s \n", there);

    return 1;
}
