#[derive(Debug, Clone, Copy, PartialEq)]
pub enum TokenKind {
    Invalid,

    // Identifiers
    Identifier,
    Integer,
    Float,
    String,

    // Delimiters
    OpenParen,
    CloseParen,
    OpenBrace,
    CloseBrace,
    OpenBracket,
    CloseBracket,

    // Punctuations
    Equal,
    Period,
    Semicolon,
    Colon,
    Comma,
    At,
    Ampersand,

    // Binary operands
    Add,
    AddEqual,
    Sub,
    SubEqual,
    Mul,
    MulEqual,
    Div,
    DivEqual,

    // Binary comparisons
    EqualEqual,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,

    // Keywords
    If,
    Else,
    For,
    While,
    Continue,
    Break,
    And,
    Or,
    Not,
    Struct,
    Fn,
}

#[derive(Debug, Clone, Copy, PartialEq)]
pub struct Span {
    pub start: usize,
    pub end: usize,
}

#[derive(Debug, Clone, Copy, PartialEq)]
pub struct Token {
    pub kind: TokenKind,
    pub span: Span,
}

impl Token {
    pub fn new(kind: TokenKind, start: usize, end: usize) -> Token {
        Token {
            kind,
            span: Span { start, end },
        }
    }

    pub fn lexeme<'a>(&self, input: &'a str) -> &'a str {
        input.get(self.span.start..self.span.end).unwrap_or("")
    }
}

pub fn variant_eq<T>(a: &T, b: &T) -> bool {
    std::mem::discriminant(a) == std::mem::discriminant(b)
}
