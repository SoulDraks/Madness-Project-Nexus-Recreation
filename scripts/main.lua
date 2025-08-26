-- Invocamos a las "clases"
dofile("scripts/classes/MadnessPopup.lua")
dofile("scripts/classes/MadnessTransition.lua")
init = dofile("scripts/menu/init.lua")
local main_menu = dofile("scripts/menu/main_menu.lua")
MadnessWorldMap = dofile("scripts/menu/worldmap.lua")
_root.UI:addChild(init)
init = init:getChild("background")
CurrentTime = 0

local objeto = MadObject.new()
objeto.name = "Temporizador"
function objeto:tick(delta)
    if Pressed then
        CurrentTime = CurrentTime + delta
        if CurrentTime > 3 then
            CurrentTime = nil
            Pressed = nil
            _root.UI:addChild(main_menu)
            main_menu.play_animations()
            self.tick = Main
            init = main_menu:getChild("game_select"):getChild("buttons"):getChild("arena_btn")
        end
    end
end
_root:addChild(objeto)

Up = false
Down = false
Left = false
Right = false
VELOCIDAD = 30
function ManejarTeclas(KeyCode)
    if KeyCode == Key.W then
        Up = true
    end
    if KeyCode == Key.S then
        Down = true
    end
    if KeyCode == Key.A then
        Left = true
    end
    if KeyCode == Key.D then
        Right = true
    end
    if KeyCode == Key.SPACE then
        print(init.pos)
        --print(init.size)
        print(init.scale)
    end
    if KeyCode == Key.M then
        --init.size_x = init.size.x + 1
        --[[
        init.scale_x = init.scale.x + 0.01
        init.scale_y = init.scale.y + 0.01
        ]]
        init.rotation = init.rotation + 1
    end
    if KeyCode == Key.N then
        --init.size_x = init.size.x - 1
        --[[
        init.scale_x = init.scale.x - 0.01
        init.scale_y = init.scale.y - 0.01
        ]]
        init.rotation = init.rotation - 1
    end
    if KeyCode == Key.L then
        --init.size_y = init.size.y + 1
        --init.scale_y = init.scale.y - 0.01
    end
    if KeyCode == Key.K then
        --init.size_y = init.size.y - 1
        --init.scale_y = init.scale.y - 0.01
    end
end

function ManejarTeclasSoltadas(KeyCode)
    if KeyCode == Key.W then
        Up = false
    end
    if KeyCode == Key.S then
        Down = false
    end
    if KeyCode == Key.A then
        Left = false
    end
    if KeyCode == Key.D then
        Right = false
    end
end
KeyManager.addEventListener(ManejarTeclas, "DOWN")
KeyManager.addEventListener(ManejarTeclasSoltadas, "UP")

function Main(self, delta)
    if Up then
        init.pos_y = init.pos.y - VELOCIDAD * delta
    end
    if Down then
        init.pos_y = init.pos.y + VELOCIDAD * delta
    end
    if Left then
        init.pos_x = init.pos.x - VELOCIDAD * delta
    end
    if Right then
        init.pos_x = init.pos.x + VELOCIDAD * delta
    end
end
