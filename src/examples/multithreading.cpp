int32 count = 0;

int32 const Capacity = 100000;
int32 numbers[Capacity] = {};

#if 0
// DEMO: RACE CONDITION
// Make sure that there is no competition over reading/writing data on different threads
// Separate read/write permissions
// use synchornization/locking primitives to prevent contested resources
// Ways to split up read/write permissions that avoid locking
//   - just don't give them the same stuff obviously
//   - give them different regions of stuff
// Times that you need/want to lock

int32 TimesProcASet = 1;
int32 TimesProcBSet = 1;

HANDLE workSemaphore;
HANDLE doneSemaphore;

void ProcA(void *data) {
  while (true) {
    WaitForSingleObject(workSemaphore, INFINITE);
    while (count < Capacity) {
      numbers[count] = TimesProcASet;
      count++;
      TimesProcASet++;
      ThreadYield();
    }
    ReleaseSemaphore(doneSemaphore, 1, NULL);
  }
}

void ProcB(void *data) {
  while (true) {
    WaitForSingleObject(workSemaphore, INFINITE);
    while (count < Capacity) {
      numbers[count] = TimesProcBSet;
      count++;
      TimesProcBSet++;
      ThreadYield();
    }
    ReleaseSemaphore(doneSemaphore, 1, NULL);
  }
}

void MyGameInit() {
  workSemaphore = CreateSemaphore(NULL, 0, 2, NULL);
  doneSemaphore = CreateSemaphore(NULL, 0, 2, NULL);

  {
    DWORD threadID;
    HANDLE threadHandle =
      CreateThread(0,
                   0,
                   (LPTHREAD_START_ROUTINE)ProcA,
                   NULL,
                   0,
                   &threadID);
    CloseHandle(threadHandle);
  }

  {
    DWORD threadID;
    HANDLE threadHandle =
      CreateThread(0,
                   0,
                   (LPTHREAD_START_ROUTINE)ProcB,
                   NULL,
                   0,
                   &threadID);
    CloseHandle(threadHandle);
  }

  ReleaseSemaphore(workSemaphore, 1, NULL);
  ReleaseSemaphore(workSemaphore, 1, NULL);

  Print("Waiting for the work to get done.");

  WaitForSingleObject(doneSemaphore, INFINITE);
  WaitForSingleObject(doneSemaphore, INFINITE);

  int32 unwritten = 0;
  for (int i = 0; i < Capacity; i++) {
    if (numbers[i] == 0) {
      unwritten++;
    }
  }

  Print("ProcA set %d, ProcB set %d", TimesProcASet, TimesProcBSet);
  Print("sum %d of %d -> %d increments lost", TimesProcASet + TimesProcBSet, Capacity, Capacity - (TimesProcASet + TimesProcBSet));
  Print("%d slots never written (both threads grabbed the same slot)", unwritten);
}

void MyGameUpdate() {

}

#elif 0
// DEMO: SIGNALING WHEN A WORKER IS DONE

int32 TimesProcCSet = 0;

HANDLE procCStartSemaphore;
HANDLE procCDoneSemaphore;

void ProcC(void *data) {
  while (true) {
    WaitForSingleObject(procCStartSemaphore, INFINITE);

    count = 0;
    while (count < Capacity) {
      numbers[count] = TimesProcCSet * count;
      count++;
      TimesProcCSet++;
    }

    ReleaseSemaphore(procCDoneSemaphore, 1, NULL);
  }
}

void MyGameInit() {
  procCStartSemaphore = CreateSemaphore(NULL, 0, 1, NULL);
  procCDoneSemaphore = CreateSemaphore(NULL, 0, 1, NULL);

  {
    DWORD threadID;
    HANDLE threadHandle =
      CreateThread(0,
                   0,
                   (LPTHREAD_START_ROUTINE)ProcC,
                   NULL,
                   0,
                   &threadID);
    CloseHandle(threadHandle);
  }
}

void MyGameUpdate() {
  if (InputPressed(Keyboard, Input_Space)) {
    Print("ProcC Started");
    ReleaseSemaphore(procCStartSemaphore, 1, NULL);
  }

  if (WaitForSingleObject(procCDoneSemaphore, 0) == WAIT_OBJECT_0) {
    Print("ProcC Finished");
  }

  Print("Waiting around %f...", Time);
}

#else
HANDLE workSemaphore;
HANDLE doneSemaphore;

int32 countA = 0;
int32 countB = 0;

void ProcA(void *data) {
  while (true) {
    WaitForSingleObject(workSemaphore, INFINITE);
    while (countA < Capacity / 2) {
      numbers[countA] = ++countA;
      ThreadYield();
    }
    ReleaseSemaphore(doneSemaphore, 1, NULL);
  }
}

void ProcB(void *data) {
  while (true) {
    WaitForSingleObject(workSemaphore, INFINITE);
    int32 startIndex = Capacity / 2;

    while (countB < Capacity / 2) {
      numbers[countB + startIndex] = ++countB;
      ThreadYield();
    }
    ReleaseSemaphore(doneSemaphore, 1, NULL);
  }
}

void MyGameInit() {
  workSemaphore = CreateSemaphore(NULL, 0, 2, NULL);
  doneSemaphore = CreateSemaphore(NULL, 0, 2, NULL);

  {
    DWORD threadID;
    HANDLE threadHandle =
      CreateThread(0,
                   0,
                   (LPTHREAD_START_ROUTINE)ProcA,
                   NULL,
                   0,
                   &threadID);
    CloseHandle(threadHandle);
  }

  {
    DWORD threadID;
    HANDLE threadHandle =
      CreateThread(0,
                   0,
                   (LPTHREAD_START_ROUTINE)ProcB,
                   NULL,
                   0,
                   &threadID);
    CloseHandle(threadHandle);
  }

  ReleaseSemaphore(workSemaphore, 1, NULL);
  ReleaseSemaphore(workSemaphore, 1, NULL);

  Print("Waiting for the work to get done.");

  WaitForSingleObject(doneSemaphore, INFINITE);
  WaitForSingleObject(doneSemaphore, INFINITE);

  int32 unwritten = 0;
  for (int i = 0; i < Capacity; i++) {
    if (numbers[i] == 0) {
      unwritten++;
    }
  }

  Print("%d slots never written (both threads grabbed the same slot)", unwritten);
}

void MyGameUpdate() {

}
#endif
