function Main()
  if GameId == "RMCP01" then
    PlayerHolder = Address.new(0x9C18F8)
    RaceManager = Address.new(0x9BD730)
    ItemDirector = Address.new(0x9C3618)
    CameraManager = Address.new(0x9C19B8)
    CameraManager2 = Address.new(0x9C19A8)
  end

  local kart_ptrs = PlayerHolder:pointer_chain(0x0, 0x20, 0x0, 0x10, 0x0)
  local kart_body = kart_ptrs:pointer_chain(0x8)
  local kart_camera = kart_ptrs:pointer_chain(0x24)
  local kart_move = kart_ptrs:pointer_chain(0x28)
  local player_model = kart_ptrs:pointer_chain(0x14)

  local camera = ReadCameraData(kart_camera)

  local kart_parts = GetKartParts(kart_body)
  local player = {kartParts = kart_parts}
  player.player = ReadPlayerData(player_model)
  player.move = ReadKartMove(kart_move)

  local race = ReadRace(RaceManager:pointer_chain(0x0))
  local item = ReadItemData(ItemDirector:pointer_chain(0x0))

  local ret = {player = player, camera = camera, race = race, item = item}
  return ret
end

-- Every kart part is linked with each other.
-- To get all parts we simply follow the backwards and forwards links until the end
function GetKartParts(start_part)
  local current_part = start_part
  local kart_parts = {}

  -- Go forwards
  while current_part:is_valid() and not current_part:eq(0) do
    local model_addr = current_part:pointer_chain(0x7C)
    local model_name = GetModelName(model_addr)
    if model_name then
      local kart_part = ReadModelData(model_addr)
      kart_part.modelName = model_name
      table.insert(kart_parts, kart_part)
    end
    current_part = current_part:pointer_chain(0x8)
  end

  -- Go backwards
  current_part = start_part:pointer_chain(0x4)
  while current_part:is_valid() and not current_part:eq(0) do
    local model_addr = current_part:pointer_chain(0x7C)
    local model_name = GetModelName(model_addr)
    if model_name then
      local kart_part = ReadModelData(model_addr)
      kart_part.modelName = model_name
      table.insert(kart_parts, kart_part)
    end
    current_part = current_part:pointer_chain(0x4)
  end

  return kart_parts
end

-- Based on https://wiki.tockdom.com/wiki/MDL0_(File_Format)
function GetModelName(model)
  local header = model:pointer_chain(0xC)
  -- verify header magic
  if not (read_string(header, 4) == "MDL0") then
    return nil
  end

  local brres_version = read_u32(header + 0x8)
  if not brres_version then
    return nil
  end
  local nr_sections = brres_version == 11 and 14 or 11
  local name_offset = read_u32(header + 0x10 + (nr_sections * 0x4))
  if not name_offset then
    return nil
  end

  local name_address = header + name_offset
  -- the size of the string located right in front of the string
  local name_size = read_u32(name_address - 0x4)
  return read_string(name_address, name_size)
end

function ReadModelData(model_addr)
  local transMatrix = read_matrix34(model_addr:pointer_chain(0x14, 0x0) + 0xc)
  if not transMatrix then
    return {}
  end

  -- +----+----+----+
  -- | X0 | Y0 | Z0 |
  -- +----+----+----+
  -- | PX | X1 | Y1 |
  -- +----+----+----+
  -- | Z1 | PY | X2 |
  -- +----+----+----+
  -- | Y2 | Z2 | PZ |
  -- +----+----+----+
  local data = {rotation = {}, position = {}}
  data.rotation.x = {x = transMatrix.w.x, y = transMatrix.x.y, z = transMatrix.y.z}
  data.rotation.y = {x = transMatrix.w.y, y = transMatrix.x.z, z = transMatrix.z.x}
  data.rotation.z = {x = transMatrix.w.z, y = transMatrix.y.x, z = transMatrix.z.y}
  data.position = {x = transMatrix.x.x, y = transMatrix.y.y, z = transMatrix.z.z}
  return data
end

