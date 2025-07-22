--- Vector2 to games
--- @class Vector2
--- @field x number
--- @field y number
Vector2 = {
    __index = function ()
        
    end
}
Vector2.x = 0.0
Vector2.y = 0.0
function Vector2.new(x, y)
    return Vector2
end

function Vector2:__add(other)
end

function Vector2:__subtract(other)
end

function Vector2:__multiply(scalar)
end

function Vector2:magnitude()
end

function Vector2:normalize()
end
