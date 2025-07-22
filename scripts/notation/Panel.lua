Panel = {}
Panel.pos = Vector2.new()
Panel.pos_x = 0
Panel.pos_y = 0
Panel.size = Vector2.new()
Panel.size_x = 0
Panel.size_y = 0
Panel.scale = Vector2.new()
Panel.scale_x = 1
Panel.scale_y = 1
Panel.name = "Panel"
Panel.rotation = 0.0
Panel.visible = true
Panel.zIndex = 0
Panel.alpha = 255
Panel.flip = false
Panel.color = Color.new(255, 255, 255, 255)
Panel.color_r = 255
Panel.color_g = 255
Panel.color_b = 255
Panel.color_a = 255
Panel.childCount = 0
Panel.borderColor = Color.new("FFFF0000")
Panel.borderSize = 0

function Panel.new() return Panel end

-- Obtiene el padre del nodo dado.
function Panel:getParent() return Panel end

function Panel:addChild(newChild) end

---@param name string
---@overload fun(index: integer)
function Panel:getChild(name) return Panel end
--- Da un array con todos los hijos
---@return table
function Panel:getChildren() return {Panel, Panel, Panel} end

---@param name string
---@overload fun(index: integer)
---@overload fun(ref)
function Panel:removeChild(name) end

---@return boolean
function Panel:isInsideTree() return true end

--- Libera manualmente el objeto actual al final del frame.
---@param freeAllChilds boolean indica si todos sus descendientes deben ser liberados o solo su padre.
function Panel:queueFree(freeAllChilds) end