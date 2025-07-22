Button = {}
Button.pos = Vector2.new()
Button.pos_x = 0
Button.pos_y = 0
Button.size = Vector2.new()
Button.size_x = 0
Button.size_y = 0
Button.scale = Vector2.new()
Button.scale_x = 1
Button.scale_y = 1
Button.name = "Button"
Button.rotation = 0.0
Button.visible = true
Button.zIndex = 0
Button.alpha = 255
Button.flip = false
Button.color = Color.new(255, 255, 255, 255)
Button.color_r = 255
Button.color_g = 255
Button.color_b = 255
Button.color_a = 255
Button.childCount = 0
Button.surface = MadObject
Button.disabled = false
Button.onPressed = nil
Button.onReleased = nil
Button.onHover = nil
Button.onLeave = nil

function Button.new()
    return Button
end

-- Obtiene el padre del nodo dado.
function Button:getParent() return Button end

function Button:addChild(newChild) end

---@param name string
---@overload fun(index: integer)
function Button:getChild(name) return Button end

--- Da un array con todos los hijos
---@return table
function Button:getChildren() return {Button, Button, Button} end

---@param name string
---@overload fun(index: integer)
---@overload fun(ref: userdata)
function Button:removeChild(name) end

--- Libera manualmente al Button actual al final del frame.
---@param freeAllChilds boolean indica si todos sus descendientes deben ser liberados o solo su padre.
function Button:queueFree(freeAllChilds) end

---@return boolean
function Button:isInsideTree() return true end