if not game:IsLoaded() then
	game.Loaded:Wait()
end

local Players = game:GetService("Players")
local UIS = game:GetService("UserInputService")
local TextService = game:GetService("TextService")
local RunService = game:GetService("RunService")

local PlayerGui = Players.LocalPlayer:WaitForChild("PlayerGui")

local TITLE = "temporary ui"

local DEFAULT_SIZE = Vector2.new(780, 540)
local MIN_SIZE = Vector2.new(280, 320)
local MAX_SIZE = Vector2.new(1200, 850)

local MINIMIZED_SIZE = 56

local LINE_HEIGHT = 21
local EDITOR_FONT = Enum.Font.Code
local TEXT_SIZE = 16
local TAB_TEXT = "    "

local CHUNK_SIZE = 48
local EXTRA_LINES = 30
local HIGHLIGHT_DELAY = 0.04
local MAX_OUTPUT_ENTRIES = 200

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
	Success = Color3.fromRGB(95, 215, 135)
}

local SYNTAX_COLORS = {
	Keyword = "#C586C0",
	String = "#CE9178",
	Number = "#B5CEA8",
	Comment = "#6A9955",
	Builtin = "#4FC1FF",
	Boolean = "#569CD6"
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
	["while"] = true
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
	CFrame = true
}

local Old = PlayerGui:FindFirstChild("MiniLuaIDE")

if Old then
	Old:Destroy()
end

local function addCorner(object, radius)
	local corner = Instance.new("UICorner")
	corner.CornerRadius = UDim.new(0, radius or 6)
	corner.Parent = object

	return corner
end

local function addStroke(object, color, thickness)
	local stroke = Instance.new("UIStroke")

	stroke.Color = color or COLORS.Border
	stroke.Thickness = thickness or 1
	stroke.ApplyStrokeMode = Enum.ApplyStrokeMode.Border
	stroke.Parent = object

	return stroke
end

local function makeButton(parent, text, width, color)
	local button = Instance.new("TextButton")

	local name = text:gsub("%s+", "")
	name = name:gsub("[^%w]", "")

	button.Name = name .. "Button"

	button.Size = UDim2.fromOffset(width, 30)

	button.BackgroundColor3 =
		color or Color3.fromRGB(45, 47, 56)

	button.BorderSizePixel = 0

	button.AutoButtonColor = true
	button.Active = true

	button.Text = text
	button.TextColor3 = COLORS.Text
	button.TextSize = 14
	button.Font = Enum.Font.GothamMedium
	button.TextTruncate = Enum.TextTruncate.AtEnd

	button.Parent = parent

	addCorner(button, 5)

	return button
end

local ScreenGui = Instance.new("ScreenGui")

ScreenGui.Name = "MiniLuaIDE"
ScreenGui.ResetOnSpawn = false
ScreenGui.IgnoreGuiInset = false
ScreenGui.ZIndexBehavior = Enum.ZIndexBehavior.Sibling

pcall(function()
	ScreenGui.ScreenInsets =
		Enum.ScreenInsets.CoreUISafeInsets
end)

pcall(function()
	ScreenGui.SafeAreaCompatibility =
		Enum.SafeAreaCompatibility.None
end)

ScreenGui.Parent = PlayerGui

local MainWindow = Instance.new("Frame")

MainWindow.Name = "MainWindow"

MainWindow.Size =
	UDim2.fromOffset(
		DEFAULT_SIZE.X,
		DEFAULT_SIZE.Y
	)

MainWindow.Position =
	UDim2.fromOffset(0, 0)

MainWindow.BackgroundColor3 =
	COLORS.Background

MainWindow.BorderSizePixel = 0

MainWindow.ClipsDescendants = true
MainWindow.Active = true

MainWindow.Parent = ScreenGui

local MainCorner =
	addCorner(MainWindow, 9)

addStroke(
	MainWindow,
	COLORS.Border,
	1
)

local TopBar = Instance.new("Frame")

TopBar.Name = "TopBar"

TopBar.Size =
	UDim2.new(
		1,
		0,
		0,
		38
	)

TopBar.BackgroundColor3 =
	COLORS.TopBar

TopBar.BorderSizePixel = 0
TopBar.Active = true

TopBar.Parent = MainWindow

local TopCorner =
	addCorner(TopBar, 9)

local DragHandle = Instance.new("Frame")

DragHandle.Name = "DragHandle"

DragHandle.Size =
	UDim2.new(
		1,
		-48,
		1,
		0
	)

DragHandle.BackgroundTransparency = 1
DragHandle.BorderSizePixel = 0

DragHandle.Active = true

DragHandle.ZIndex = 2

DragHandle.Parent = TopBar

local TitleLabel = Instance.new("TextLabel")

TitleLabel.Name = "Title"

TitleLabel.Size =
	UDim2.new(
		1,
		-13,
		1,
		0
	)

TitleLabel.Position =
	UDim2.fromOffset(
		13,
		0
	)

TitleLabel.BackgroundTransparency = 1

TitleLabel.Active = false
TitleLabel.ZIndex = 3

TitleLabel.Text = TITLE

TitleLabel.TextColor3 =
	COLORS.Text

TitleLabel.TextSize = 15

TitleLabel.Font =
	Enum.Font.GothamBold

TitleLabel.TextXAlignment =
	Enum.TextXAlignment.Left

TitleLabel.Parent =
	DragHandle

local MinimizeButton =
	Instance.new("TextButton")

MinimizeButton.Name =
	"MinimizeButton"

MinimizeButton.Size =
	UDim2.fromOffset(
		48,
		38
	)

MinimizeButton.Position =
	UDim2.new(
		1,
		-48,
		0,
		0
	)

MinimizeButton.BackgroundTransparency = 1

MinimizeButton.Text = "-"

MinimizeButton.TextColor3 =
	COLORS.SubText

MinimizeButton.TextSize = 21

MinimizeButton.Font =
	Enum.Font.GothamBold

MinimizeButton.AutoButtonColor = false
MinimizeButton.Active = true

MinimizeButton.ZIndex = 4

MinimizeButton.Parent =
	TopBar

local Toolbar =
	Instance.new("Frame")

Toolbar.Name = "Toolbar"

Toolbar.Position =
	UDim2.fromOffset(
		0,
		38
	)

Toolbar.Size =
	UDim2.new(
		1,
		0,
		0,
		42
	)

Toolbar.BackgroundColor3 =
	COLORS.Toolbar

Toolbar.BorderSizePixel = 0

Toolbar.Parent =
	MainWindow

local ToolbarPadding =
	Instance.new("UIPadding")

ToolbarPadding.PaddingLeft =
	UDim.new(0, 8)

ToolbarPadding.PaddingRight =
	UDim.new(0, 8)

ToolbarPadding.PaddingTop =
	UDim.new(0, 6)

ToolbarPadding.Parent =
	Toolbar

local ToolbarLayout =
	Instance.new("UIListLayout")

ToolbarLayout.FillDirection =
	Enum.FillDirection.Horizontal

ToolbarLayout.HorizontalAlignment =
	Enum.HorizontalAlignment.Left

ToolbarLayout.VerticalAlignment =
	Enum.VerticalAlignment.Top

ToolbarLayout.SortOrder =
	Enum.SortOrder.LayoutOrder

ToolbarLayout.Padding =
	UDim.new(0, 6)

ToolbarLayout.Parent =
	Toolbar

local ExecuteButton =
	makeButton(
		Toolbar,
		"▶ Execute",
		112,
		COLORS.Run
	)

local ClipboardButton =
	makeButton(
		Toolbar,
		"Execute Clipboard",
		152,
		COLORS.Accent
	)

local ClearEditorButton =
	makeButton(
		Toolbar,
		"Clear Editor",
		100
	)

