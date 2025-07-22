-- Nueva version del codigo

local yesno = Image.load("assets/popups/yesno/surface.png")
yesno.pos = Vector2.new(259, 206)
yesno.name = "yesno"
yesno.myType = "yesno"

local borders = Image.load("assets/popups/yesno/borders.png")
borders.pos = Vector2.new(10, 11)
borders.name = "borders"
yesno:addChild(borders)

local title = Label.new()
title.pos = Vector2.new(46, 16)
title.scale = Vector2.new(1.06, 0.92)
title.text = "ARE YOU SURE?"
title.color = Color.new("FFFF0000")
title:loadFont("assets/fonts/Knockout HTF69-FullLiteweight.ttf", 50)
title.name = "title"
yesno:addChild(title)

local myTextField = Label.new()
myTextField.scale = Vector2.new(0.59, 0.64)
myTextField.text = "(you will lose all current progress)"
myTextField.color = Color.new("FFFFFFFF")
myTextField:loadFont("assets/fonts/Arial.ttf", 20)
myTextField.name = "myTextField"
yesno:addChild(myTextField)
yesno.myTextField = myTextField

local surfaces = {Image.load("assets/popups/yesno/button_surface.png"), Image.load("assets/popups/yesno/button_surface_pressed.png")}
local function onHover(self)
    self.surface = surfaces[2]
end
local function onLeave(self)
    self.surface = surfaces[1]
end
local cancelBtn = Button.new()
local cancelText = Label.new()
local cancelBtnArrow = Image.load("assets/popups/yesno/arrow.png")
local proceedBtn = Button.new()
local proceedText = Label.new()
local proceedBtnArrow = Image.load("assets/popups/yesno/arrow.png")

cancelBtn.pos = Vector2.new(43, 81)
cancelBtn.name = "cancelBtn"
cancelBtn.surface = surfaces[1]
cancelBtn.size = cancelBtn.surface.size
cancelBtn.onHover = onHover
cancelBtn.onLeave = onLeave
function cancelBtn:onReleased()
    MadnessPopup:pressNo()
end
cancelText.pos = Vector2.new(36, 12)
cancelText.scale = Vector2.new(0.74, 0.74)
cancelText.color = Color.new("FFFFFFFF")
cancelText.text = "CANCEL"
cancelText:loadFont("assets/fonts/Knockout HTF48-Featherweight.ttf", 40)
cancelBtnArrow.pos = Vector2.new(25, 16)
cancelBtnArrow.scale = Vector2.new(1.38, 1.38)
cancelBtnArrow.flip = true
cancelBtn:addChild(cancelText)
cancelBtn:addChild(cancelBtnArrow)

proceedBtn.pos = Vector2.new(173, 81)
proceedBtn.name = "proceedBtn"
proceedBtn.surface = surfaces[1]
proceedBtn.size = cancelBtn.surface.size
proceedBtn.onHover = onHover
proceedBtn.onLeave = onLeave
function proceedBtn:onReleased()
    MadnessPopup:pressYes(true)
end
proceedText.pos = Vector2.new(23, 12)
proceedText.scale = Vector2.new(0.74, 0.74)
proceedText.color = Color.new("FFFFFFFF")
proceedText.text = "PROCEED"
proceedText:loadFont("assets/fonts/Knockout HTF48-Featherweight.ttf", 40)
proceedBtnArrow.pos = Vector2.new(102, 16)
proceedBtnArrow.scale = Vector2.new(1.38, 1.38)
proceedBtn:addChild(proceedText)
proceedBtn:addChild(proceedBtnArrow)
yesno.buttons = {myNoBtn = cancelBtn, myYesBtn = proceedBtn}

yesno:addChild(cancelBtn)
yesno:addChild(proceedBtn)

return yesno