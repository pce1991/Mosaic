
MemoryArena arena = {};
DynamicArray<Token> tokens;

enum TokenType {
    TokenType_EntityType,
    TokenType_LeftParen,
    TokenType_RightParen,
    TokenType_Float,
};

void MyMosaicInit() {
    AllocateMemoryArena(&arena, Megabytes(8));
    FileHandle file = {};
    OpenFileForRead("data/file_parse_example.txt", &file, &arena);

    tokens = MakeDynamicArray<Token>(&arena, 8);

    // stage 1 is to tokenize
    while (file.offset < file.size) {
        Token t = {};

        ConsumeBytesPassing(&file, IsWhitespace);

        t.start = &((char *)file.data)[file.offset];
        t.length = 1;

        if (ConsumeIdentifierToken(&file, &t.start, &t.length)) {
            t.type = TokenType_EntityType;
        }
        else if (ConsumeByteMatching(&file, '(')) {
            t.type = TokenType_LeftParen;
        }
        else if (ConsumeByteMatching(&file, ')')) {
            t.type = TokenType_RightParen;
        }
        else if (ConsumeFloatLiteral(&file, &t.start, &t.length)) {
            t.type = TokenType_Float;
        }
        else {
            Print("ERROR");
        }

        PushBack(&tokens, t);

        // do this at the end to eat up any trailing whitespace
        ConsumeBytesPassing(&file, IsWhitespace);
    }

    int32 tokenIndex = 0;

    while (tokenIndex < tokens.count) {
        Token t = tokens[tokenIndex];

        Print("%d) %.*s", tokenIndex, t.length, t.start);

        tokenIndex++;
    }
}

void MyMosaicUpdate() {
    
}
