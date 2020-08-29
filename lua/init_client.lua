celerytest.create_glcontext("Celerytest", 1024, 600, false)

local function should_quit(event)
    local escape = event.type == "key_up" and event.kname == "Escape"
    return event.type == "quit" or escape
end

local glview = celerytest.create("GLView2D", 1024, 600)
celerytest.set_root_view(glview)

while true do
    local event = celerytest.poll()
    if event ~= nil then
        if should_quit(event) then celerytest.remove_glcontext(); return; end
    end
    celerytest.sleep(10)
end