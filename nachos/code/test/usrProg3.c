#include "syscall.h"

#define MSGMAX 100
#define IDMAX 100
#define  NULL  ((void *)0)

void Print(char *ptr, OpenFileId output)
{
    char* prefix = "usrProg3: ";
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
    void* buff = NULL;
    char answer[MSGMAX] ;
    char message[MSGMAX];
    char sender[IDMAX];
    int isDummy = 0;

    char* ans = "Answer from P3\n\n";
    char* prog1 = "../test/usrProg1";
    char* recvMsg = "Received message: \n";
    char* sndAns = "Sending answer: \n";
    char* exitMsg = "usrProg3 exits!!!\n\n";
    setArray(ans, answer);
    setArray(prog1, sender);

    
    // Test for dummy message
    WaitMessage(sender, message, &buff);
    Print(recvMsg, output);
    Print(message, output);

    Print(exitMsg, output);
    Exit(0);
}
