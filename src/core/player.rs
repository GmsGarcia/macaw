use image::{load_from_memory, GenericImageView};
use rand::Rng;
use ratatui::style::Color;

use super::map::Map;

#[derive(Clone)]
pub struct Player {
    pub name: String,
    pub pos: Point,
    pub color: Color,
    pub spawned: bool
}

impl Player {
    pub fn new(name: String) -> Self {
        Player{
            name: name.to_string(),
            pos: Point{x: 0, y: 0},
            color: Color::Rgb(255, 0, 255),
            spawned: false
        } 
    }

    pub fn spawn(&mut self, map: &Map) {
        let data = load_from_memory(map.collision).unwrap();

        if !self.spawned {
            while !self.spawned {
                // get random pos
                let x = rand::thread_rng().gen_range(0..map.width);
                let y = rand::thread_rng().gen_range(0..map.height);
                
                let block = data.get_pixel(x, y).0;

                if is_walkable(block) {
                    self.pos = Point{x: (x*2).try_into().unwrap(), y: y.try_into().unwrap()};
                    self.spawned = true;
                }
            }
        } 
    }

    pub fn despawn(&mut self) {
        self.pos = Point{x: 0, y: 0};
        self.spawned = false;
    }

    pub fn move_player(&mut self, map: &Map, direction: String) {
        let data = load_from_memory(map.collision).unwrap();

        if self.spawned {
            match direction.as_str() {
                "up" => {
                    if self.pos.y > 0 {
                        let next = data.get_pixel((self.pos.x/2).into(), (self.pos.y - 1).into()).0;
                        if is_walkable(next) {
                            self.pos.y -= 1;
                        }
                    }
                },
                "down" => {
                    if u32::from(self.pos.y) < map.height - 1 {
                        let next = data.get_pixel((self.pos.x/2).into(), (self.pos.y + 1).into()).0;
                        if is_walkable(next) {
                            self.pos.y += 1;
                        }
                    }
                },
                "left" => {
                    if self.pos.x >= 2 {
                        let next = data.get_pixel((self.pos.x/2 - 1).into(), self.pos.y.into()).0;
                        if is_walkable(next) {
                            self.pos.x -= 2;
                        }
                    }
                },
                "right" => {
                    if u32::from(self.pos.x) <= (map.width-2) * 2 {
                        let next = data.get_pixel((self.pos.x/2 + 1).into(), self.pos.y.into()).0;
                        if is_walkable(next) {
                            self.pos.x += 2;
                        }
                    }
                },
                _ => {}
            }
        }
    }

    pub fn toggle_color(&mut self) {
        self.color = if self.color== Color::Rgb(255, 0, 255) { Color::Rgb(0, 255, 0) } else { Color::Rgb(255, 0, 255) };
    }
}

fn is_walkable(block: [u8; 4]) -> bool {
    if block[0] == 255 && block[1] == 255 && block[2] == 255 {
        return true;
    } else {
        return false;
    }

}

#[derive(Clone)]
pub struct Point {
    pub x: u16,
    pub y: u16,
}
