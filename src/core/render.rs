use std::{io::{stdout, Error}, vec};
use crossterm::{cursor, execute, style::Color, terminal::{self, ClearType}};
use image::{load_from_memory, GenericImageView};
use ratatui::{self, backend::CrosstermBackend, layout::{Constraint, Direction, Layout, Rect}, style::Stylize, widgets::{Block, Borders, Paragraph}, Terminal};
use rand::Rng;

use super::{player::Player, map::Map};

pub struct Render {}

impl Render {
    pub fn init(&self, player: &Player, map: &Option<Map>) {
        let mut term = Terminal::new(CrosstermBackend::new(stdout())).unwrap(); 
        term.clear().unwrap();

        execute!(
            stdout(),
            terminal::Clear(ClearType::Purge),
            cursor::SetCursorStyle::SteadyUnderScore,
            cursor::Hide,
        ).unwrap();

        let _ = self.update(player, map);
    }
    
    pub fn clear(&self) {
        let mut term = Terminal::new(CrosstermBackend::new(stdout())).unwrap(); 
        term.clear().unwrap();

        execute!(
            stdout(),
            terminal::Clear(ClearType::Purge),
            cursor::SetCursorStyle::SteadyUnderScore,
            cursor::Hide,
        ).unwrap();
    }

    pub fn update(&self, player: &Player, map: &Option<Map>) -> Result<(), Error> {
        let mut term = Terminal::new(CrosstermBackend::new(stdout()))?; 
        
        let _a = term.draw(|frame| {
            let size = terminal::size().unwrap();
            let area = Rect{ x: 1, y: size.1 - 2, width: size.0-2, height: 1 };
            
            let cursor = Paragraph::new("").bg(Color::Black);
            frame.render_widget(cursor, area);
        });

        match map {
            Some(map) => {
                let _b = term.draw(|frame| {
                     let chunks = Layout::default()
                        .direction(Direction::Vertical)
                        .constraints(
                            [
                                Constraint::Fill(1),
                                Constraint::Length(3)
                            ],
                        )
                        .split(frame.size());

                    //let fs = vec!(1; chunks[0].as_size().height.into());
                    let map_rows = vec!(1; map.height.try_into().unwrap());
                    let c_rows = Constraint::from_lengths(map_rows);

                    let map_columns = vec!(2; map.width.try_into().unwrap());
                    let c_columns = Constraint::from_lengths(map_columns);

                    let game = Layout::default()
                        .direction(Direction::Vertical)
                        .constraints(c_rows)
                        .split(chunks[0]);

                    let data = load_from_memory(map.data).unwrap();

                    for (r, row) in game.iter().enumerate()  {
                        let cols  = Layout::default()
                            .direction(Direction::Horizontal)
                            .constraints(c_columns.clone())
                            .split(*row);

                        for (c, col) in cols.iter().enumerate() {
                            let pixel = data.get_pixel(c.try_into().unwrap(), r.try_into().unwrap()).0;

                            let mut color = Color::Yellow;

                            if pixel[0] == 255 && pixel[1] == 0 && pixel[2] == 0 {
                                color = Color::Red;
                            } else if pixel[0] == 0 && pixel[1] == 255 && pixel[2] == 0 {
                                color = Color::Green;
                            } else if pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 255 {
                                color = Color::Blue;
                            } else if pixel[0] == 255 && pixel[1] == 0 && pixel[2] == 255 {
                                color = Color::Magenta;
                            } 

                            let block = Paragraph::new("").bg(color);
                            frame.render_widget(block, *col)
                        }
                    }

                    let coords = Paragraph::new(format!("{:},{:} - {:}", player.pos.x, player.pos.y, map.name)).block(Block::default().title("Coords").borders(Borders::ALL).bg(Color::Black));
                    frame.render_widget(coords, chunks[1]);
                
                    let cursor = Paragraph::new("").bg(Color::Yellow);
                    frame.render_widget(cursor, Rect{x: player.pos.x, y: player.pos.y, width: 2, height: 1});
                });
            },
            None => {
                let _c = term.draw(|frame| {
                     let chunks = Layout::default()
                        .direction(Direction::Vertical)
                        .constraints(
                            [
                                Constraint::Fill(1),
                                Constraint::Length(3)
                            ],
                        )
                        .split(frame.size());

                    let coords = Paragraph::new(format!("({:},{:})", player.pos.x, player.pos.y)).block(Block::default().title("Coords").borders(Borders::ALL).bg(Color::Black));
                    frame.render_widget(coords, chunks[1]);
                
                    let cursor = Paragraph::new("").bg(Color::Yellow);
                    frame.render_widget(cursor, Rect{x: player.pos.x, y: player.pos.y, width: 2, height: 1});
                });
            }
        }
        Ok(())
    }
}


