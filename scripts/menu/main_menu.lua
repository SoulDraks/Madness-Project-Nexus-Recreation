-- <-------- Nueva Version del Codigo. -------->

-- Configura un boton automaticamente de la seleccion de modos de juego.
local function setup_button(button, surfaces, pos)
    button.surface = surfaces[1]
    button.size = surfaces[1].size
    button.pos = pos
    button.scale = Vector2.new(0.67, 0.67)
    function button:onHover()
        if not button.has_been_pressed then
            self.surface = surfaces[2]
        end
    end
    function button:onLeave()
        if not button.has_been_pressed then
            self.surface = surfaces[1]
        end
    end
end

-- Configura el texto de un boton de la seleccion de modos de juego.
local function setup_label(label, text, pos)
    label.pos = pos
    label.text = text
    label.scale = Vector2.new(0.51, 0.51)
    label.color = Color.new("FFFFFFFF")
    label:loadFont("assets/fonts/font_game_select.ttf", 70)
end

-- Configura un boton 
local function setup_button_sub_menu(button, surface, text, border, border_animation)
    surface.text = text
    surface:loadFont("assets/fonts/font_game_select.ttf", 35)
    button.surface = surface
    button.color = Color.new("FFF62F2F")
    button.size = surface.size
    function button:onHover()
        self.color = Color.new("FFFFFFFF")
        border.globalPos = self.globalPos
        border.pos_x = border.pos.x - 5
        border.size_x = self.size.x + 10
        border_animation:play()
    end
    function button:onLeave()
        self.color = Color.new("FFF62F2F")
        border_animation:stop()
        border.visible = false
    end
end

local main_menu = MadObject.new()
main_menu.name = "main_menu"

-- Seccion del Fondo del menu
local background = MadObject.new()
background.name = "background"
main_menu:addChild(background)

local bg = Image.load("assets/menu/main/bg.png")
bg.name = "bg"
bg.pos = Vector2.new(-14, 0.96)
bg.size = Vector2.new(931, 572)
background:addChild(bg)

local grid = Image.load("assets/menu/main/grid.png")
grid.name = "grid"
grid.pos = Vector2.new(0.06, 4.1)
grid.scale_y = 1.1
grid.color_a = 45
background:addChild(grid)

local title = Image.load("assets/menu/main/title.png")
title.name = "title"
title.pos = Vector2.new(231, 100)
title.scale = Vector2.new(0.69, 0.69)
background:addChild(title)

local lines_numbers = Image.load("assets/menu/main/lines_numbers/line.png")
lines_numbers.name = "lines_numbers"
lines_numbers.pos = Vector2.new(751, -129)
lines_numbers.size_x = lines_numbers.size.x + 2
background:addChild(lines_numbers)

local numbers = MadObject.new() -- Contenedor de los numeros
numbers.name = "numbers"
lines_numbers:addChild(numbers)

local numbers1 = Image.load("assets/menu/main/lines_numbers/numbers.png")
numbers1.name = "numbers1"
numbers1.pos = Vector2.new(-26, 57)
numbers1.scale = Vector2.new(0.65, 0.65)
numbers:addChild(numbers1)

local numbers2 = Image.load("assets/menu/main/lines_numbers/numbers.png")
numbers2.name = "numbers2"
numbers2.pos = Vector2.new(-26, -576)
numbers2.scale = Vector2.new(0.65, 0.65)
numbers:addChild(numbers2)

local panel = Panel.new()
panel.name = "panel"
panel.size = Vector2.new(850, 550)
panel.color = Color.new("FF000000")
background:addChild(panel)

-- Seccion del "game_select".
local game_select = MadObject.new() -- Contenedor de todos los otros objetos.
game_select.name = "game_select"
main_menu:addChild(game_select)

local lines_bg = Image.load("assets/menu/main/game_select/lines_bg.png")
lines_bg.name = "lines_bg"
lines_bg.pos_y = 300
lines_bg.scale = Vector2.new(1, 0.02)
lines_bg.visible = false
game_select:addChild(lines_bg)

local line1 = Image.load("assets/menu/main/game_select/line.png")
line1.name = "line1"
line1.pos = Vector2.new(882, 300)
game_select:addChild(line1)

local line2 = Image.load("assets/menu/main/game_select/line.png")
line2.name = "line2"
line2.pos = Vector2.new(882, 300)
game_select:addChild(line2)

local bg_civs = MadObject.new()
bg_civs.name = "bg_civs"
bg_civs.pos = Vector2.new(0, 258)
bg_civs.visible = false
game_select:addChild(bg_civs)

