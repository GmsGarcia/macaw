mod render;
mod player;

use std::io::stdout;

use crossterm::{cursor, execute};
use render::Render;
use player::Player;

pub struct Core {
    render: Render,
    player: Player,
    map: Option<Map>
}

impl Core {
    pub fn new(name: String) -> Self {
        let core = Core{
            render: Render{},
            player: Player::new(name),
            map: None
        };
        let _ = core.render.init(&core.player);
        core
    }

    pub fn load_map(&mut self, map_name: String) {
        self.map = Some(Map::new(map_name));
    }

    pub fn player_action(&mut self, action: String) {
        match action.as_str() {
            "up" => self.player.move_player("up".to_string()),
            "down" => self.player.move_player("down".to_string()),
            "left" => self.player.move_player("left".to_string()),
            "right" => self.player.move_player("right".to_string()),
            "clear" => self.render.init(&self.player),
            _ => todo!()
        };

        execute!(stdout(), cursor::SavePosition).unwrap();
        let _ = self.render.update(&self.player);
    }

    pub fn resize(&self) {
        let _ = self.render.update(&self.player);
    }
}

struct Map {
    pub name: String,
    width: usize, 
    height: usize
}

impl Map {
    pub fn new(file: String) -> Self {
        Map{
            name: file.to_string(),
            width: 20,
            height: 20
        }
    }
}
