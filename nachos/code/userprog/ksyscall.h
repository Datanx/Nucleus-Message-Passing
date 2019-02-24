/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__ 
#define __USERPROG_KSYSCALL_H__ 

#include "kernel.h"




void SysHalt()
{
  kernel->interrupt->Halt();
}


int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

// void SysSendMessage(std::string receiverId, std::string sndMsg, Buffer** buffer)   ////
// {
//   // reading the receiverId
//   int recvAddr = (int)kernel->machine->ReadRegister(4);
//   string recvId = "";
//   int tmp;
//   kernel->machine->ReadMem(recvAddr, 1, &tmp);
//   if (tmp == NULL)
//   {
//     kernel->machine->ReadMem(recvAddr, 1, &tmp);
//   }
//   recvAddr++;
//   while (tmp != 0)
//   {
//     recvId += (char)tmp;
//     kernel->machine->ReadMem(recvAddr, 1, &tmp);
//     if (tmp == NULL)
//     {
//       kernel->machine->ReadMem(recvAddr, 1, &tmp);
//     }
//     recvAddr++;
//   }

//   // reading the sndMsg
//   int sndMsgAddr = (int)kernel->machine->ReadRegister(5);
//   string sndMsg = "";

//   kernel->machine->ReadMem(sndMsgAddr, 1, &tmp);
//   if (tmp == NULL)
//   {
//     kernel->machine->ReadMem(sndMsgAddr, 1, &tmp);
//   }
//   sndMsgAddr++;
//   while (tmp != 0)
//   {
//     sndMsg += (char)tmp;
//     kernel->machine->ReadMem(sndMsgAddr, 1, &tmp);
//     if (tmp == NULL)
//     {
//       kernel->machine->ReadMem(sndMsgAddr, 1, &tmp);
//     }
//     sndMsgAddr++;
//   }

//   // read buffer address

//   Buffer **buffer = (Buffer **)kernel->machine->ReadRegister(6);

//   // assign the buffer address
//   ASSERT(kernel->currentThread->commCnt <= MaxComm);
//   int poolNo = kernel->usedBuffers->FindAndSet();       // first empty buffer
//   if (poolNo != -1)
//   {
//     kernel->currentThread->commCnt++;
//     *buffer = kernel->bufferPool[poolNo];
//   }

//   // check and get the receiver
//   // char recvName[20];
//   // strcpy(recvName, recvId.c_str());

//   Thread *target;           // communication target
//   map<string, Thread *>::iterator iter = kernel->scheduler->threadReg.find(recvId);
//   if (iter == kernel->scheduler->threadReg.end())
//   {
//     cout << "\n Cannot find target thread.";
//     exit(0);
//   }
//   else
//   {
//     target = iter->second;
//   }

//   // assemble buffer and append to receiver's bufferQueue
//   (*buffer)->senderId = kernel->currentThread->tId;
//   (*buffer)->receiverId = recvId;
//   (*buffer)->msg = sndMsg;
  

//   target->bufferQueue->Append(*buffer);

//   // activate the receiver
//   if (kernel->scheduler->sleepList->IsInList(target))
//   {
//     kernel->scheduler->sleepList->Remove(target);

//     // ReadyToRun assumes that interrupts are disabled!
//     kernel->interrupt->SetLevel(IntOff);
//     kernel->scheduler->ReadyToRun(target);
//     kernel->interrupt->SetLevel(IntOn);
//   }
// }

// void SysWaitMessage()   ////
// {
//   // reading the SenderId
//   int sendAddr = (int)kernel->machine->ReadRegister(4);
//   string sendId = "";
//   int tmp;
//   kernel->machine->ReadMem(sendAddr, 1, &tmp);
//   if (tmp == NULL)
//   {
//     kernel->machine->ReadMem(sendAddr, 1, &tmp);
//   }
//   sendAddr++;
//   while (tmp != 0)
//   {
//     sendId += (char)tmp;
//     kernel->machine->ReadMem(sendAddr, 1, &tmp);
//     if (tmp == NULL)
//     {
//       kernel->machine->ReadMem(sendAddr, 1, &tmp);
//     }
//     sendAddr++;
//   }

//   // read recMsg address
//   string* recMsgAddr = (string*)kernel->machine->ReadRegister(5);

//   // read buffer address
//   Buffer **buffer = (Buffer **)kernel->machine->ReadRegister(6);

//   // traverse the bufferQueue to look for sender
// 	ListIterator<Buffer*> *iter(kernel->currentThread->bufferQueue); 

//   bool findFlag = false;
// 	for (; !iter->IsDone(); iter->Next()) {
//     if(iter->Item()->senderId == sendId)
//     {
//       findFlag = true;
//       break;
//     }
//   }

