#include <stddef.h>

#include "token.h"

Token_t new_token(TokenType_t type, Literal_t* literal) {
    Token_t token = {
        type,
        literal,
    };

    return token;
}
