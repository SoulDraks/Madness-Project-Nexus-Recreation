Gif = {}
Gif.pos = Vector2.new()
Gif.pos_x = 0
Gif.pos_y = 0
Gif.size = Vector2.new()
Gif.size_x = 0
Gif.size_y = 0
Gif.scale = Vector2.new()
Gif.scale_x = 1
Gif.scale_y = 1
Gif.name = "Gif"
Gif.rotation = 0.0
Gif.visible = true
Gif.zIndex = 0
Gif.alpha = 255
Gif.flip = false
Gif.color = Color.new(255, 255, 255, 255)
Gif.color_r = 255
Gif.color_g = 255
Gif.color_b = 255
Gif.color_a = 255
Gif.childCount = 0
Gif.loop = true
Gif.playing = false

--- Will load a gif in the specified path.
---@param path string
function Gif.load(path)
    return Gif
end

-- Obtiene el padre del nodo dado.
function Gif:getParent() return Gif end

function Gif:addChild(newChild) end

---@param name string
---@overload fun(index: integer)
function Gif:getChild(name) return Gif end

--- Da un array con todos los hijos
---@return table
function Gif:getChildren() return {Gif, Gif, Gif} end

---@param name string
---@overload fun(index: integer)
---@overload fun(ref: userdata)
function Gif:removeChild(name) end

--- Libera manualmente al Gif actual al final del frame.
---@param freeAllChilds boolean indica si todos sus descendientes deben ser liberados o solo su padre.
function Gif:queueFree(freeAllChilds) end

---@return boolean
function Gif:isInsideTree() return true end

--- Will return the current frame.
--- @return integer
function Gif:getCurrentFrame()
    return 1
end

--- Will return the total frames of the gif.
--- @return integer
function Gif:getFramesCount()
    return 30
end

function Gif:addFrame()
end

---@param frame integer
function Gif:getFrame(frame)
end

---@param frame integer
function Gif:removeFrame(frame)
end

---@param frame integer
function Gif:gotoAndStop(frame)
end

---@param frame integer
function Gif:gotoAndPlay(frame)
end