mod core;

use core::Core;

fn main() {
    let mut core = Core::new("ANON".to_string());
    core.run();
}
