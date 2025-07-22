MadnessTransition = MadObject.new()
MadnessTransition.transitionLock = false -- Bandera para saber si hay una transicion en proceso.
MadnessTransition.currentTransition = nil
MadnessTransition.myPhase = 0
MadnessTransition.myAction = function() end

-- Creamos las transcisiones
local transitions = {}

local function fadeTransition()
    local fade = Animation.new()
    fade.name = "fade"

    local blackPanel = Panel.new()
    blackPanel.color = Color.new("00000000")
    blackPanel.size = Window.size
    fade:addChild(blackPanel)

    -- Animacion de la transicion.
    fade:setFrames(28)
    fade:addKeyFrame(1, blackPanel, "color_a", 0)
    fade:addKeyFrame(8, blackPanel, "color_a", 0xFF)
    fade:addKeyFrame(8, MadnessTransition, "myPhase", 1, AnimationInterpolationType.NONE)
    fade:addKeyFrame(9, blackPanel, "color_a", 0xFF)
    fade:addKeyFrame(25, blackPanel, "color_a", 0x00)
    function fade:onAnimationFinished()
        blackPanel.color_a = 0
        MadnessTransition:completeTransition()
    end

    return fade
end

local function endgameTransition()
    local endgame = Animation.new()
    endgame.name = "endgame"

    -- Panel oscuro del fondo.
    local blackPanel = Panel.new()
    blackPanel.name = "blackPanel"
    blackPanel.color = Color.new("FF000000")
    blackPanel.size = Window.size
    blackPanel.visible = false
    endgame:addChild(blackPanel)

    -- Imagenes de estatica.
    local static = MadObject.new() -- Contenedor de las imagenes de estatica
    static.name = "static"
    static.color_a = 144
    local staticImgs = {} -- Array auxiliar para poder manipular las imagenes sin acceder manualmente a los hijos de "static"
    for i = 1, 3 do
        staticImgs[i] = Image.noise(Window.size)
        staticImgs[i].visible = false
        staticImgs[i].name = "staticImg" .. i
        static:addChild(staticImgs[i])
    end
    local redPanel = Panel.new() -- Panel rojo para hacer que la estatica se vea roja.
    redPanel.name = "redPanel"
    redPanel.color = Color.new("84FF0000")
    redPanel.size = Window.size
    redPanel.color_a = 77
    static:addChild(redPanel)
    endgame:addChild(static)

    -- Animacion de la estatica.
    local staticAnim = Animation.new()
    staticAnim.name = "staticAnim"
    staticAnim:setFrames(3)
    staticAnim.loop = true
    staticAnim:addKeyFrame(1, staticImgs[3], "visible", false)
    staticAnim:addKeyFrame(1, staticImgs[1], "visible", true)
    staticAnim:addKeyFrame(2, staticImgs[1], "visible", false)
    staticAnim:addKeyFrame(2, staticImgs[2], "visible", true)
    staticAnim:addKeyFrame(3, staticImgs[2], "visible", false)
    staticAnim:addKeyFrame(3, staticImgs[3], "visible", true)

    endgame:addChild(staticAnim)

    -- Animacion de la transcision.
    endgame:setFrames(94)
    endgame:addFunctionKeyFrame(1, function()
        blackPanel.visible = false
        static.visible = true
        static.color_a = 0x75
        redPanel.color_a = 0x84
        staticAnim:play()
    end)
    endgame:addKeyFrame(4, static, "visible", false)
    endgame:addKeyFrame(10, static, "visible", true)
    endgame:addKeyFrame(15, static, "visible", false)
    endgame:addKeyFrame(17, static, "visible", true)
    endgame:addKeyFrame(19, static, "visible", false)
    endgame:addKeyFrame(27, static, "visible", true)
    endgame:addKeyFrame(27, static, "color_a", 0x75)
    endgame:addKeyFrame(27, redPanel, "color_a", 0x84)
    endgame:addKeyFrame(38, static, "color_a", 0xFF)
    endgame:addKeyFrame(38, redPanel, "color_a", 0xFF)
    endgame:addKeyFrame(38, blackPanel, "visible", true)
    endgame:addKeyFrame(38, redPanel, "color_a", 0xFF)
    endgame:addKeyFrame(38, static, "color_a", 0x75)
    endgame:addKeyFrame(48, redPanel, "color_a", 0x84)
    endgame:addKeyFrame(72, static, "color_a", 0)
    endgame:addKeyFrame(72, MadnessTransition, "myPhase", 1, AnimationInterpolationType.NONE)
    endgame:addKeyFrame(73, blackPanel, "visible", true)
    endgame:addKeyFrame(73, blackPanel, "color_a", 255)
    endgame:addKeyFrame(94, blackPanel, "color_a", 0)
    function endgame:onAnimationFinished()
        static.color_a = 144
        redPanel.color_a = 0x84
        staticAnim:stop()
        staticImgs[1].visible = true
        for i = 2, 3 do
            staticImgs[i].visible = false
        end
        MadnessTransition:completeTransition()
    end

    return endgame
end

local function instantTransition()
    local instant = Animation.new()
    instant.name = "instant"

    local blackPanel = Panel.new()
    blackPanel.color = Color.new("FF000000")
    blackPanel.size = Window.size
    instant:addChild(blackPanel)

    -- Animacion de la transicion.
    instant:setFrames(19)
    instant:addKeyFrame(1, blackPanel, "color_a", 0xFF, AnimationInterpolationType.NONE)
    instant:addKeyFrame(9, MadnessTransition, "myPhase", 1, AnimationInterpolationType.NONE)
    instant:addKeyFrame(9, blackPanel, "color_a", 0xFF)
    instant:addKeyFrame(17, blackPanel, "color_a", 0)
    function instant:onAnimationFinished()
        MadnessTransition:completeTransition()
    end
    
    return instant
end

transitions.fade = fadeTransition()
transitions.endgame = endgameTransition()
transitions.instant = instantTransition()
MadnessTransition.transitions = transitions

function MadnessTransition:tick(delta)
    if self.currentTransition ~= nil and self.myPhase == 1 then
        self.myPhase = 2
        if self.myAction ~= nil then
            self.myAction()
        end
        -- Sacamos y volvemos a poner la transcision en la raiz por si se añado algo y quedo encima de la trnasicision.
        _root.UI:removeChild(self.currentTransition)
        _root.UI:addChild(self.currentTransition)
    end
end

function MadnessTransition:createTransition(inTrans, inAction)
    if self.transitions[inTrans] == nil then
        return
    end
    if self.transitionLock then
        if self.myAction ~= nil then
            self.myAction()
        end
        self.currentTransition:stop()
        self:completeTransition()
    end
    self.currentTransition = self.transitions[inTrans]
    self.myAction = inAction
    _root.UI:addChild(self.currentTransition)
    self.currentTransition:play()
end

function MadnessTransition:completeTransition()
    self.transitionLock = false
    _root.UI:removeChild(self.currentTransition)
    self.currentTransition = nil
    self.myPhase = 0
    self.myAction = nil
end

_root:addChild(MadnessTransition)