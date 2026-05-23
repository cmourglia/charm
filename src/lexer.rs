use crate::token::{Token, TokenKind};
use phf::{Map, phf_map};

static KEYWORDS: Map<&'static str, TokenKind> = phf_map! {
    "if" => TokenKind::If,
    "else" => TokenKind::Else,
    "for" => TokenKind::For,
    "while" => TokenKind::While,
    "continue" => TokenKind::Continue,
    "break" => TokenKind::Break,
    "and" => TokenKind::And,
    "or" => TokenKind::Or,
    "not" => TokenKind::Not,
    "struct" => TokenKind::Struct,
    "fn" => TokenKind::Fn,
};

pub fn lex(input: &str) -> Vec<Token> {
    Lexer::new(input).collect()
}

struct Lexer<'a> {
    input: &'a str,

    current: usize,
}

impl<'a> Lexer<'a> {
    pub fn new(input: &'a str) -> Self {
        Self { input, current: 0 }
    }

    fn two_chars_token(&mut self, one_char: TokenKind, two_chars: TokenKind) -> TokenKind {
        if self.peek() == Some('=') {
            self.consume();
            two_chars
        } else {
            one_char
        }
    }

    fn number(&mut self) -> TokenKind {
        self.consume_while(|c| c.is_ascii_digit());

        if self.peek() == Some('.') && matches!(self.peek_next(), Some('0'..='9')) {
            self.consume();
            self.consume_while(|c| c.is_ascii_digit());

            TokenKind::Float
        } else {
            TokenKind::Integer
        }
    }

    fn identifier(&mut self, start: usize) -> TokenKind {
        self.consume_while(|c| c.is_alphanumeric() || c == '_');

        let identifier_str = &self.input[start..self.current];

        *KEYWORDS
            .get(identifier_str)
            .unwrap_or(&TokenKind::Identifier)
    }

    fn string(&mut self) -> TokenKind {
        while let Some(next) = self.advance() {
            if next == '"' {
                return TokenKind::String;
            }
        }

        TokenKind::Invalid
    }

    fn advance(&mut self) -> Option<char> {
        let mut chars = self.input[self.current..].char_indices();
        let next = chars.next();

        if let Some(ch) = next {
            self.current += chars.offset();
            Some(ch.1)
        } else {
            None
        }
    }

    fn consume(&mut self) {
        let _ = self.advance();
    }

    fn consume_while(&mut self, predicate: impl Fn(char) -> bool) {
        while self.peek().is_some_and(&predicate) {
            self.consume();
        }
    }

    fn peek(&self) -> Option<char> {
        self.input[self.current..].chars().next()
    }

    fn peek_next(&self) -> Option<char> {
        let mut chars = self.input[self.current..].chars();
        chars.next();
        chars.next()
    }

    fn skip_whitespace(&mut self) {
        self.consume_while(|c| c.is_whitespace());
    }
}

impl<'a> Iterator for Lexer<'a> {
    type Item = Token;