local bg_civs1 = Image.load("assets/menu/main/game_select/bg_civs.png")
bg_civs1.name = "bg_civs1"
bg_civs:addChild(bg_civs1)

local bg_civs2 = Image.load("assets/menu/main/game_select/bg_civs.png")
bg_civs2.name = "bg_civs2"
bg_civs2.pos_x = 1004
bg_civs:addChild(bg_civs2)

local panel_red = Panel.new()
panel_red.name = "panel_red"
panel_red.color = Color.new(255, 0, 0, 255)
panel_red.visible = false
panel_red.size = lines_bg.size
panel_red.pos = Vector2.new(0, 224)
game_select:addChild(panel_red)

-- Seccion de los botones de los modos de juego.
local buttons = MadObject.new() -- Contenedor de los botones.
buttons.name = "buttons"
game_select:addChild(buttons)

local story_btn = Button.new()
local story_btn_normal = Image.load("assets/menu/main/game_select/story_mode.png")
local story_btn_hovered = Image.load("assets/menu/main/game_select/story_mode_hovered.png")
local story_btn_pressed = Image.load("assets/menu/main/game_select/story_mode_pressed.png")
story_btn.name = "story_btn"
setup_button(story_btn, {story_btn_normal, story_btn_hovered}, Vector2.new(471, 227))
local story_text = Label.new()
story_text.name = "story_text"
setup_label(story_text, "STORY MODE", Vector2.new(15, 17))
story_btn:addChild(story_text)
buttons:addChild(story_btn)

local arena_btn = Button.new()
local arena_btn_normal = Image.load("assets/menu/main/game_select/arena_combat.png")
local arena_btn_hovered = Image.load("assets/menu/main/game_select/arena_combat_hovered.png")
local arena_btn_pressed = Image.load("assets/menu/main/game_select/arena_combat_pressed.png")
arena_btn.name = "arena_btn"
setup_button(arena_btn, {arena_btn_normal, arena_btn_hovered}, Vector2.new(579, 227))
local arena_text = Label.new()
arena_text.name = "arena_text"
setup_label(arena_text, "ARENA COMBAT", Vector2.new(15, 17))
arena_btn:addChild(arena_text)
buttons:addChild(arena_btn)

local game_opt_btn = Button.new()
local game_opt_btn_normal = Image.load("assets/menu/main/game_select/game_options.png")
local game_opt_btn_hovered = Image.load("assets/menu/main/game_select/game_options_hovered.png")
game_opt_btn.name = "game_opt_btn"
setup_button(game_opt_btn, {game_opt_btn_normal, game_opt_btn_hovered}, Vector2.new(582, 435))
local game_opt_text = Label.new()
game_opt_text.name = "game_opt_text"
setup_label(game_opt_text, "GAME OPTIONS", Vector2.new(15, 17))
game_opt_text.name = "game_opt_text"
game_opt_btn:addChild(game_opt_text)
buttons:addChild(game_opt_btn)
buttons.visible = false

-- Hacemos los submenus para cada modo de juego.
local sub_menus = MadObject.new() -- Contenedor de los sub menus.
sub_menus.name = "sub_menus"
game_select:addChild(sub_menus)

-- Sub menu del modo "STORY MODE"
local story_mode_sub = MadObject.new()
story_mode_sub.name = "story"
story_mode_sub.visible = false
sub_menus:addChild(story_mode_sub)

local border_texts = Image.load("assets/menu/main/game_select/sub_menus/border_texts.png")
border_texts.visible = false
border_texts.name = "border_texts"
local border_texts_anim = Animation.new() -- Esta animacion se hace al final. 

local story_lines_buttons = Image.load("assets/menu/main/game_select/sub_menus/options_lines_story.png")
story_lines_buttons.name = "lines_buttons"
story_lines_buttons.pos = Vector2.new(501, 261)
story_lines_buttons.scale = Vector2.new(0.68, 0.68)
local story_new_game_btn = Button.new()
story_new_game_btn.pos = Vector2.new(379, 294)
local story_continue_btn = Button.new()
story_continue_btn.pos = Vector2.new(384, 346)
local story_more_madness_btn = Button.new()
story_more_madness_btn.pos = Vector2.new(328, 397)
story_mode_sub:addChild(story_lines_buttons)
story_mode_sub:addChild(story_new_game_btn)
story_mode_sub:addChild(story_continue_btn)
story_mode_sub:addChild(story_more_madness_btn)
setup_button_sub_menu(story_new_game_btn, Label.new(), "NEW GAME", border_texts, border_texts_anim)
story_new_game_btn.surface.name = "story_new_game_btn"
setup_button_sub_menu(story_continue_btn, Label.new(), "CONTINUE", border_texts, border_texts_anim)
setup_button_sub_menu(story_more_madness_btn, Label.new(), "MORE MADNESS", border_texts, border_texts_anim)
story_more_madness_btn.color = Color.new("FF272826")
story_more_madness_btn.onHover = nil
story_more_madness_btn.onLeave = nil
story_new_game_btn.name = "story_new_game_btn"

