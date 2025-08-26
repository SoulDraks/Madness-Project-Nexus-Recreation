local init = MadObject.new()
init.pos = Vector2.new(0, 0)
init.name = "init"

local background = Image.load("assets/menu/init/background.png")
background.name = "background"
background.pos = Vector2.new(-4, 0)
background.color_a = 58
background.sizeMode = SizeMode.ZOOM
init:addChild(background)

local background_animation = Animation.new()
background_animation.loop = true
background_animation:setFrames(9)
background_animation:addKeyFrame(1, background, "pos", Vector2.new(-4, 0))
background_animation:addKeyFrame(2, background, "pos", Vector2.new(-4, -2))
background_animation:addKeyFrame(3, background, "pos", Vector2.new(-4, -4))
background_animation:addKeyFrame(4, background, "pos", Vector2.new(-4, -6))
background_animation:addKeyFrame(5, background, "pos", Vector2.new(-4, -9))
background_animation:addKeyFrame(6, background, "pos", Vector2.new(-4, -11))
background_animation:addKeyFrame(7, background, "pos", Vector2.new(-4, -13))
background_animation:addKeyFrame(8, background, "pos", Vector2.new(-4, -15))
background_animation:addKeyFrame(9, background, "pos", Vector2.new(-4, -17))
background_animation:play()

local title_letters = Image.load("assets/menu/init/title.png")
title_letters.pos = Vector2.new(76, 103)
title_letters.scale = Vector2.new(1.31, 1.31)
title_letters.name = "title_letters"
init:addChild(title_letters)

--local title = Gif.load("assets/menu/init/frames.gif")
--title.playing = true
--title.name = "title"
--title.scale = Vector2.new(0.97, 0.97)
--init:addChild(title)
--title.globalPos = Vector2.new(-266, -42)

local launch_button_base = Image.load("assets/menu/init/launch2.png")
launch_button_base.name = "launch_button_base"
launch_button_base.pos = Vector2.new(350, 339)
launch_button_base.scale = Vector2.new(0.75, 0.75)
init:addChild(launch_button_base)

local launch_button = Button.new()
local launch_surface1 = Image.load("assets/menu/init/launch.png")
launch_surface1.name = "launch_surface1"
local launch_surface2 = Image.load("assets/menu/init/launch_on_hover.png")
launch_surface1.name = "launch_surface2"
launch_button.name = "launch_button"
launch_button.surface = launch_surface1
launch_button.size = launch_surface1.size
function launch_button:onReleased()
    _root.UI:removeChild(init.name)
    background_animation:stop()
    Pressed = true
    init:queueFree(true)
end
function launch_button:onHover()
    self.surface = launch_surface2
end
function launch_button:onLeave()
    self.surface = launch_surface1
end
launch_button_base:addChild(launch_button)
launch_button.globalScale = Vector2.new(1, 1)

local launch_arrows = Image.load("assets/menu/init/arrows.png")
launch_arrows.name = "launch_arrows"
launch_arrows.pos = Vector2.new(10, 23)
launch_arrows.scale = Vector2.new(0.99, 0.99)
launch_button:addChild(launch_arrows)

local launch_text = Label.new()
launch_text.name = "launch_text"
launch_text.text = "LAUNCH"
launch_text.color = Color.new("FFFFFFFF")
launch_text.pos = Vector2.new(17, 13)
launch_text.scale = Vector2.new(0.47, 0.47)
launch_text:loadFont("assets/fonts/Knockout HTF48-Featherweight.ttf", 100)
launch_button:addChild(launch_text)

local init_bar = Image.load("assets/menu/init/bar.png")
init_bar.name = "init_bar"
init_bar.pos = Vector2.new(337, 368)
init:addChild(init_bar)

local numbers = Image.load("assets/menu/init/numbers.png")
numbers.name = "numbers"
numbers.pos = Vector2.new(798, -80)
numbers.scale = Vector2.new(0.71, 0.71)
init:addChild(numbers)

return init