local ClearOutputButton =
	makeButton(
		Toolbar,
		"Clear Output",
		104
	)

ExecuteButton.LayoutOrder = 1
ClipboardButton.LayoutOrder = 2
ClearEditorButton.LayoutOrder = 3
ClearOutputButton.LayoutOrder = 4

local EditorPanel =
	Instance.new("Frame")

EditorPanel.Name =
	"EditorPanel"

EditorPanel.BackgroundColor3 =
	COLORS.Panel

EditorPanel.BorderSizePixel = 0

EditorPanel.ClipsDescendants = true

EditorPanel.Parent =
	MainWindow

addCorner(
	EditorPanel,
	6
)

addStroke(
	EditorPanel,
	COLORS.Border,
	1
)

local Gutter =
	Instance.new("Frame")

Gutter.Name =
	"LineNumberGutter"

Gutter.Size =
	UDim2.new(
		0,
		52,
		1,
		0
	)

Gutter.BackgroundColor3 =
	COLORS.Gutter

Gutter.BorderSizePixel = 0

Gutter.ClipsDescendants = true

Gutter.Parent =
	EditorPanel

local GutterSeparator =
	Instance.new("Frame")

GutterSeparator.Size =
	UDim2.new(
		0,
		1,
		1,
		0
	)

GutterSeparator.Position =
	UDim2.new(
		1,
		-1,
		0,
		0
	)

GutterSeparator.BackgroundColor3 =
	COLORS.Border

GutterSeparator.BorderSizePixel = 0

GutterSeparator.Parent =
	Gutter

local EditorScroll =
	Instance.new("ScrollingFrame")

EditorScroll.Name =
	"EditorScroll"

EditorScroll.Position =
	UDim2.fromOffset(
		52,
		0
	)

EditorScroll.Size =
	UDim2.new(
		1,
		-52,
		1,
		0
	)

EditorScroll.BackgroundColor3 =
	COLORS.Editor

EditorScroll.BorderSizePixel = 0

EditorScroll.ClipsDescendants = true

EditorScroll.ScrollBarThickness = 7

EditorScroll.ScrollBarImageColor3 =
	Color3.fromRGB(
		80,
		83,
		95
	)

EditorScroll.CanvasSize =
	UDim2.fromOffset(
		0,
		0
	)

EditorScroll.ScrollingDirection =
	Enum.ScrollingDirection.XY

EditorScroll.Parent =
	EditorPanel

addCorner(
	EditorScroll,
	6
)

local CodeEditor =
	Instance.new("TextBox")

CodeEditor.Name =
	"CodeEditor"

CodeEditor.Position =
	UDim2.fromOffset(
		10,
		7
	)

CodeEditor.Size =
	UDim2.new(
		1,
		-22,
		0,
		300
	)

CodeEditor.BackgroundTransparency = 1
CodeEditor.BorderSizePixel = 0

CodeEditor.ClearTextOnFocus = false

CodeEditor.MultiLine = true
CodeEditor.TextEditable = true
CodeEditor.Active = true
CodeEditor.RichText = false
CodeEditor.TextScaled = false
CodeEditor.ClipsDescendants = true

CodeEditor.TextWrapped = false

CodeEditor.TextColor3 =
	COLORS.Text

CodeEditor.PlaceholderColor3 =
	Color3.fromRGB(
		90,
		94,
		105
	)

CodeEditor.PlaceholderText =
	"-- opiumware ios temp yay"

CodeEditor.TextSize =
	TEXT_SIZE

CodeEditor.Font =
	EDITOR_FONT

CodeEditor.TextXAlignment =
	Enum.TextXAlignment.Left

CodeEditor.TextYAlignment =
	Enum.TextYAlignment.Top

CodeEditor.Text =
	[[print("Hello from the editor!")]]

-- Text itself is drawn by syntax labels.
CodeEditor.TextTransparency = 1

CodeEditor.TextStrokeTransparency = 1

CodeEditor.ZIndex = 5

CodeEditor.Parent =
	EditorScroll

pcall(function()
	CodeEditor.ShowNativeInput = false
end)

pcall(function()
	CodeEditor.MaxVisibleGraphemes = -1
end)

pcall(function()
	CodeEditor.LineHeight =
		LINE_HEIGHT / TEXT_SIZE
end)

-------------------------------------------------------
-- Custom caret for the transparent editor input layer
-------------------------------------------------------

local CustomCaret =
	Instance.new("Frame")

CustomCaret.Name =
	"CustomCaret"

CustomCaret.Size =
	UDim2.fromOffset(
		2,
		LINE_HEIGHT - 4
	)

CustomCaret.BackgroundColor3 = COLORS.Text
CustomCaret.BackgroundTransparency = 0

CustomCaret.BorderSizePixel = 0

CustomCaret.Visible = false

CustomCaret.Active = false

CustomCaret.ZIndex = 10

CustomCaret.Parent =
	EditorScroll


-------------------------------------------------------

local OutputPanel =
	Instance.new("Frame")

OutputPanel.Name =
	"OutputPanel"

OutputPanel.BackgroundColor3 =
	COLORS.Panel

OutputPanel.BorderSizePixel = 0

OutputPanel.ClipsDescendants = true

OutputPanel.Parent =
	MainWindow

addCorner(
	OutputPanel,
	6
)

addStroke(
	OutputPanel,
	COLORS.Border,
	1
)

local OutputHeader =
	Instance.new("Frame")

OutputHeader.Name =
	"OutputHeader"

OutputHeader.Size =
	UDim2.new(
		1,
		0,
		0,
		28
	)

OutputHeader.BackgroundColor3 =
	COLORS.Gutter

OutputHeader.BorderSizePixel = 0

OutputHeader.Parent =
	OutputPanel

local OutputTitle =
	Instance.new("TextLabel")

OutputTitle.Size =
	UDim2.new(
		1,
		-20,
		1,
		0
	)

OutputTitle.Position =
	UDim2.fromOffset(
		10,
		0
	)

OutputTitle.BackgroundTransparency = 1

OutputTitle.Text =
	"OUTPUT"

OutputTitle.TextColor3 =
	COLORS.SubText

OutputTitle.TextSize = 12

OutputTitle.Font =
	Enum.Font.GothamBold

OutputTitle.TextXAlignment =
	Enum.TextXAlignment.Left

OutputTitle.Parent =
	OutputHeader

addCorner(
	OutputHeader,
	6
)

local OutputScroll =
	Instance.new("ScrollingFrame")

OutputScroll.Name =
	"OutputScroll"

OutputScroll.Position =
	UDim2.fromOffset(
		0,
		28
	)

OutputScroll.Size =
	UDim2.new(
		1,
		0,
		1,
		-28
	)

OutputScroll.BackgroundColor3 =
	COLORS.Editor

OutputScroll.BorderSizePixel = 0

OutputScroll.ScrollBarThickness = 6

OutputScroll.ScrollBarImageColor3 =
	Color3.fromRGB(
		80,
		83,
		95
	)

OutputScroll.AutomaticCanvasSize =
	Enum.AutomaticSize.Y

OutputScroll.CanvasSize =
	UDim2.fromOffset(
		0,
		0
	)

OutputScroll.Parent =
	OutputPanel

addCorner(
	OutputScroll,
	6
)

local OutputPadding =
	Instance.new("UIPadding")

OutputPadding.PaddingLeft =
	UDim.new(0, 9)

OutputPadding.PaddingRight =
	UDim.new(0, 9)

OutputPadding.PaddingTop =
	UDim.new(0, 7)

OutputPadding.PaddingBottom =
	UDim.new(0, 7)

OutputPadding.Parent =
	OutputScroll

local OutputLayout =
	Instance.new("UIListLayout")

