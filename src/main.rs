use crate::lexer::lex;

mod lexer;
mod token;

fn main() {
    let tokens = lex("azerty123");

    println!("{:?}", tokens);
}
