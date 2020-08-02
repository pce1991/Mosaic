
// @TODO: allow strings longer than one line of text!

struct Typing {
    int32 documentLength;
    char *document;

    DynamicArray<Str> strings;

    Str inputString;

    real32 timeStarted;
    real32 timeSubmitted;

    int32 stringIndex;
    int32 charIndex;
    int32 score;
    int32 maxScore;
};

Typing *typing = NULL;

void MyInit() {
    Game->myData = malloc(sizeof(Typing));
    memset(Game->myData, 0, sizeof(Typing));
    
    typing = (Typing *)Game->myData;

    typing->inputString.string = (char *)malloc(256);
    typing->inputString.length = 0;

    typing->stringIndex = -1;
    
    FILE *file = fopen("data/examples/typing_game/words.txt", "r");

    // Copy over the file into document because I dont want to keep the file handle open when runnig.

    if (file != NULL) {
        int c = fgetc(file);

        while (c != EOF) {
            if (c != '\r') {
                typing->documentLength++;
            }
            c = fgetc(file);
        }
    }

    typing->document = (char *)malloc(typing->documentLength);

    fseek(file, 0, SEEK_SET);
    for (int i = 0; i < typing->documentLength; i++) {
        int c = fgetc(file);

        if (c == '\r') {
            continue;
        }

        typing->document[i] = c;
    }
    
    fclose(file);

    Str currStr = {};
    
    for (int i = 0; i < typing->documentLength; i++) {
        char c = typing->document[i];

        char cPrev = 0;
        if (i > 0) {
            cPrev = typing->document[i - 1];
        }

        if (c == '\n' && cPrev == '\n') {
            PushBack(&typing->strings, currStr);
            currStr.length = 0;
        }
        else {
            // @TODO: make sure its a letter
            if (currStr.length == 0) {
                currStr.string = &typing->document[i];
            }

            if (c != '\n') {
                currStr.length++;    
            }
        }
    }
}

void MyGameUpdate() {

    if (typing->stringIndex < 0) {
        if (typing->timeSubmitted > 0) {
            r32 timeElapsed = typing->timeSubmitted - typing->timeStarted;
            DrawTextScreen(&Game->monoFont, V2(800, 100), 24, V4(1), true, "last time %.2f", timeElapsed);

            DrawTextScreen(&Game->monoFont, V2(800, 180), 24, V4(1), true, "score %d/%d %.1f", typing->score, typing->maxScore, 100.0f * (typing->score / (1.0f * typing->maxScore)));
        }
        
        DrawTextScreen(&Game->monoFont, V2(400, 400), 24, V4(1), false, "Press any key to begin");

        typing->inputString.length = 0;
        memset(typing->inputString.string, 0, 256);

        if (Input->charCount > 0) {
            typing->timeStarted = Game->time;

            typing->stringIndex = RandiRange(0, typing->strings.count);
        }
    }
    else {
        Str *inputStr = &typing->inputString;
        Str currStr = typing->strings[typing->stringIndex];
        
        if (InputPressed(Input, Input_Return)) {
            typing->timeSubmitted = Game->time;
            typing->stringIndex = -1;

            typing->score = 0;

            for (int i = 0; i < currStr.length; i++) {
                if (inputStr->string[i] == currStr.string[i]) {
                    typing->score++;
                }
            }

            typing->maxScore = currStr.length;
        }
        else if (InputPressed(Input, Input_Backspace)) {
            if (typing->inputString.length > 0) {
                typing->inputString.string[--typing->inputString.length] = 0;
            }
        }
        // We do this in an else if because we dont want to insert any of the non printable characters
        // into the string.
        else if (Input->charCount > 0) {
            typing->inputString.length++;
            typing->inputString.string[typing->inputString.length - 1] = Input->inputChars[0];
        }

        r32 timeElapsed = Game->time - typing->timeStarted;
        DrawTextScreen(&Game->monoFont, V2(800, 100), 24, V4(1), true, "%.2f", timeElapsed);
    
        
        DrawTextScreen(&Game->monoFont, V2(400, 400), 24, V4(1), false, "%.*s", currStr.length, currStr.string);


        DrawTextScreen(&Game->monoFont, V2(400, 500), 24, V4(1), false, "%.*s", inputStr->length, inputStr->string);

        // This is a pretty hacked in version. For a more robust implementation look at LayoutGlyphs
        vec2 cursorPos = V2(400, 500);
        for (int i = 0; i < inputStr->length; i++) {
            char c = inputStr->string[i];
            int32 codepoint = c - 32;
            cursorPos.x += Game->monoFont.glyphs[codepoint].xAdvance * 24.0f * Game->monoFont.emSize;
        }

        DrawRectScreen(cursorPos, V2(24.0f, 48.0f), V4(0.5f, 0.5f, 0.5f, 0.5f));        
    }
}
