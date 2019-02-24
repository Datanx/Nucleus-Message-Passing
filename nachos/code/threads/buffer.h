#ifndef BUFFER_H
#define BUFFER_H

class Buffer{
public:
    char* senderId;
    char* receiverId;
    char* msg;
    bool msgFlag = false;
    bool dumFlag = false;
};

#endif // BUFFER_H