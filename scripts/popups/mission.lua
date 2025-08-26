local function createIconName(text)
    local iconName = MadObject.new()
    -- Label del Texto del nombre
    local label = Label.new()
    label.name = "Text"
    label.text = text
    label.color = Color.new("FFFFFFFF")
    label.pos_x = 5
    label.scale = Vector2.new(0.69, 0.68)
    label:loadFont("assets/fonts/Knockout HTF48-Featherweight.ttf", 20)
    iconName:addChild(label)
    -- Barra roja que aparece al lado del label
    local redBar = Panel.new()
    redBar.name = "redBar"
    redBar.size = Vector2.new(2, 10)
    redBar.color = Color.new("FFFF0000")
    iconName:addChild(redBar)
    return iconName
end

local mission = MadObject.new()
mission.name = "mission"
mission.myType = "mission"
mission.pos = Vector2.new(287, 120)

local surface = Image.load("assets/popups/mission/surface.png")
surface.name = "surface"
mission:addChild(surface)

local redBar = Gif.load("assets/popups/mission/redBar.gif")
redBar.pos = Vector2.new(-99, -20)
redBar.playing = true
redBar.name = "redBar"
mission:addChild(redBar)

local closeBtn = Button.new()
local closeBtnNormalSurface = Image.load("assets/popups/mission/closeBtn/normalSurface.png")
local closeBtnPressedSurface = Image.load("assets/popups/mission/closeBtn/pressedSurface.png")
local X = Label.new()
closeBtn.name = "closeBtn"
closeBtn.surface = closeBtnNormalSurface
closeBtn.pos = Vector2.new(288, 8)
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
function closeBtn:onReleased()
    MadnessPopup:pressNo()
end
mission:addChild(closeBtn)

local icon1 = dofile("scripts/character/iconChar.lua")
icon1.name = "icon1"
icon1.pos = Vector2.new(32, 152)
icon1.scale = Vector2.new(0.77, 0.77)
mission:addChild(icon1)

local icon2 = dofile("scripts/character/iconChar.lua")
icon2.name = "icon2"
icon2.pos = Vector2.new(108, 152)
icon2.scale = Vector2.new(0.77, 0.77)
mission:addChild(icon2)

local icon3 = dofile("scripts/character/iconChar.lua")
icon3.name = "icon3"
icon3.pos = Vector2.new(170, 152)
icon3.scale = Vector2.new(0.77, 0.77)
mission:addChild(icon3)

local icon4 = dofile("scripts/character/iconChar.lua")
icon4.name = "icon4"
icon4.pos = Vector2.new(232, 152)
icon4.scale = Vector2.new(0.77, 0.77)
mission:addChild(icon4)

-- Nombres de los personajes
local charName1 = createIconName("Hank")
charName1.pos = Vector2.new(32, 217)
charName1.name = "charName1"
mission:addChild(charName1)

local charName2 = createIconName("Sanford")
charName2.pos = Vector2.new(106, 217)
charName2.name = "charName2"
mission:addChild(charName2)

local bar1 = Panel.new()
bar1.name = "bar1"
bar1.pos = Vector2.new(21, 131)
bar1.size = Vector2.new(285, 1)
bar1.color = Color.new("FFFF0000")
mission:addChild(bar1)

local bar2 = Panel.new()
bar2.name = "bar2"
bar2.pos = Vector2.new(21, 234)
bar2.size = Vector2.new(285, 1)
bar2.color = Color.new("FFFF0000")
mission:addChild(bar2)

local myTitle = Label.new()
myTitle.name = "myTitle"
myTitle.pos = Vector2.new(15, 12)
myTitle.scale = Vector2.new(0.88, 0.88)
myTitle.text = "[1-G] BAR"
myTitle.color = Color.new("FFFF0000")
myTitle:loadFont("assets/fonts/Knockout HTF69-FullLiteweight.ttf", 40)
mission:addChild(myTitle)

