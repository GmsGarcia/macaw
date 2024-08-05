mod render;
mod player;
mod map;

use std::io::stdout;

use crossterm::{cursor, execute};
use render::Render;
use player::Player;
use map::{Map, MapModel};

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
            map: Some(Map::new(MapModel::Default))
        };
        let _ = core.render.init(&core.player, &core.map);
        core
    }

    pub fn load_map(&mut self, map_model: MapModel) {
        self.map = Some(Map::new(map_model));
    }

    pub fn player_action(&mut self, action: String) {
        match &self.map {
            Some(map) => {
                match action.as_str() {
                    "up" => self.player.move_player(map, "up".to_string()),
                    "down" => self.player.move_player(map, "down".to_string()),
                    "left" => self.player.move_player(map, "left".to_string()),
                    "right" => self.player.move_player(map, "right".to_string()),
                    "clear" => self.render.clear(),
                    "change" => {
                        self.load_map(if map.model == MapModel::Default { MapModel::Alternative } else { MapModel::Default });
                        self.render.clear()
                    }
                    _ => todo!()
                }
            },
            None => todo!()
        };

        execute!(stdout(), cursor::SavePosition).unwrap();
        let _ = self.render.update(&self.player, &self.map);
    }

    pub fn resize(&self) {
        let _ = self.render.update(&self.player, &self.map);
    }
}
