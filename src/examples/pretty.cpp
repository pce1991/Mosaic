
// Make something visually appealing and easily modifiable for demonstrations
// I want something that appeals to to programmers and also studends who might think programming isnt for them.

struct Quad {
    vec2 position;
    vec2 scale;
    real32 rotation;
    
    vec2 velocity;
    real32 angVelocity;

    real32 spawnTime;
    real32 duration;
    
    vec4 color;
    Sprite *sprite;
};

struct Particles {
    DynamicArray<Quad> quads;
};

Particles *pretty = NULL;

void MyInit() {
    Game->myData = (Particles *)malloc(sizeof(Particles));

    pretty = (Particles *)Game->myData;
    
    memset(pretty, 0, sizeof(Particles));

}

void MyGameUpdate() {

    if (InputPressed(Input, Input_Space)) {
        
        for (int i = 0; i < 100; i++) {
            Quad q = {};
            q.position = V2(0);
            q.velocity = V2(RandfRange(-5, 5), RandfRange(-5, 5));

            q.scale = V2(RandfRange(0.1f, 0.3f));

            q.rotation = RandfRange(0, 2 * _PI);
        
            q.angVelocity = RandfRange(0.1f, 0.3f);

            q.spawnTime = Game->time;
            q.duration = RandfRange(1.0f, 2.0f);

            q.color = V4(RandfRange(0, 1),
                         RandfRange(0, 1),
                         RandfRange(0, 1),
                         1.0f);

            PushBack(&pretty->quads, q);
        }
    }
    
    for (int i = 0; i < pretty->quads.count; i++) {
        Quad *q = &pretty->quads[i];

        q->position = q->position + q->velocity * Game->deltaTime;
        q->rotation = q->rotation + q->angVelocity * Game->deltaTime;

        DrawRect(q->position, q->scale, q->rotation, q->color);
    }

    // @TODO: functions that do different things to the quads when you press a key
    // RotateAroundPoint()
    // ShootAwayFromPoint(mouseCursor)
    // AttractToPoint()

    // Setup different keys to do different things.
    // Maybe add buttons you can click to spawn them.

    
}


