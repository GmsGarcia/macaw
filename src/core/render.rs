use std::{io::{stdout, Error}, vec};
use crossterm::{cursor, execute, style::Color, terminal::{self, ClearType}};
use ratatui::{self, backend::CrosstermBackend, layout::{Constraint, Direction, Layout, Rect}, style::Stylize, widgets::{Block, Borders, Clear, Paragraph, Widget}, Terminal};
use rand::Rng;

use super::player::Player;

const NAMED_COLORS: [ratatui::prelude::Color; 7] = [
    ratatui::prelude::Color::Black,
    ratatui::prelude::Color::Red,
    ratatui::prelude::Color::Green,
    ratatui::prelude::Color::Blue,
    ratatui::prelude::Color::Magenta,
    ratatui::prelude::Color::Cyan,
    ratatui::prelude::Color::White,
];

pub struct Render {
    //pub map: Map
}

impl Render {
    pub fn init(&self, player: &Player) {
        let mut term = Terminal::new(CrosstermBackend::new(stdout())).unwrap(); 
        term.clear().unwrap();

        execute!(
            stdout(),
            terminal::Clear(ClearType::Purge),
            cursor::Hide,
        ).unwrap();

        let _ = self.update(player);
    }

    pub fn load_map() {

    }

    pub fn update(&self, player: &Player) -> Result<(), Error> {
        let mut term = Terminal::new(CrosstermBackend::new(stdout()))?; 
        
        let _a = term.draw(|frame| {
            let size = terminal::size().unwrap();
            let area = Rect{ x: 1, y: size.1 - 2, width: size.0-1, height: 1 };
            
            let cursor = Paragraph::new("").bg(Color::Black);
            frame.render_widget(cursor, area);
        });
        
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

            //let rows = vec!(1; chunks[0].as_size().height.into());
            let m_rows = vec!(1; 5);
            let c_rows = Constraint::from_lengths(m_rows);

            let columns = vec!(2; (chunks[0].as_size().width/2).into());
            let c_columns = Constraint::from_lengths(columns);

            let game = Layout::default()
                .direction(Direction::Vertical)
                .constraints(c_rows)
                .split(chunks[0]);

            for (_, row) in game.iter().enumerate()  {
                let cols  = Layout::default()
                    .direction(Direction::Horizontal)
                    .constraints(c_columns.clone())
                    .split(*row);

                for (_, col) in cols.iter().enumerate() {
                    let color = rand::thread_rng().gen_range(0..NAMED_COLORS.len()); 
                    let block = Paragraph::new("").bg(NAMED_COLORS[color]);
                    frame.render_widget(block, *col)
                }
            }

            let coords = Paragraph::new(format!("({:},{:})", player.pos.x, player.pos.y)).block(Block::default().title("Coords").borders(Borders::ALL).bg(Color::Black));
            frame.render_widget(coords, chunks[1]);
            
            let cursor = Paragraph::new("").bg(Color::Yellow);
            frame.render_widget(cursor, Rect{x: player.pos.x, y: player.pos.y, width: 2, height: 1});
        });
        Ok(())
    }
}


