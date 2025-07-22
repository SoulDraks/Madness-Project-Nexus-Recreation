local mission = MadObject.new()
mission.name = "mission"
mission.myType = "mission"
mission.pos = Vector2.new(265, 135)

local surface = Image.load("assets/popups/mission/surface.png")
surface.name = "surface"
mission:addChild(surface)

local redBar = Gif.load("assets/popups/mission/redBar.gif")
redBar.pos = Vector2.new(-99, -17)
redBar.playing = true
redBar.name = "redBar"
mission:addChild(redBar)

local closeBtn = Button.new()
local closeBtnNormalSurface = Image.load("assets/popups/mission/closeBtn/normalSurface.png")
local closeBtnPressedSurface = Image.load("assets/popups/mission/closeBtn/pressedSurface.png")
local X = Label.new()
closeBtn.name = "closeBtn"
closeBtn.surface = closeBtnNormalSurface
closeBtn.pos = Vector2.new(290, 11)
closeBtn.size = closeBtnNormalSurface.size
closeBtn.scale = Vector2.new(1.12, 1.12)
X.text = "X"
X.color = Color.new("FFFFFFFF")
X:loadFont("assets/fonts/Arial.ttf", 20)
X.pos = Vector2.new(6, 4)
X.scale = Vector2.new(0.6, 0.6)
closeBtn:addChild(X)
function closeBtn:onHover()
    self.surface = closeBtnPressedSurface
end
function closeBtn:onLeave()
    self.surface = closeBtnNormalSurface
end
function closeBtn:onPressed()
    MadnessPopup:pressNo()
end
mission:addChild(closeBtn)

return mission