-- Sub menu del modo "ARENA COMBAT"
local arena_sub = MadObject.new()
arena_sub.name = "arena"
arena_sub.visible = false
sub_menus:addChild(arena_sub)

local arena_lines_buttons = Image.load("assets/menu/main/game_select/sub_menus/options_lines_arena.png")
arena_lines_buttons.pos = Vector2.new(504, 262)
arena_lines_buttons.scale = Vector2.new(1.01, 1.01)
local arena_new_game_btn = Button.new()
arena_new_game_btn.pos = Vector2.new(379, 294)
local arena_continue_btn = Button.new()
arena_continue_btn.pos = Vector2.new(384, 346)
arena_sub:addChild(arena_lines_buttons)
arena_sub:addChild(arena_new_game_btn)
arena_sub:addChild(arena_continue_btn)
setup_button_sub_menu(arena_new_game_btn, Label.new(), "NEW GAME", border_texts, border_texts_anim)
setup_button_sub_menu(arena_continue_btn, Label.new(), "CONTINUE", border_texts, border_texts_anim)

local function disableButtons()
    local children = buttons:getChildren()
    for i = 1, #children do
        children[i].disabled = true
    end
end

local function enableButtons()
    local children = buttons:getChildren()
    for i = 1, #children do
        children[i].disabled = false
    end
end

-- Configuramos lo que hace al presionar los botones "STORY MODE", "ARENA COMBAT" y "GAME OPTIONS"
function story_btn:onReleased()
    self.surface = story_btn_pressed
    story_text.color = Color.new("FF000000")
    self.has_been_pressed = true
    story_mode_sub.visible = true
    if arena_btn.has_been_pressed then
        arena_btn.has_been_pressed = false
        arena_btn.surface = arena_btn_normal
        arena_text.color = Color.new("FFFFFFFF")
        arena_sub.visible = false
    end
end

function arena_btn:onReleased()
    self.surface = arena_btn_pressed
    arena_text.color = Color.new("FF000000")
    self.has_been_pressed = true
    arena_sub.visible = true
    if story_btn.has_been_pressed then
        story_btn.has_been_pressed = false
        story_btn.surface = story_btn_normal
        story_text.color = Color.new("FFFFFFFF")
        story_mode_sub.visible = false
    end
end

local function pressYesStory()
    MadnessTransition:createTransition("fade", function()
        _root.UI:removeChild(main_menu.name)
        _root.UI:addChild(MadnessWorldMap)
        MadnessWorldMap:adjustMap()
        init = MadnessWorldMap:getChild("XD")
    end)
end

local function pressNoStory()
    enableButtons()
    story_new_game_btn.disabled = false
    story_continue_btn.disabled = false
end

local function pressYesArena()
end

local function pressNoArena()
    enableButtons()
    arena_new_game_btn.disabled = false
    arena_continue_btn.disabled = false
end

local function showWorldMap()
    
end

-- Definimos las acciones de los botones de los sub menus
function story_new_game_btn:onPressed()
    disableButtons()
    self.disabled = true
    story_continue_btn.disabled = true
    MadnessPopup:addPopup("yesno", "You\'re about to ruin the entirety of your data.", nil, 425, 180, main_menu, -1, pressYesStory, pressNoStory)
end

function arena_new_game_btn:onPressed()
    disableButtons()
    self.disabled = true
    arena_continue_btn.disabled = true
    MadnessPopup:addPopup("yesno", "You\'re about to ruin the entirety of your data.", nil, 425, 180, main_menu, -1, pressYesArena, pressNoArena)
end

sub_menus:addChild(border_texts)

local border = Image.load("assets/menu/main/border.png")
border.name = "border"
border.scale_y = 1.05
main_menu:addChild(border)

-- Hacemos finalmente las animaciones.
local init_animation = Animation.new()
local bg_civs_anim = Animation.new()
local lines_numbers_anim = Animation.new()

