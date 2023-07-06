#!/usr/bin/env lua

local n = require('newt')

n.Init()
n.Cls()

local dir_item = {}
local file_item = {}
local cur_dir = {"home"}
local cur_path = ""
local path_pos = 0
local emul = ""

local function dirLookup()
    dir_item = {".."}
    file_item = {}
    local cp = "/" .. table.concat(cur_dir, "/")
    if #cp < #cur_path then
        n.DrawRootText(path_pos, 0, string.format(string.format("%%-%ds", #cur_path + 2), ""))
    end
    cur_path = cp
    n.DrawRootText(path_pos, 0, "[" .. cur_path .. "]")
    local p = io.popen('ls -Ap "' .. cur_path .. '"') -- Open directory look for files, save data in p. By giving '-type f' as parameter, it returns all files.
    for fn in p:lines() do
        if fn ~= './' then
            local lc = string.sub(fn, string.len(fn)) or false
            if lc then
                if lc == '/' then -- dir
                    table.insert(dir_item, fn:sub(1, -2))
                else
                    table.insert(file_item, fn)
                end
            end
        end
    end
end

local function getSelectFile(list_files, get_ext)
    local sel = list_files:GetCurrent()
    if sel then
        local file_name = file_item[sel]
        if file_name then
            local file_ext = file_name:match("^.+(%..+)$")
            if file_ext then
                if get_ext then
                    return file_ext
                else
                    return file_name
                end
            end
        end
    end
end

local function main()
    local cols, rows = n.GetScreenSize()
    local width = 80

    if width > cols then
        width = cols
    end
    path_pos = (cols - width) // 2

    -- n.PushHelpLine(nil)
    -- n.OpenWindow(1, 1, cols - 2, rows - 3)
    n.CenteredWindow(width, rows - 2)
    -- local text_box = n.Textbox(cols - 40, 0, 20, rows - 3, 0)
    -- text_box:SetText("")
    dirLookup()

    local list_width = width // 2
    local lflags = n.FLAG_BORDER + n.FLAG_SCROLL + n.FLAG_RETURNEXIT

    local list_dir = n.Listbox(0, 0, rows - 2, lflags)
    list_dir:SetWidth(list_width)
    list_dir:AppendEntry(dir_item)

    local list_files = n.Listbox(list_width, 1, rows - 3, lflags)
    list_files:AppendEntry(file_item)
    list_files:SetWidth(list_width)

    local label = n.Label(list_width + 1, 0, "Hello\nworld")

    local form = n.Form()
    form:AddComponents(label, list_dir, list_files)
    -- form:AddHotKey({n.KEY_UP, n.KEY_DOWN})

    -- n.ClearKeyBuffer()
    -- n.WaitForKey()

    -- os.execute("sleep 5")

    form:SetTimer(500);
    while true do
        local r, v = form:Run()
        local sel
        if r == n.EXIT_COMPONENT then -- and v and v:ID() == list:ID() then
            if v:ID() == list_dir:ID() then
                sel = list_dir:GetCurrent()
                if sel == 1 then -- ../
                    table.remove(cur_dir)
                else
                    table.insert(cur_dir, dir_item[sel])
                end
                dirLookup()
                list_dir:Clear()
                list_dir:AppendEntry(dir_item)
                list_files:Clear()
                list_files:AppendEntry(file_item)
                n.Refresh()
            elseif v:ID() == list_files:ID() then
                local f_name = getSelectFile(list_files, false)
                if f_name then
                    -- local file = io.open("/tmp/GAME_FILE", "w")
                    -- file:write(f_name)
                    -- io.close(file)
                    -- io.stderr:write(cur_path .. "/" .. f_name)
                    io.popen('echo ' .. emul .. " " .. cur_path .. "/" .. f_name .. ' >&3')
                    break
                end
            end
        elseif r == n.EXIT_HOTKEY then
            break
        elseif r == n.EXIT_TIMER then
            local f_ext = getSelectFile(list_files, true)
            emul = "mednafen"
            if f_ext then
                local sys = "unknown"
                if f_ext == ".bin" then
                    sys = "sega"
                    emul = ""
                elseif f_ext == ".smc" then
                    sys = "SuperNintendo"
                    emul = "snes9x-sdl"
                elseif f_ext == ".nes" then
                    sys = "Nintendo"
                    emul = ""
                elseif f_ext == ".gba" then
                    sys = "GameBoyAdvance"
                    emul = "VisualBoyAdvance" -- mgba
                elseif f_ext == ".gb" then
                    sys = "GameBoy"
                    emul = "VisualBoyAdvance"
                end
                label:SetText(sys)
            end
        end
    end

    n.Finished()
    return sel
end

local ok, sel = xpcall(main, debug.traceback)
if not ok then
    n.Finished()
    print(sel)
end

