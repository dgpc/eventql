/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <assert.h>
#include "tokenize.h"

namespace fnordmetric {
namespace query {

void tokenizeQuery(
    const char** cur,
    const char* end,
    std::vector<Token>* token_list) {
  char quote_char = 0;
  Token::kTokenType string_type = Token::T_STRING;

next:

  /* skip whitespace */
  while (**cur == ' ' && *cur < end) {
    (*cur)++;
  }

  if (*cur >= end) {
    return;
  }

  /* single character tokens */
  switch (**cur) {
    case ';': {
      token_list->emplace_back(Token::T_SEMICOLON);
      (*cur)++;
      return;
    }

    case ',': {
      token_list->emplace_back(Token::T_COMMA);
      (*cur)++;
      goto next;
    }

    case '.': {
      token_list->emplace_back(Token::T_DOT);
      (*cur)++;
      goto next;
    }

    case '(': {
      token_list->emplace_back(Token::T_LPAREN);
      (*cur)++;
      goto next;
    }

    case ')': {
      token_list->emplace_back(Token::T_RPAREN);
      (*cur)++;
      goto next;
    }

    case '=': {
      token_list->emplace_back(Token::T_EQUAL);
      (*cur)++;
      goto next;
    }

    case '+': {
      token_list->emplace_back(Token::T_PLUS);
      (*cur)++;
      goto next;
    }

    case '-': {
      token_list->emplace_back(Token::T_MINUS);
      (*cur)++;
      goto next;
    }

    case '*': {
      token_list->emplace_back(Token::T_ASTERISK);
      (*cur)++;
      goto next;
    }

    case '!': {
      token_list->emplace_back(Token::T_BANG);
      (*cur)++;
      goto next;
    }

    case '/': {
      token_list->emplace_back(Token::T_SLASH);
      (*cur)++;
      goto next;
    }

    case '^': {
      token_list->emplace_back(Token::T_CIRCUMFLEX);
      (*cur)++;
      goto next;
    }

    case '~': {
      token_list->emplace_back(Token::T_TILDE);
      (*cur)++;
      goto next;
    }

    case '%': {
      token_list->emplace_back(Token::T_PERCENT);
      (*cur)++;
      goto next;
    }

    case '&': {
      token_list->emplace_back(Token::T_AMPERSAND);
      (*cur)++;
      goto next;
    }

    case '|': {
      token_list->emplace_back(Token::T_PIPE);
      (*cur)++;
      goto next;
    }

    /* numeric literals */
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': {
      const char* begin = *cur;
      for (; (**cur >= '0' && **cur <= '9') || **cur == '.'; (*cur)++);
      token_list->emplace_back(Token::T_NUMERIC, begin, *cur - begin);
      goto next;
    }

    case '`':
      string_type = Token::T_IDENTIFIER;
      /* fallthrough */

    case '"':
    case '\'':
      quote_char = **cur;
      (*cur)++;
      /* fallthrough */
  }

  /* multi char tokens */
  const char* begin = *cur;
  size_t len = 0;

  /* quoted strings */
  if (quote_char) {
    int escape_level = 0;
    while (*cur < end) {
      if (**cur == quote_char) {
        if (escape_level % 2 == 0) break;
      } else if (**cur == '\\') {
        escape_level++;
      } else {
        escape_level = 0;
      }
      (*cur)++;
      len++;
    }
    (*cur)++;
    token_list->emplace_back(string_type, begin, len);
    quote_char = 0;
    string_type = Token::T_STRING;
    goto next;
  }

  /* keywords and identifiers (i.e table, field names) */
  while (
      **cur != ' ' &&
      **cur != ',' &&
      **cur != '.' &&
      **cur != ';' &&
      **cur != '(' &&
      **cur != ')' &&
      **cur != '=' &&
      **cur != '+' &&
      **cur != '-' &&
      **cur != '*' &&
      **cur != '!' &&
      **cur != '/' &&
      **cur != '^' &&
      **cur != '~' &&
      **cur != '%' &&
      **cur != '&' &&
      **cur != '|' &&
      *cur < end) {
    len++;
    (*cur)++;
  }

  Token token(Token::T_IDENTIFIER, begin, len);

  if (token == "AS") {
    token_list->emplace_back(Token::T_AS);
    goto next;
  }

  if (token == "ASC") {
    token_list->emplace_back(Token::T_ASC);
    goto next;
  }

  if (token == "DESC") {
    token_list->emplace_back(Token::T_DESC);
    goto next;
  }

  if (token == "NOT") {
    token_list->emplace_back(Token::T_NOT);
    goto next;
  }

  if (token == "TRUE") {
    token_list->emplace_back(Token::T_TRUE);
    goto next;
  }

  if (token == "FALSE") {
    token_list->emplace_back(Token::T_TRUE);
    goto next;
  }

  if (token == "SELECT") {
    token_list->emplace_back(Token::T_SELECT);
    goto next;
  }

  if (token == "FROM") {
    token_list->emplace_back(Token::T_FROM);
    goto next;
  }

  if (token == "WHERE") {
    token_list->emplace_back(Token::T_WHERE);
    goto next;
  }

  if (token == "GROUP") {
    token_list->emplace_back(Token::T_GROUP);
    goto next;
  }

  if (token == "ORDER") {
    token_list->emplace_back(Token::T_ORDER);
    goto next;
  }

  if (token == "BY") {
    token_list->emplace_back(Token::T_BY);
    goto next;
  }

  if (token == "AND") {
    token_list->emplace_back(Token::T_AND);
    goto next;
  }

  if (token == "OR") {
    token_list->emplace_back(Token::T_OR);
    goto next;
  }

  if (token == "DIV") {
    token_list->emplace_back(Token::T_DIV);
    goto next;
  }

  if (token == "MOD") {
    token_list->emplace_back(Token::T_MOD);
    goto next;
  }

  if (token == "<<") {
    token_list->emplace_back(Token::T_LSHIFT);
    goto next;
  }

  if (token == ">>") {
    token_list->emplace_back(Token::T_RSHIFT);
    goto next;
  }

  token_list->push_back(token);
  goto next; // poor mans tail recursion optimization
}

}
}
