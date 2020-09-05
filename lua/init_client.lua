local w = 1024
local h = 600

celerytest.create_glcontext("Celerytest", w, h, false)

local function should_quit(e)
    local escape = e.type == "key_up" and e.kname == "Escape"
    return e.type == "quit" or escape
end

local glview = celerytest.create("GLView2D", { w = w, h = h, })
celerytest.set_root_view(glview)

local text = celerytest.create("GuiTextCtrl", {
    color = { a = 0xFF, b = 0xff, g = 0x00, r = 0xff, },
    font = "Neucha-Regular.ttf",
    text = "This is cool!",
    size = 20,
    rect = { x = 10, y = 10, },
})

celerytest.gui_insert(0, text)

while true do
    while true do
        local event = celerytest.poll()
        if event ~= nil then
            if should_quit(event) then
                celerytest.remove_glcontext()
                celerytest.gui_remove(0)
                celerytest.remove(text)
                -- Unlike what happened before I am now requiring you to remove GLViews explicitly
                celerytest.remove(glview)
                return
            end
        else break
        end
    end
    celerytest.render()
    celerytest.sleep(10)
end