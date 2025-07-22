Animation = {}
Animation.pos = Vector2.new()
Animation.pos_x = 0
Animation.pos_y = 0
Animation.size = Vector2.new()
Animation.size_x = 0
Animation.size_y = 0
Animation.scale = Vector2.new()
Animation.scale_x = 1
Animation.scale_y = 1
Animation.name = "Animation"
Animation.rotation = 0.0
Animation.visible = true
Animation.zIndex = 0
Animation.alpha = 255
Animation.flip = false
Animation.color = Color.new(255, 255, 255, 255)
Animation.color_r = 255
Animation.color_g = 255
Animation.color_b = 255
Animation.color_a = 255
Animation.childCount = 0
Animation.loop = false
Animation.fps = 30
-- Atributo de solo lectura que verifica si se esta reproduciendo.
Animation.playing = false
-- Funcion que es llamada cuando la animacion termina.
Animation.onAnimationFinished = nil

function Animation.new()
    return Animation
end

-- Obtiene el padre del nodo dado.
function Animation:getParent() return Animation end

function Animation:addChild(newChild) end

---@param name string
---@overload fun(index: integer)
function Animation:getChild(name) return Animation end

--- Da un array con todos los hijos
---@return table
function Animation:getChildren() return {Animation, Animation, Animation} end

---@param name string
---@overload fun(index: integer)
---@overload fun(ref: userdata)
function Animation:removeChild(name) end

--- Libera manualmente al Animation actual al final del frame.
---@param freeAllChilds boolean indica si todos sus descendientes deben ser liberados o solo su padre.
function Animation:queueFree(freeAllChilds) end

---@return boolean
function Animation:isInsideTree() return true end

---@param inFrame integer
---@param target table
---@param property string
---@param value any
---@param interpolationType integer|nil
function Animation:addKeyFrame(inFrame, target, property, value, interpolationType) end
--- func desc
---@param inFrame integer
---@param inFunction function
function Animation:addFunctionKeyFrame(inFrame, inFunction) end
--- func desc
---@param inFrames integer
function Animation:setFrames(inFrames) end
function Animation:play() end
function Animation:playBackwards() end
function Animation:stop() end

AnimationInterpolationType = {
    NONE = 0,
    LINEAR = 1,
    QUADRATIC = 2,
    CUBIC = 3,
    SINUSOIDAL = 4,
    EASEINOUT = 5
}