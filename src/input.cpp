
InputQueue AllocateInputQueue(int32 capacity, int32 deviceCount) {
    InputQueue queue;

    queue.count = 0;
    queue.capacity = capacity;
    queue.size = sizeof(InputEvent) * capacity;
    queue.events = (InputEvent *)malloc(queue.size);
    memset(queue.events, 0, queue.size);

    queue.deviceCount = deviceCount;
    queue.deviceStates = (InputDeviceState *)malloc(sizeof(InputDeviceState) * deviceCount);
    
    for (int i = 0; i < deviceCount; i++) {
        InputDeviceState *deviceState = &queue.deviceStates[i];
        
        deviceState->framesHeld = (int32 *)malloc(sizeof(int32) * Input_Count);
        deviceState->released = (bool *)malloc(sizeof(bool) * Input_Count);
        deviceState->pressed = (bool *)malloc(sizeof(bool) * Input_Count);
        deviceState->timePressed = (real32 *)malloc(sizeof(real32) * Input_Count);

        memset(deviceState->framesHeld, -1, sizeof(int32) * Input_Count);
        memset(deviceState->pressed, 0, sizeof(bool) * Input_Count);
        memset(deviceState->released, 0, sizeof(bool) * Input_Count);
        memset(deviceState->timePressed, 0, sizeof(real32) * Input_Count);
    }

    queue.charSize = 32;
    queue.charCount = 0;
    queue.inputChars = (char *)malloc(queue.charSize);
    
    return queue;
}

void PushInputPress(InputQueue *queue, InputID input, int32 deviceID = 0) {
    // @WARNING: this could go out of bounds of our input buffer
    InputEvent e;
    e.input = input;
    e.release = false;
    e.deviceID = deviceID;
    queue->events[queue->count++] = e;
}

void PushInputRelease(InputQueue *queue, InputID input, int32 deviceID = 0) {
    InputEvent e;
    e.input = input;
    e.release = true;
    e.deviceID = deviceID;
    queue->events[queue->count++] = e;
}

void PushInputChar(InputQueue *queue, char c) {
    queue->inputChars[queue->charCount++] = c;
}

// User functions
bool InputPressed(InputQueue *queue, InputID input, int32 deviceID = 0) {
    return queue->deviceStates[deviceID].pressed[input];
}

bool InputReleased(InputQueue *queue, InputID input, int32 deviceID = 0) {
    return queue->deviceStates[deviceID].released[input];
}

bool InputHeld(InputQueue *queue, InputID input, int32 deviceID = 0) {
    return queue->deviceStates[deviceID].framesHeld[input] > 0;
}

bool InputHeldSeconds(InputQueue *queue, InputID input, real32 time, int32 deviceID = 0) {
    bool held = queue->deviceStates[deviceID].framesHeld[input] > 0;

    return held && (Game->time - queue->deviceStates[deviceID].timePressed[input] > time);
}


bool InputPressed(InputID input, int32 deviceID = 0) {
    return Input->deviceStates[deviceID].pressed[input];
}

bool InputReleased(InputID input, int32 deviceID = 0) {
    return Input->deviceStates[deviceID].released[input];
}

bool InputHeld(InputID input, int32 deviceID = 0) {
    return Input->deviceStates[deviceID].framesHeld[input] > 0;
}

bool InputHeldSeconds(InputID input, real32 time, int32 deviceID = 0) {
    bool held = Input->deviceStates[deviceID].framesHeld[input] > 0;

    return held && (Game->time - Input->deviceStates[deviceID].timePressed[input] > time);
}


// @NOTE: to be cleared at the end of the frame so we have access to inputChars thruout update. 
void ClearInputQueue(InputQueue *queue) {
    queue->count = 0;
    memset(queue->events, 0, queue->size);

    queue->charCount = 0;
    memset(queue->inputChars, 0, queue->charSize);
}

void UpdateInput(InputQueue *queue) {

    queue->mousePosNorm.x = queue->mousePos.x / (Game->screenWidth * 1.0f);
    queue->mousePosNorm.y = queue->mousePos.y / (Game->screenHeight * 1.0f);

    // @DESIGN: this is exactly the type of equation its very useful to know!
    queue->mousePosNormSigned.x = (queue->mousePosNorm.x - 0.5f) * 2;
    queue->mousePosNormSigned.y = (queue->mousePosNorm.y - 0.5f) * 2;

    for (int d = 0; d < queue->deviceCount; d++) {
        InputDeviceState *deviceState = &queue->deviceStates[d];
        
        for (int i = 0; i < Input_Count; i++) {
            deviceState->released[i] = false;
            
            // @NOTE: until we get a release event we consider a key to be pressed
            if (deviceState->framesHeld[i] >= 0) {
                deviceState->framesHeld[i]++;
                deviceState->pressed[i] = false;
            }
        }
    }
    
    for (int i = 0; i < queue->count; i++) {
        InputEvent event = queue->events[i];
        InputID input = event.input;
        

        //printf("event %d %d frames %d\n", i, event.release, queue->framesHeld[input]);

        InputDeviceState *deviceState = &queue->deviceStates[event.deviceID];

        if (event.release) {
            deviceState->timePressed[input] = -1;
            deviceState->framesHeld[input] = -1;
            deviceState->pressed[input] = false;
            deviceState->released[input] = true;
        }
        else {
            if (deviceState->framesHeld[input] < 0) {
                //printf("pressed\n");

                deviceState->timePressed[input] = Game->time;
                deviceState->framesHeld[input] = 0;
                deviceState->pressed[input] = true;
                deviceState->released[input] = false;
            }
            else {
                // @NOTE: we shouldnt even get a pressed event when the key is being held
            }
        }
    }
}
