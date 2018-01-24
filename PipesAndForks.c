#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#define BUFFER_SIZE 1024

char* translater(char* word);

//Sourced from https://stackoverflow.com/questions/18078008/c-delete-the-i-th-character-from-a-writable-char-array
void delete_char(char *str, int i);

int main(void)
{

	int     inputOutput[2];
	int     inputTranslate[2];
	int     translateOutput[2];
	pid_t   childpid1;
	pid_t   childpid2;
	char    buffer[1];
    char    in;

	pipe(inputOutput);
	pipe(inputTranslate);
	
	system ("/bin/stty raw");


	if((childpid1 = fork()) == -1)
    {
        perror("fork");
        exit(1);
    }

    if(childpid1 == 0)
    {
    	//child
    	//input
		do  
		{
        	/* type a T to break out of the loop, since CTRL-D won't work raw */
            in = getchar();
    	    buffer[0] = in;
	        write(inputOutput[1], buffer, (1));
	        write(inputTranslate[1], buffer, (1));
        } while(1);


    }
    else 
    {
		//parent
		//fork again
		if((childpid2 = fork()) == -1)
    	{
       		perror("fork");
       		exit(1);
    	}


    	if(childpid2 == 0)
    	{
    		//child
    		//output
    		while(read(inputOutput[0], buffer, sizeof(buffer)) > 0)
            {
                printf("%c", buffer[0]);
                fflush(stdout);
            }
    	}
    	else
    	{
    		//parent
    		//translate
    		char temp[BUFFER_SIZE];
    		char newline[2] = {'\r', '\n'};
            char* send;
    		int counter = 0;
            //printf("child1: %d child2: %d\r\n", childpid1, childpid2);
    		while(read(inputTranslate[0], buffer, sizeof(buffer)) > 0)
            {
	        	temp[counter] = buffer[0];
	        	counter++;
	        	if(counter == BUFFER_SIZE - 1)
	        	{
	        		printf("overflow\n");
	        	}
	        	if(buffer[0] == 'E')
	        	{
                    //Enter
                    send = translater(temp);
	        		write(inputOutput[1], newline, 2);
	        		write(inputOutput[1], send, counter);
	        		write(inputOutput[1], newline, 2);
	        		memset(temp, 0, sizeof (temp));
	        		counter = 0;
	        	}
                else if(buffer[0] == 'T')
                {
                    //Normal Termination
                    send = translater(temp);
                    write(inputOutput[1], newline, 2);
                    write(inputOutput[1], send, counter);
                    write(inputOutput[1], newline, 2);

                    system ("/bin/stty cooked");

                    close(inputTranslate[0]);
                    close(inputTranslate[1]);

                    close(inputOutput[0]);
                    close(inputOutput[1]);

                    close(translateOutput[0]);
                    close(translateOutput[1]);

                    kill(childpid1, 9);
                    kill(childpid2, 9);
                    break;
                }
                else if(buffer[0] == 11)
                {
                    //Abnormal Termination
                    system ("/bin/stty cooked");

                    close(inputTranslate[0]);
                    close(inputTranslate[1]);

                    close(inputOutput[0]);
                    close(inputOutput[1]);

                    kill(childpid1, 9);
                    kill(childpid2, 9);
                    break;
                }
            }
            
    	}
    }
    return 0;
}



char *translater(char *word) {
    int len = strlen(word);
    int i;
    char *final = malloc(len+1);
    for(i=0; i<=len; i++) {         // include terminator
        if(word[i] == 'a')
        {
            final[i] = 'z';
        }
        else if(word[i] == 'E')
        {
            //Delete E and shift up
            delete_char(word, i);
            final[i] = word[i];
        }
        else if(word[i] == 'T')
        {
            //Delete T and shift up
            delete_char(word, i);
            final[i] = word[i];
        }
        else if(word[i] == 'X')
        {
            //Delete X and previous char and shift up
            delete_char(word, i);
            delete_char(word, i - 1);
            final[i - 1] = word[i];

        }
        else
        {
            final[i] = word[i];
        }
    }
    return final;
}

void delete_char(char *str, int i) {
    int len = strlen(str);

    for (; i < len - 1 ; i++)
    {
       str[i] = str[i+1];
    }

    str[i] = '\0';
}