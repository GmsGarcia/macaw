mod render;
mod player;
mod map;

use render::Screen;
use player::Player;
use map::{Map, MapModel};

pub struct GameData {
    pub player: Player,
    pub map: Map
}

pub struct Core {
    screen: Screen,
    player: Player,
    map: Option<Map>
}

impl Core {
    pub fn new(name: String) -> Self {
        let mut core = Core{
            screen: Screen::new(),
            player: Player::new(name),
            map: Some(Map::new(MapModel::Default))
        };
        core.screen.clear();
        core.screen.render(core.player.clone(), core.map.clone());
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
                    "clear" => self.screen.clear(),
                    "despawn" => self.player.despawn(),
                    "spawn" => self.player.spawn(map),
                    "change" => {
                        self.load_map(if map.model == MapModel::Default { MapModel::Alternative } else { MapModel::Default });
                        self.player.toggle_color();
                    }
                    _ => todo!()
                }
            },
            None => todo!()
        };

        self.screen.render(self.player.clone(), self.map.clone());
    }

    pub fn resize(&mut self) {
        self.screen.clear();
        self.screen.render(self.player.clone(), self.map.clone());
    }
}