OutputLayout.SortOrder =
	Enum.SortOrder.LayoutOrder

OutputLayout.Padding =
	UDim.new(0, 3)

OutputLayout.Parent =
	OutputScroll

local StatusBar =
	Instance.new("Frame")

StatusBar.Name =
	"StatusBar"

StatusBar.BackgroundColor3 =
	COLORS.TopBar

StatusBar.BorderSizePixel = 0

StatusBar.Parent =
	MainWindow

addCorner(
	StatusBar,
	9
)

local StatusText =
	Instance.new("TextLabel")

StatusText.Name =
	"StatusText"

StatusText.Size =
	UDim2.new(
		0.7,
		-10,
		1,
		0
	)

StatusText.Position =
	UDim2.fromOffset(
		10,
		0
	)

StatusText.BackgroundTransparency = 1

StatusText.Text = ""

StatusText.TextColor3 =
	COLORS.SubText

StatusText.TextSize = 12

StatusText.Font =
	Enum.Font.Gotham

StatusText.TextXAlignment =
	Enum.TextXAlignment.Left

StatusText.TextTruncate =
	Enum.TextTruncate.AtEnd

StatusText.Parent =
	StatusBar

local CursorText =
	Instance.new("TextLabel")

CursorText.Name =
	"CursorText"

CursorText.Size =
	UDim2.new(
		0.3,
		-10,
		1,
		0
	)

CursorText.Position =
	UDim2.new(
		0.7,
		0,
		0,
		0
	)

CursorText.BackgroundTransparency = 1

CursorText.Text =
	"Ln 1, Col 1"

CursorText.TextColor3 =
	COLORS.SubText

CursorText.TextSize = 12

CursorText.Font =
	Enum.Font.Gotham

CursorText.TextXAlignment =
	Enum.TextXAlignment.Right

CursorText.Parent =
	StatusBar

local ResizeHandle =
	Instance.new("TextButton")

ResizeHandle.Name =
	"ResizeHandle"

ResizeHandle.Size =
	UDim2.fromOffset(
		28,
		28
	)

ResizeHandle.Position =
	UDim2.new(
		1,
		-28,
		1,
		-28
	)

ResizeHandle.BackgroundTransparency = 1

ResizeHandle.Text = "◢"

ResizeHandle.TextColor3 =
	COLORS.SubText

ResizeHandle.TextSize = 14

ResizeHandle.Font =
	Enum.Font.Code

ResizeHandle.ZIndex = 20
ResizeHandle.Active = true

ResizeHandle.Parent =
	MainWindow

local minimized = false
local autoSize = true

local previousSize =
	DEFAULT_SIZE

local centerPosition = nil

local outputCounter = 0

local outputLabels = {}

local globalConnections = {}

local dragging = false
local dragPointer = nil
local dragStart = nil
local dragWindowStart = nil

local resizing = false
local resizePointer = nil
local resizeStart = nil
local resizeWindowStart = nil
local resizeWindowSize = nil

local function trackGlobal(connection)
	table.insert(globalConnections, connection)
	return connection
end

local function getViewportSize()

	local size =
		ScreenGui.AbsoluteSize

	if size.X > 1
		and size.Y > 1
	then
		return size
	end

	local camera =
		workspace.CurrentCamera

	if camera then
		return camera.ViewportSize
	end

	return Vector2.new(
		1920,
		1080
	)
end

local function getEdgePadding()

	if UIS.TouchEnabled then
		return 12
	end

	return 24
end

local function getAvailableSize()

	local viewport =
		getViewportSize()

	local padding =
		getEdgePadding()

	return Vector2.new(

		math.max(
			1,
			viewport.X - padding * 2
		),

		math.max(
			1,
			viewport.Y - padding * 2
		)
	)
end

local function clampStartingSize(
	available,
	defaultValue,
	minValue,
	maxValue
)

	local result =
		math.min(
			available,
			defaultValue,
			maxValue
		)

	if available >= minValue then

		return math.max(
			minValue,
			result
		)

	end

	return available
end

local function getIdealSize()

	local available =
		getAvailableSize()

	return Vector2.new(

		clampStartingSize(
			available.X,
			DEFAULT_SIZE.X,
			MIN_SIZE.X,
			MAX_SIZE.X
		),

		clampStartingSize(
			available.Y,
			DEFAULT_SIZE.Y,
			MIN_SIZE.Y,
			MAX_SIZE.Y
		)
	)
end

local function getWindowSize()

	return Vector2.new(

		MainWindow.Size.X.Offset,

		MainWindow.Size.Y.Offset
	)
end

local function getWindowPosition()

	return Vector2.new(

		MainWindow.Position.X.Offset,

		MainWindow.Position.Y.Offset
	)
end

local function clampWindowPosition(
	position,
	windowSize
)

	local viewport =
		getViewportSize()

	local padding =
		getEdgePadding()

	windowSize =
		windowSize
		or getWindowSize()

	local maximum =
		Vector2.new(

			math.max(
				padding,
				viewport.X
					- windowSize.X
					- padding
			),

			math.max(
				padding,
				viewport.Y
					- windowSize.Y
					- padding
			)
		)

	return Vector2.new(

		math.clamp(
			position.X,
			padding,
			maximum.X
		),

		math.clamp(
			position.Y,
			padding,
			maximum.Y
		)
	)
end

local function centerWindow(
	windowSize
)

	local viewport =
		getViewportSize()

	windowSize =
		windowSize
		or getWindowSize()

	local position =
		Vector2.new(

			(viewport.X - windowSize.X) * 0.5,

			(viewport.Y - windowSize.Y) * 0.5
		)

	MainWindow.Position =
		UDim2.fromOffset(

			math.floor(
				position.X
			),

			math.floor(
				position.Y
			)
		)
end

local function keepWindowOnScreen()

	local position =
		clampWindowPosition(
			getWindowPosition()
		)

	MainWindow.Position =
		UDim2.fromOffset(
			position.X,
			position.Y
		)
end

local function escapeRichText(text)

	text =
		text:gsub(
			"&",
			"&amp;"
		)

	text =
		text:gsub(
			"<",
			"&lt;"
		)

	text =
		text:gsub(
			">",
			"&gt;"
		)

	return text
end

local function coloredText(
	text,
	color
)

	return
		'<font color="'
		.. color
		.. '">'
		.. escapeRichText(text)
		.. "</font>"
end

local function findStringEnd(
	text,
	position,
	quote
)

	position += 1

	while position <= #text do

		local character =
			text:sub(
				position,
				position
			)

		if character == "\\" then

			position += 2

		elseif character == quote then

			return position

		else

			position += 1

		end
	end

	return #text
end

local function findLongStringEnd(
	text,
	position
)

	local ending =
		text:find(
			"]]",
			position + 2,
			true
		)

	return
		ending
		and ending + 1
		or #text
end