function ReadPlayerData(player_addr)
  local data = {rotation = {}, position = {}}
  local cur_animation = read_u16(player_addr + 0xFA)
  local last_animation = read_u16(player_addr + 0xFC)
  local is_inside_drift = read_u8(player_addr + 0xFE)
  data.cur_animation = cur_animation
  data.last_animation = last_animation
  data.steering = read_float(player_addr + 0x58)
  data.steering_abs = read_float(player_addr + 0x54)
  data.is_inside_drift = is_inside_drift


  local transMatrix = read_matrix44(player_addr + 0x114)
  if not transMatrix then
    return {}
  end
  data.rotation.x = {x = transMatrix.w.w, y = transMatrix.x.w, z = transMatrix.y.w}
  data.rotation.y = {x = transMatrix.w.x, y = transMatrix.x.x, z = transMatrix.y.x}
  data.rotation.z = {x = transMatrix.w.y, y = transMatrix.x.y, z = transMatrix.y.y}
  data.position = {x = transMatrix.w.z, y = transMatrix.x.z, z = transMatrix.y.z}
  return data
end

function OGReadCameraData(camera_manager)
  local camera_address = camera_manager:pointer_chain(0x0) + 0x6C
  local proj_matrix = read_matrix44(camera_address + 0x30)
  -- local proj_matrix_v = read_float_vec(camera_address + 0x30, 12)
  if not proj_matrix then
    return {}
  end

  local data = {rotation = {}}
  data.rotation.x = {x = proj_matrix.w.y, y = proj_matrix.w.z, z = proj_matrix.x.w}
  data.rotation.y = {x = proj_matrix.x.y, y = proj_matrix.x.z, z = proj_matrix.y.w}
  data.rotation.z = {x = proj_matrix.y.y, y = proj_matrix.y.z, z = proj_matrix.w.w}
  return data
end

function ReadCameraData(camera_addr)
  local transform_matrix = read_matrix44(camera_addr + 0x34)
  if not transform_matrix then
    return {}
  end

  local data = {rotation = {}, position = {}}
  data.rotation.x = {x = transform_matrix.w.w, y = transform_matrix.w.x, z = transform_matrix.w.y}
  data.rotation.y = {x = transform_matrix.x.w, y = transform_matrix.x.x, z = transform_matrix.x.y}
  data.rotation.z = {x = transform_matrix.y.w, y = transform_matrix.y.x, z = transform_matrix.y.y}
  data.position = {x = transform_matrix.z.w, y = transform_matrix.z.x, z = transform_matrix.z.y}


  -- At the end of the race, the camera makes use of a different 'cinematic' fov.
  -- We check if this is valid, if so we use that instead of the regular fov.
  -- These values are seemingly out of bounds of the camera object, but they seem to always be at this offset
  local vec2 = read_float_vec(camera_addr + 0x388, 2)
  if tostring(vec2[1]) ~= "nan" then
    data.fov = vec2[2]
  else
    local fov = read_float(camera_addr + 0x11C)
    data.fov = fov
  end

  return data
end

function ReadKartMove(move_addr)
  local data = {}
  data.speed = read_float(move_addr + 0x20)
  data.mt_charge = read_u16(move_addr + 0xfe)
  data.smt_charge = read_u16(move_addr + 0x100)
  data.ssmt_charge = read_u16(move_addr + 0x14C)
  return data
end

function ReadRace(race_manager)
  local timer_addr = race_manager:pointer_chain(0x14)

  local data = {}
  data.frame_count = read_u32(race_manager + 0x20)
  data.minutes = read_u16(timer_addr + 0x8)
  data.seconds = read_u8(timer_addr + 0xA)
  data.milliseconds = read_u16(timer_addr + 0xC)
  -- 0x3 is FINISHED_RACE
  data.state = read_u32(race_manager + 0x28)

  return data
end

function ReadItemData(item_director)
  local inventory = item_director:pointer_chain(0x14) + 0x88
  local item_id = read_u32(inventory + 0x4)
  local item_count = read_u32(inventory + 0x8)
  local data = {id = item_id, count = item_count}
  return data
end

