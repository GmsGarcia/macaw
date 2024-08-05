use crossterm::terminal;
use image::{load_from_memory, GenericImageView};

use super::map::Map;

pub struct Player {
    pub name: String,
    pub pos: Point
}

impl Player {
    pub fn new(name: String) -> Self {
        Player{
            name: name.to_string(),
            pos: Point{x: 0, y: 0}
        } 
    }

    pub fn move_player(&mut self, map: &Map, direction: String) {
        let data = load_from_memory(map.data).unwrap();

        match direction.as_str() {
            "up" => {
                if self.pos.y > 0 {
                    let next = data.get_pixel((self.pos.x/2).into(), (self.pos.y - 1).into()).0;
                    if (next[0] == 0 && next[1] == 255 && next[2] == 0) || (next[0] == 255 && next[1] == 0 && next[2] == 0) {
                        self.pos.y -= 1;
                    }
                }
            },
            "down" => {
                if u32::from(self.pos.y) < map.height - 1 {
                    let next = data.get_pixel((self.pos.x/2).into(), (self.pos.y + 1).into()).0;
                    if (next[0] == 0 && next[1] == 255 && next[2] == 0) || (next[0] == 255 && next[1] == 0 && next[2] == 0) {
                        self.pos.y += 1;
                    }
                }
            },
            "left" => {
                if self.pos.x >= 2 {
                    let next = data.get_pixel((self.pos.x/2 - 1).into(), self.pos.y.into()).0;
                    if (next[0] == 0 && next[1] == 255 && next[2] == 0) || (next[0] == 255 && next[1] == 0 && next[2] == 0) {
                        self.pos.x -= 2;
                    }
                }
            },
            "right" => {
                if u32::from(self.pos.x) <= (map.width-2) * 2 {
                    let next = data.get_pixel((self.pos.x/2 + 1).into(), self.pos.y.into()).0;
                    if (next[0] == 0 && next[1] == 255 && next[2] == 0) || (next[0] == 255 && next[1] == 0 && next[2] == 0) {
                        self.pos.x += 2;
                    }
                }
            },
            _ => {}
        }
    }
}

pub struct Point {
    pub x: u16,
    pub y: u16,
}