local function highlightLua(text)

	local output =
		table.create(
			math.max(
				8,
				#text // 4
			)
		)

	local position = 1
	local length = #text

	local function append(value)

		output[
			#output + 1
		] = value

	end

	while position <= length do

		local character =
			text:sub(
				position,
				position
			)

		local two =
			text:sub(
				position,
				position + 1
			)

		local four =
			text:sub(
				position,
				position + 3
			)

		if four == "--[[" then

			local ending =
				findLongStringEnd(
					text,
					position + 2
				)

			append(
				coloredText(
					text:sub(
						position,
						ending
					),
					SYNTAX_COLORS.Comment
				)
			)

			position =
				ending + 1

		elseif two == "--" then

			local ending =
				text:find(
					"\n",
					position,
					true
				)

			ending =
				ending
				and ending - 1
				or length

			append(
				coloredText(
					text:sub(
						position,
						ending
					),
					SYNTAX_COLORS.Comment
				)
			)

			position =
				ending + 1

		elseif
			character == '"'
			or character == "'"
		then

			local ending =
				findStringEnd(
					text,
					position,
					character
				)

			append(
				coloredText(
					text:sub(
						position,
						ending
					),
					SYNTAX_COLORS.String
				)
			)

			position =
				ending + 1

		elseif two == "[[" then

			local ending =
				findLongStringEnd(
					text,
					position
				)

			append(
				coloredText(
					text:sub(
						position,
						ending
					),
					SYNTAX_COLORS.String
				)
			)

			position =
				ending + 1

		elseif character:match(
			"[%a_]"
		) then

			local ending =
				position + 1

			while
				ending <= length
				and text
					:sub(
						ending,
						ending
					)
					:match(
						"[%w_]"
					)
			do

				ending += 1

			end

			local word =
				text:sub(
					position,
					ending - 1
				)

			local color

			if
				word == "true"
				or word == "false"
				or word == "nil"
			then

				color =
					SYNTAX_COLORS.Boolean

			elseif KEYWORDS[word] then

				color =
					SYNTAX_COLORS.Keyword

			elseif BUILTINS[word] then

				color =
					SYNTAX_COLORS.Builtin

			end

			if color then

				append(
					coloredText(
						word,
						color
					)
				)

			else

				append(
					escapeRichText(
						word
					)
				)

			end

			position =
				ending

		elseif character:match(
			"%d"
		) then

			local ending =
				position + 1

			while
				ending <= length
				and text
					:sub(
						ending,
						ending
					)
					:match(
						"[%w_%.]"
					)
			do

				ending += 1

			end

			append(
				coloredText(
					text:sub(
						position,
						ending - 1
					),
					SYNTAX_COLORS.Number
				)
			)

			position =
				ending

		else

			append(
				escapeRichText(
					character
				)
			)

			position += 1

		end
	end

	return table.concat(output)
end

local function normalizeQuotes(text)

	text =
		text:gsub(
			"“",
			'"'
		)

	text =
		text:gsub(
			"”",
			'"'
		)

	text =
		text:gsub(
			"„",
			'"'
		)

	text =
		text:gsub(
			"‟",
			'"'
		)

	text =
		text:gsub(
			"‘",
			"'"
		)

	text =
		text:gsub(
			"’",
			"'"
		)

	text =
		text:gsub(
			"‚",
			"'"
		)

	text =
		text:gsub(
			"‛",
			"'"
		)

	return text
end

local cachedText = nil

local lines = {""}

local lineStarts = {1}

local longestLine = ""

local longestWidth = 0

local syntaxLabels = {}
local numberLabels = {}
local chunks = {}

local renderPending = false

local function destroySyntaxLabels()

	for _, object in pairs(
		syntaxLabels
	) do

		object:Destroy()

	end

	for _, object in pairs(
		numberLabels
	) do

		object:Destroy()

	end

	table.clear(
		syntaxLabels
	)

	table.clear(
		numberLabels
	)
end

------------------------------------------------
-- Highlight parsing
-- IMPORTANT:
-- labels are NOT destroyed while typing.
------------------------------------------------

local function parseSource(text)

	cachedText = text

	table.clear(lines)
	table.clear(lineStarts)
	table.clear(chunks)

	longestLine = ""

	local position = 1

	local length =
		#text

	while true do

		lineStarts[
			#lineStarts + 1
		] = position

		local newline =
			text:find(
				"\n",
				position,
				true
			)

		local line

		if newline then

			line =
				text:sub(
					position,
					newline - 1
				)

		else

			line =
				text:sub(
					position
				)

		end

		lines[
			#lines + 1
		] = line

		if #line >
			#longestLine
		then

			longestLine =
				line

		end

		if not newline then
			break
		end

		position =
			newline + 1

		if position >
			length + 1
		then
			break
		end
	end

	if #lines == 0 then

		lines[1] = ""
		lineStarts[1] = 1

	end

	if longestLine == "" then

		longestWidth = 0

	else

		local expanded =
			longestLine:gsub(
				"\t",
				TAB_TEXT
			)

		local success,
			size =
			pcall(function()

				return TextService:GetTextSize(

					expanded,

					TEXT_SIZE,

					EDITOR_FONT,

					Vector2.new(
						1000000,
						LINE_HEIGHT
					)
				)

			end)

		if success then

			longestWidth =
				size.X

		else

			longestWidth =
				#longestLine
				* TEXT_SIZE
				* 0.61

		end
	end
end

local function createSyntaxLabel()

	local label =
		Instance.new("TextLabel")

	label.BackgroundTransparency = 1
	label.BorderSizePixel = 0

	label.RichText = true

	label.TextWrapped = false

	label.TextColor3 =
		COLORS.Text

	label.TextSize =
		TEXT_SIZE

	label.Font =
		EDITOR_FONT

	label.TextXAlignment =
		Enum.TextXAlignment.Left

	label.TextYAlignment =
		Enum.TextYAlignment.Top

	label.ZIndex = 1

	pcall(function()

		label.LineHeight =
			LINE_HEIGHT
			/ TEXT_SIZE

	end)

	pcall(function()

		label.MaxVisibleGraphemes =
			-1

	end)

	label.Name =
		"SyntaxChunk"

	label.Parent =
		EditorScroll

	return label
end

local function createNumberLabel()

	local label =
		Instance.new("TextLabel")

	label.BackgroundTransparency = 1
	label.BorderSizePixel = 0

	label.TextWrapped = false
	label.RichText = false

	label.TextColor3 =
		Color3.fromRGB(
			100,
			105,
			118
		)

	label.TextSize =
		TEXT_SIZE

	label.Font =
		EDITOR_FONT

	label.TextXAlignment =
		Enum.TextXAlignment.Right

	label.TextYAlignment =
		Enum.TextYAlignment.Top

	pcall(function()

		label.LineHeight =
			LINE_HEIGHT
			/ TEXT_SIZE

	end)

	pcall(function()

		label.MaxVisibleGraphemes =
			-1

	end)

	label.Parent =
		Gutter

	return label
end

local function getChunk(
	startLine
)

	local cached =
		chunks[startLine]

	if cached then
		return cached
	end

	local endLine =
		math.min(
			#lines,
			startLine
				+ CHUNK_SIZE
				- 1
		)

	local source =
		table.concat(
			lines,
			"\n",
			startLine,
			endLine
		)

	local numbers =
		table.create(
			endLine
				- startLine
				+ 1
		)

	for line =
		startLine,
		endLine
	do

		numbers[
			line
				- startLine
				+ 1
		] =
			tostring(line)

	end

	local success,
		highlighted =
		pcall(
			highlightLua,
			source
		)

	if not success then

		highlighted =
			escapeRichText(
				source
			)

		StatusText.Text =
			"Syntax highlight fallback active"

	end

	cached = {

		endLine =
			endLine,

		highlighted =
			highlighted,

		lineNumbers =
			table.concat(
				numbers,
				"\n"
			)
	}

	chunks[startLine] =
		cached

	return cached
end

local function getCanvasSize()

	local lineCount =
		math.max(
			1,
			#lines
		)

	local height =
		math.max(

			EditorPanel.AbsoluteSize.Y
				- 14,

			lineCount
				* LINE_HEIGHT
				+ 20
		)

	local width =
		math.max(

			EditorScroll.AbsoluteSize.X
				- 25,

			longestWidth + 40
			, CodeEditor.TextBounds.X + 40
		)

	return Vector2.new(
		width,
		height
	)
end

local rendererFallback =
	false

