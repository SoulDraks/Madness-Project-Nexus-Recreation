local MadnessWorldMap = MadObject.new()

-- Deshabilita todos los iconos que esten desbloqueados para que no puedan presionarse.
local function disableIconLevels(icons)
    for i = 1, icons.childCount do
        local icon = icons:getChild(i)
        if icon.unlocked then
            icon.disabled = true
        end
    end
end

-- Deshabilita todos los iconos que esten desbloqueados para que no puedan presionarse.
local function enableIconLevels(icons)
    for i = 1, icons.childCount do
        local icon = icons:getChild(i)
        if icon.unlocked then
            icon.disabled = false
        end
    end
end

local function pressProceed()
    enableIconLevels(MadnessWorldMap:getChild("world1"):getChild("icons"))
    MadnessWorldMap:getChild("arrowLevel"):getChild(1):stop()
end

local function createIconLevel(inPath, unlocked, borderSize)
    if borderSize == nil then
        borderSize = 3
    end
    local iconLevel = Button.new()
    local icon = Image.load(inPath)
    iconLevel.size = icon.size
    local redBorder = icon:makeGlowBorder(Color.new("FFFF0000"), 16, borderSize)
    local blackShadow = icon:makeImageColor(Color.new("B0000000"))
    local redShadow = icon:makeImageColor(Color.new("54FF2C2C"))
    redBorder.pos = (icon.size - redBorder.size) / 2
    -- Dejamos que por defecto, el boton del nivel este bloqueado
    redBorder.visible = false
    redShadow.visible = false
    blackShadow.visible = true
    iconLevel.disabled = true
    iconLevel.size = icon.size
    -- Nombremos a cada objeto
    icon.name = "icon"
    redBorder.name = "redBorder"
    blackShadow.name = "blackShadow"
    redShadow.name = "redShadow"
    iconLevel:addChild(redBorder)
    iconLevel:addChild(icon) -- Lo ponemos encima de el bordeado rojo porque de lo contrario, tapara al icono
    iconLevel:addChild(blackShadow)
    iconLevel:addChild(redShadow)
    iconLevel.unlocked = unlocked
    if unlocked then
        redBorder.visible = true
        blackShadow.visible = false
        iconLevel.disabled = false
    end
    function iconLevel:onReleased()
        disableIconLevels(MadnessWorldMap:getChild("world1"):getChild("icons"))
        MadnessPopup:addPopup("mission", "In order to bring about the end of the Nexus Project,\nHank has been hired to enlist the help of incarcerated\nagents, Sanford and Deimos.en hired to enlist the help of\nincarcerated agents, Sanford and Deimos.", nil, 0, 0, nil, 10, nil, nil)
    end
    function iconLevel:onHover()
        redShadow.visible = true
    end
    function iconLevel:onLeave()
        redShadow.visible = false
    end
    return iconLevel
end

-- Desbloquea el icono de nivel para que ahora sea jugable o seleccionable.
local function enableIconLevel(iconLevel, unlocked)
    iconLevel.unlocked = unlocked
    iconLevel.disabled = false
    iconLevel:getChild("redBorder").visible = true
    iconLevel:getChild("blackShadow").visible = false
end

-- Arrays que contiene los personajes que estaran en el nivel dado
local World_1 = {
    {"hank"}, -- Nivel 1-A
    {"sanford", "deimos"}, -- Nivel 1-B
    {"sanford", "deimos"}, -- Nivel 1-C
    {"sanford", "deimos"}, -- Nivel 1-D
    {"sanford", "deimos"}, -- Nivel 1-E
    {"sanford", "deimos"}, -- Nivel 1-F
    {"sanford", "deimos"}, -- Nivel 1-G
    {"hank", "sanford", "deimos"},   -- Nivel 1-H
}
local World_1_5 = {
    {"jesus2"}, -- Nivel 1.5-A
    {"jesus2"}, -- Nivel 1.5-B
    {"jesus2"}, -- Nivel 1.5-C
    {"jesus2"}, -- Nivel 1.5-D
    {"jesus2"}, -- Nivel 1.5-E
    {"jesus2"}, -- Nivel 1.5-F
}

local background = Image.load("assets/worldmap/background.png")
background.name = "background"
background.pos = Vector2.new(-37, -81)
MadnessWorldMap:addChild(background)

