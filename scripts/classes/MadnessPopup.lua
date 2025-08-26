MadnessPopup = MadObject.new()
MadnessPopup.name = "MadnessPopup"
MadnessPopup.popups = {} -- Array de los popups cargados para reutilizarlos
MadnessPopup.pendingPopups = {}
MadnessPopup.appearing = false
MadnessPopup.alpha = 0

local appearingAnimation = Animation.new() -- Animacion que se reproduce cuando aparece el popup.
appearingAnimation:setFrames(7)
appearingAnimation:addKeyFrame(1, MadnessPopup, "alpha", 0)
appearingAnimation:addKeyFrame(7, MadnessPopup, "alpha", 255)
function appearingAnimation:onAnimationFinished()
    MadnessPopup.appearing = false
    if MadnessPopup.currentPopup.buttons ~= nil then
        MadnessPopup.currentPopup.buttons.myNoBtn.disabled = false
        MadnessPopup.currentPopup.buttons.myYesBtn.disabled = false
    end
end
MadnessPopup.appearingAnimation = appearingAnimation

local darkPanel = Panel.new() -- Panel oscuro que aparece al fondo para oscurecer el fondo.
local darkPanelAnimation = Animation.new()
darkPanel.name = "darkPanel"
darkPanel.color = Color.new("00000000")
darkPanel.size = Window.size
darkPanelAnimation:setFrames(7)
darkPanelAnimation:addKeyFrame(1, darkPanel, "color_a", 0)
darkPanelAnimation:addKeyFrame(7, darkPanel, "color_a", 120)
MadnessPopup.darkPanel = darkPanel
MadnessPopup.darkPanelAnimation = darkPanelAnimation

_root:addChild(MadnessPopup)

function MadnessPopup:tick(delta)
    -- Procesar el Popup actual.
    if self.currentPopup ~= nil then
        local currentPopup = self.currentPopup
        -- Procesar el timer.
        if currentPopup.myTimer > 1 then
            currentPopup.myTimer = currentPopup.myTimer - delta
        end
        if currentPopup.myTimer <= 1 and self.isDialogue(currentPopup) then
            if currentPopup.myYesFunction ~= nil then
                self:pressYes(false)
            else
                self:closePopup()
            end
        end
        if self.appearing and not self.appearingAnimation.playing then
            self.appearingAnimation:play()
            self.darkPanelAnimation:play()
        end
        currentPopup.color_a = self.alpha
    end
end

---@param inTimer number
---@param inYesFunction function
function MadnessPopup:addBuffer(inTimer, inYesFunction)
    local buffer = {
        myType = "buffer",
        myTimer = inTimer,
        myYesFunction = inYesFunction,
        buffer = true
    }
    self.pendingPopups[#self.pendingPopups+1] = buffer
    if self.currentPopup == nil then
        self:beginPopup()
    end
end

---@param inType string
---@param inText string|nil
---@param inChar table|nil
---@param inStickX number
---@param inStickY number
---@param inLinkedItem table|nil
---@param inTimer number
---@param inYesFunction function|nil
---@param inNoFunction function|nil
function MadnessPopup:addPopup(inType, inText, inChar, inStickX, inStickY, inLinkedItem, inTimer, inYesFunction, inNoFunction)
    local popup
    if self.popups[inType] == nil then
        -- Si el popup no existe, los cargamos
        local path = "scripts/popups/" .. inType .. ".lua"
        popup = dofile(path)
        self.popups[inType] = popup
    else
        popup = self.popups[inType]
    end
    popup.myNoFunction = inNoFunction
    popup.myYesFunction = inYesFunction
    popup.myText = inText
    popup.myChar = inChar
    popup.myX = inStickX
    popup.myY = inStickY
    popup.myLinkedItem = inLinkedItem
    popup.myTimer = inTimer
    popup.buffer = false
    self.pendingPopups[#self.pendingPopups+1] = popup
    if self.currentPopup == nil then
        self:beginPopup()
    end
end

-- Elimina el Popup actual y pone el primer popup pendiente como el actual.
function MadnessPopup:closePopup()
    if self.currentPopup ~= nil and not self.currentPopup.buffer then
        _root.UI:removeChild(self.currentPopup.name)
    end
    self.currentPopup = nil
    if #self.pendingPopups > 0 then
        self:beginPopup()
    elseif self.darkPanel:isInsideTree() then
        _root.UI:removeChild(self.darkPanel)
    end
end

function MadnessPopup:beginPopup()
    local currentPopup = self.pendingPopups[1]
    table.remove(self.pendingPopups, 1)
    if not currentPopup.buffer then -- ¿No es un buffer el popup actual?
        local myTextField = currentPopup:getChild("myTextField")
        if myTextField ~= nil then
            myTextField.text = currentPopup.myText
        end
        if currentPopup.myType == "yesno" then
            if myTextField.text ~= "(you will lose all current progress)" then
                myTextField.pos = Vector2.new(42, 55)
            else
                myTextField.pos = Vector2.new(77, 55)
            end
        end
        if currentPopup.buttons ~= nil then
            currentPopup.buttons.myNoBtn.disabled = true
            currentPopup.buttons.myYesBtn.disabled = true
        end
        self.alpha = 0
        self.darkPanel.color_a = 0
        _root.UI:addChild(self.darkPanel)
        _root.UI:addChild(currentPopup)
        self.appearing = true
        self.visible = true
    end
    self.currentPopup = currentPopup
    init = currentPopup
end

-- Implementalo mamahuevo
function MadnessPopup:moveTarget()
    
end

---@param inPlayerPressedYesManually boolean
function MadnessPopup:pressYes(inPlayerPressedYesManually)
    if inPlayerPressedYesManually then
        -- Aca se deberia reproducir el sonido 'menu4'. recordalo mamaguevo
    end
    if self.currentPopup.myYesFunction ~= nil then
        self.currentPopup.myYesFunction()
    end
    self:closePopup()
    collectgarbage("collect")
end

function MadnessPopup:pressNo()
    if self.currentPopup.myNoFunction ~= nil then
        self.currentPopup.myNoFunction()
    end
    self:closePopup()
end

function MadnessPopup.isDialogue(inPopup)
    return inPopup.myType == "lines" or inPopup.myType == "talk" or inPopup.myType == "help" or inPopup.buffer
end