//   if(!findFlag)
//   {
//     // sleep the current thread
//     kernel->interrupt->SetLevel(IntOff);
//     kernel->currentThread->Sleep(false);
//     kernel->interrupt->SetLevel(IntOn);
//   }

//   // assign recMsg and *buffer
//   *buffer = iter->Item();
//   *recMsgAddr = (*buffer)->msg;

// }

// void SysSendAnswer(bool& result, std::string ansMsg, Buffer** buffer)           ////
// {
//   // reading the ansMsg
//   int ansMsgAddr = (int)kernel->machine->ReadRegister(5);
//   string ansMsg = "";

//   int tmp;
//   kernel->machine->ReadMem(ansMsgAddr, 1, &tmp);
//   if (tmp == NULL)
//   {
//     kernel->machine->ReadMem(ansMsgAddr, 1, &tmp);
//   }
//   ansMsgAddr++;
//   while (tmp != 0)
//   {
//     ansMsg += (char)tmp;
//     kernel->machine->ReadMem(ansMsgAddr, 1, &tmp);
//     if (tmp == NULL)
//     {
//       kernel->machine->ReadMem(ansMsgAddr, 1, &tmp);
//     }
//     ansMsgAddr++;
//   }

//   // read buffer address
//   Buffer **buffer = (Buffer **)kernel->machine->ReadRegister(6);

//   // find the sender thread* in the threadReg
//   Thread* target;
//   map<string, Thread *>::iterator iter = kernel->scheduler->threadReg.find((*buffer)->senderId);
//   if (iter == kernel->scheduler->threadReg.end())
//   {
//     cout << "\n Cannot find target thread.";
//     exit(0);
//   }
//   else
//   {
//     target = iter->second;
//   }

//   // construct buffer for answer
//   (*buffer)->msg = ansMsg;
//   (*buffer)->msgFlag = true;

//   // Append buffer to target's bufferQueue
//   target->bufferQueue->Append(*buffer);

//   // Remove buffer from current thread's bufferQueue
//   kernel->currentThread->bufferQueue->Remove(*buffer);

//   // Activate target
//   if (kernel->scheduler->sleepList->IsInList(target))
//   {
//     kernel->scheduler->sleepList->Remove(target);

//     // ReadyToRun assumes that interrupts are disabled!
//     kernel->interrupt->SetLevel(IntOff);
//     kernel->scheduler->ReadyToRun(target);
//     kernel->interrupt->SetLevel(IntOn);
//   }
// }

// void SysWaitAnswer(bool* result, std::string* ansMsg, Buffer** buffer)          ////
// {
//   // read result
//   bool* result  = (bool*)kernel->machine->ReadRegister(4);

//   // read ansMsg
//   string* ansMsg  = (string*)kernel->machine->ReadRegister(5);

//   // read buffer address
//   Buffer **buffer = (Buffer **)kernel->machine->ReadRegister(6);

//   // traverse the sender's bufferQueue to look for *buffer
// 	ListIterator<Buffer*> *iter(kernel->currentThread->bufferQueue); 

//   bool findFlag = false;
// 	for (; !iter->IsDone(); iter->Next()) {
//     if(iter->Item() == *buffer)
//     {
//       findFlag = true;
//       break;
//     }
//   }

//   if(!findFlag)
//   {
//     // sleep the current thread
//     kernel->interrupt->SetLevel(IntOff);
//     kernel->currentThread->Sleep(false);
//     kernel->interrupt->SetLevel(IntOn);
//   }

//   // assign the result and ansMsg
//   if ((*buffer)->dumFlag) *result = true;
//   *ansMsg = (*buffer)->msg;

//   // Remove buffer from current thread's bufferQueue
//   kernel->currentThread->bufferQueue->Remove(*buffer);

//   // reset usedBuffers
//   int i = 0;
//   for(; i < Max; i++)
//   {
//     if(kernel->bufferPool[i] == *buffer)
//     {
//       break;
//     }
//   }
//   kernel->usedBuffers->Clear(i);
// }

// void Exit()
// {
//   // traverse the exiting thead's bufferQueue and send dummy message
// 	ListIterator<Buffer*> *iter(kernel->currentThread->bufferQueue); 

//   Buffer* tempBuff;
// 	for (; !iter->IsDone(); iter->Next()) {
//     // set the dummy Buffer
//     tempBuff = iter->Item();
//     tempBuff->msgFlag = true;
//     tempBuff->dumFlag = true;
//     tempBuff->msg = "Dummy Message: " + tempBuff->receiverId + " has exited!";

//     // find the thread* of sender and apppend dummy buffer into its bufferQueue
//     kernel->scheduler->threadReg[tempBuff->senderId]->bufferQueue->Append(tempBuff);
//   }

//   kernel->currentThread->Finish();
// }






#endif /* ! __USERPROG_KSYSCALL_H__ */