local function enableRendererFallback(
	reason
)

	if rendererFallback then
		return
	end

	rendererFallback = true

	destroySyntaxLabels()

	CodeEditor.TextTransparency = 0

	StatusText.Text =
		"Plain-text renderer fallback active"

	warn(
		"[MiniLuaIDE] renderer fallback: "
		.. tostring(reason)
	)
end

local function renderSyntax()

	if
		rendererFallback
		or not ScreenGui.Parent
		or minimized
	then
		return
	end

	local lineCount =
		math.max(
			1,
			#lines
		)

	local scrollY =
		EditorScroll.CanvasPosition.Y

	local viewportHeight =
		math.max(
			1,
			EditorScroll.AbsoluteWindowSize.Y
		)

	local firstVisible =
		math.floor(
			math.max(
				0,
				scrollY - 7
			)
			/ LINE_HEIGHT
		)
		+ 1

	local lastVisible =
		math.ceil(
			(
				scrollY
					+ viewportHeight
			)
			/ LINE_HEIGHT
		)
		+ 1

	firstVisible =
		math.max(
			1,
			firstVisible
				- EXTRA_LINES
		)

	lastVisible =
		math.min(
			lineCount,
			lastVisible
				+ EXTRA_LINES
		)

	local firstChunk =
		math.floor(
			(
				firstVisible - 1
			)
			/ CHUNK_SIZE
		)
		* CHUNK_SIZE
		+ 1

	local lastChunk =
		math.floor(
			(
				lastVisible - 1
			)
			/ CHUNK_SIZE
		)
		* CHUNK_SIZE
		+ 1

	local canvasSize =
		getCanvasSize()

	local visible =
		{}

	for chunkStart =
		firstChunk,
		lastChunk,
		CHUNK_SIZE
	do

		visible[
			chunkStart
		] = true

		local chunk =
			getChunk(
				chunkStart
			)

		local lineAmount =
			chunk.endLine
			- chunkStart
			+ 1

		local y =
			7
			+ (
				chunkStart - 1
			)
			* LINE_HEIGHT

		local syntaxLabel =
			syntaxLabels[
				chunkStart
			]

		if not syntaxLabel then

			syntaxLabel =
				createSyntaxLabel()

			syntaxLabels[
				chunkStart
			] =
				syntaxLabel

		end

		------------------------------------------------
		-- UPDATE EXISTING LABEL INSTEAD OF DESTROYING
		------------------------------------------------

		syntaxLabel.Text =
			chunk.highlighted
		syntaxLabel.Visible = not CodeEditor:IsFocused()

		syntaxLabel.Position =
			UDim2.fromOffset(
				10,
				y
			)

		syntaxLabel.Size =
			UDim2.fromOffset(

				canvasSize.X,

				lineAmount
					* LINE_HEIGHT
					+ 4
			)

		local numberLabel =
			numberLabels[
				chunkStart
			]

		if not numberLabel then

			numberLabel =
				createNumberLabel()

			numberLabels[
				chunkStart
			] =
				numberLabel

		end

		numberLabel.Text =
			chunk.lineNumbers

		numberLabel.Position =
			UDim2.fromOffset(

				0,

				y - scrollY
			)

		numberLabel.Size =
			UDim2.new(

				1,
				-8,
				0,

				lineAmount
					* LINE_HEIGHT
					+ 4
			)
	end

	for index,
		object in pairs(
			syntaxLabels
		)
	do

		if not visible[index] then

			object:Destroy()

			syntaxLabels[index] =
				nil

		end
	end

	for index,
		object in pairs(
			numberLabels
		)
	do

		if not visible[index] then

			object:Destroy()

			numberLabels[index] =
				nil

		end
	end
end

local function requestRender()

	if renderPending then
		return
	end

	renderPending = true

	task.delay(
		HIGHLIGHT_DELAY,
		function()

			renderPending = false

			if
				ScreenGui.Parent
				and not rendererFallback
			then

				local success,
					reason =
					pcall(
						renderSyntax
					)

				if not success then

					enableRendererFallback(
						reason
					)

				end
			end
		end
	)
end

local function updateCanvas()

	local size =
		getCanvasSize()

	CodeEditor.Position =
		UDim2.fromOffset(
			10,
			7
		)

	CodeEditor.Size =
		UDim2.fromOffset(
			size.X,
			size.Y
		)

	EditorScroll.CanvasSize =
		UDim2.fromOffset(
			size.X + 15,
			size.Y + 15
		)

	requestRender()
end

local function updateSource()

	local text =
		CodeEditor.Text or ""

	if text ~= cachedText then

		parseSource(
			text
		)

	end

	updateCanvas()
end

local function updateCaretVisibility()
	-- Native text, selection and caret share the same layout while editing.
	CustomCaret.Visible = false
	CodeEditor.TextTransparency = (CodeEditor:IsFocused() or rendererFallback
		or CodeEditor.Text == "") and 0 or 1
	for _, label in pairs(syntaxLabels) do
		label.Visible = not CodeEditor:IsFocused()
	end
end

local function getLineFromPosition(
	position
)

	local count =
		#lineStarts

	if count <= 1 then
		return 1
	end

	local low = 1
	local high = count
	local result = 1

	while low <= high do

		local middle =
			math.floor(
				(low + high)
				/ 2
			)

		if lineStarts[middle]
			<= position
		then

			result =
				middle

			low =
				middle + 1

		else

			high =
				middle - 1

		end
	end

	return result
end

local function ensureCursorVisible(
	x,
	y
)

	local canvas =
		EditorScroll.CanvasPosition

	local window =
		EditorScroll.AbsoluteWindowSize

	if
		window.X <= 0
		or window.Y <= 0
	then
		return
	end

	local newX =
		canvas.X

	local newY =
		canvas.Y

	local margin = 18

	if x <
		canvas.X + margin
	then

		newX =
			math.max(
				0,
				x - margin
			)

	elseif x >
		canvas.X
			+ window.X
			- margin
	then

		newX =
			math.max(
				0,
				x
					- window.X
					+ margin
			)

	end

	if y <
		canvas.Y + margin
	then

		newY =
			math.max(
				0,
				y - margin
			)

	elseif
		y + LINE_HEIGHT
		>
		canvas.Y
			+ window.Y
			- margin
	then

		newY =
			math.max(

				0,

				y
					- window.Y
					+ LINE_HEIGHT
					+ margin
			)

	end

	if
		newX ~= canvas.X
		or newY ~= canvas.Y
	then

		EditorScroll.CanvasPosition =
			Vector2.new(
				newX,
				newY
			)

	end
end

local function updateCursor()

	local cursor =
		CodeEditor.CursorPosition

	if cursor == -1 then
		updateCaretVisibility()
		return
	end

	if cachedText
		~= CodeEditor.Text
	then

		parseSource(
			CodeEditor.Text
			or ""
		)

		updateCanvas()
	end

	cursor =
		math.clamp(

			cursor,

			1,

			#CodeEditor.Text + 1
		)

	local line =
		getLineFromPosition(
			cursor
		)

	local lineStart =
		lineStarts[line]
		or 1

	local column =
		cursor
			- lineStart
			+ 1

	CursorText.Text =
		string.format(

			"Ln %d, Col %d",

			line,

			column
		)

	local before =
		CodeEditor.Text:sub(

			lineStart,

			math.max(
				lineStart - 1,
				cursor - 1
			)
		)

	before =
		before:gsub(
			"\n",
			""
		)

	before =
		before:gsub(
			"\r",
			""
		)

	before =
		before:gsub(
			"\t",
			TAB_TEXT
		)

	local width = 0

	if #before > 0 then

		local success,
			size =
			pcall(function()

				return TextService:GetTextSize(

					before,

					TEXT_SIZE,

					EDITOR_FONT,

					Vector2.new(
						1000000,
						LINE_HEIGHT
					)
				)

			end)

		if success then

			width =
				size.X

		else

			width =
				#before
				* TEXT_SIZE
				* 0.61

		end
	end

	local cursorX =
		10 + width

	local cursorY =
		7
		+ (
			line - 1
		)
			* LINE_HEIGHT

	CustomCaret.Position =
		UDim2.fromOffset(
			math.floor(cursorX),
			math.floor(cursorY + 2)
		)

	CustomCaret.Size =
		UDim2.fromOffset(
			2,
			LINE_HEIGHT - 4
		)

	updateCaretVisibility()

	ensureCursorVisible(
		cursorX,
		cursorY
	)
