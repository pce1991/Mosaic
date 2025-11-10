 
MemoryArena arena = {};

struct Boid {
  vec2 position;
  vec2 velocity;
  float32 rotation;
  vec4 color;

  vec2 target;
};

DynamicArray<Boid> boids;

bool BoidsProcAFinished = false;
bool BoidsProcAStart = false;

bool BoidsProcBFinished = false;
bool BoidsProcBStart = false;

void UpdateBoidsA(void *data) {
  while (true) {
    if (BoidsProcAStart) {
      
      float32 dist = 0;
      vec2 dir = {};
      for (int32 i = 0; i < boids.count / 2; i++) {
        dist = Distance(boids[i].position, boids[i].target);

        if (dist < 0.1f) {
          boids[i].target.x = RandfRange(-8, 8);
          boids[i].target.y = RandfRange(-4, 4);

          dir = Normalize(boids[i].target - boids[i].position);
          boids[i].velocity = dir * RandfRange(0.1f, 0.3f);
        }
        
        boids[i].position = boids[i].position + boids[i].velocity * DeltaTime;
      }

      BoidsProcAStart = false;
      BoidsProcAFinished = true;
    }
  }
}

void UpdateBoidsB(void *data) {
  while (true) {
    if (BoidsProcBStart) {
      float32 dist = 0;
      vec2 dir = {};
      for (int32 i = boids.count / 2; i < boids.count; i++) {
        dist = Distance(boids[i].position, boids[i].target);

        if (dist < 0.1f) {
          boids[i].target.x = RandfRange(-8, 8);
          boids[i].target.y = RandfRange(-4, 4);

          dir = Normalize(boids[i].target - boids[i].position);
          boids[i].velocity = dir * RandfRange(0.1f, 0.3f);
        }
        
        boids[i].position = boids[i].position + boids[i].velocity * DeltaTime;
      }

      BoidsProcBStart = false;
      BoidsProcBFinished = true;
    }
  }
}

void MyInit() {
  AllocateMemoryArena(&arena, Megabytes(8));

  {
    DWORD threadID;
    HANDLE threadHandle =
      CreateThread(0,
                   0,
                   (LPTHREAD_START_ROUTINE)UpdateBoidsA,
                   NULL,
                   0,
                   &threadID);
    //CloseHandle(threadHandle);
  }

  {
    DWORD threadID;
    HANDLE threadHandle =
      CreateThread(0,
                   0,
                   (LPTHREAD_START_ROUTINE)UpdateBoidsB,
                   NULL,
                   0,
                   &threadID);
    //CloseHandle(threadHandle);
  }

  boids = MakeDynamicArray<Boid>(&arena, 10000);

  for (int i = 0; i < 10000; i++) {
    Boid boid = {};
    boid.position.x = RandfRange(-8, 8);
    boid.position.y = RandfRange(-4, 4);

    boid.target.x = RandfRange(-8, 8);
    boid.target.y = RandfRange(-4, 4);

    boid.color.r = RandfRange(0.1f, 0.3f);
    boid.color.g = RandfRange(0.3f, 0.5f);
    boid.color.b = RandfRange(0.5f, 0.9f);
    boid.color.a = 1;

    boid.rotation = RandfRange(0.0f, _2PI);

    vec2 dir = Normalize(boid.target - boid.position);
    boid.velocity = dir * RandfRange(0.1f, 0.3f);
    //boid.velocity = RandDir() * RandfRange(0.2f, 0.32f);

    PushBack(&boids, boid);
  }
}

void MyGameUpdate() {
  Print("%f", DeltaTime);

#if 1
  BoidsProcAStart = true;
  BoidsProcBStart = true;
  BoidsProcAFinished = false;
  BoidsProcBFinished = false;
      

  while (!BoidsProcAFinished && !BoidsProcBFinished) {
    continue;
  }
  
#else
  for (int i = 0; i < boids.count; i++) {
    boids[i].position = boids[i].position + boids[i].velocity * DeltaTime;
  }
#endif

  
  
  vec2 size = V2(0.05f, 0.05f);
  vec4 color = V4(1, 1, 1, 1);
  for (int i = 0; i < boids.count; i++) {
    DrawRect(&Game->rectBuffer, boids[i].position, size, boids[i].rotation, boids[i].color);
  }
}

