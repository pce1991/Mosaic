
int32 count = 0;

// if this is too low our first thread will just finish before we
// get a chance to. 
int32 const Capacity = 30000000;
int32 numbers[Capacity] = {};

#if 0
bool ProcAFinished = false;
int32 ProcATimesSet = 0;

bool ProcBFinished = false;
int32 ProcBTimesSet = 0;

// we don't necessarily have to pass in a pointer, but a thread
// requires that the proc could take one. 
void ProcA(void *data) {
  ProcAFinished = false;
  while (true) {
    if (count < Capacity) {
      numbers[count] = ProcATimesSet;
      count++;

      ProcATimesSet++;
    }
    else {
      ProcAFinished = true;
      break;
    }
  }
}

void ProcB(void *data) {
  while (true) {
    if (count < Capacity) {
      numbers[count] = ProcBTimesSet;
      count++;

      ProcBTimesSet++;
    }
    else {
      ProcBFinished = true;
      break;
    }
  }
}

void MyInit() {
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

  // this is just stalling our main thread until ProcA and ProcB
  // have both finished. 
  while (!ProcAFinished && !ProcBFinished) {
    continue;
  }

  for (int i = 0; i < Capacity; i++) {
    Print("%d) %d", i, numbers[i]);
  }

  Print("ProcA times set %d", ProcATimesSet);
  Print("ProcB times set %d", ProcBTimesSet);
}

void MyGameUpdate() {

}

#else

bool ProcCRunning = false;
bool ProcCFinished = false;

int32 TimesProcCSet = 0;

void ProcC(void *data) {
  while (true) {
    if (ProcCRunning) {
      if (count < Capacity) {
        numbers[count] = TimesProcCSet * count;
        count++;

        TimesProcCSet++;

        ProcCFinished = false;
      }
      else {
        ProcCFinished = true;
      }
    }
  }
}

void MyInit() {

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

  // As long as ProcC isn't running we know that it's okay to
  // touch the variable count because ProcC isn't using it. 
  if (!ProcCRunning) {
    if (InputPressed(Keyboard, Input_Space)) {
      Print("ProcC Started");
      count = 0;
      ProcCRunning = true;
      // only set this here so that the thread doens't set it
      // to false before our update detects that it's finished.
    }
  }

  // waiting for it to finish
  // if (ProcCRunning) {
  //   while (!ProcCFinished) {
  //     continue;
  //   }

  //   for (int i = 0; i < Capacity; i++) {
  //     Print("%d) %d", i, numbers[i]);
  //   }

  //   Print("ProcC Finished");
  //   ProcCRunning = false;
  // }

  // continuing while it runs
  if (ProcCRunning) {
    if (ProcCFinished) {
      // for (int i = 0; i < Capacity; i++) {
      //   Print("%d) %d", i, numbers[i]);
      // }

      Print("ProcC Finished");
      ProcCRunning = false;
    }
  }

  Print("Waiting around %f...", Time);
}


#endif

