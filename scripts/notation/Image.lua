SizeMode = {
    STRETCH = 0,
    ZOOM = 1,
    CUT = 2
}

Image = {}
Image.pos = Vector2.new()
Image.pos_x = 0
Image.pos_y = 0
Image.size = Vector2.new()
Image.size_x = 0
Image.size_y = 0
Image.scale = Vector2.new()
Image.scale_x = 1
Image.scale_y = 1
Image.name = "Image"
Image.rotation = 0.0
Image.visible = true
Image.zIndex = 0
Image.alpha = 255
Image.flip = false
Image.color = Color.new(255, 255, 255, 255)
Image.color_r = 255
Image.color_g = 255
Image.color_b = 255
Image.color_a = 255
Image.childCount = 0
Image.path = "path.png"
Image.sizeMode = SizeMode.STRETCH

--- Genera una instancia de Image de la imagen en la ruta especificada.
---@param path string
function Image.load(path)
    return Image
end

--- Generada una imagen de ruido con el tamaño indicado
---@param width number
---@param height number
---@overload fun(size: Vector2)
function Image.noise(width, height) return Image end

-- Obtiene el padre del nodo dado.
function Image:getParent() return Image end

function Image:addChild(newChild) end

---@param name string
---@overload fun(index: integer)
function Image:getChild(name) return Image end
--- Da un array con todos los hijos
---@return table
function Image:getChildren() return {Image, Image, Image} end

---@param name string
---@overload fun(index: integer)
---@overload fun(ref: table)
function Image:removeChild(name) end

--- Libera manualmente al Image actual al final del frame.
---@param freeAllChilds boolean indica si todos sus descendientes deben ser liberados o solo su padre.
function Image:queueFree(freeAllChilds) end

---@return boolean
function Image:isInsideTree() return true end

--- Genera una nueva imagen que actua como bordeado de la imagen original.
---@param colorBorder Color
---@param intensity integer el valor debe ser de 0 a 100
---@param borderSize integer
---@return table
function Image:makeGlowBorder(colorBorder, intensity, borderSize) return Image end

--- Genera una nueva imagen pero le aplica un tinte o transforma todos sus pixeles con el color indicado
---@param color Color
---@return table
function Image:makeImageColor(color) return Image end