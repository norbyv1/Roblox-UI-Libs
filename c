game.Loaded:Wait()

local Players = game:GetService("Players")
local UserInputService = game:GetService("UserInputService")
local TextService = game:GetService("TextService")

local playerGui = Players.LocalPlayer:WaitForChild("PlayerGui")

local WINDOW_TITLE = "temporary ui"
local DESIGN_SIZE = Vector2.new(780, 540)
local MIN_WINDOW_SIZE = Vector2.new(280, 320)
local MAX_WINDOW_SIZE = Vector2.new(1200, 850)
local MINIMIZED_SIZE = 56

local LINE_HEIGHT = 21
local EDITOR_FONT = Enum.Font.Code
local EDITOR_TEXT_SIZE = 16
local INDENT = "    "

local CHUNK_LINE_COUNT = 48
local VISIBLE_LINE_BUFFER = 30
local HIGHLIGHT_DEBOUNCE = 0.04

local COLORS = {
    Background = Color3.fromRGB(20, 21, 26),
    TopBar = Color3.fromRGB(29, 30, 37),
    Toolbar = Color3.fromRGB(24, 25, 31),
    Panel = Color3.fromRGB(16, 17, 21),
    Editor = Color3.fromRGB(13, 14, 18),
    Gutter = Color3.fromRGB(21, 22, 27),
    Text = Color3.fromRGB(225, 228, 235),
    SubText = Color3.fromRGB(140, 145, 158),
    Border = Color3.fromRGB(55, 58, 68),
    Accent = Color3.fromRGB(75, 150, 255),
    Run = Color3.fromRGB(55, 170, 100),
    Warning = Color3.fromRGB(240, 175, 65),
    Error = Color3.fromRGB(235, 80, 85),
    Success = Color3.fromRGB(95, 215, 135),
}

local HIGHLIGHT_COLORS = {
    Keyword = "#C586C0",
    String = "#CE9178",
    Number = "#B5CEA8",
    Comment = "#6A9955",
    Builtin = "#4FC1FF",
    Boolean = "#569CD6",
}

local KEYWORDS = {
    ["and"] = true,
    ["break"] = true,
    ["continue"] = true,
    ["do"] = true,
    ["else"] = true,
    ["elseif"] = true,
    ["end"] = true,
    ["export"] = true,
    ["for"] = true,
    ["function"] = true,
    ["if"] = true,
    ["in"] = true,
    ["local"] = true,
    ["not"] = true,
    ["or"] = true,
    ["repeat"] = true,
    ["return"] = true,
    ["then"] = true,
    ["type"] = true,
    ["until"] = true,
    ["while"] = true,
}

local BUILTINS = {
    assert = true,
    collectgarbage = true,
    error = true,
    getfenv = true,
    getmetatable = true,
    ipairs = true,
    loadstring = true,
    next = true,
    pairs = true,
    pcall = true,
    print = true,
    rawequal = true,
    rawget = true,
    rawset = true,
    select = true,
    setfenv = true,
    setmetatable = true,
    tostring = true,
    type = true,
    tonumber = true,
    unpack = true,
    xpcall = true,
    coroutine = true,
    string = true,
    table = true,
    math = true,
    os = true,
    task = true,
    game = true,
    workspace = true,
    script = true,
    Enum = true,
    Instance = true,
    Vector2 = true,
    Vector3 = true,
    UDim = true,
    UDim2 = true,
    Color3 = true,
    CFrame = true,
}

local oldGui = playerGui:FindFirstChild("MiniLuaIDE")
if oldGui then
    oldGui:Destroy()
end

local function addCorner(parent, radius)
    local corner = Instance.new("UICorner")
    corner.CornerRadius = UDim.new(0, radius or 6)
    corner.Parent = parent
    return corner
end

local function addStroke(parent, color, thickness)
    local stroke = Instance.new("UIStroke")
    stroke.Color = color or COLORS.Border
    stroke.Thickness = thickness or 1
    stroke.ApplyStrokeMode = Enum.ApplyStrokeMode.Border
    stroke.Parent = parent
    return stroke
end

local function createButton(parent, text, width, backgroundColor)
    local button = Instance.new("TextButton")
    local buttonName = text:gsub("%s+", "")
    buttonName = buttonName:gsub("[^%w]", "")
    button.Name = buttonName .. "Button"
    button.Size = UDim2.fromOffset(width, 30)
    button.BackgroundColor3 = backgroundColor or Color3.fromRGB(45, 47, 56)
    button.BorderSizePixel = 0
    button.AutoButtonColor = true
    button.Text = text
    button.TextColor3 = COLORS.Text
    button.TextSize = 14
    button.Font = Enum.Font.GothamMedium
    button.Parent = parent
    addCorner(button, 5)
    return button
end

local screenGui = Instance.new("ScreenGui")
screenGui.Name = "MiniLuaIDE"
screenGui.ResetOnSpawn = false
screenGui.IgnoreGuiInset = false
screenGui.ZIndexBehavior = Enum.ZIndexBehavior.Sibling

pcall(function()
    screenGui.ScreenInsets = Enum.ScreenInsets.CoreUISafeInsets
end)

pcall(function()
    screenGui.SafeAreaCompatibility = Enum.SafeAreaCompatibility.None
end)

screenGui.Parent = playerGui

local mainWindow = Instance.new("Frame")
mainWindow.Name = "MainWindow"
mainWindow.Size = UDim2.fromOffset(DESIGN_SIZE.X, DESIGN_SIZE.Y)
mainWindow.Position = UDim2.fromOffset(0, 0)
mainWindow.BackgroundColor3 = COLORS.Background
mainWindow.BorderSizePixel = 0
mainWindow.ClipsDescendants = true
mainWindow.Active = true
mainWindow.Parent = screenGui

local windowCorner = addCorner(mainWindow, 9)
addStroke(mainWindow, COLORS.Border, 1)

local topBar = Instance.new("Frame")
topBar.Name = "TopBar"
topBar.Size = UDim2.new(1, 0, 0, 38)
topBar.BackgroundColor3 = COLORS.TopBar
topBar.BorderSizePixel = 0
topBar.Active = true
topBar.Parent = mainWindow
local topBarCorner = addCorner(topBar, 9)

local dragHandle = Instance.new("Frame")
dragHandle.Name = "DragHandle"
dragHandle.Size = UDim2.new(1, -48, 1, 0)
dragHandle.BackgroundTransparency = 1
dragHandle.BorderSizePixel = 0
dragHandle.Active = true
dragHandle.ZIndex = 2
dragHandle.Parent = topBar

local title = Instance.new("TextLabel")
title.Name = "Title"
title.Size = UDim2.new(1, -13, 1, 0)
title.Position = UDim2.fromOffset(13, 0)
title.BackgroundTransparency = 1
title.Active = false
title.ZIndex = 3
title.Text = WINDOW_TITLE
title.TextColor3 = COLORS.Text
title.TextSize = 15
title.Font = Enum.Font.GothamBold
title.TextXAlignment = Enum.TextXAlignment.Left
title.Parent = dragHandle

local minimizeButton = Instance.new("TextButton")
minimizeButton.Name = "MinimizeButton"
minimizeButton.Size = UDim2.fromOffset(48, 38)
minimizeButton.Position = UDim2.new(1, -48, 0, 0)
minimizeButton.BackgroundTransparency = 1
minimizeButton.Text = "-"
minimizeButton.TextColor3 = COLORS.SubText
minimizeButton.TextSize = 21
minimizeButton.Font = Enum.Font.GothamBold
minimizeButton.AutoButtonColor = false
minimizeButton.Active = true
minimizeButton.ZIndex = 4
minimizeButton.Parent = topBar