    fn next(&mut self) -> Option<Self::Item> {
        self.skip_whitespace();

        let start = self.current;
        let c = self.advance()?;

        let kind = match c {
            '(' => TokenKind::OpenParen,
            ')' => TokenKind::CloseParen,
            '{' => TokenKind::OpenBrace,
            '}' => TokenKind::CloseBrace,
            '[' => TokenKind::OpenBracket,
            ']' => TokenKind::CloseBracket,
            ';' => TokenKind::Semicolon,
            ',' => TokenKind::Comma,
            ':' => TokenKind::Colon,
            '.' => TokenKind::Period,
            '@' => TokenKind::At,
            '&' => TokenKind::Ampersand,
            '=' => self.two_chars_token(TokenKind::Equal, TokenKind::EqualEqual),
            '!' => self.two_chars_token(TokenKind::Invalid, TokenKind::NotEqual),
            '+' => self.two_chars_token(TokenKind::Add, TokenKind::AddEqual),
            '-' => self.two_chars_token(TokenKind::Sub, TokenKind::SubEqual),
            '*' => self.two_chars_token(TokenKind::Mul, TokenKind::MulEqual),
            '/' => self.two_chars_token(TokenKind::Div, TokenKind::DivEqual),
            '<' => self.two_chars_token(TokenKind::Less, TokenKind::LessEqual),
            '>' => self.two_chars_token(TokenKind::Greater, TokenKind::GreaterEqual),
            '"' => self.string(),
            '0'..='9' => self.number(),
            _ if c.is_alphabetic() || c == '_' => self.identifier(start),
            _ => TokenKind::Invalid,
        };

        Some(Token::new(kind, start, self.current))
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn assert_tokens(input: &str, expected: &[Token]) {
        let actual = lex(input);
        assert_eq!(
            actual.len(),
            expected.len(),
            "token count mismatch\nactual: {actual:#?}\nexpected: {expected:#?}"
        );

        for (i, (actual, expected)) in actual.iter().zip(expected.iter()).enumerate() {
            let actual_text = actual.lexeme(input);
            let expected_text = expected.lexeme(input);
            assert_eq!(
                actual, expected,
                "token {i} mismatch, actual text: {actual_text:?} ; expected text: {expected_text:?}"
            );
        }
    }

    #[test]
    fn punctuation() {
        let str = r#"(){}[];,:.+-*/+=-=*=/==!===<<=>>=@&"#;
        let expected = vec![
            Token::new(TokenKind::OpenParen, 0, 1),
            Token::new(TokenKind::CloseParen, 1, 2),
            Token::new(TokenKind::OpenBrace, 2, 3),
            Token::new(TokenKind::CloseBrace, 3, 4),
            Token::new(TokenKind::OpenBracket, 4, 5),
            Token::new(TokenKind::CloseBracket, 5, 6),
            Token::new(TokenKind::Semicolon, 6, 7),
            Token::new(TokenKind::Comma, 7, 8),
            Token::new(TokenKind::Colon, 8, 9),
            Token::new(TokenKind::Period, 9, 10),
            Token::new(TokenKind::Add, 10, 11),
            Token::new(TokenKind::Sub, 11, 12),
            Token::new(TokenKind::Mul, 12, 13),
            Token::new(TokenKind::Div, 13, 14),
            Token::new(TokenKind::AddEqual, 14, 16),
            Token::new(TokenKind::SubEqual, 16, 18),
            Token::new(TokenKind::MulEqual, 18, 20),
            Token::new(TokenKind::DivEqual, 20, 22),
            Token::new(TokenKind::Equal, 22, 23),
            Token::new(TokenKind::NotEqual, 23, 25),
            Token::new(TokenKind::EqualEqual, 25, 27),
            Token::new(TokenKind::Less, 27, 28),
            Token::new(TokenKind::LessEqual, 28, 30),
            Token::new(TokenKind::Greater, 30, 31),
            Token::new(TokenKind::GreaterEqual, 31, 33),
            Token::new(TokenKind::At, 33, 34),
            Token::new(TokenKind::Ampersand, 34, 35),
        ];

        assert_tokens(str, &expected);
    }

    #[test]
    fn test_string() {
        let str = r#"
        "this is a test"
        "this is a multiline
        string, that spans
        multiple lines"
        "this is a last string"
        "#;
        let expected = vec![
            Token::new(TokenKind::String, 9, 25),
            Token::new(TokenKind::String, 34, 105),
            Token::new(TokenKind::String, 114, 137),
        ];
        assert_tokens(str, &expected);
    }

    #[test]
    fn test_invalid_string() {
        let str = r#""this is a test"#;
        let expected = vec![Token::new(TokenKind::Invalid, 0, 15)];
        assert_tokens(str, &expected);
    }

    #[test]
    fn test_number() {
        let str = r#"123 456 123.456 .123. 123.123.456"#;
        let expected = vec![
            Token::new(TokenKind::Integer, 0, 3),
            Token::new(TokenKind::Integer, 4, 7),
            Token::new(TokenKind::Float, 8, 15),
            Token::new(TokenKind::Period, 16, 17),
            Token::new(TokenKind::Integer, 17, 20),
            Token::new(TokenKind::Period, 20, 21),
            Token::new(TokenKind::Float, 22, 29),
            Token::new(TokenKind::Period, 29, 30),
            Token::new(TokenKind::Integer, 30, 33),
        ];
        assert_tokens(str, &expected);
    }

    #[test]
    fn test_identifier() {
        let str = r#"if else for while continue break and or not struct fn titi tata tutu _tita tati_ _tutu123 _42"#;
        let expected = vec![
            Token::new(TokenKind::If, 0, 2),
            Token::new(TokenKind::Else, 3, 7),
            Token::new(TokenKind::For, 8, 11),
            Token::new(TokenKind::While, 12, 17),
            Token::new(TokenKind::Continue, 18, 26),
            Token::new(TokenKind::Break, 27, 32),
            Token::new(TokenKind::And, 33, 36),
            Token::new(TokenKind::Or, 37, 39),
            Token::new(TokenKind::Not, 40, 43),
            Token::new(TokenKind::Struct, 44, 50),
            Token::new(TokenKind::Fn, 51, 53),
            Token::new(TokenKind::Identifier, 54, 58),
            Token::new(TokenKind::Identifier, 59, 63),
            Token::new(TokenKind::Identifier, 64, 68),
            Token::new(TokenKind::Identifier, 69, 74),
            Token::new(TokenKind::Identifier, 75, 80),
            Token::new(TokenKind::Identifier, 81, 89),
            Token::new(TokenKind::Identifier, 90, 93),
        ];
        assert_tokens(str, &expected);
    }
}
