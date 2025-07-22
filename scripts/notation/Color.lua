--- @class Color 
--- @field r integer
--- @field g integer
--- @field b integer
--- @field a integer
Color = {}
Color.r = 255
Color.g = 255
Color.b = 255
Color.a = 255

---@overload fun(r: number, g: number, b: number, a: number): Color
---@overload fun(hex: string): Color
function Color.new(...)  return Color end
