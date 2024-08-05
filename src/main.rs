mod core;

use std::io::Error;
use crossterm::event::{self, Event, KeyCode, KeyEventKind};

use core::Core;

fn main() -> Result<(), Error> {

    let mut core = Core::new("ANON".to_string());

    // Loop
    loop {
        match event::read().unwrap() {
            Event::Key(event) => {
                if event.kind == KeyEventKind::Press {
                    match event.code {
                        KeyCode::Char('h') => core.player_action("left".to_string()),
                        KeyCode::Char('j') => core.player_action("down".to_string()),
                        KeyCode::Char('k') => core.player_action("up".to_string()),
                        KeyCode::Char('l') => core.player_action("right".to_string()),
                        
                        KeyCode::Char('c') => core.player_action("clear".to_string()),
                        KeyCode::Char('q') => core.player_action("change".to_string()),
                        _ => ()
                    };
                }
            },
            Event::Resize(_,_) => {
                core.resize();
            }
            _ => {}
        }
    }
}
