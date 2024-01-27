-- LIBRARY FUNCTIONS

-- The ID of the current game
GameId = nil

-- A memory address
Address = {}
function Address.is_valid() end
function Address.pointer_chain() end

-- Memory read functions
function read_u8(addr) end
function read_u16(addr) end
function read_u32(addr) end
function read_u64(addr) end
function read_float(addr) end
function read_vec3(addr) end
function read_vec4(addr) end
function read_matrix34(addr) end
function read_matrix44(addr) end

function read_string(addr, size) end
function read_float_vec(addr, size) end
function read_u32_vec(addr, size) end
