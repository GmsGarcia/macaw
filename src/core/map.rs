use image::{load_from_memory, GenericImageView};

#[derive(PartialEq)]
pub struct Map {
    pub name: String,
    pub model: MapModel,
    pub data: &'static [u8],
    pub width: u32, 
    pub height: u32
}

impl Map {
    pub fn new(map_model: MapModel) -> Self {
        let map = load_from_memory(map_model.clone().get_data()).unwrap();
        Map{
            name: map_model.clone().get_name(),
            model: map_model.clone(),
            data: map_model.clone().get_data(),
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
    
    fn get_data(&self) -> &'static [u8] {
        match *self {
            MapModel::Default => include_bytes!("./../maps/default.png"),
            MapModel::Alternative => include_bytes!("./../maps/alternative.png"),
        }
    } 
}