local toolbar = Instance.new("Frame")
toolbar.Name = "Toolbar"
toolbar.Position = UDim2.fromOffset(0, 38)
toolbar.Size = UDim2.new(1, 0, 0, 42)
toolbar.BackgroundColor3 = COLORS.Toolbar
toolbar.BorderSizePixel = 0
toolbar.Parent = mainWindow

local toolbarPadding = Instance.new("UIPadding")
toolbarPadding.PaddingLeft = UDim.new(0, 8)
toolbarPadding.PaddingRight = UDim.new(0, 8)
toolbarPadding.PaddingTop = UDim.new(0, 6)
toolbarPadding.Parent = toolbar

local toolbarLayout = Instance.new("UIListLayout")
toolbarLayout.FillDirection = Enum.FillDirection.Horizontal
toolbarLayout.HorizontalAlignment = Enum.HorizontalAlignment.Left
toolbarLayout.VerticalAlignment = Enum.VerticalAlignment.Top
toolbarLayout.Padding = UDim.new(0, 6)
toolbarLayout.Parent = toolbar

local executeButton = createButton(toolbar, "▶ Execute", 96, COLORS.Run)
local clearEditorButton = createButton(toolbar, "Clear Editor", 100)
local clearOutputButton = createButton(toolbar, "Clear Output", 104)

local editorPanel = Instance.new("Frame")
editorPanel.Name = "EditorPanel"
editorPanel.BackgroundColor3 = COLORS.Panel
editorPanel.BorderSizePixel = 0
editorPanel.ClipsDescendants = true
editorPanel.Parent = mainWindow
addCorner(editorPanel, 6)
addStroke(editorPanel, COLORS.Border, 1)

local lineNumberGutter = Instance.new("Frame")
lineNumberGutter.Name = "LineNumberGutter"
lineNumberGutter.Size = UDim2.new(0, 52, 1, 0)
lineNumberGutter.BackgroundColor3 = COLORS.Gutter
lineNumberGutter.BorderSizePixel = 0
lineNumberGutter.ClipsDescendants = true
lineNumberGutter.Parent = editorPanel

local gutterDivider = Instance.new("Frame")
gutterDivider.Size = UDim2.new(0, 1, 1, 0)
gutterDivider.Position = UDim2.new(1, -1, 0, 0)
gutterDivider.BackgroundColor3 = COLORS.Border
gutterDivider.BorderSizePixel = 0
gutterDivider.Parent = lineNumberGutter

local editorScroll = Instance.new("ScrollingFrame")
editorScroll.Name = "EditorScroll"
editorScroll.Position = UDim2.fromOffset(52, 0)
editorScroll.Size = UDim2.new(1, -52, 1, 0)
editorScroll.BackgroundColor3 = COLORS.Editor
editorScroll.BorderSizePixel = 0
editorScroll.ClipsDescendants = true
editorScroll.ScrollBarThickness = 7
editorScroll.ScrollBarImageColor3 = Color3.fromRGB(80, 83, 95)
editorScroll.CanvasSize = UDim2.fromOffset(0, 0)
editorScroll.ScrollingDirection = Enum.ScrollingDirection.XY
editorScroll.Parent = editorPanel
addCorner(editorScroll, 6)

local codeEditor = Instance.new("TextBox")
codeEditor.Name = "CodeEditor"
codeEditor.Position = UDim2.fromOffset(10, 7)
codeEditor.Size = UDim2.new(1, -22, 0, 300)
codeEditor.BackgroundTransparency = 1
codeEditor.BorderSizePixel = 0
codeEditor.ClearTextOnFocus = false
codeEditor.MultiLine = true
codeEditor.TextEditable = true
codeEditor.TextWrapped = false
codeEditor.TextColor3 = COLORS.Text
codeEditor.PlaceholderColor3 = Color3.fromRGB(90, 94, 105)
codeEditor.PlaceholderText = "-- opiumware ios temp yay"
codeEditor.TextSize = EDITOR_TEXT_SIZE
codeEditor.Font = EDITOR_FONT
codeEditor.TextXAlignment = Enum.TextXAlignment.Left
codeEditor.TextYAlignment = Enum.TextYAlignment.Top
codeEditor.Text = [[print("Hello from the editor!")]]
codeEditor.TextTransparency = 1
codeEditor.TextStrokeTransparency = 1
codeEditor.ZIndex = 5
codeEditor.Parent = editorScroll

pcall(function()
    codeEditor.MaxVisibleGraphemes = -1
end)

pcall(function()
    codeEditor.LineHeight = LINE_HEIGHT / EDITOR_TEXT_SIZE
end)

local customCaret = Instance.new("Frame")
customCaret.Name = "CustomCaret"
customCaret.Size = UDim2.fromOffset(2, EDITOR_TEXT_SIZE + 2)
customCaret.BackgroundColor3 = COLORS.Text
customCaret.BorderSizePixel = 0
customCaret.Visible = false
customCaret.Active = false
customCaret.ZIndex = 10
customCaret.Parent = editorScroll

local outputPanel = Instance.new("Frame")
outputPanel.Name = "OutputPanel"
outputPanel.BackgroundColor3 = COLORS.Panel
outputPanel.BorderSizePixel = 0
outputPanel.ClipsDescendants = true
outputPanel.Parent = mainWindow
addCorner(outputPanel, 6)
addStroke(outputPanel, COLORS.Border, 1)

local outputHeader = Instance.new("Frame")
outputHeader.Name = "OutputHeader"
outputHeader.Size = UDim2.new(1, 0, 0, 28)
outputHeader.BackgroundColor3 = COLORS.Gutter
outputHeader.BorderSizePixel = 0
outputHeader.Parent = outputPanel

local outputHeaderText = Instance.new("TextLabel")
outputHeaderText.Size = UDim2.new(1, -20, 1, 0)
outputHeaderText.Position = UDim2.fromOffset(10, 0)
outputHeaderText.BackgroundTransparency = 1
outputHeaderText.Text = "OUTPUT"
outputHeaderText.TextColor3 = COLORS.SubText
outputHeaderText.TextSize = 12
outputHeaderText.Font = Enum.Font.GothamBold
outputHeaderText.TextXAlignment = Enum.TextXAlignment.Left
outputHeaderText.Parent = outputHeader
addCorner(outputHeader, 6)

local outputScroll = Instance.new("ScrollingFrame")
outputScroll.Name = "OutputScroll"
outputScroll.Position = UDim2.fromOffset(0, 28)
outputScroll.Size = UDim2.new(1, 0, 1, -28)
outputScroll.BackgroundColor3 = COLORS.Editor
outputScroll.BorderSizePixel = 0
outputScroll.ScrollBarThickness = 6
outputScroll.ScrollBarImageColor3 = Color3.fromRGB(80, 83, 95)
outputScroll.AutomaticCanvasSize = Enum.AutomaticSize.Y
outputScroll.CanvasSize = UDim2.fromOffset(0, 0)
outputScroll.Parent = outputPanel
addCorner(outputScroll, 6)

