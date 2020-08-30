local w = 1024
local h = 600

celerytest.create_glcontext("Celerytest", w, h, false)

local function should_quit(e)
    local escape = e.type == "key_up" and e.kname == "Escape"
    return e.type == "quit" or escape
end

local glview = celerytest.create("GLView2D", w, h)
celerytest.set_root_view(glview)

while true do
    while true do
        local event = celerytest.poll()
        if event ~= nil then
            if should_quit(event) then
                celerytest.remove_glcontext()
                return
            end
        else break end
    end
    celerytest.render()
    celerytest.sleep(10)
end