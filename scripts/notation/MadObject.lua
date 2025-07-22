--- Clase base para todas las clases en el juego.
MadObject = {}
MadObject.pos = Vector2.new()
MadObject.pos_x = 0
MadObject.pos_y = 0
MadObject.size = Vector2.new()
MadObject.size_x = 0
MadObject.size_y = 0
MadObject.scale = Vector2.new()
MadObject.scale_x = 1
MadObject.scale_y = 1
MadObject.name = "MadObject"
MadObject.rotation = 0.0
MadObject.visible = true
MadObject.zIndex = 0
MadObject.alpha = 255
MadObject.flip = false
MadObject.color = Color.new(255, 255, 255, 255)
MadObject.color_r = 255
MadObject.color_g = 255
MadObject.color_b = 255
MadObject.color_a = 255
MadObject.childCount = 0

function MadObject.new() return MadObject end

-- Obtiene el padre del nodo dado.
function MadObject:getParent() return MadObject end

function MadObject:addChild(newChild) end

---@param name string
---@overload fun(index: integer)
function MadObject:getChild(name) return MadObject end

--- Da un array con todos los hijos
---@return table
function MadObject:getChildren() return {MadObject, MadObject, MadObject} end

function MadObject:removeChild(name) end

---@return boolean
function MadObject:isInsideTree() return true end

--- Libera manualmente al MadObject actual al final del frame.
---@param freeAllChilds boolean indica si todos sus descendientes deben ser liberados o solo su padre.
function MadObject:queueFree(freeAllChilds) end

_root = MadObject.new()
_root.UI = MadObject.new()