local outputPadding = Instance.new("UIPadding")
outputPadding.PaddingLeft = UDim.new(0, 9)
outputPadding.PaddingRight = UDim.new(0, 9)
outputPadding.PaddingTop = UDim.new(0, 7)
outputPadding.PaddingBottom = UDim.new(0, 7)
outputPadding.Parent = outputScroll

local outputLayout = Instance.new("UIListLayout")
outputLayout.SortOrder = Enum.SortOrder.LayoutOrder
outputLayout.Padding = UDim.new(0, 3)
outputLayout.Parent = outputScroll

local statusBar = Instance.new("Frame")
statusBar.Name = "StatusBar"
statusBar.BackgroundColor3 = COLORS.TopBar
statusBar.BorderSizePixel = 0
statusBar.Parent = mainWindow
addCorner(statusBar, 9)

local statusText = Instance.new("TextLabel")
statusText.Name = "StatusText"
statusText.Size = UDim2.new(0.7, -10, 1, 0)
statusText.Position = UDim2.fromOffset(10, 0)
statusText.BackgroundTransparency = 1
statusText.Text = ""
statusText.TextColor3 = COLORS.SubText
statusText.TextSize = 12
statusText.Font = Enum.Font.Gotham
statusText.TextXAlignment = Enum.TextXAlignment.Left
statusText.TextTruncate = Enum.TextTruncate.AtEnd
statusText.Parent = statusBar

local cursorText = Instance.new("TextLabel")
cursorText.Name = "CursorText"
cursorText.Size = UDim2.new(0.3, -10, 1, 0)
cursorText.Position = UDim2.new(0.7, 0, 0, 0)
cursorText.BackgroundTransparency = 1
cursorText.Text = "Ln 1, Col 1"
cursorText.TextColor3 = COLORS.SubText
cursorText.TextSize = 12
cursorText.Font = Enum.Font.Gotham
cursorText.TextXAlignment = Enum.TextXAlignment.Right
cursorText.Parent = statusBar

local resizeHandle = Instance.new("TextButton")
resizeHandle.Name = "ResizeHandle"
resizeHandle.Size = UDim2.fromOffset(28, 28)
resizeHandle.Position = UDim2.new(1, -28, 1, -28)
resizeHandle.BackgroundTransparency = 1
resizeHandle.Text = "◢"
resizeHandle.TextColor3 = COLORS.SubText
resizeHandle.TextSize = 14
resizeHandle.Font = Enum.Font.Code
resizeHandle.ZIndex = 20
resizeHandle.Active = true
resizeHandle.Parent = mainWindow

local isMinimized = false
local autoFitToViewport = true
local savedExpandedSize = DESIGN_SIZE
local savedExpandedCenter = nil
local outputCount = 0
local topDragDetector = nil
local minimizedDragDetector = nil

local function getViewportSize()
    local screenSize = screenGui.AbsoluteSize
    if screenSize.X > 1 and screenSize.Y > 1 then
        return screenSize
    end

    local camera = workspace.CurrentCamera
    if camera then
        return camera.ViewportSize
    end

    return Vector2.new(1920, 1080)
end

local function getScreenMargin()
    if UserInputService.TouchEnabled then
        return 12
    end
    return 24
end

local function getAvailableWindowSize()
    local viewport = getViewportSize()
    local margin = getScreenMargin()

    return Vector2.new(
        math.max(1, viewport.X - margin * 2),
        math.max(1, viewport.Y - margin * 2)
    )
end

local function fitDimension(available, preferred, minimum, maximum)
    local value = math.min(available, preferred, maximum)

    if available >= minimum then
        return math.max(minimum, value)
    end

    return available
end

local function getResponsiveWindowSize()
    local available = getAvailableWindowSize()

    return Vector2.new(
        fitDimension(
            available.X,
            DESIGN_SIZE.X,
            MIN_WINDOW_SIZE.X,
            MAX_WINDOW_SIZE.X
        ),
        fitDimension(
            available.Y,
            DESIGN_SIZE.Y,
            MIN_WINDOW_SIZE.Y,
            MAX_WINDOW_SIZE.Y
        )
    )
end

local function getWindowSize()
    return Vector2.new(
        mainWindow.Size.X.Offset,
        mainWindow.Size.Y.Offset
    )
end

local function getWindowPosition()
    return Vector2.new(
        mainWindow.Position.X.Offset,
        mainWindow.Position.Y.Offset
    )
end

local function clampPosition(position, windowSize)
    local viewport = getViewportSize()
    local margin = getScreenMargin()
    local size = windowSize or getWindowSize()

    local maximum = Vector2.new(
        math.max(margin, viewport.X - size.X - margin),
        math.max(margin, viewport.Y - size.Y - margin)
    )

    return Vector2.new(
        math.clamp(position.X, margin, maximum.X),
        math.clamp(position.Y, margin, maximum.Y)
    )
end

local function centerWindow(windowSize)
    local viewport = getViewportSize()
    local size = windowSize or getWindowSize()

    local position = Vector2.new(
        (viewport.X - size.X) * 0.5,
        (viewport.Y - size.Y) * 0.5
    )

    mainWindow.Position = UDim2.fromOffset(
        math.floor(position.X),
        math.floor(position.Y)
    )
end

local function keepWindowVisible()
    local position = clampPosition(getWindowPosition())
    mainWindow.Position = UDim2.fromOffset(position.X, position.Y)
end

local function escapeRichText(text)

    local escaped = text:gsub("&", "&amp;")
    escaped = escaped:gsub("<", "&lt;")
    escaped = escaped:gsub(">", "&gt;")
    return escaped
end

local function colorToken(text, color)
    return '<font color="' .. color .. '">' .. escapeRichText(text) .. "</font>"
end

local function findQuotedStringEnd(source, startIndex, quote)
    local index = startIndex + 1

    while index <= #source do
        local character = source:sub(index, index)

        if character == "\\" then
            index += 2
        elseif character == quote then
            return index
        else
            index += 1
        end
    end

    return #source
end

local function findLongStringEnd(source, startIndex)
    local closingBracket = source:find("]]", startIndex + 2, true)
    return closingBracket and closingBracket + 1 or #source
end

