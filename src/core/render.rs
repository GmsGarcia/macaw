use std::{io::{stdout, Stdout}, vec};
use crossterm::{cursor, execute, style::Color, terminal::{self, ClearType}};
use image::{load_from_memory, GenericImageView};
use ratatui::{self, backend::CrosstermBackend, buffer::Buffer, layout::{Constraint, Direction, Layout, Rect}, style::Stylize, widgets::{Block, Borders, Paragraph, StatefulWidget, Widget}, Terminal};

use super::{map::Map, player::Player, GameData};

pub struct Screen {
    terminal: Terminal<CrosstermBackend<Stdout>>,
    window: Window,
    info: Info
}

impl Screen {
    pub fn new() -> Self {
        let size = terminal::size().unwrap();

        let mut screen = Screen{
            terminal: Terminal::new(CrosstermBackend::new(stdout())).unwrap(),
            window: Window{ area: Rect{ x: 0, y: 0, width: size.0, height: size.1-4 } },
            info: Info{ area: Rect{ x: 0, y: size.1 - 3, width: size.0, height: 3 } },
        };

        screen.clear();
        screen
    }

    pub fn render(&mut self, mut player: Player, map: Option<Map>) {
        self.terminal.draw(|frame| {
            match map {
                Some(map) => {
                    let mut game_data = GameData{ player: player.clone(), map: map.clone() };
                    
                    frame.render_stateful_widget(self.window.clone(), self.window.area, &mut game_data);
                    frame.render_stateful_widget(self.info.clone(), self.info.area, &mut player);
                },
                None => {

                }
            }
        });
    }

    pub fn clear(&mut self) {
        self.terminal.clear().unwrap();

        execute!(
            stdout(),
            terminal::Clear(ClearType::Purge),
            cursor::SetCursorStyle::SteadyUnderScore,
            cursor::Hide,
        ).unwrap();
    }
}

#[derive(Clone)]
pub struct Window {
    area: Rect
}

#[derive(Clone)]
pub struct Info {
    area: Rect
}

impl StatefulWidget for Window {
    type State = GameData;

    fn render(self, area: Rect, buf: &mut Buffer, mut game_data: &mut Self::State) {
        let map = &game_data.map;
        let player = &game_data.player;

        let map_rows = vec!(1; map.height.try_into().unwrap());
        let c_rows = Constraint::from_lengths(map_rows);

        let map_columns = vec!(2; map.width.try_into().unwrap());
        let c_columns = Constraint::from_lengths(map_columns);

        let game = Layout::default()
            .direction(Direction::Vertical)
            .constraints(c_rows)
            .split(area);

        // loads textures from mem
        let texture = load_from_memory(map.texture).unwrap();
        let upper_texture = load_from_memory(map.upper_texture).unwrap();

        // renders lower texture
        for (r, row) in game.iter().enumerate()  {
            let cols  = Layout::default()
                .direction(Direction::Horizontal)
                .constraints(c_columns.clone())
                .split(*row);

            for (c, col) in cols.iter().enumerate() {
                let pixel = texture.get_pixel(c.try_into().unwrap(), r.try_into().unwrap()).0;

                let color = Color::Rgb { r: pixel[0], g: pixel[1], b: pixel[2] };
                let block = Paragraph::new("").bg(color);
                block.render(*col, buf)
            }
        }

        // renders cursor between lower and upper textures
        if player.spawned {
            let cursor = Paragraph::new("").bg(player.color);
            cursor.render(Rect{x: player.pos.x, y: player.pos.y, width: 2, height: 1}, buf);
        }

        // renders upper texture
        for (r, row) in game.iter().enumerate()  {
            let cols  = Layout::default()
                .direction(Direction::Horizontal)
                .constraints(c_columns.clone())
                .split(*row);

            for (c, col) in cols.iter().enumerate() {
                let pixel = upper_texture.get_pixel(c.try_into().unwrap(), r.try_into().unwrap()).0;

                if pixel == [0,0,0,0] {
                    continue;
                } else {
                    let color = Color::Rgb { r: pixel[0], g: pixel[1], b: pixel[2] };
                    let block = Paragraph::new("").bg(color);
                    block.render(*col, buf)
                }
            }
        }
    }
}

impl StatefulWidget for Info {
    type State = Player;

    fn render(self, area: Rect, buf: &mut Buffer, player: &mut Self::State) {
        let info = Paragraph::new(format!("{:},{:} - {:} - {:}", player.pos.x/2, player.pos.y, player.spawned, player.name)).block(Block::default().title("Info").borders(Borders::ALL).bg(Color::Black));
        info.render(area, buf);
    }
}
