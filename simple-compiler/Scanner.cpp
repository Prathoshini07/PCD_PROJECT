#include "Scanner.h"

ScannerClass::ScannerClass(std::string input)
    : mFileName(input), mLineNumber(1)
{
    MSG("Initializing ScannerClass constructor");
    mFin.open(mFileName, std::ios::binary);
    if (!mFin.is_open())
    {
        std::cout << "Could not open: " << input << std::endl;
        exit(EXIT_FAILURE);
    }
}

ScannerClass::~ScannerClass()
{
    MSG("Closing the file stream");
    mFin.close();
}

int ScannerClass::getLineNumber()
{
    return mLineNumber;
}

Token ScannerClass::getNextToken()
{
    StateMachine sm = StateMachine();
    MachineState state;
    char next;
    TokenType type = BAD_TOKEN;
    std::string lexeme = "";

    // Ignore whitespace and check EOF before processing tokens
    do {
        next = this->mFin.get();
        if (mFin.eof()) return Token(ENDFILE_TOKEN, "EOF");  
        if (next == '\n') mLineNumber++;
    } while (isspace(next));

    // DEBUG: Track the character being processed
    std::cout << "Processing character: " << next << std::endl;

    // Check if we are in a valid state
    state = sm.UpdateState(next, type);
    std::cout << "State after update: " << state << std::endl;

    if (isalpha(next)) 
    {
        lexeme += next;
        while (isalnum(this->mFin.peek()) || this->mFin.peek() == '_') 
        {
            lexeme += this->mFin.get();
        }

        // Reserved keywords
        if (lexeme == "void") type = VOID_TOKEN;
        else if (lexeme == "main") type = MAIN_TOKEN;
        else if (lexeme == "int") type = INT_TOKEN;
        else if (lexeme == "const") type = CONST_TOKEN;
        else type = IDENTIFIER_TOKEN;
    }
    else if (isdigit(next)) 
    {
        lexeme += next;
        while (isdigit(this->mFin.peek())) 
        {
            lexeme += this->mFin.get();
        }
        type = INTEGER_TOKEN;
    }
    else 
    {
        lexeme += next;
        char peek = this->mFin.peek();
        if (mFin.eof()) peek = '\0'; 

        switch (next) 
        {
        case '(': type = LEFT_PAREN_TOKEN; break;
        case ')': type = RIGHT_PAREN_TOKEN; break;
        case '{': type = LEFT_BRACE_TOKEN; break;
        case '}': type = RIGHT_BRACE_TOKEN; break;
        case '[': type = LEFT_BRACKET_TOKEN; break;
        case ']': type = RIGHT_BRACKET_TOKEN; break;
        case ';': type = SEMICOLON_TOKEN; break;
        case ',': type = COMMA_TOKEN; break;
        case '.': 
            if (peek == '.') { 
                lexeme += this->mFin.get();
                if (this->mFin.peek() == '.') { lexeme += this->mFin.get(); type = ELLIPSIS_TOKEN; }
                else type = BAD_TOKEN; 
            } else { type = DOT_TOKEN; }
            break;
        case '+': 
            if (peek == '+') { lexeme += this->mFin.get(); type = INCREMENT_TOKEN; }
            else if (peek == '=') { lexeme += this->mFin.get(); type = PLUS_EQUALS_TOKEN; }
            else { type = PLUS_TOKEN; }
            break;
        case '-': 
            if (peek == '-') { lexeme += this->mFin.get(); type = DECREMENT_TOKEN; }
            else if (peek == '=') { lexeme += this->mFin.get(); type = MINUS_EQUALS_TOKEN; }
            else if (peek == '>') { lexeme += this->mFin.get(); type = ARROW_TOKEN; }
            else { type = MINUS_TOKEN; }
            break;
        case '*': type = (peek == '=') ? MULTIPLY_EQUALS_TOKEN : MULTIPLY_TOKEN; break;
        case '/': 
            if (peek == '/') { type = SINGLE_LINE_COMMENT_TOKEN; }
            else if (peek == '*') { type = MULTI_LINE_COMMENT_TOKEN; }
            else if (peek == '=') { type = DIVIDE_EQUALS_TOKEN; }
            else { type = DIVIDE_TOKEN; }
            break;
        case '=': type = (peek == '=') ? EQUALITY_TOKEN : ASSIGNMENT_TOKEN; break;
        case '&': type = (peek == '&') ? LOGICAL_AND_TOKEN : BITWISE_AND_TOKEN; break;
        case '|': type = (peek == '|') ? LOGICAL_OR_TOKEN : BITWISE_OR_TOKEN; break;
        case '!': type = (peek == '=') ? NOT_EQUALS_TOKEN : LOGICAL_NOT_TOKEN; break;
        case '^': type = BITWISE_XOR_TOKEN; break;
        case '~': type = BITWISE_NOT_TOKEN; break;
        case '<': 
            if (peek == '<') { lexeme += this->mFin.get(); type = LEFT_SHIFT_TOKEN; }
            else if (peek == '=') { lexeme += this->mFin.get(); type = LESS_THAN_EQUALS_TOKEN; }
            else { type = LESS_THAN_TOKEN; }
            break;
        case '>': 
            if (peek == '>') { lexeme += this->mFin.get(); type = RIGHT_SHIFT_TOKEN; }
            else if (peek == '=') { lexeme += this->mFin.get(); type = GREATER_THAN_EQUALS_TOKEN; }
            else { type = GREATER_THAN_TOKEN; }
            break;
        case '#': type = HASH_TOKEN; break;
        case '%': 
            if (peek == 'd') { lexeme += this->mFin.get(); type = FORMAT_INT_TOKEN; } 
            else if (peek == 'f') { lexeme += this->mFin.get(); type = FORMAT_FLOAT_TOKEN; } 
            else { type = MODULUS_TOKEN; }
            break;
        default: type = BAD_TOKEN; break;
        }
    }

    // Return the token
    Token tok = Token(type, lexeme, this->mFileName, mLineNumber);
    tok.CheckReserved();

    std::cout << "Token Type: " << Token::GetTokenTypeName(tok.GetTokenType())
              << " | Lexeme: " << tok.GetLexeme() << std::endl;

    return tok;
}

Token ScannerClass::peekNextToken()
{
    std::streampos pos = mFin.tellg();
    int line = mLineNumber;
    Token token = getNextToken();
    if (!mFin)
    {
        mFin.clear();
    }
    mFin.seekg(pos);
    mLineNumber = line;
    return token;
}