local function highlightLuau(source)
    local result = table.create(math.max(8, #source // 4))
    local index = 1
    local sourceLength = #source

    local function emit(value)
        result[#result + 1] = value
    end

    while index <= sourceLength do
        local character = source:sub(index, index)
        local nextTwo = source:sub(index, index + 1)
        local nextFour = source:sub(index, index + 3)

        if nextFour == "--[[" then
            local finish = findLongStringEnd(source, index + 2)
            emit(
                colorToken(
                    source:sub(index, finish),
                    HIGHLIGHT_COLORS.Comment
                )
            )
            index = finish + 1

        elseif nextTwo == "--" then
            local newline = source:find("\n", index, true)
            local finish = newline and newline - 1 or sourceLength

            emit(
                colorToken(
                    source:sub(index, finish),
                    HIGHLIGHT_COLORS.Comment
                )
            )

            index = finish + 1

        elseif character == '"' or character == "'" then
            local finish = findQuotedStringEnd(source, index, character)

            emit(
                colorToken(
                    source:sub(index, finish),
                    HIGHLIGHT_COLORS.String
                )
            )

            index = finish + 1

        elseif nextTwo == "[[" then
            local finish = findLongStringEnd(source, index)

            emit(
                colorToken(
                    source:sub(index, finish),
                    HIGHLIGHT_COLORS.String
                )
            )

            index = finish + 1

        elseif character:match("[%a_]") then
            local finish = index + 1

            while finish <= sourceLength
                and source:sub(finish, finish):match("[%w_]") do
                finish += 1
            end

            local token = source:sub(index, finish - 1)
            local color

            if token == "true"
                or token == "false"
                or token == "nil" then
                color = HIGHLIGHT_COLORS.Boolean

            elseif KEYWORDS[token] then
                color = HIGHLIGHT_COLORS.Keyword

            elseif BUILTINS[token] then
                color = HIGHLIGHT_COLORS.Builtin
            end

            emit(
                color
                    and colorToken(token, color)
                    or escapeRichText(token)
            )

            index = finish

        elseif character:match("%d") then
            local finish = index + 1

            while finish <= sourceLength
                and source:sub(finish, finish):match("[%w_%.]") do
                finish += 1
            end

            emit(
                colorToken(
                    source:sub(index, finish - 1),
                    HIGHLIGHT_COLORS.Number
                )
            )

            index = finish

        else
            emit(escapeRichText(character))
            index += 1
        end
    end

    return table.concat(result)
end

local function normalizeAsciiQuotes(text)

    local normalized = text
    normalized = normalized:gsub("“", '"')
    normalized = normalized:gsub("”", '"')
    normalized = normalized:gsub("„", '"')
    normalized = normalized:gsub("‟", '"')
    normalized = normalized:gsub("‘", "'")
    normalized = normalized:gsub("’", "'")
    normalized = normalized:gsub("‚", "'")
    normalized = normalized:gsub("‛", "'")
    return normalized
end

local documentText = nil
local documentLines = { "" }
local documentLineStarts = { 1 }

local longestLine = ""
local measuredLongestWidth = 0

local syntaxChunks = {}
local gutterChunks = {}
local chunkCache = {}

local scheduledRenderGeneration = 0

local function destroyChunkLabels()
    for _, label in pairs(syntaxChunks) do
        label:Destroy()
    end

    for _, label in pairs(gutterChunks) do
        label:Destroy()
    end

    table.clear(syntaxChunks)
    table.clear(gutterChunks)
end

local function rebuildDocumentCache(text)
    documentText = text

    table.clear(documentLines)
    table.clear(documentLineStarts)
    table.clear(chunkCache)

    longestLine = ""

    local position = 1
    local textLength = #text

    while true do
        documentLineStarts[#documentLineStarts + 1] = position

        local newline = text:find("\n", position, true)
        local line

        if newline then
            line = text:sub(position, newline - 1)
        else
            line = text:sub(position)
        end

        documentLines[#documentLines + 1] = line

        if #line > #longestLine then
            longestLine = line
        end

        if not newline then
            break
        end

        position = newline + 1

        if position > textLength + 1 then
            break
        end
    end

    if #documentLines == 0 then
        documentLines[1] = ""
        documentLineStarts[1] = 1
    end

    if longestLine == "" then
        measuredLongestWidth = 0
    else
        local measuredLine = longestLine:gsub("\t", INDENT)
        local success, measured = pcall(function()
            return TextService:GetTextSize(
                measuredLine,
                EDITOR_TEXT_SIZE,
                EDITOR_FONT,
                Vector2.new(1000000, LINE_HEIGHT)
            )
        end)

        if success then
            measuredLongestWidth = measured.X
        else
            measuredLongestWidth =
                #longestLine * EDITOR_TEXT_SIZE * 0.61
        end
    end

    destroyChunkLabels()
end

local function createSyntaxChunk()
    local label = Instance.new("TextLabel")
    label.BackgroundTransparency = 1
    label.BorderSizePixel = 0
    label.RichText = true
    label.TextWrapped = false
    label.TextColor3 = COLORS.Text
    label.TextSize = EDITOR_TEXT_SIZE
    label.Font = EDITOR_FONT
    label.TextXAlignment = Enum.TextXAlignment.Left
    label.TextYAlignment = Enum.TextYAlignment.Top
    label.ZIndex = 1

    pcall(function()
        label.LineHeight = LINE_HEIGHT / EDITOR_TEXT_SIZE
    end)

    pcall(function()
        label.MaxVisibleGraphemes = -1
    end)

    label.Name = "SyntaxChunk"
    label.Parent = editorScroll
    return label
end

local function createGutterChunk()
    local label = Instance.new("TextLabel")
    label.BackgroundTransparency = 1
    label.BorderSizePixel = 0
    label.TextWrapped = false
    label.RichText = false
    label.TextColor3 = Color3.fromRGB(100, 105, 118)
    label.TextSize = EDITOR_TEXT_SIZE
    label.Font = EDITOR_FONT
    label.TextXAlignment = Enum.TextXAlignment.Right
    label.TextYAlignment = Enum.TextYAlignment.Top

    pcall(function()
        label.LineHeight = LINE_HEIGHT / EDITOR_TEXT_SIZE
    end)

    pcall(function()
        label.MaxVisibleGraphemes = -1
    end)

    label.Parent = lineNumberGutter
    return label
end

local function getChunkData(chunkStart)
    local cached = chunkCache[chunkStart]
    if cached then
        return cached
    end

    local chunkEnd = math.min(
        #documentLines,
        chunkStart + CHUNK_LINE_COUNT - 1
    )

    local source = table.concat(
        documentLines,
        "\n",
        chunkStart,
        chunkEnd
    )

    local numberList = table.create(chunkEnd - chunkStart + 1)

    for lineNumber = chunkStart, chunkEnd do
        numberList[lineNumber - chunkStart + 1] = tostring(lineNumber)
    end

    local highlightSucceeded, highlightResult = pcall(highlightLuau, source)
    local highlighted

    if highlightSucceeded then
        highlighted = highlightResult
    else
        highlighted = escapeRichText(source)
        statusText.Text = "Syntax highlight fallback active"
        warn("[MiniLuaIDE] syntax highlight error: " .. tostring(highlightResult))
    end

    cached = {
        endLine = chunkEnd,
        highlighted = highlighted,
        lineNumbers = table.concat(numberList, "\n"),
    }

    chunkCache[chunkStart] = cached
    return cached
end

local function getEditorDocumentSize()
    local lineCount = math.max(1, #documentLines)

    local height = math.max(
        editorPanel.AbsoluteSize.Y - 14,
        lineCount * LINE_HEIGHT + 20
    )

    local width = math.max(
        editorScroll.AbsoluteSize.X - 25,
        measuredLongestWidth + 40
    )

    return Vector2.new(width, height)
end

local renderFallbackActive = false
local lastRenderFailure = nil

local function activatePlainTextFallback(reason)
    if renderFallbackActive then
        return
    end

    renderFallbackActive = true
    lastRenderFailure = tostring(reason)

    destroyChunkLabels()
    codeEditor.TextTransparency = 0
    codeEditor.TextStrokeTransparency = 1
    customCaret.Visible = false
    statusText.Text = "Plain-text renderer fallback active"

    warn("[MiniLuaIDE] renderer fallback: " .. lastRenderFailure)
end

local function renderVisibleChunks()
    if renderFallbackActive or not screenGui.Parent or isMinimized then
        return
    end

    local lineCount = math.max(1, #documentLines)
    local scrollY = editorScroll.CanvasPosition.Y
    local viewportHeight = math.max(
        1,
        editorScroll.AbsoluteWindowSize.Y
    )

    local firstVisibleLine =
        math.floor(math.max(0, scrollY - 7) / LINE_HEIGHT) + 1

    local lastVisibleLine =
        math.ceil((scrollY + viewportHeight) / LINE_HEIGHT) + 1

    firstVisibleLine = math.max(
        1,
        firstVisibleLine - VISIBLE_LINE_BUFFER
    )

    lastVisibleLine = math.min(
        lineCount,
        lastVisibleLine + VISIBLE_LINE_BUFFER
    )

    local firstChunk =
        math.floor((firstVisibleLine - 1) / CHUNK_LINE_COUNT)
        * CHUNK_LINE_COUNT
        + 1

    local lastChunk =
        math.floor((lastVisibleLine - 1) / CHUNK_LINE_COUNT)
        * CHUNK_LINE_COUNT
        + 1

    local documentSize = getEditorDocumentSize()
    local usedChunks = {}

    for chunkStart = firstChunk, lastChunk, CHUNK_LINE_COUNT do
        usedChunks[chunkStart] = true

        local data = getChunkData(chunkStart)
        local chunkLineCount = data.endLine - chunkStart + 1
        local chunkY = 7 + (chunkStart - 1) * LINE_HEIGHT

        local syntaxLabel = syntaxChunks[chunkStart]

        if not syntaxLabel then
            syntaxLabel = createSyntaxChunk()
            syntaxChunks[chunkStart] = syntaxLabel
            syntaxLabel.Text = data.highlighted
        end

        syntaxLabel.Position = UDim2.fromOffset(10, chunkY)
        syntaxLabel.Size = UDim2.fromOffset(
            documentSize.X,
            chunkLineCount * LINE_HEIGHT + 4
        )

        local gutterLabel = gutterChunks[chunkStart]

        if not gutterLabel then
            gutterLabel = createGutterChunk()
            gutterChunks[chunkStart] = gutterLabel
            gutterLabel.Text = data.lineNumbers
        end

        gutterLabel.Position = UDim2.fromOffset(
            0,
            chunkY - scrollY
        )

        gutterLabel.Size = UDim2.new(
            1,
            -8,
            0,
            chunkLineCount * LINE_HEIGHT + 4
        )
    end

    for chunkStart, label in pairs(syntaxChunks) do
        if not usedChunks[chunkStart] then
            label:Destroy()
            syntaxChunks[chunkStart] = nil
        end
    end

    for chunkStart, label in pairs(gutterChunks) do
        if not usedChunks[chunkStart] then
            label:Destroy()
            gutterChunks[chunkStart] = nil
        end
    end
end

local function scheduleVisibleRender()
    scheduledRenderGeneration += 1
    local myGeneration = scheduledRenderGeneration

    task.delay(HIGHLIGHT_DEBOUNCE, function()
        if myGeneration ~= scheduledRenderGeneration then
            return
        end

        if screenGui.Parent and not renderFallbackActive then
            local renderSucceeded, renderError = pcall(renderVisibleChunks)

            if not renderSucceeded then
                activatePlainTextFallback(renderError)
            end
        end
    end)
end

local function updateEditorGeometry()
    local documentSize = getEditorDocumentSize()

    codeEditor.Position = UDim2.fromOffset(10, 7)
    codeEditor.Size = UDim2.fromOffset(
        documentSize.X,
        documentSize.Y
    )

    editorScroll.CanvasSize = UDim2.fromOffset(
        documentSize.X + 15,
        documentSize.Y + 15
    )

    scheduleVisibleRender()
end

local function updateEditorContent()
    local text = codeEditor.Text or ""

    if text ~= documentText then
        rebuildDocumentCache(text)
    end

    updateEditorGeometry()
end

local caretBlinkStarted = os.clock()

local function resetCaretBlink()
    caretBlinkStarted = os.clock()

    if not renderFallbackActive
        and codeEditor:IsFocused()
        and codeEditor.CursorPosition ~= -1 then
        customCaret.Visible = true
    end
end

task.spawn(function()
    while screenGui.Parent do
        task.wait(0.08)

        if not renderFallbackActive
            and codeEditor:IsFocused()
            and codeEditor.CursorPosition ~= -1 then

            local elapsed = os.clock() - caretBlinkStarted
            customCaret.Visible = (elapsed % 1) < 0.5
        else
            customCaret.Visible = false
        end
    end
end)

local function findCursorLine(cursorPosition)
    local lineCount = #documentLineStarts

    if lineCount <= 1 then
        return 1
    end

    local low = 1
    local high = lineCount
    local result = 1

    while low <= high do
        local middle = math.floor((low + high) / 2)

        if documentLineStarts[middle] <= cursorPosition then
            result = middle
            low = middle + 1
        else
            high = middle - 1
        end
    end

    return result
end

local function keepCaretVisible(caretX, caretY)
    local current = editorScroll.CanvasPosition
    local viewport = editorScroll.AbsoluteWindowSize

    if viewport.X <= 0 or viewport.Y <= 0 then
        return
    end

    local nextX = current.X
    local nextY = current.Y
    local margin = 18

    if caretX < current.X + margin then
        nextX = math.max(0, caretX - margin)
    elseif caretX > current.X + viewport.X - margin then
        nextX = math.max(
            0,
            caretX - viewport.X + margin
        )
    end

    if caretY < current.Y + margin then
        nextY = math.max(0, caretY - margin)
    elseif caretY + LINE_HEIGHT
        > current.Y + viewport.Y - margin then

        nextY = math.max(
            0,
            caretY
                - viewport.Y
                + LINE_HEIGHT
                + margin
        )
    end

    if nextX ~= current.X or nextY ~= current.Y then
        editorScroll.CanvasPosition =
            Vector2.new(nextX, nextY)
    end
end

local function updateCursor()
    local cursorPosition = codeEditor.CursorPosition

    if cursorPosition == -1 then
        customCaret.Visible = false
        return
    end

    if documentText ~= codeEditor.Text then
        rebuildDocumentCache(codeEditor.Text or "")
        updateEditorGeometry()
    end

    cursorPosition = math.clamp(
        cursorPosition,
        1,
        #codeEditor.Text + 1
    )

    local lineNumber = findCursorLine(cursorPosition)
    local lineStart = documentLineStarts[lineNumber] or 1

    local column = cursorPosition - lineStart + 1

    cursorText.Text = string.format(
        "Ln %d, Col %d",
        lineNumber,
        column
    )

    local beforeCaret = codeEditor.Text:sub(
        lineStart,
        math.max(lineStart - 1, cursorPosition - 1)
    )

    beforeCaret = beforeCaret:gsub("\n", "")
    beforeCaret = beforeCaret:gsub("\r", "")
    beforeCaret = beforeCaret:gsub("\t", INDENT)

    local caretOffsetX = 0

    if #beforeCaret > 0 then
        local success, bounds = pcall(function()
            return TextService:GetTextSize(
                beforeCaret,
                EDITOR_TEXT_SIZE,
                EDITOR_FONT,
                Vector2.new(1000000, LINE_HEIGHT)
            )
        end)

        if success then
            caretOffsetX = bounds.X
        else
            caretOffsetX =
                #beforeCaret * EDITOR_TEXT_SIZE * 0.61
        end
    end

    local caretX = 10 + caretOffsetX
    local caretY = 7 + (lineNumber - 1) * LINE_HEIGHT

    if not renderFallbackActive then
        customCaret.Position = UDim2.fromOffset(
            caretX,
            caretY
        )

        resetCaretBlink()
    else
        customCaret.Visible = false
    end

    keepCaretVisible(
        caretX,
        caretY
    )
end

local function updateLineNumberScroll()
    scheduleVisibleRender()
end

local function updateToolbar()
    local width = math.max(1, mainWindow.AbsoluteSize.X - 16)
    local gap = 6
    local buttonWidth = math.floor((width - gap * 2) / 3)
    local compact = buttonWidth < 92

    buttonWidth = math.max(70, buttonWidth)

    executeButton.Size = UDim2.fromOffset(buttonWidth, 30)
    clearEditorButton.Size = UDim2.fromOffset(buttonWidth, 30)
    clearOutputButton.Size = UDim2.fromOffset(buttonWidth, 30)

    local textSize = compact and 11 or 14

    executeButton.TextSize = textSize
    clearEditorButton.TextSize = textSize
    clearOutputButton.TextSize = textSize
end

local function updateLayout()
    if isMinimized then
        return
    end

    local windowWidth = mainWindow.AbsoluteSize.X
    local windowHeight = mainWindow.AbsoluteSize.Y

    local sidePadding = 8
    local topContent = 86
    local statusHeight = 24
    local panelGap = 8

    local panelSpace = math.max(
        2,
        windowHeight
            - topContent
            - statusHeight
            - panelGap * 2
    )

    local compact = panelSpace < 300

    local minimumEditorHeight = compact and 90 or 120
    local minimumOutputHeight = compact and 72 or 105

    local outputHeight
    local editorHeight

    if panelSpace < minimumEditorHeight + minimumOutputHeight then
        outputHeight = math.max(
            1,
            math.floor(panelSpace * 0.3)
        )

        editorHeight = math.max(
            1,
            panelSpace - outputHeight
        )
    else
        local maximumOutputHeight = math.min(
            220,
            panelSpace - minimumEditorHeight
        )

        outputHeight = math.clamp(
            math.floor(panelSpace * 0.3),
            minimumOutputHeight,
            maximumOutputHeight
        )

        editorHeight = panelSpace - outputHeight
    end

    local panelWidth = math.max(
        120,
        windowWidth - sidePadding * 2
    )

    editorPanel.Position =
        UDim2.fromOffset(sidePadding, topContent)

    editorPanel.Size =
        UDim2.fromOffset(panelWidth, editorHeight)

    outputPanel.Position =
        UDim2.fromOffset(
            sidePadding,
            topContent + editorHeight + panelGap
        )

    outputPanel.Size =
        UDim2.fromOffset(panelWidth, outputHeight)

    statusBar.Position =
        UDim2.new(0, 0, 1, -statusHeight)

    statusBar.Size =
        UDim2.new(1, 0, 0, statusHeight)

    resizeHandle.Position =
        UDim2.new(1, -28, 1, -28)

    updateToolbar()
    updateEditorGeometry()
    scheduleVisibleRender()
end

local function scrollOutputToBottom()
    task.defer(function()
        if not outputScroll.Parent then
            return
        end

        outputScroll.CanvasPosition = Vector2.new(
            0,
            math.max(
                0,
                outputLayout.AbsoluteContentSize.Y
                    - outputScroll.AbsoluteWindowSize.Y
            )
        )
    end)
end

local function appendOutput(message, kind)
    outputCount += 1

    local label = Instance.new("TextLabel")

    label.Name = "Output_" .. outputCount
    label.Size = UDim2.new(1, -2, 0, 20)
    label.AutomaticSize = Enum.AutomaticSize.Y
    label.BackgroundTransparency = 1
    label.TextWrapped = true
    label.RichText = false
    label.TextSize = 14
    label.Font = EDITOR_FONT
    label.TextXAlignment = Enum.TextXAlignment.Left
    label.TextYAlignment = Enum.TextYAlignment.Top
    label.LayoutOrder = outputCount

    if kind == "warning" then
        label.Text = "[WARNING] " .. tostring(message)
        label.TextColor3 = COLORS.Warning

    elseif kind == "error" then
        label.Text = "[ERROR] " .. tostring(message)
        label.TextColor3 = COLORS.Error

    elseif kind == "success" then
        label.Text = "[IDE] " .. tostring(message)
        label.TextColor3 = COLORS.Success

    elseif kind == "system" then
        label.Text = "[IDE] " .. tostring(message)
        label.TextColor3 = COLORS.Accent

    else
        label.Text = tostring(message)
        label.TextColor3 = COLORS.Text
    end

    label.Parent = outputScroll
    scrollOutputToBottom()
end

local function clearOutput()
    for _, child in ipairs(outputScroll:GetChildren()) do
        if child:IsA("TextLabel") then
            child:Destroy()
        end
    end

    outputCount = 0
end

local function executeCode(source)
    local compiled, compileError = loadstring(source)

    if not compiled then
        appendOutput(
            "Failed to compile: " .. tostring(compileError),
            "error"
        )
        return
    end

    local succeeded, runtimeError = pcall(compiled)

    if not succeeded then
        appendOutput(
            "failed to execute: " .. tostring(runtimeError),
            "error"
        )
    else
        appendOutput(
            "code executed successfully",
            "success"
        )
    end
end

local function insertIndent()
    local cursorPosition = codeEditor.CursorPosition

    if cursorPosition == -1 then
        codeEditor.Text ..= INDENT
        codeEditor.CursorPosition = #codeEditor.Text + 1
        return
    end

    local beforeCursor =
        codeEditor.Text:sub(1, cursorPosition - 1)

    local afterCursor =
        codeEditor.Text:sub(cursorPosition)

    codeEditor.Text =
        beforeCursor .. INDENT .. afterCursor

    codeEditor.CursorPosition =
        cursorPosition + #INDENT
end

local function saveExpandedCenter()
    local position = getWindowPosition()
    local size = getWindowSize()

    savedExpandedCenter = position + size / 2
end

local function setMinimized(minimized)
    if minimized == isMinimized then
        return
    end

    if minimized then
        savedExpandedSize = getWindowSize()
        saveExpandedCenter()

        isMinimized = true

        if topDragDetector then
            topDragDetector.Enabled = false
        end

        if minimizedDragDetector then
            minimizedDragDetector.Enabled = true
        end

        toolbar.Visible = false
        editorPanel.Visible = false
        outputPanel.Visible = false
        statusBar.Visible = false
        resizeHandle.Visible = false
        title.Visible = false

        local miniSize =
            Vector2.new(MINIMIZED_SIZE, MINIMIZED_SIZE)

        local center =
            savedExpandedCenter
            or (getWindowPosition() + getWindowSize() / 2)

        local miniPosition =
            clampPosition(center - miniSize / 2, miniSize)

        mainWindow.Size =
            UDim2.fromOffset(miniSize.X, miniSize.Y)

        mainWindow.Position =
            UDim2.fromOffset(
                miniPosition.X,
                miniPosition.Y
            )

        windowCorner.CornerRadius = UDim.new(1, 0)
        topBarCorner.CornerRadius = UDim.new(1, 0)

        topBar.Size = UDim2.fromScale(1, 1)
        dragHandle.Size = UDim2.fromScale(1, 1)

        minimizeButton.Position = UDim2.fromOffset(0, 0)
        minimizeButton.Size = UDim2.fromScale(1, 1)

        minimizeButton.Text = "↗"
        minimizeButton.TextSize = 24

        customCaret.Visible = false

    else
        isMinimized = false

        if topDragDetector then
            topDragDetector.Enabled = true
        end

        if minimizedDragDetector then
            minimizedDragDetector.Enabled = false
        end

        local restoredSize = savedExpandedSize

        if autoFitToViewport then
            restoredSize = getResponsiveWindowSize()
        else
            local available = getAvailableWindowSize()

            restoredSize = Vector2.new(
                math.min(restoredSize.X, available.X),
                math.min(restoredSize.Y, available.Y)
            )
        end

        mainWindow.Size =
            UDim2.fromOffset(
                restoredSize.X,
                restoredSize.Y
            )

        windowCorner.CornerRadius = UDim.new(0, 9)
        topBarCorner.CornerRadius = UDim.new(0, 9)

        topBar.Size = UDim2.new(1, 0, 0, 38)
        dragHandle.Size = UDim2.new(1, -48, 1, 0)

        minimizeButton.Position =
            UDim2.new(1, -48, 0, 0)

        minimizeButton.Size =
            UDim2.fromOffset(48, 38)

        minimizeButton.Text = "-"
        minimizeButton.TextSize = 21

        title.Visible = true
        toolbar.Visible = true
        editorPanel.Visible = true
        outputPanel.Visible = true
        statusBar.Visible = true
        resizeHandle.Visible = true

        local center = savedExpandedCenter

        if not center then
            center = getViewportSize() / 2
        end

        local restoredPosition =
            clampPosition(
                center - restoredSize / 2,
                restoredSize
            )

        mainWindow.Position =
            UDim2.fromOffset(
                restoredPosition.X,
                restoredPosition.Y
            )

        updateLayout()
        scheduleVisibleRender()
    end
end

local dragging = false
local dragInput = nil
local dragStart = nil
local dragStartPosition = nil
local dragDistance = 0
local dragStartedMinimized = false
local suppressMinimizeActivation = false

local function getInputPosition(input)
    local position = input.Position
    return Vector2.new(position.X, position.Y)
end

local function suppressMinimizeTap()
    suppressMinimizeActivation = true

    task.delay(0.35, function()
        suppressMinimizeActivation = false
    end)
end

local function finishDrag(input)
    if not dragging then
        return
    end

    if input
        and input ~= dragInput
        and not (
            dragInput
            and dragInput.UserInputType
                == Enum.UserInputType.Touch
            and input.UserInputType
                == Enum.UserInputType.Touch
        ) then
        return
    end

    if dragStartedMinimized and dragDistance > 7 then
        suppressMinimizeTap()
    end

    dragging = false
    dragInput = nil
end

local function beginDrag(input)
    if input.UserInputType ~= Enum.UserInputType.MouseButton1
        and input.UserInputType ~= Enum.UserInputType.Touch then
        return
    end

    if dragging and dragInput == input then
        return
    end

    dragging = true
    dragInput = input
    dragStart = getInputPosition(input)
    dragStartPosition = getWindowPosition()
    dragDistance = 0
    dragStartedMinimized = isMinimized

    input.Changed:Connect(function()
        if input.UserInputState == Enum.UserInputState.End then
            finishDrag(input)
        end
    end)
end

local function isPointInside(guiObject, point)
    local position = guiObject.AbsolutePosition
    local size = guiObject.AbsoluteSize

    return point.X >= position.X
        and point.X <= position.X + size.X
        and point.Y >= position.Y
        and point.Y <= position.Y + size.Y
end

local function updateDrag(input)
    if not dragging or not dragInput then
        return
    end

    if dragInput.UserInputType == Enum.UserInputType.Touch then
        if input.UserInputType ~= Enum.UserInputType.Touch then
            return
        end
    elseif input.UserInputType
        ~= Enum.UserInputType.MouseMovement then
        return
    end

    local delta = getInputPosition(input) - dragStart

    dragDistance = math.max(
        dragDistance,
        delta.Magnitude
    )

    if dragStartedMinimized and dragDistance > 7 then
        suppressMinimizeActivation = true
    end

    local nextPosition =
        clampPosition(dragStartPosition + delta)

    mainWindow.Position =
        UDim2.fromOffset(
            nextPosition.X,
            nextPosition.Y
        )

    savedExpandedCenter =
        nextPosition + getWindowSize() / 2
end

local function beginDetectorDrag(inputPosition)
    dragging = true
    dragInput = nil
    dragStart = inputPosition
    dragStartPosition = getWindowPosition()
    dragDistance = 0
    dragStartedMinimized = isMinimized
end

local function updateDetectorDrag(inputPosition)
    if not dragging or not dragStart then
        return
    end

    local delta = inputPosition - dragStart

    dragDistance = math.max(
        dragDistance,
        delta.Magnitude
    )

    if dragStartedMinimized and dragDistance > 7 then
        suppressMinimizeActivation = true
    end

    local nextPosition =
        clampPosition(dragStartPosition + delta)

    mainWindow.Position =
        UDim2.fromOffset(
            nextPosition.X,
            nextPosition.Y
        )

    savedExpandedCenter =
        nextPosition + getWindowSize() / 2
end

local function finishDetectorDrag()
    if not dragging or dragInput then
        return
    end

    if dragStartedMinimized and dragDistance > 7 then
        suppressMinimizeTap()
    end

    dragging = false
    dragStart = nil
end

local function setupUIDragDetectors()
    local topDetector
    local miniDetector

    local success = pcall(function()
        topDetector = Instance.new("UIDragDetector")
        miniDetector = Instance.new("UIDragDetector")

        topDetector.Name = "TopBarDragDetector"
        miniDetector.Name = "MinimizedDragDetector"

        topDetector.DragStyle =
            Enum.UIDragDetectorDragStyle.TranslatePlane

        miniDetector.DragStyle =
            Enum.UIDragDetectorDragStyle.TranslatePlane

        topDetector.ResponseStyle =
            Enum.UIDragDetectorResponseStyle.CustomOffset

        miniDetector.ResponseStyle =
            Enum.UIDragDetectorResponseStyle.CustomOffset

        topDetector.Parent = dragHandle
        miniDetector.Parent = minimizeButton
    end)

    if not success then
        if topDetector then
            topDetector:Destroy()
        end

        if miniDetector then
            miniDetector:Destroy()
        end

        return false
    end

    topDragDetector = topDetector
    minimizedDragDetector = miniDetector

    topDragDetector.Enabled = not isMinimized
    minimizedDragDetector.Enabled = isMinimized

    topDragDetector.DragStart:Connect(function(inputPosition)
        if not isMinimized then
            beginDetectorDrag(inputPosition)
        end
    end)

    topDragDetector.DragContinue:Connect(function(inputPosition)
        if not isMinimized then
            updateDetectorDrag(inputPosition)
        end
    end)

    topDragDetector.DragEnd:Connect(
        finishDetectorDrag
    )

    minimizedDragDetector.DragStart:Connect(
        function(inputPosition)
            if isMinimized then
                beginDetectorDrag(inputPosition)
            end
        end
    )

    minimizedDragDetector.DragContinue:Connect(
        function(inputPosition)
            if isMinimized then
                updateDetectorDrag(inputPosition)
            end
        end
    )

    minimizedDragDetector.DragEnd:Connect(
        finishDetectorDrag
    )

    return true
end

local usingUIDragDetector = setupUIDragDetectors()

if not usingUIDragDetector then
    dragHandle.InputBegan:Connect(beginDrag)

    minimizeButton.InputBegan:Connect(function(input)
        if isMinimized then
            beginDrag(input)
        end
    end)

    UserInputService.InputChanged:Connect(updateDrag)
    UserInputService.TouchMoved:Connect(updateDrag)

    UserInputService.InputEnded:Connect(finishDrag)
    UserInputService.TouchEnded:Connect(finishDrag)
end

local resizing = false
local resizeInput = nil
local resizeStart = nil
local resizeStartSize = nil

local function beginResize(input)
    if isMinimized then
        return
    end

    if input.UserInputType ~= Enum.UserInputType.MouseButton1
        and input.UserInputType ~= Enum.UserInputType.Touch then
        return
    end

    resizing = true
    resizeInput = input
    resizeStart = getInputPosition(input)
    resizeStartSize = getWindowSize()
    autoFitToViewport = false

    input.Changed:Connect(function()
        if input.UserInputState == Enum.UserInputState.End then
            resizing = false
            resizeInput = nil
            savedExpandedSize = getWindowSize()
        end
    end)
end

local function updateResize(input)
    if not resizing or not resizeInput then
        return
    end

    if resizeInput.UserInputType == Enum.UserInputType.Touch then
        if input.UserInputType ~= Enum.UserInputType.Touch then
            return
        end
    elseif input.UserInputType
        ~= Enum.UserInputType.MouseMovement then
        return
    end

    local delta =
        getInputPosition(input) - resizeStart

    local available = getAvailableWindowSize()

    local width = math.clamp(
        resizeStartSize.X + delta.X,
        math.min(MIN_WINDOW_SIZE.X, available.X),
        math.min(MAX_WINDOW_SIZE.X, available.X)
    )

    local height = math.clamp(
        resizeStartSize.Y + delta.Y,
        math.min(MIN_WINDOW_SIZE.Y, available.Y),
        math.min(MAX_WINDOW_SIZE.Y, available.Y)
    )

    mainWindow.Size =
        UDim2.fromOffset(width, height)

    keepWindowVisible()
    updateLayout()
end

resizeHandle.InputBegan:Connect(beginResize)
UserInputService.InputChanged:Connect(updateResize)

local function updateForViewport()
    if isMinimized then
        keepWindowVisible()
        return
    end

    local oldSize = getWindowSize()
    local newSize

    if autoFitToViewport then
        newSize = getResponsiveWindowSize()
    else
        local available = getAvailableWindowSize()

        newSize = Vector2.new(
            math.min(oldSize.X, available.X),
            math.min(oldSize.Y, available.Y)
        )
    end

    mainWindow.Size =
        UDim2.fromOffset(newSize.X, newSize.Y)

    if autoFitToViewport
        or oldSize.X > newSize.X
        or oldSize.Y > newSize.Y then
        centerWindow(newSize)
    else
        keepWindowVisible()
    end

    updateLayout()
end

local cameraConnection

local function watchCamera()
    if cameraConnection then
        cameraConnection:Disconnect()
        cameraConnection = nil
    end

    local camera = workspace.CurrentCamera

    if camera then
        cameraConnection =
            camera:GetPropertyChangedSignal(
                "ViewportSize"
            ):Connect(updateForViewport)
    end
end

workspace:GetPropertyChangedSignal(
    "CurrentCamera"
):Connect(function()
    watchCamera()
    updateForViewport()
end)

screenGui:GetPropertyChangedSignal(
    "AbsoluteSize"
):Connect(updateForViewport)

local normalizingQuotes = false

local function refreshEditor()
    if normalizingQuotes then
        return
    end

    local text = codeEditor.Text or ""
    local normalizedText = text

    if text:find("\226\128", 1, true) then
        normalizedText = normalizeAsciiQuotes(text)
    end

    if normalizedText ~= text then
        normalizingQuotes = true

        local cursorPosition = codeEditor.CursorPosition
        local normalizedCursorPosition

        if cursorPosition >= 1 then
            normalizedCursorPosition =
                #normalizeAsciiQuotes(
                    text:sub(1, cursorPosition - 1)
                ) + 1
        end

        codeEditor.Text = normalizedText

        if normalizedCursorPosition then
            codeEditor.CursorPosition = math.min(
                normalizedCursorPosition,
                #normalizedText + 1
            )
        end

        normalizingQuotes = false
    end

    updateEditorContent()
    updateCursor()
end

codeEditor:GetPropertyChangedSignal(
    "Text"
):Connect(refreshEditor)

codeEditor:GetPropertyChangedSignal(
    "CursorPosition"
):Connect(function()
    updateCursor()
    resetCaretBlink()
end)

codeEditor.Focused:Connect(function()
    resetCaretBlink()
    updateCursor()
end)

codeEditor.FocusLost:Connect(function()
    customCaret.Visible = false
end)

editorScroll:GetPropertyChangedSignal(
    "CanvasPosition"
):Connect(updateLineNumberScroll)

mainWindow:GetPropertyChangedSignal(
    "AbsoluteSize"
):Connect(updateLayout)

executeButton.Activated:Connect(function()
    executeCode(codeEditor.Text)
end)

clearEditorButton.Activated:Connect(function()
    codeEditor.Text = ""
    codeEditor:CaptureFocus()
end)

clearOutputButton.Activated:Connect(function()
    clearOutput()
end)

minimizeButton.Activated:Connect(function()
    if suppressMinimizeActivation then
        suppressMinimizeActivation = false
        return
    end

    setMinimized(not isMinimized)
end)

UserInputService.InputBegan:Connect(function(input, gameProcessed)
    if not screenGui.Parent then
        return
    end

    if not usingUIDragDetector
        and (
            input.UserInputType
                == Enum.UserInputType.MouseButton1
            or input.UserInputType
                == Enum.UserInputType.Touch
        ) then

        local point = getInputPosition(input)

        if isMinimized then
            if isPointInside(minimizeButton, point) then
                beginDrag(input)
            end
        elseif isPointInside(dragHandle, point) then
            beginDrag(input)
        end
    end

    local editorFocused = codeEditor:IsFocused()

    local controlDown =
        UserInputService:IsKeyDown(
            Enum.KeyCode.LeftControl
        )
        or UserInputService:IsKeyDown(
            Enum.KeyCode.RightControl
        )

    local commandDown =
        UserInputService:IsKeyDown(
            Enum.KeyCode.LeftMeta
        )
        or UserInputService:IsKeyDown(
            Enum.KeyCode.RightMeta
        )

    local modifierDown =
        controlDown or commandDown

    if (not gameProcessed or editorFocused)
        and modifierDown
        and input.KeyCode == Enum.KeyCode.Return then

        executeCode(codeEditor.Text)
        return
    end

    if (not gameProcessed or editorFocused)
        and modifierDown
        and input.KeyCode == Enum.KeyCode.L then

        clearOutput()
        return
    end

    if editorFocused
        and input.KeyCode == Enum.KeyCode.Tab then

        insertIndent()
        return
    end

    if gameProcessed then
        return
    end
end)

watchCamera()

local initialSize = getResponsiveWindowSize()

mainWindow.Size =
    UDim2.fromOffset(
        initialSize.X,
        initialSize.Y
    )

centerWindow(initialSize)

rebuildDocumentCache(codeEditor.Text or "")
updateLayout()
updateEditorContent()
updateCursor()
scheduleVisibleRender()

appendOutput("initialized", "success")
