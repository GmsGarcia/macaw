use crossterm::terminal;

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

    pub fn move_player(&mut self, direction: String) {
        match direction.as_str() {
            "up" => {
                if self.pos.y > 0 {
                    self.pos.y -= 1;
                }
            },
            "down" => {
                if self.pos.y < terminal::size().unwrap().1-4 {
                    self.pos.y += 1;
                }
            },
            "left" => {
                if self.pos.x >= 2 {
                    self.pos.x -= 2;
                }
            },
            "right" => {
                if self.pos.x <= terminal::size().unwrap().0-4 {
                    self.pos.x += 2;
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
