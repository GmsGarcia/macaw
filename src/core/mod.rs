mod render;
mod player;
mod map;

use std::collections::HashSet;

use crossterm::event::{self, Event, KeyCode, KeyEventKind};
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
    map: Option<Map>,
    keys: HashSet<KeyCode>
}

impl Core {
    pub fn new(name: String) -> Self {
        let mut core = Core{
            screen: Screen::new(),
            player: Player::new(name),
            map: None, 
            keys: HashSet::new() 
        };
        core.screen.clear();
        core.screen.render(core.player.clone(), core.map.clone());
        core
    }

    pub fn run(&mut self) {
        loop {
            match event::read().unwrap() {
                Event::Key(event) => {
                    match event.kind {
                        KeyEventKind::Press => {
                            self.keys.insert(event.code);
                        },
                        KeyEventKind::Release => {
                            self.keys.remove(&event.code);
                        },
                        _ => {}
                    }

                    if let Some(_) = &self.map {
                        self.handle_ingame_events()
                    } else {
                        self.handle_menu_events()
                    }   
                },
                Event::Resize(_,_) => {
                    self.resize();
                }
                _ => {}
            }

            self.screen.render(self.player.clone(), self.map.clone());
        }
    }

    pub fn handle_menu_events(&mut self) {
        for key in self.keys.clone() {
            match key {
                KeyCode::Char('n') => self.load_map(MapModel::House),
                KeyCode::Char('c') => self.screen.clear(),
                _ => {}
            };
        }
    }

    pub fn handle_ingame_events(&mut self) {
        for key in self.keys.clone() {
            match key {
                KeyCode::Char('k') => self.player.move_player(&self.map, "up".to_string()),
                KeyCode::Char('h') => self.player.move_player(&self.map, "left".to_string()),
                KeyCode::Char('j') => self.player.move_player(&self.map, "down".to_string()),
                KeyCode::Char('l') => self.player.move_player(&self.map, "right".to_string()),

                KeyCode::Char('c') => self.screen.clear(),
                KeyCode::Char('r') => self.player.spawn(&self.map),
                KeyCode::Char('q') => {
                    if self.player.spawned {
                        self.load_map(if self.map.clone().unwrap().model == MapModel::Default { MapModel::Alternative } else { MapModel::Default });
                        self.player.toggle_color();
                    }
                },
                _ => {}
            }
        }
    }

    pub fn load_map(&mut self, map_model: MapModel) {
        self.map = Some(Map::new(map_model));
    }

    pub fn resize(&mut self) {
        self.screen.clear();
        self.screen.render(self.player.clone(), self.map.clone());
    }
}
