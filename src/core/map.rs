use image::{load_from_memory, GenericImageView};

#[derive(Clone, PartialEq)]
pub struct Map {
    pub name: String,
    pub model: MapModel,
    pub texture: MapTexture,
    pub width: u32, 
    pub height: u32
}

impl Map {
    pub fn new(model: MapModel) -> Self {
        let map = load_from_memory(model.clone().get_collision()).unwrap();
        Map{
            name: model.clone().get_name(),
            model: model.clone(),
            texture: MapTexture::new(model),
            width: map.dimensions().0,
            height: map.dimensions().1
        }
    }
}

#[derive(Clone, PartialEq)]
pub struct MapTexture {
    pub ground: &'static [u8],
    pub upper: &'static [u8],
    pub lower: &'static [u8],
    pub collision: &'static [u8],
}

impl MapTexture {
    fn new(model: MapModel) -> Self {
        MapTexture {
            ground: model.clone().get_ground_texture(),
            upper: model.clone().get_upper_texture(),
            lower: model.clone().get_lower_texture(),
            collision: model.clone().get_collision()
        }
    }
}

#[derive(Clone, PartialEq)]
pub enum MapModel {
    Default,
    Alternative, 
    House 
}

impl MapModel {
    fn get_name(&self) -> String {
        match *self {
            MapModel::Default => "Default".to_string(),
            MapModel::Alternative => "Alternative".to_string(),
            MapModel::House => "House".to_string(),
        }
    } 
    
    fn get_ground_texture(&self) -> &'static [u8] {
        match *self {
            MapModel::Default => include_bytes!("./../maps/overworld/ground.png"),
            MapModel::Alternative => include_bytes!("./../maps/alternative/ground.png"),
            MapModel::House => include_bytes!("./../maps/house/ground.png"),
        }
    } 

    fn get_upper_texture(&self) -> &'static [u8] {
        match *self {
            MapModel::Default => include_bytes!("./../maps/overworld/upper.png"),
            MapModel::Alternative => include_bytes!("./../maps/alternative/upper.png"),
            MapModel::House => include_bytes!("./../maps/house/upper.png"),
        }
    } 
    
    fn get_lower_texture(&self) -> &'static [u8] {
        match *self {
            MapModel::Default => include_bytes!("./../maps/overworld/lower.png"),
            MapModel::Alternative => include_bytes!("./../maps/alternative/lower.png"),
            MapModel::House => include_bytes!("./../maps/house/lower.png"),
        }
    }

    fn get_collision(&self) -> &'static [u8] {
        match *self {
            MapModel::Default => include_bytes!("./../maps/overworld/collision.png"),
            MapModel::Alternative => include_bytes!("./../maps/alternative/collision.png"),
            MapModel::House => include_bytes!("./../maps/house/collision.png"),
        }
    }
}
