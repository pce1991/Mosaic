
void AllocateInputManager(InputManager *input, MemoryArena *arena, int32 capacity, int32 deviceCapacity) {
    input->events = MakeDynamicArray<InputEvent>(arena, capacity);

    input->deviceCount = deviceCapacity;
    input->devices = (InputDevice *)malloc(sizeof(InputDevice) * deviceCapacity);

    memset(input->devices, 0, input->deviceCount * sizeof(InputDevice));

    input->charSize = 32;
    input->charCount = 0;
    input->inputChars = (char *)malloc(input->charSize);
}

void AllocateInputDevice(InputDevice *device, InputDeviceType type, int32 discreteCount, int32 analogueCount) {
    device->type = type;
    device->discreteCount = discreteCount;
    
    device->framesHeld = (int32 *)malloc(sizeof(int32) * discreteCount);
    device->released = (bool *)malloc(sizeof(bool) * discreteCount);
    device->pressed = (bool *)malloc(sizeof(bool) * discreteCount);
    device->timePressed = (real32 *)malloc(sizeof(real32) * discreteCount);

    device->analogueCount = analogueCount;
    device->prevAnalogue = (real32 *)malloc(sizeof(real32) * analogueCount);
    device->analogue = (real32 *)malloc(sizeof(real32) * analogueCount);

    memset(device->framesHeld, -1, sizeof(int32) * discreteCount);
    memset(device->pressed, 0, sizeof(bool) * discreteCount);
    memset(device->released, 0, sizeof(bool) * discreteCount);
    memset(device->timePressed, 0, sizeof(real32) * discreteCount);

    memset(device->analogue, 0, sizeof(real32) * analogueCount);
    memset(device->prevAnalogue, 0, sizeof(real32) * analogueCount);
}

void PushInputEvent(InputManager *input, InputEvent e) {
    PushBack(&input->events, e);
}

void PushInputChar(InputManager *input, char c) {
    input->inputChars[input->charCount++] = c;
}

// User functions

bool InputPressed(InputDevice *device, int32 inputID) {
    return device->pressed[inputID] && device->framesHeld[inputID] == 0;
}

bool InputReleased(InputDevice *device, int32 inputID) {
    return device->released[inputID];
}

bool InputHeld(InputDevice *device, int32 inputID) {
    return device->framesHeld[inputID] > 0;
}

bool InputHeldSeconds(InputDevice *device, int32 inputID, real32 time) {
    bool held = device->framesHeld[inputID] > 0;

    return held && (Game->time - device->timePressed[inputID] > time);
}

// @NOTE: to be cleared at the end of the frame so we have access to inputChars thruout update. 
void ClearInputManager(InputManager *input) {
    DynamicArrayClear(&input->events);

    input->charCount = 0;
    memset(input->inputChars, 0, input->charSize);
}

void UpdateInput(InputManager *input) {

    input->mousePosNorm.x = input->mousePos.x / (Game->screenWidth * 1.0f);
    input->mousePosNorm.y = input->mousePos.y / (Game->screenHeight * 1.0f);

    // @DESIGN: this is exactly the type of equation its very useful to know!
    input->mousePosNormSigned.x = (input->mousePosNorm.x - 0.5f) * 2;
    input->mousePosNormSigned.y = (input->mousePosNorm.y - 0.5f) * 2;

    for (int d = 0; d < input->deviceCount; d++) {
        InputDevice *device = &input->devices[d];

        if (device->type == InputDeviceType_Invalid) { continue; }
        
        for (int i = 0; i < device->discreteCount; i++) {
            device->released[i] = false;
            
            // @NOTE: until we get a release event we consider a key to be pressed
            if (device->framesHeld[i] >= 0) {
                device->framesHeld[i]++;
                device->pressed[i] = false;
            }
        }
    }
    
    for (int i = 0; i < input->events.count; i++) {
        InputEvent event = input->events[i];
        int32 index = event.index;
        
        //printf("event %d %d frames %d\n", i, event.release, input->framesHeld[input]);

        //InputDevice *device = &input->devices[event.deviceID];
        InputDevice *device = event.device;

        if (!event.discreteValue) {
            if (device->framesHeld[index] > 0) {
                device->released[index] = true;
            }
            
            device->timePressed[index] = -1;
            device->framesHeld[index] = -1;
            device->pressed[index] = false;
        }
        else {
            if (device->framesHeld[index] < 0) {
                //printf("pressed\n");

                device->timePressed[index] = Game->time;
                device->framesHeld[index] = 0;
                device->pressed[index] = true;
                device->released[index] = false;
            }
            else {
                // @NOTE: we shouldnt even get a pressed event when the key is being held
            }
        }
    }
}