-- Creamos el titulo del nivel
local title = MadObject.new()
title.name = "titleWorldMap"
title.pos = Vector2.new(309, 18)
-- Label del titulo
local titleTxt = Label.new()
titleTxt:loadFont("assets/fonts/Knockout HTF48-Featherweight.ttf", 70)
titleTxt.name = "titleTxt"
titleTxt.scale = Vector2.new(0.95, 0.95)
titleTxt.color = Color.new("FFFF0000")
titleTxt.text = "EPISODE 1: PROJECT NEXUS"
titleTxt:applyShadowFilter(Color.new("FF000000"), 5, 10.0, 1)
titleTxt:applyGlowFilter(Color.new("FFC50000"), 20, 3)
titleTxt:applyOutline(Color.new("FF000000"), 2)
title:addChild(titleTxt)

-- Flechita brillante.
local arrows = MadObject.new()
arrows.name = "arrows"
local arrowNormal1 = Image.load("assets/worldmap/title/arrow.png")
local arrowNormal2 = Image.load("assets/worldmap/title/arrow.png")
local arrowLight1 = Image.load("assets/worldmap/title/arrowLight.png")
local arrowLight2 = Image.load("assets/worldmap/title/arrowLight.png")
arrowNormal1.name = "arrowNormal1"
arrowNormal1.pos = Vector2.new(499, 2)
arrowNormal1.scale = Vector2.new(0.95, 0.95)

arrowNormal2.name = "arrowNormal2"
arrowNormal2.pos = Vector2.new(-28, 1)
arrowNormal2.scale = Vector2.new(0.95, 0.95)
arrowNormal2.flip = true

arrowLight1.name = "arrowLight1"
arrowLight1.pos = Vector2.new(499, 2)
arrowLight1.scale = Vector2.new(0.95, 0.95)
arrowLight1.visible = false

arrowLight2.name = "arrowNormal2"
arrowLight2.pos = Vector2.new(-28, 1)
arrowLight2.scale = Vector2.new(0.95, 0.95)
arrowLight2.visible = false
arrowLight2.flip = true

-- Animacion de brillo
local arrowLightAnimation = Animation.new()
arrowLightAnimation.loop = true
arrowLightAnimation:setFrames(24)
arrowLightAnimation:addKeyFrame(1, arrowLight1, "visible", false)
arrowLightAnimation:addKeyFrame(1, arrowLight2, "visible", false)
arrowLightAnimation:addKeyFrame(12, arrowLight1, "visible", true)
arrowLightAnimation:addKeyFrame(12, arrowLight2, "visible", true)

arrows:addChild(arrowNormal1)
arrows:addChild(arrowNormal2)
arrows:addChild(arrowLight1)
arrows:addChild(arrowLight2)
arrows:addChild(arrowLightAnimation)

title:addChild(arrows)
MadnessWorldMap:addChild(title)

-- Creamos los iconos de los niveles.
local world1 = MadObject.new()
world1.visible = true
world1.name = "world1"
local world1_icons = MadObject.new()
world1_icons.name = "icons"
world1:addChild(world1_icons)
local level_1_A = createIconLevel("assets/worldmap/level_1_A.png", true)
level_1_A.name = "level_1_A"
level_1_A.pos = Vector2.new(115, 137)
world1_icons:addChild(level_1_A)
local level_1_B = createIconLevel("assets/worldmap/level_1_B.png", false)
level_1_B.name = "level_1_B"
level_1_B.pos = Vector2.new(73, 304)
world1_icons:addChild(level_1_B)
local level_1_C = createIconLevel("assets/worldmap/level_1_C.png", false)
level_1_C.name = "level_1_C"
level_1_C.pos = Vector2.new(396, 155)
world1_icons:addChild(level_1_C)
local level_1_D = createIconLevel("assets/worldmap/level_1_D.png", false)
level_1_D.name = "level_1_D"
level_1_D.pos = Vector2.new(542, 100)
world1_icons:addChild(level_1_D)
local level_1_E = createIconLevel("assets/worldmap/level_1_E.png", false)
level_1_E.name = "level_1_E"
level_1_E.pos = Vector2.new(223, 353)
world1_icons:addChild(level_1_E)
local level_1_F = createIconLevel("assets/worldmap/level_1_F.png", false)
level_1_F.name = "level_1_F"
level_1_F.pos = Vector2.new(384, 274)
world1_icons:addChild(level_1_F)
local level_1_G = createIconLevel("assets/worldmap/level_1_G.png", false)
level_1_G.name = "level_1_G"
level_1_G.pos = Vector2.new(657, 185)
world1_icons:addChild(level_1_G)
local level_1_H = createIconLevel("assets/worldmap/level_1_H.png", false)
level_1_H.name = "level_1_H"
level_1_H.pos = Vector2.new(585, 392)
world1_icons:addChild(level_1_H)
MadnessWorldMap:addChild(world1)