local myTextField = Label.new()
myTextField.name = "myTextField"
myTextField.pos = Vector2.new(23, 56)
myTextField.scale = Vector2.new(0.57, 0.57)
myTextField.color = Color.new("FFFFFFFF")
myTextField:loadFont("assets/fonts/Arial.ttf", 20)
mission:addChild(myTextField)

local borders = MadObject.new()
borders.name = "borders"
mission:addChild(borders)

local border1 = Panel.new()
border1.color = Color.new("FFFF0000")
border1.pos = Vector2.new(8, 6)
border1.size = Vector2.new(8, 1)
borders:addChild(border1)

local border2 = Panel.new()
border2.color = Color.new("FFFF0000")
border2.pos = Vector2.new(8, 6)
border2.size = Vector2.new(1, 8)
borders:addChild(border2)

local border3 = Panel.new()
border3.color = Color.new("FFFF0000")
border3.pos = Vector2.new(306, 6)
border3.size = Vector2.new(8, 1)
borders:addChild(border3)

local border4 = Panel.new()
border4.color = Color.new("FFFF0000")
border4.pos = Vector2.new(313, 6)
border4.size = Vector2.new(1, 8)
borders:addChild(border4)

local border5 = Panel.new()
border5.color = Color.new("FFFF0000")
border5.pos = Vector2.new(306, 278)
border5.size = Vector2.new(8, 1)
borders:addChild(border5)

local border6 = Panel.new()
border6.color = Color.new("FFFF0000")
border6.pos = Vector2.new(313, 270)
border6.size = Vector2.new(1, 8)
borders:addChild(border6)

local border7 = Panel.new()
border7.color = Color.new("FFFF0000")
border7.pos = Vector2.new(9, 278)
border7.size = Vector2.new(8, 1)
borders:addChild(border7)

local border8 = Panel.new()
border8.color = Color.new("FFFF0000")
border8.pos = Vector2.new(9, 270)
border8.size = Vector2.new(1, 8)
borders:addChild(border8)

local difficultyTxt = Label.new()
difficultyTxt.name = "difficultyTxt"
difficultyTxt.text = "DIFFICULTY"
difficultyTxt.color = Color.new("FFCC0000")
difficultyTxt.pos = Vector2.new(196, 238)
difficultyTxt.scale = Vector2.new(0.81, 0.81)
difficultyTxt:loadFont("assets/fonts/Knockout HTF49-Liteweight.ttf", 18)
mission:addChild(difficultyTxt)

local proceedBtn = Button.new()
local proceedBtnSurface = Image.load("assets/buttons/proceed_normal.png")
local proceedBtnSurfacePressed = Image.load("assets/buttons/proceed_hover.png")
local proceedTxt = Label.new()
local proceedArrow = Image.load("assets/buttons/arrow.png")

proceedBtn.name = "proceedBtn"
proceedBtn.pos = Vector2.new(31, 239)
proceedBtn.scale = Vector2.new(0.77, 0.77)
proceedBtn.surface = proceedBtnSurface
proceedBtn.size = proceedBtnSurface.size
function proceedBtn:onHover()
    self.surface = proceedBtnSurfacePressed
end
function proceedBtn:onLeave()
    self.surface = proceedBtnSurface
end
function proceedBtn:onReleased()
    MadnessPopup:pressYes(true)
end

proceedTxt.text = "PROCEED"
proceedTxt.color = Color.new("FFFFFFFF")
proceedTxt.pos = Vector2.new(20, 10)
proceedTxt.scale = Vector2.new(0.75, 0.75)
proceedTxt:loadFont("assets/fonts/Knockout HTF48-Featherweight.ttf", 40)
proceedBtn:addChild(proceedTxt)

proceedArrow.pos = Vector2.new(101, 13)
proceedArrow.scale = Vector2.new(1.38, 1.38)
proceedBtn:addChild(proceedArrow)

mission:addChild(proceedBtn)

return mission