end

local function resizeButtons()

	local width =
		math.max(
			1,
			MainWindow.AbsoluteSize.X
				- 16
		)

	local spacing = 6
	local compact = width < 520
	local executeWidth
	local clipboardWidth
	local clearEditorWidth
	local clearOutputWidth

	if compact then

		local buttonWidth =
			math.max(
				1,
				math.floor(
					(
						width
							- spacing * 3
					)
					/ 4
				)
			)

		executeWidth = buttonWidth
		clipboardWidth = buttonWidth
		clearEditorWidth = buttonWidth
		clearOutputWidth = buttonWidth

		ExecuteButton.Text = "▶ Run"
		ClipboardButton.Text = "Run Clip"
		ClearEditorButton.Text = "Clear"
		ClearOutputButton.Text = "Output"

	else

		executeWidth = 112
		clipboardWidth = 152
		clearEditorWidth = 112
		clearOutputWidth = 112

		ExecuteButton.Text = "▶ Execute"
		ClipboardButton.Text = "Execute Clipboard"
		ClearEditorButton.Text = "Clear Editor"
		ClearOutputButton.Text = "Clear Output"

	end

	ExecuteButton.Size =
		UDim2.fromOffset(
			executeWidth,
			30
		)

	ClipboardButton.Size =
		UDim2.fromOffset(
			clipboardWidth,
			30
		)

	ClearEditorButton.Size =
		UDim2.fromOffset(
			clearEditorWidth,
			30
		)

	ClearOutputButton.Size =
		UDim2.fromOffset(
			clearOutputWidth,
			30
		)

	local textSize =
		compact
		and 11
		or 14

	ExecuteButton.TextSize =
		textSize

	ClipboardButton.TextSize =
		textSize

	ClearEditorButton.TextSize =
		textSize

	ClearOutputButton.TextSize =
		textSize
end

local function updateLayout()

	if minimized then
		return
	end

	local width =
		MainWindow.AbsoluteSize.X

	local height =
		MainWindow.AbsoluteSize.Y

	local padding = 8

	local top = 86

	local statusHeight = 24

	local spacing = 8

	local availableHeight =
		math.max(

			2,

			height
				- top
				- statusHeight
				- spacing * 2
		)

	local compact =
		availableHeight < 300

	local minEditor =
		compact
		and 90
		or 120

	local minOutput =
		compact
		and 72
		or 105

	local outputHeight
	local editorHeight

	if availableHeight
		< minEditor
			+ minOutput
	then

		outputHeight =
			math.max(

				1,

				math.floor(
					availableHeight
						* 0.3
				)
			)

		editorHeight =
			math.max(

				1,

				availableHeight
					- outputHeight
			)

	else

		local maxOutput =
			math.min(

				220,

				availableHeight
					- minEditor
			)

		outputHeight =
			math.clamp(

				math.floor(
					availableHeight
						* 0.3
				),

				minOutput,

				maxOutput
			)

		editorHeight =
			availableHeight
				- outputHeight

	end

	local panelWidth =
		math.max(

			120,

			width
				- padding * 2
		)

	EditorPanel.Position =
		UDim2.fromOffset(
			padding,
			top
		)

	EditorPanel.Size =
		UDim2.fromOffset(
			panelWidth,
			editorHeight
		)

	OutputPanel.Position =
		UDim2.fromOffset(

			padding,

			top
				+ editorHeight
				+ spacing
		)

	OutputPanel.Size =
		UDim2.fromOffset(
			panelWidth,
			outputHeight
		)

	StatusBar.Position =
		UDim2.new(
			0,
			0,
			1,
			-statusHeight
		)

	StatusBar.Size =
		UDim2.new(
			1,
			0,
			0,
			statusHeight
		)

	ResizeHandle.Position =
		UDim2.new(
			1,
			-28,
			1,
			-28
		)

	resizeButtons()

	updateCanvas()
end

local function scrollOutputBottom()

	task.defer(function()

		if not OutputScroll.Parent then
			return
		end

		OutputScroll.CanvasPosition =
			Vector2.new(

				0,

				math.max(

					0,

					OutputLayout.AbsoluteContentSize.Y
						- OutputScroll.AbsoluteWindowSize.Y
				)
			)

	end)
end

local function addOutput(
	message,
	messageType
)
	if not ScreenGui.Parent then
		return
	end

	outputCounter += 1

	local label =
		Instance.new(
			"TextLabel"
		)

	label.Name =
		"Output_"
		.. outputCounter

	label.Size =
		UDim2.new(
			1,
			-2,
			0,
			20
		)

	label.AutomaticSize =
		Enum.AutomaticSize.Y

	label.BackgroundTransparency = 1

	label.TextWrapped = true

	label.RichText = false

	label.TextSize = 14

	label.Font =
		EDITOR_FONT

	label.TextXAlignment =
		Enum.TextXAlignment.Left

	label.TextYAlignment =
		Enum.TextYAlignment.Top

	label.LayoutOrder =
		outputCounter

	if messageType == "warning" then

		label.Text =
			"[WARNING] "
			.. tostring(
				message
			)

		label.TextColor3 =
			COLORS.Warning

	elseif messageType == "error" then

		label.Text =
			"[ERROR] "
			.. tostring(
				message
			)

		label.TextColor3 =
			COLORS.Error

	elseif messageType == "success" then

		label.Text =
			"[IDE] "
			.. tostring(
				message
			)

		label.TextColor3 =
			COLORS.Success

	elseif messageType == "system" then

		label.Text =
			"[IDE] "
			.. tostring(
				message
			)

		label.TextColor3 =
			COLORS.Accent

	else

		label.Text =
			tostring(
				message
			)

		label.TextColor3 =
			COLORS.Text

	end

	label.Parent =
		OutputScroll

	table.insert(
		outputLabels,
		label
	)

	if #outputLabels > MAX_OUTPUT_ENTRIES then

		local oldest =
			table.remove(
				outputLabels,
				1
			)

		if oldest then
			oldest:Destroy()
		end
	end

	scrollOutputBottom()
end

local function clearOutput()

	for _, label in ipairs(
		outputLabels
	) do

		if label.Parent then
			label:Destroy()
		end
	end

	table.clear(outputLabels)
	outputCounter = 0
	OutputScroll.CanvasPosition = Vector2.new(0, 0)
end

local function executeCode(source)
	if type(loadstring) ~= "function" then
		addOutput("Script execution is unavailable in this environment", "error")
		return
	end

	if type(source) ~= "string"
		or source:match("^%s*$")
	then

		addOutput(
			"Nothing to execute",
			"warning"
		)

		return
	end

	local compileOK, compiled, compileError = pcall(loadstring, source)
	if not compileOK then
		addOutput("Failed to compile: " .. tostring(compiled), "error")
		return
	end

	if not compiled then

		addOutput(

			"Failed to compile: "
				.. tostring(
					compileError
				),

			"error"
		)

		return
	end

	local success,
		runtimeError =
		pcall(
			compiled
		)

	if not success then

		addOutput(

			"Failed to execute: "
				.. tostring(
					runtimeError
				),

			"error"
		)

	else

		addOutput(

			"code executed successfully",

			"success"
		)

	end
