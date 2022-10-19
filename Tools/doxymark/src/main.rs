extern crate xml;

mod class;
mod document;
mod element;
mod manifest;
mod utility;

use manifest::Manifest;

fn main() {
    println!("{}: {}", env!("CARGO_PKG_NAME"), env!("CARGO_PKG_VERSION"));

    let mut path = "";
    let args: Vec<String> = std::env::args().collect();
    for (index, arg) in args.iter().enumerate() {
        if arg == "--path" && index < args.len() - 1 {
            path = &args[index + 1];
        }
    }

    if path.is_empty() {
        println!("--path argument not specified. Pass path to base of xml documentation files.");
        return;
    }

    let _manifest = Manifest::load(path);
}
