use image::{load_from_memory, GenericImageView};

#[derive(PartialEq, Clone)]
pub struct Map {
    pub name: String,
    pub model: MapModel,
    pub texture: &'static [u8],
    pub upper_texture: &'static [u8],
    pub collision: &'static [u8],
    pub width: u32, 
    pub height: u32
}

impl Map {
    pub fn new(map_model: MapModel) -> Self {
        let map = load_from_memory(map_model.clone().get_collision()).unwrap();
        Map{
            name: map_model.clone().get_name(),
            model: map_model.clone(),
            texture: map_model.clone().get_texture(),
            upper_texture: map_model.clone().get_upper_texture(),
            collision: map_model.clone().get_collision(),
            width: map.dimensions().0,
            height: map.dimensions().1
        }
    }
}

#[derive(Clone, PartialEq)]
pub enum MapModel {
    Default,
    Alternative 
}

impl MapModel {
    fn get_name(&self) -> String {
        match *self {
            MapModel::Default => "Default".to_string(),
            MapModel::Alternative => "Alternative".to_string(),
        }
    } 
    
    fn get_texture(&self) -> &'static [u8] {
        match *self {
            MapModel::Default => include_bytes!("./../maps/default.png"),
            MapModel::Alternative => include_bytes!("./../maps/alternative.png"),
        }
    } 

    fn get_upper_texture(&self) -> &'static [u8] {
        match *self {
            MapModel::Default => include_bytes!("./../maps/default_upper.png"),
            MapModel::Alternative => include_bytes!("./../maps/alternative_upper.png"),
        }
    } 

    fn get_collision(&self) -> &'static [u8] {
        match *self {
            MapModel::Default => include_bytes!("./../maps/default_collision.png"),
            MapModel::Alternative => include_bytes!("./../maps/alternative_collision.png"),
        }
    }
}
