extern crate libc;
mod media;
mod core_player;

use std::ffi::CStr;
use libc::{c_char};
use core_player::Player;
use media::types::*;

#[no_mangle]
pub extern fn media_player_create() -> *mut Player {
    Box::into_raw(Box::new(Player::new()))
}

#[no_mangle]
pub extern fn media_player_destory(ptr: *mut Player) {
    if ptr.is_null() { 
        return 
    }
    unsafe {
        Box::from_raw(ptr);
    }
}

#[no_mangle]
pub extern fn media_player_initialize(view: *mut View) -> i32 {
    println!("media_player_initialize");
    return -1;
}

#[no_mangle]
pub extern fn media_player_load_file(url: *const c_char) -> i32 {
    let url = unsafe{ CStr::from_ptr(url) };
    let url = url.to_str().unwrap();
    println!("media_player_load_file: {}", url);
    return -1;
}