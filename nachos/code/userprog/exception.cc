
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

//#include <cstring>
#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"

#define IDMAX 100
#define MSGMAX 100
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------

void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);
	int addr, size, value;

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	switch (which)
	{
	case SyscallException:
		switch (type)
		{
		case SC_Halt:
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

			SysHalt();

			ASSERTNOTREACHED();
			break;

		case SC_Add:
			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

			/* Process SysAdd Systemcall*/
			int result;
			result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
							/* int op2 */ (int)kernel->machine->ReadRegister(5));

			DEBUG(dbgSys, "Add returning with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);

			/* Modify return point */
			{
				/* set previous programm counter (debugging only)*/
				kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

				/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
				kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

				/* set next programm counter for brach execution */
				kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			}

			return;

			ASSERTNOTREACHED();

			break;

		case SC_Write:
		{
			addr = (int)kernel->machine->ReadRegister(4);
			size = (int)kernel->machine->ReadRegister(5);
			for (int i = 0; i < size; i++)
			{
				if (kernel->machine->ReadMem(addr, 1, &value))
				{
					printf("%c", (char)value);
					addr++;
				}
			}

			// cout << "\n Write system call made by " << kernel->currentThread->getName() << "\n";
			/* Modify return point */
			{
				/* set previous programm counter (debugging only)*/
				kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

				/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
				kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

				/* set next programm counter for brach execution */
				kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			}
			return;

			ASSERTNOTREACHED();
		}

		break;

		//  void SendMessage(char* receiverId, char* sndMsg, Buffer** buffer);
		case SC_SendMessage:              
		{
			// Reading the receiverId
			int recvAddr = (int)kernel->machine->ReadRegister(4);
			char recvId[IDMAX];
			int tmp;
			kernel->machine->ReadMem(recvAddr, 1, &tmp);
			if (tmp == NULL)
			{
				kernel->machine->ReadMem(recvAddr, 1, &tmp);
			}
			recvAddr++;

			int i = 0;
			while (tmp != 0)
			{
				recvId[i] = (char)tmp;
				kernel->machine->ReadMem(recvAddr, 1, &tmp);
				if (tmp == NULL)
				{
					kernel->machine->ReadMem(recvAddr, 1, &tmp);
				}
				recvAddr++;
				i++;
			}
			recvId[i] = '\0';
			// Reading the sndMsg
			int sndMsgAddr = (int)kernel->machine->ReadRegister(5);
			char sndMsg[MSGMAX];

			kernel->machine->ReadMem(sndMsgAddr, 1, &tmp);
			if (tmp == NULL)
			{
				kernel->machine->ReadMem(sndMsgAddr, 1, &tmp);
			}
			sndMsgAddr++;

			i = 0;
			while (tmp != 0)
			{
				sndMsg[i] = (char)tmp;
				kernel->machine->ReadMem(sndMsgAddr, 1, &tmp);
				if (tmp == NULL)
				{
					kernel->machine->ReadMem(sndMsgAddr, 1, &tmp);
				}
				sndMsgAddr++;
				i++;
			}
			sndMsg[i] = '\0';
			// Read buffer address
			// Buffer is the buffer address in the bufferPool
			// TempBuf is buffer pointer in mips from user program
			Buffer *buffer;
			int tempBuff = kernel->machine->ReadRegister(6);


			// Assign the buffer address
			ASSERT(kernel->currentThread->commCnt <= MaxComm);
			int poolNo = kernel->usedBuffers->FindAndSet(); // first empty buffer
			if (poolNo != -1)
			{
				kernel->currentThread->commCnt++;
				buffer = kernel->bufferPool[poolNo];

				// Write back to user program
				kernel->machine->WriteMem(tempBuff, 4, (int)buffer);
			}

			Thread *target;     // Communication target
			map<char*, Thread *>::iterator iter = kernel->scheduler->threadReg.begin();
			while(iter != kernel->scheduler->threadReg.end()){

				// Compare the Thread Name
				if(!strcmp(iter->first, recvId))
				{
					target = iter->second;
					break;
				}
				iter++;
			}
			
			if (iter == kernel->scheduler->threadReg.end())
			{
				cout << "\n Cannot find target thread.";
				exit(0);
			}

			// Assemble buffer and append to receiver's bufferQueue
			buffer->senderId = kernel->currentThread->getName();
			buffer->receiverId = recvId;
			buffer->msg = sndMsg;
			target->bufferQueue->Append(buffer);

			// Activate the receiver
			if (kernel->scheduler->sleepList->IsInList(target))
			{
				kernel->scheduler->sleepList->Remove(target);

				// ReadyToRun assumes that interrupts are disabled!
				kernel->interrupt->SetLevel(IntOff);
				kernel->scheduler->ReadyToRun(target);
				kernel->interrupt->SetLevel(IntOn);
			}

			/* Modify return point */
			{
				/* set previous programm counter (debugging only)*/
				kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

				/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
				kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

				/* set next programm counter for brach execution */
				kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			}
		}
		return;
		ASSERTNOTREACHED();
		break;

		// void WaitMessage(char* senderId, char** recMsg, Buffer** buffer);
		case SC_WaitMessage:
		{
			// Reading the SenderId
			int sendAddr = kernel->machine->ReadRegister(4);
			char sendId[IDMAX];
			int tmp;
			kernel->machine->ReadMem(sendAddr, 1, &tmp);
			if (tmp == NULL)
			{
				kernel->machine->ReadMem(sendAddr, 1, &tmp);
			}
			sendAddr++;

			int i = 0;
			while (tmp != 0)
			{
				sendId[i] = (char)tmp;
				kernel->machine->ReadMem(sendAddr, 1, &tmp);
				if (tmp == NULL)
				{
					kernel->machine->ReadMem(sendAddr, 1, &tmp);
				}
				sendAddr++;
				i++;
			}
			sendId[i] = '\0';

			// Read recMsg address
			int recMsgAddr = kernel->machine->ReadRegister(5);

			// Read buffer address
			Buffer *buffer;
			int tempBuff = kernel->machine->ReadRegister(6);

			// Traverse the bufferQueue to look for sender
			ListIterator<Buffer *> *iter = new ListIterator<Buffer *>(kernel->currentThread->bufferQueue);

			bool findFlag = false;
			for (; !iter->IsDone(); iter->Next())
			{
				if (!strcmp(iter->Item()->senderId, sendId))
				{
					findFlag = true;
					break;
				}
			}

			if (!findFlag)
			{
				// Sleep the current thread
				kernel->interrupt->SetLevel(IntOff);
				kernel->currentThread->Sleep(false);
				kernel->interrupt->SetLevel(IntOn);

				// Traverse the bufferQueue to look for sender
				ListIterator<Buffer *> *iter_ = new ListIterator<Buffer *>(kernel->currentThread->bufferQueue);

				for (; !iter_->IsDone(); iter_->Next())
				{
					if (!strcmp(iter_->Item()->senderId, sendId))
					{
						findFlag = true;
						break;
					}
				}
				iter = iter_;
			}

			// Assign recMsg and buffer
			buffer = iter->Item();

			// Write back buffer to the user program
			kernel->machine->WriteMem(tempBuff, 4, (int)buffer);

			for(int i = 0; i < MSGMAX; i++)
			{
				kernel->machine->WriteMem(recMsgAddr, 1, (int)(buffer->msg[i]));
				recMsgAddr++;
			}
			

			/* Modify return point */
			{
				/* set previous programm counter (debugging only)*/
				kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

				/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
				kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
				/* set next programm counter for brach execution */
				kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			}
		}
		return;
		ASSERTNOTREACHED();
		break;

		// void SendAnswer(int* result, char* ansMsg, Buffer* buffer); 
		case SC_SendAnswer: 
		{
			// Reading the ansMsg
			int ansMsgAddr = kernel->machine->ReadRegister(5);
			char ansMsg[MSGMAX];

			int tmp;
			kernel->machine->ReadMem(ansMsgAddr, 1, &tmp);
			if (tmp == NULL)
			{
				kernel->machine->ReadMem(ansMsgAddr, 1, &tmp);
			}
			ansMsgAddr++;

			int i = 0;
			while (tmp != 0)
			{
				ansMsg[i] = (char)tmp;
				kernel->machine->ReadMem(ansMsgAddr, 1, &tmp);
				if (tmp == NULL)
				{
					kernel->machine->ReadMem(ansMsgAddr, 1, &tmp);
				}
				ansMsgAddr++;
				i++;
			}
			ansMsg[i] = '\0';

			// Read buffer address	
			int tempBuff = kernel->machine->ReadRegister(6);
			Buffer *buffer = (Buffer*)tempBuff;

			// Find the sender thread* in the threadReg
			Thread *target;
			map<char*, Thread *>::iterator iter = kernel->scheduler->threadReg.begin();
			while(iter != kernel->scheduler->threadReg.end()){

				// Compare the Thread Name
				if(!strcmp(iter->first, buffer->senderId))
				{
					target = iter->second;
					break;
				}
				iter++;
			}

			if (iter == kernel->scheduler->threadReg.end())
			{
				cout << "\n Cannot find target thread.";
				exit(0);
			}

			// Construct buffer for answer
			buffer->msg = ansMsg;
			buffer->msgFlag = true;

			// Append buffer to target's bufferQueue
			target->bufferQueue->Append(buffer);

			// Remove buffer from current thread's bufferQueue
			kernel->currentThread->bufferQueue->Remove(buffer);

			// Activate target
			if (kernel->scheduler->sleepList->IsInList(target))
			{
				kernel->scheduler->sleepList->Remove(target);

				// ReadyToRun assumes that interrupts are disabled!
				kernel->interrupt->SetLevel(IntOff);
				kernel->scheduler->ReadyToRun(target);
				kernel->interrupt->SetLevel(IntOn);
			}

			/* Modify return point */
			{
				/* set previous programm counter (debugging only)*/
				kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

				/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
				kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

				/* set next programm counter for brach execution */
				kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			}
		}
		return;
		ASSERTNOTREACHED();
		break;

		// void WaitAnswer(int* result, char** ansMsg, Buffer* buffer);
		case SC_WaitAnswer:
		{
			// Read result
			int result = kernel->machine->ReadRegister(4);

			// Read ansMsg
			int ansMsg = kernel->machine->ReadRegister(5);

			// Read buffer address
			int tempBuff = kernel->machine->ReadRegister(6);
			Buffer *buffer = (Buffer*)tempBuff;

			ListIterator<Buffer *> *iter = new ListIterator<Buffer *>(kernel->currentThread->bufferQueue);

			bool findFlag = false;
			for (; !iter->IsDone(); iter->Next())
			{
				if (iter->Item() == buffer)
				{
					findFlag = true;
					break;
				}
			}

			if (!findFlag)
			{
				
				// Sleep the current thread
				kernel->interrupt->SetLevel(IntOff);
				kernel->currentThread->Sleep(false);
				kernel->interrupt->SetLevel(IntOn);
/*                               // Traverse the bufferQueue to look for sender
                                 ListIterator<Buffer *> *iter_ = new ListIterator<Buffer *>(kernel->currentThread->bufferQueue);
 
                                 for (; !iter_->IsDone(); iter_->Next())
                                 {
					 if (iter_->Item()->senderId == originalSenderId)
                                         {
                                                 findFlag = true;
                                                 break;
                                         }
                                 }
                                 iter = iter_;
				buffer = iter_->Item();
*/
			}

			// Assign the result and ansMsg
			if (buffer->dumFlag)
			{
				int tempRes = 1;
				kernel->machine->WriteMem(result, 4, tempRes);
			}
			for(int i = 0; i < MSGMAX; i++)
			{       
				kernel->machine->WriteMem(ansMsg, 1, (int)(buffer->msg[i]));
				ansMsg++;
			}
			//cout<<"usrProg1: Received answer: \n"<<endl;
			//cout<<"usrProg1: Answer from Prog2\n"<<endl;
			////
			// Remove buffer from current thread's bufferQueue
			kernel->currentThread->bufferQueue->Remove(buffer);

			// Reset usedBuffers
			int i = 0;
			for (; i < BUFFMAX; i++)
			{
				if (kernel->bufferPool[i] == buffer)
				{
				//	kernel->usedBuffers->Clear(i);////no need to delete from BufferPool, just mark as unused
					break;
				}
			}
			kernel->usedBuffers->Clear(i);
			

			/* Modify return point */
			{
				/* set previous programm counter (debugging only)*/
				kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

				/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
				kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

				/* set next programm counter for brach execution */
				kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			}
		}
		return;
		ASSERTNOTREACHED();
		break;

		case SC_Exit:
		{
			// traverse the exiting thead's bufferQueue and send dummy message
			ListIterator<Buffer *> *iter = new ListIterator<Buffer *>(kernel->currentThread->bufferQueue);

			Buffer *tempBuff;
			for (; !iter->IsDone(); iter->Next())
			{
				// set the dummy Buffer
				tempBuff = iter->Item();
				tempBuff->msgFlag = true;
				tempBuff->dumFlag = true;
				tempBuff->msg = "This is a dummy message\n";
                               
				// find the thread* of sender and apppend dummy buffer into its bufferQueue
				kernel->scheduler->threadReg[tempBuff->senderId]->bufferQueue->Append(tempBuff);

				// Activate target
				if (kernel->scheduler->sleepList->IsInList(kernel->scheduler->threadReg[tempBuff->senderId]))
				{
					kernel->scheduler->sleepList->Remove(kernel->scheduler->threadReg[tempBuff->senderId]);

					// ReadyToRun assumes that interrupts are disabled!
					kernel->interrupt->SetLevel(IntOff);
					kernel->scheduler->ReadyToRun(kernel->scheduler->threadReg[tempBuff->senderId]);
					kernel->interrupt->SetLevel(IntOn);
				}
			}

			kernel->currentThread->Finish();
		}
		break;

		default:
			cerr << "Unexpected system call " << type << "\n";
			break;
		}
		break;
	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
	ASSERTNOTREACHED();
}
