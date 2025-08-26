local iconChar = MadObject.new()

local bg = Image.load("assets/character/iconChar/bg.png")
bg.name = "bg"
bg.size = Vector2.new(76, 79)
bg.sizeMode = SizeMode.CUT
iconChar:addChild(bg)

local borders = Image.load("assets/character/iconChar/borders.png")
borders.name = "borders"
iconChar:addChild(borders)

return iconChar