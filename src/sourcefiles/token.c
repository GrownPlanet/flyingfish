#include <stddef.h>

#include "token.h"

Token_t new_token(TokenType_t type, size_t line, Literal_t* literal) {
    Token_t token = {
        type,
        line,
        literal,
    };

    return token;
}