end

local function executeClipboard()

	if type(getclipboard) ~= "function" then

		addOutput(
			"Clipboard access is not supported by this executor",
			"error"
		)

		return
	end

	local success,
		source =
		pcall(getclipboard)

	if not success then

		addOutput(
			"Failed to read clipboard: "
				.. tostring(source),
			"error"
		)

		return
	end

	if type(source) ~= "string"
		or source:match("^%s*$")
	then

		addOutput(
			"Clipboard does not contain executable text",
			"warning"
		)

		return
	end

	executeCode(source)
end


local function insertTextAtSelection(
	text,
	cursorOverride,
	selectionOverride
)

	local source =
		CodeEditor.Text
		or ""

	local cursor =
		cursorOverride
		or CodeEditor.CursorPosition

	if cursor < 1 then
		cursor = #source + 1
	end

	cursor =
		math.clamp(
			cursor,
			1,
			#source + 1
		)

	local selection =
		selectionOverride

	if selection == nil then
		selection =
			CodeEditor.SelectionStart
	end

	local rangeStart = cursor
	local rangeEnd = cursor

	if selection
		and selection >= 1
	then
		selection =
			math.clamp(
				selection,
				1,
				#source + 1
			)

		rangeStart =
			math.min(cursor, selection)

		rangeEnd =
			math.max(cursor, selection)
	end

	CodeEditor.Text =
		source:sub(1, rangeStart - 1)
		.. text
		.. source:sub(rangeEnd)

	CodeEditor.CursorPosition =
		rangeStart + #text

	CodeEditor.SelectionStart = -1
end

local function saveCenter()

	local position =
		getWindowPosition()

	local size =
		getWindowSize()

	centerPosition =
		position
		+ size / 2
end

local function setMinimized(
	value
)

	if value == minimized then
		return
	end

	if value then
		CodeEditor:ReleaseFocus(false)

		previousSize =
			getWindowSize()

		saveCenter()

		minimized = true

		dragging = false
		resizing = false
		dragPointer = nil
		resizePointer = nil

		Toolbar.Visible = false
		EditorPanel.Visible = false
		OutputPanel.Visible = false
		StatusBar.Visible = false
		ResizeHandle.Visible = false
		TitleLabel.Visible = false

		local size =
			Vector2.new(
				MINIMIZED_SIZE,
				MINIMIZED_SIZE
			)

		local center =
			centerPosition
			or (
				getWindowPosition()
				+ getWindowSize()
					/ 2
			)

		local position =
			clampWindowPosition(
				center
					- size / 2,
				size
			)

		MainWindow.Size =
			UDim2.fromOffset(
				size.X,
				size.Y
			)

		MainWindow.Position =
			UDim2.fromOffset(
				position.X,
				position.Y
			)

		MainCorner.CornerRadius =
			UDim.new(
				1,
				0
			)

		TopCorner.CornerRadius =
			UDim.new(
				1,
				0
			)

		TopBar.Size =
			UDim2.fromScale(
				1,
				1
			)

		DragHandle.Size =
			UDim2.fromScale(
				1,
				1
			)

		MinimizeButton.Position =
			UDim2.fromOffset(
				0,
				0
			)

		MinimizeButton.Size =
			UDim2.fromScale(
				1,
				1
			)

		MinimizeButton.Text =
			"↗"

		MinimizeButton.TextSize =
			24

		CustomCaret.Visible = false

	else

		minimized = false

		local size =
			previousSize

		if autoSize then

			size =
				getIdealSize()

		else

			local available =
				getAvailableSize()

			size =
				Vector2.new(

					math.min(
						size.X,
						available.X
					),

					math.min(
						size.Y,
						available.Y
					)
				)
		end

		MainWindow.Size =
			UDim2.fromOffset(
				size.X,
				size.Y
			)

		MainCorner.CornerRadius =
			UDim.new(
				0,
				9
			)

		TopCorner.CornerRadius =
			UDim.new(
				0,
				9
			)

		TopBar.Size =
			UDim2.new(
				1,
				0,
				0,
				38
			)

		DragHandle.Size =
			UDim2.new(
				1,
				-48,
				1,
				0
			)

		MinimizeButton.Position =
			UDim2.new(
				1,
				-48,
				0,
				0
			)

		MinimizeButton.Size =
			UDim2.fromOffset(
				48,
				38
			)

		MinimizeButton.Text =
			"-"

		MinimizeButton.TextSize =
			21

		TitleLabel.Visible = true
		Toolbar.Visible = true
		EditorPanel.Visible = true
		OutputPanel.Visible = true
		StatusBar.Visible = true
		ResizeHandle.Visible = true

		local center =
			centerPosition

		if not center then

			center =
				getViewportSize()
				/ 2

		end

		local position =
			clampWindowPosition(
				center
					- size / 2,
				size
			)

		MainWindow.Position =
			UDim2.fromOffset(
				position.X,
				position.Y
			)

		updateLayout()
		requestRender()
	end
end

local function isPointerStart(input)

	return input.UserInputType
		== Enum.UserInputType.MouseButton1
		or input.UserInputType
			== Enum.UserInputType.Touch
end

local function getInputPosition(input)

	return Vector2.new(
		input.Position.X,
		input.Position.Y
	)
end

local function pointerMatches(input, pointer)

	if not pointer then
		return false
	end

	if pointer.UserInputType
		== Enum.UserInputType.Touch
	then
		return input == pointer
	end

	return input.UserInputType
		== Enum.UserInputType.MouseMovement
end

DragHandle.InputBegan:Connect(function(input)

	if minimized
		or not isPointerStart(input)
	then
		return
	end

	dragging = true
	resizing = false
	dragPointer = input
	resizePointer = nil
	dragStart = getInputPosition(input)
	dragWindowStart = getWindowPosition()
end)

ResizeHandle.InputBegan:Connect(function(input)

	if minimized
		or not isPointerStart(input)
	then
		return
	end

	resizing = true
	dragging = false
	resizePointer = input
	dragPointer = nil
	resizeStart = getInputPosition(input)
	resizeWindowStart = getWindowPosition()
	resizeWindowSize = getWindowSize()
	autoSize = false
end)

trackGlobal(UIS.InputChanged:Connect(function(input)

	if dragging
		and pointerMatches(input, dragPointer)
	then

		local delta =
			getInputPosition(input)
			- dragStart

		local position =
			clampWindowPosition(
				dragWindowStart + delta
			)

		MainWindow.Position =
			UDim2.fromOffset(
				math.floor(position.X),
				math.floor(position.Y)
			)

	elseif resizing
		and pointerMatches(input, resizePointer)
	then

		local delta =
			getInputPosition(input)
			- resizeStart

		local viewport =
			getViewportSize()

		local edgePadding =
			getEdgePadding()

		local maximumWidth =
			math.max(
				1,
				math.min(
					MAX_SIZE.X,
					viewport.X
						- resizeWindowStart.X
						- edgePadding
				)
			)

		local maximumHeight =
			math.max(
				1,
				math.min(
					MAX_SIZE.Y,
					viewport.Y
						- resizeWindowStart.Y
						- edgePadding
				)
			)

		local size =
			Vector2.new(
				math.clamp(
					resizeWindowSize.X + delta.X,
					math.min(MIN_SIZE.X, maximumWidth),
					maximumWidth
				),
				math.clamp(
					resizeWindowSize.Y + delta.Y,
					math.min(MIN_SIZE.Y, maximumHeight),
					maximumHeight
				)
			)

		previousSize = size

		MainWindow.Size =
			UDim2.fromOffset(
				math.floor(size.X),
				math.floor(size.Y)
			)
	end
end))

