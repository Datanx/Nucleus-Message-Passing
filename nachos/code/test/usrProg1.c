#ifdef UNIX_
#include <stdio.h>
#else
#include "syscall.h"
#endif /* UNIX */

#define MSGMAX 100
#define IDMAX 100
#define NULL ((void *)0)

void Print(char *ptr, OpenFileId output)
{
    char* prefix = "usrProg1: ";
    int i = 0;
    char *target = ptr;
    Write(prefix, 12, output);
    while (*target != '\0')
    {
        Write(target, 1, output);
        target++;
        i++;
    }
    Write(target, 1, output);
}

void setArray(char* src, char* des)
{
    int i = 0;
    char* ptr = src;
    while(*ptr != '\0')
    {
        des[i] = *ptr;
        i++;
        ptr++;
    }
    des[i] = '\0';
}

int main()
{
    OpenFileId output = ConsoleOutput;

    void *buff = NULL;
    void *buff2 = NULL;
    int isDumm = 0;
    char tmp;
    char* msg = "Message from P1\n\n";
    
    char *sndMsg = "Sending message: \n";
    char *ansRecv = "Answer received: \n";
    char *prog2 = "../test/usrProg2"; 
    char *prog3 = "../test/usrProg3";

    char* title1 = "\n======== First Test: P1 --> P2, P2 --> P1 ========\n";
    char* title2 = "\n======== Second Test: P1 --> P3, P3 exit, dummy reply--> P1 ========\n";   

    char message[MSGMAX];
    char receiver[MSGMAX];
    char receiver2[MSGMAX];
    char answer[MSGMAX];
    char answer2[MSGMAX];

    setArray(msg, message);
    setArray(prog2, receiver);
    setArray(prog3, receiver2);

    // Test for normal(one-on-one) communication
    Write(title1, 52, output);    

    Print(sndMsg, output);
    Print(msg, output);
    SendMessage(receiver, message, &buff);

    WaitAnswer(&isDumm, answer, buff);

    Print(ansRecv, output);
    Print(answer, output);


    // Test for dummy answer	
    Write(title2, 70, output);   

    Print(sndMsg, output);
    Print(msg, output);
    SendMessage(receiver2, message, &buff2);

    WaitAnswer(&isDumm, answer2, buff2);
    Print(ansRecv, output);
    Print(answer2, output);

    Exit(0);
}
