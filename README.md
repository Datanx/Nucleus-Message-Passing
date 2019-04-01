# Nucleus-Message-Passing (C++)
Implement four system calls as described in the papers, to be called by user programs (i.e. C programs):
1. SendMessage: An asynchronous system call
2. WaitMessage: A synchronous system call
3. SendAnswer: An asynchronous system call
4. WaitAnswer: A synchronous system call

# REFERENCE:
## I.	Send Message (receiver, message, buffer)
- copy message into the first available buffer within the pool and delivers it in the queue of named receiver
- the receiver is activated if waiting on a message
- Sender continues after being informed of the “identity of the message buffer” (why this is important, because later on, answer if any   will be sent back using that message buffer ==> connection set up)

## II.	Wait Message (sender, message, buffer)
- delay requesting process until a message arrives in the process's queue
- on return, i.e., when process is ready to proceed), it is supplied:
   + name of sender
   + contents of message
   + identity of the message buffer (why need this? ok, later on, we will see some kind of piggy back, the same message buffer will be        used to store the answer. So this identity is some kind of connection between sender and receiver)
   + the buffer is removed from the queue, and made ready to transmit answer

## III.	Send Answer (result, answer, buffer)
- copy an answer in which message has been received (the identity in charge...)
- deliver it to queue of original sender (there is no sender name in the  call, how do i know what is original sender? again by the       identity of message buffer given when call WAIT MESSAGE)
- the answering process continues immediately
- the sender of the message is activated if it is waiting for the answer

## IV.	Wait Answer (result, answer, buffer)
- delays the requesting process until answer arrives in a given buffer (hence, send message need to know the identity of message buffer   where the message was sent, so that it know where to wait on)
- On arrival, the answer is copied into the process, and the buffer is  returned to the pool.
- Result: specify whether the answer is:
   + response from another process or
   + dummy answer generated by system nucleus in case of non-existing receiver


•	Implement the buffers correct in the Kernel.<br>
•	Processes should be able to communicate with any arbitrary process running in the system, but you need to work out some way of          identifying them (at compilation time, so we can’t use the PID).<br>
•	The same message buffer should be used for the same two processes to communicate.<br>
•	If a process terminates (and you are required to show an example of this happening). This can be done by invoking the Exit system        call mid-conversation. The system should send a dummy message out to the waiting process.<br>
•	If a process dies while it has sent messages, then those messages should still be receivable by their co-communicators.<br>
•	A limit on the total number of messages sent by the process should be configurable and enforceable.<br>
•	No other process should be able to interfere with communication (save ids).<br>
•	Present sufficient test cases to show the correctness of the implementation.<br>

<a href="https://996.icu"><img src="https://img.shields.io/badge/link-996.icu-red.svg" alt="996.icu"></a>