trackGlobal(UIS.InputEnded:Connect(function(input)

	if dragPointer
		and (
			input == dragPointer
			or input.UserInputType
				== Enum.UserInputType.MouseButton1
		)
	then
		dragging = false
		dragPointer = nil
	end

	if resizePointer
		and (
			input == resizePointer
			or input.UserInputType
				== Enum.UserInputType.MouseButton1
		)
	then
		resizing = false
		resizePointer = nil
	end
end))

ExecuteButton.Activated:Connect(
	function()

		executeCode(
			CodeEditor.Text
		)

	end
)

ClipboardButton.Activated:Connect(
	function()

		executeClipboard()

	end
)

ClearEditorButton.Activated:Connect(
	function()

		CodeEditor.Text = ""
		EditorScroll.CanvasPosition = Vector2.new(0, 0)

		CodeEditor:CaptureFocus()

	end
)

ClearOutputButton.Activated:Connect(
	function()

		clearOutput()

	end
)

MinimizeButton.Activated:Connect(
	function()

		setMinimized(
			not minimized
		)

	end
)

-- One edit transaction per frame; Roblox gets the first chance to handle Return.
local lastCursor, lastSelection = 1, -1
local pendingEdit = nil
local nativeNewlineSeen = false
local focusEpoch = 0

local function rememberSelection()
	if CodeEditor.CursorPosition >= 1 then
		lastCursor = CodeEditor.CursorPosition
		lastSelection = CodeEditor.SelectionStart
	end
end

local function queueEdit(text, restoreFocus)
	if minimized or pendingEdit then
		return
	end
	rememberSelection()
	pendingEdit = {
		text = text,
		source = CodeEditor.Text,
		cursor = lastCursor,
		selection = lastSelection,
		epoch = focusEpoch,
		restoreFocus = restoreFocus,
	}
end

CodeEditor:GetPropertyChangedSignal("Text"):Connect(function()
	local old, current = cachedText or "", CodeEditor.Text
	local first, oldEnd, newEnd = 1, #old, #current
	while first <= math.min(oldEnd, newEnd) and old:byte(first) == current:byte(first) do
		first += 1
	end
	while oldEnd >= first and newEnd >= first and old:byte(oldEnd) == current:byte(newEnd) do
		oldEnd -= 1
		newEnd -= 1
	end
	local newline = current:find("\n", first, true)
	nativeNewlineSeen = nativeNewlineSeen or (newline ~= nil and newline <= newEnd)
	updateSource()
	rememberSelection()
	updateCursor()
end)

CodeEditor:GetPropertyChangedSignal("CursorPosition"):Connect(function()
	rememberSelection()
	updateCursor()
end)

CodeEditor:GetPropertyChangedSignal("SelectionStart"):Connect(function()
	rememberSelection()
	updateCaretVisibility()
end)

CodeEditor.Focused:Connect(function()
	focusEpoch += 1
	rememberSelection()
	updateCaretVisibility()
	updateCursor()
end)

CodeEditor.FocusLost:Connect(function(enterPressed)
	if enterPressed then
		queueEdit("\n", true)
	else
		focusEpoch += 1
		pendingEdit = nil
	end
	updateCaretVisibility()
	requestRender()
end)

CodeEditor.ReturnPressedFromOnScreenKeyboard:Connect(function()
	queueEdit("\n", true)
end)

trackGlobal(RunService.Heartbeat:Connect(function()
	local edit = pendingEdit
	pendingEdit = nil
	local nativeReturn = nativeNewlineSeen
	nativeNewlineSeen = false
	if not edit or minimized or not ScreenGui.Parent or edit.epoch ~= focusEpoch then
		return
	end
	local focused = UIS:GetFocusedTextBox()
	if focused and focused ~= CodeEditor then
		return
	end
	if not focused and not edit.restoreFocus then
		return
	end
	local unchanged = CodeEditor.Text == edit.source
	local cursor, selection = lastCursor, lastSelection
	if not focused then
		CodeEditor:CaptureFocus()
	end
	if unchanged and not (edit.text == "\n" and nativeReturn) then
		insertTextAtSelection(edit.text, edit.cursor, edit.selection)
	elseif not focused then
		CodeEditor.CursorPosition = math.clamp(cursor, 1, #CodeEditor.Text + 1)
		CodeEditor.SelectionStart = selection >= 1
			and math.clamp(selection, 1, #CodeEditor.Text + 1) or -1
	end
	rememberSelection()
	updateCursor()
end))

EditorScroll:GetPropertyChangedSignal(
	"CanvasPosition"
):Connect(function()

	requestRender()

end)

MainWindow:GetPropertyChangedSignal(
	"AbsoluteSize"
):Connect(function()

	updateLayout()

end)

trackGlobal(UIS.InputBegan:Connect(function(input, _processed)
	-- Processed input includes typing; scope shortcuts to this exact TextBox.
	if minimized or UIS:GetFocusedTextBox() ~= CodeEditor then
		return
	end
	local modifier = UIS:IsKeyDown(Enum.KeyCode.LeftControl)
		or UIS:IsKeyDown(Enum.KeyCode.RightControl)
		or UIS:IsKeyDown(Enum.KeyCode.LeftMeta)
		or UIS:IsKeyDown(Enum.KeyCode.RightMeta)
	local isReturn = input.KeyCode == Enum.KeyCode.Return
		or input.KeyCode == Enum.KeyCode.KeypadEnter
	if modifier and isReturn then
		-- Let native text editing own Return; execution uses a separate shortcut.
		task.spawn(executeCode, CodeEditor.Text)
	elseif modifier and input.KeyCode == Enum.KeyCode.L then
		clearOutput()
	elseif isReturn then
		queueEdit("\n", true)
	elseif not modifier and input.KeyCode == Enum.KeyCode.Tab then
		queueEdit(TAB_TEXT, false)
	end
end))

trackGlobal(UIS.WindowFocusReleased:Connect(function()
	dragging, resizing = false, false
	dragPointer, resizePointer = nil, nil
	pendingEdit = nil
	focusEpoch += 1
	CodeEditor:ReleaseFocus(false)
end))

local cameraConnection

local function connectCamera()

	if cameraConnection then

		cameraConnection:Disconnect()

		cameraConnection = nil

	end

	local camera =
		workspace.CurrentCamera

	if camera then

		cameraConnection =
			camera
				:GetPropertyChangedSignal(
					"ViewportSize"
				)
				:Connect(function()

					local size =
						getWindowSize()

					local available =
						getAvailableSize()

					if not minimized then

						size =
							Vector2.new(

								math.min(
									size.X,
									available.X
								),

								math.min(
									size.Y,
									available.Y
								)
							)

						MainWindow.Size =
							UDim2.fromOffset(
								size.X,
								size.Y
							)

					end

					keepWindowOnScreen()

				end)

	end
end

trackGlobal(workspace:GetPropertyChangedSignal(
	"CurrentCamera"
):Connect(function()

	connectCamera()

end))

ScreenGui.AncestryChanged:Connect(function(_, parent)

	if parent then
		return
	end

	for _, connection in ipairs(
		globalConnections
	) do
		connection:Disconnect()
	end

	table.clear(globalConnections)

	if cameraConnection then
		cameraConnection:Disconnect()
		cameraConnection = nil
	end
end)

connectCamera()

local initialSize =
	getIdealSize()

MainWindow.Size =
	UDim2.fromOffset(
		initialSize.X,
		initialSize.Y
	)

centerWindow(
	initialSize
)

parseSource(
	CodeEditor.Text
	or ""
)

updateLayout()
updateSource()
updateCursor()
requestRender()


addOutput(
	"initialized",
	"success"
)
