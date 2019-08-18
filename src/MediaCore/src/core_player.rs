extern crate libc;

pub struct Player {
    id: i32
}

impl Player {
    pub fn new() -> Player {
        Player{
            id: 0
        }
    }
}