-- Hacemos primero la animacion de inicio.
init_animation:setFrames(25)
init_animation:addFunctionKeyFrame(1, function ()
    buttons.visible = false
    story_mode_sub.visible = false
    arena_sub.visible = false
    story_btn.surface = story_btn_normal
    story_btn.has_been_pressed = false
    arena_btn.surface = arena_btn_normal
    arena_btn.has_been_pressed = false
    bg_civs.visible = false
    lines_bg.visible = false
    lines_bg.pos_y = 300
    lines_bg.scale_x, lines_bg.scale_y = 1, 0.02
    panel_red.visible = false
    line1.pos = Vector2.new(882, 300)
    line2.pos = Vector2.new(882, 300)
end)
init_animation:addKeyFrame(1, panel, "color_a", 255, AnimationInterpolationType.LINEAR)
init_animation:addKeyFrame(7, panel, "color_a", 0, AnimationInterpolationType.LINEAR)
init_animation:addKeyFrame(8, line1, "pos", Vector2.new(882, 300), AnimationInterpolationType.SINUSOIDAL)
init_animation:addKeyFrame(8, line2, "pos", Vector2.new(882, 300), AnimationInterpolationType.SINUSOIDAL)
init_animation:addKeyFrame(14, line1, "pos", Vector2.new(-44, 300), AnimationInterpolationType.SINUSOIDAL)
init_animation:addKeyFrame(14, line2, "pos", Vector2.new(-44, 300), AnimationInterpolationType.SINUSOIDAL)
init_animation:addKeyFrame(14, lines_bg, "visible", true)
init_animation:addKeyFrame(15, lines_bg, "pos", Vector2.new(0, 300), AnimationInterpolationType.SINUSOIDAL)
init_animation:addKeyFrame(15, lines_bg, "scale", Vector2.new(1, 0.02), AnimationInterpolationType.SINUSOIDAL)
init_animation:addKeyFrame(15, line1, "visible", true)
init_animation:addKeyFrame(15, line2, "visible", true)
init_animation:addKeyFrame(15, line1, "pos", Vector2.new(-44, 300), AnimationInterpolationType.SINUSOIDAL)
init_animation:addKeyFrame(15, line2, "pos", Vector2.new(-44, 300), AnimationInterpolationType.SINUSOIDAL)
init_animation:addKeyFrame(20, line1, "pos", Vector2.new(-44, 224), AnimationInterpolationType.SINUSOIDAL)
init_animation:addKeyFrame(20, line2, "pos", Vector2.new(-44, 473), AnimationInterpolationType.SINUSOIDAL)
init_animation:addKeyFrame(20, lines_bg, "pos", Vector2.new(0, 224), AnimationInterpolationType.SINUSOIDAL)
init_animation:addKeyFrame(20, lines_bg, "scale", Vector2.new(1, 1), AnimationInterpolationType.SINUSOIDAL)
init_animation:addKeyFrame(20, panel_red, "visible", true)
init_animation:addKeyFrame(20, buttons, "visible", true)
init_animation:addKeyFrame(20, bg_civs, "visible", true)
init_animation:addFunctionKeyFrame(20, function ()
    bg_civs_anim:play()
end)
init_animation:addKeyFrame(21, panel_red, "visible", false)
init_animation:addKeyFrame(22, panel_red, "visible", true)
init_animation:addKeyFrame(22, panel_red, "color_a", 255, AnimationInterpolationType.LINEAR)
init_animation:addKeyFrame(25, panel_red, "color_a", 0, AnimationInterpolationType.LINEAR)

-- Ahora hacemos la animacion de las lineas de numeros del fondo.
lines_numbers_anim.loop = true
lines_numbers_anim:setFrames(438)
lines_numbers_anim:addKeyFrame(1, numbers, "pos", Vector2.new(0, 0), AnimationInterpolationType.LINEAR)
lines_numbers_anim:addKeyFrame(438, numbers, "pos", Vector2.new(0, 632), AnimationInterpolationType.LINEAR)

-- Despues hacemos la animacion de los grunts que estan al fondo.
bg_civs_anim.loop = true
bg_civs_anim:setFrames(362)
bg_civs_anim:addKeyFrame(1, bg_civs, "pos", Vector2.new(0, 258), AnimationInterpolationType.LINEAR)
bg_civs_anim:addKeyFrame(362, bg_civs, "pos", Vector2.new(-1004, 258), AnimationInterpolationType.LINEAR)

-- Finalmente hacemos la animacion de los bordes de los textos
border_texts_anim.loop = true
border_texts_anim:addKeyFrame(1, border_texts, "visible", true)
border_texts_anim:addKeyFrame(4, border_texts, "visible", false)

main_menu.play_animations = function()
    init_animation:play()
    lines_numbers_anim:play()
end

return main_menu