-- Hacemos la flechita del nivel.
local arrowLevel = MadObject.new() -- Contenedor para mover la flechita sin calcular su posicion.
arrowLevel.scale = Vector2.new(0.62, 0.62)
local arrow = Image.load("assets/worldmap/arrowLevel.png")
local redArrow = arrow:makeImageColor(Color.new("FFFF0000"))
redArrow.visible = false
arrowLevel:addChild(arrow)
arrowLevel:addChild(redArrow)
arrowLevel.name = "arrowLevel"
local arrowLevelAnimation = Animation.new()
arrowLevelAnimation.loop = true
arrowLevelAnimation:setFrames(28)
arrowLevelAnimation:addKeyFrame(1, arrow, "pos", Vector2.new(-20, -50), AnimationInterpolationType.EASEINOUT)
arrowLevelAnimation:addKeyFrame(1, redArrow, "pos", Vector2.new(-20, -50), AnimationInterpolationType.EASEINOUT)
arrowLevelAnimation:addKeyFrame(2, redArrow, "visible", true)
arrowLevelAnimation:addKeyFrame(5, redArrow, "visible", false)
arrowLevelAnimation:addKeyFrame(14, arrow, "pos", Vector2.new(-20, -30), AnimationInterpolationType.EASEINOUT)
arrowLevelAnimation:addKeyFrame(14, redArrow, "pos", Vector2.new(-20, -30), AnimationInterpolationType.EASEINOUT)
arrowLevelAnimation:addKeyFrame(25, redArrow, "visible", true)
arrowLevelAnimation:addKeyFrame(28, arrow, "pos", Vector2.new(-20, -50), AnimationInterpolationType.EASEINOUT)
arrowLevelAnimation:addKeyFrame(28, redArrow, "pos", Vector2.new(-20, -50), AnimationInterpolationType.EASEINOUT)
arrowLevelAnimation:addKeyFrame(28, redArrow, "visible", false)
arrowLevel:addChild(arrowLevelAnimation)
MadnessWorldMap:addChild(arrowLevel)

local btnProceedEpisode1_5 = Button.new()
local redBorder = Panel.new()
redBorder.borderColor = Color.new("FFFF0000")
redBorder.borderSize = 2
redBorder.visible = false
btnProceedEpisode1_5:addChild(redBorder)

function MadnessWorldMap:adjustMap()
    local unlockedLevels = {true, true, true, true, true, false, false, false}
    local icons = world1_icons:getChildren()
    for i = 1, #unlockedLevels do
        if icons[i] == nil then
            break
        end
        if unlockedLevels[i] and not icons[i].unlocked then -- ¿Estaba bloqueado pero ahora esta desbloqueado?
            enableIconLevel(icons[i], true)
        elseif unlockedLevels[i - 1] then -- ¿El nivel anterior fue completado?
            enableIconLevel(icons[i], false)
        end
    end
    local levelSelected = 1 
    -- Buscamos el ultimo nivel desbloqueado
    for i = #unlockedLevels, 1, -1 do
        if unlockedLevels[i] then
            levelSelected = i + 1
            break
        end
    end
    -- Ajustamos la flecha de nivel al ultimo nivel desbloqueado
    local iconPos = icons[levelSelected].pos
    local iconSize = icons[levelSelected].size
    arrowLevel.pos = Vector2.new(iconPos.x + iconSize.x / 2, iconPos.y)
    arrowLevelAnimation:play()
    arrowLightAnimation:play()
end

return MadnessWorldMap