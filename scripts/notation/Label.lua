Label = {}
Label.pos = Vector2.new()
Label.pos_x = 0
Label.pos_y = 0
Label.size = Vector2.new()
Label.size_x = 0
Label.size_y = 0
Label.scale = Vector2.new()
Label.scale_x = 1
Label.scale_y = 1
Label.name = "Label"
Label.rotation = 0.0
Label.visible = true
Label.zIndex = 0
Label.alpha = 255
Label.flip = false
Label.color = Color.new(255, 255, 255, 255)
Label.color_r = 255
Label.color_g = 255
Label.color_b = 255
Label.color_a = 255
Label.childCount = 0
Label.text = ""
Label.fontPath = ""
Label.fontSize = 0

function Label.new() return Label end

-- Obtiene el padre del nodo dado.
function Label:getParent() return Label end

function Label:addChild(newChild) end

---@param name string
---@overload fun(index: integer)
function Label:getChild(name) return Label end
--- Da un array con todos los hijos
---@return table
function Label:getChildren() return {Label, Label, Label} end

---@param name string
---@overload fun(index: integer)
---@overload fun(ref: userdata)
function Label:removeChild(name) end

--- Libera manualmente el objeto actual al final del frame.
---@param freeAllChilds boolean indica si todos sus descendientes deben ser liberados o solo su padre.
function Label:queueFree(freeAllChilds) end

---@return boolean
function Label:isInsideTree() return true end

---@param path string
---@param fontSize integer
function Label:loadFont(path, fontSize) end

---@param size integer
function Label:changeFontSize(size) end

---@param glowColor Color
---@param intensity integer
---@param glowSize integer
function Label:applyGlowFilter(glowColor, intensity, glowSize) end


---@param shadowColor Color
---@param blurRadius integer
---@param shadowDistance number
---@param sigma number
function Label:applyShadowFilter(shadowColor, blurRadius, shadowDistance, sigma) end