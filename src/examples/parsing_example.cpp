
enum CreatureType {
    CreatureType_Buffalo,
    CreatureType_Seahorse,
};

struct Creature {
    CreatureType type;
    vec2i position;
};

struct MyData {
    DynamicArray<Creature> creatures;
};

MyData *Data = NULL;


enum MyTokenType {
  MyTokenType_Invalid,
  MyTokenType_Comma,
  MyTokenType_Int,
  MyTokenType_Identifier,
};

struct Token {
  MyTokenType type;
  char *start; // this is the pointer to
  // the start of the token in the file.

  int length; 
  // how far from the start we read.
};

bool IsAlpha(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

bool IsDigit(char c) {
    return c >= '0' && c <= '9';
}

void ParseFile() {
    char *path = "data/my_test_file.txt";

    FILE *file = fopen(path, "r");

    if (file == NULL) { return; }

    fseek(file, 0, SEEK_END);
    int32 length = ftell(file);
    fseek(file, 0, SEEK_SET);
    // @NOTE: length may be longer than the number of bytes actually in the file
    // because of "return-line feed translation"
    //https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/ftell-ftelli64?view=vs-2019
    //This is why we recalculate the length of the buffer
    
    
    char *buffer = (char *)malloc(length + 1);
    memset(buffer, 0, length);

    fread(buffer, 1, length, file);
    length = strlen(buffer);
    

    Print("%s", buffer);

    DynamicArray<Token> tokens = MakeDynamicArray<Token>(128);
    Token currToken = {};

    int32 offset = 0;
    int c = buffer[offset];
    while (offset < length) {

        while (c == ' ' || c == '\n' || c == '\r') {
            goto Advance;
        }
        
        if (c == ',') {
            currToken.type = MyTokenType_Comma;
            currToken.start = &buffer[offset];
            currToken.length = 1;
            
            goto AddToken;
        }

        while (IsDigit(c)) {
            if (currToken.start == NULL) {
                currToken.type = MyTokenType_Int;
                currToken.start = &buffer[offset];
            }
            
            currToken.length++;

            char next = buffer[offset + 1];
            if (!IsDigit(next)) {
                goto AddToken;
                break;
            }

            offset++;
            c = buffer[offset];
        }

        while (IsAlpha(c)) {
            if (currToken.start == NULL) {
                currToken.type = MyTokenType_Identifier;
                currToken.start = &buffer[offset];
            }
            
            currToken.length++;

            char next = buffer[offset + 1];
            if (!IsAlpha(next)) {
                goto AddToken;
                break;
            }
            
            offset++;
            c = buffer[offset];
        }

        // We want a label here so we can skip over once we've created our token.
        // This is to avoid having to add it in all of our checks and to simplify the control
        // flow where we guarantee we only do one token at a time.
    AddToken:
        PushBack(&tokens, currToken);
        currToken = {};

    Advance:
        offset++;
        c = buffer[offset];
    }

    Creature creature = {};
    
    int32 tokenIndex = 0;
    while (tokenIndex < tokens.count) {
        Token t = tokens[tokenIndex];

        if (t.type == MyTokenType_Identifier) {
            if (strncmp(t.start, "Buffalo", t.length) == 0) {
                creature.type = CreatureType_Buffalo;
            }
            if (strncmp(t.start, "Seahorse", t.length) == 0) {
                creature.type = CreatureType_Seahorse;
            }

            tokenIndex++;
            t = tokens[tokenIndex];
            if (t.type != MyTokenType_Comma) {
                Print("ERROR PARSING TOKENS, EXPECTED COMMA");
            }
        }

        tokenIndex++;
        t = tokens[tokenIndex];

        if (t.type == MyTokenType_Int) {
            creature.position.x = strtoll(t.start, NULL, 10);

            tokenIndex++;
            t = tokens[tokenIndex];
            if (t.type != MyTokenType_Comma) {
                Print("ERROR PARSING TOKENS, EXPECTED COMMA");
            }
        }

        tokenIndex++;
        t = tokens[tokenIndex];
        
        if (t.type == MyTokenType_Int) {
            creature.position.y = strtoll(t.start, NULL, 10);

            tokenIndex++;
            t = tokens[tokenIndex];
            if (t.type != MyTokenType_Comma) {
                Print("ERROR PARSING TOKENS, EXPECTED COMMA");
            }
        }

        PushBack(&Data->creatures, creature);
        creature = {};

        tokenIndex++;
        t = tokens[tokenIndex];
    }

    for (int i = 0; i < Data->creatures.count; i++) {
        Creature c = Data->creatures[i];

        Print("creature type %d, position %d %d", c.type, c.position.x, c.position.y);
    }
}

void MyInit() {
    Game->myData = malloc(sizeof(MyData));
    memset(Game->myData, 0, sizeof(MyData));

    Data = (MyData *)Game->myData;

    ParseFile();
}

void MyGameUpdate() {
    
}
