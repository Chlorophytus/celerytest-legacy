celerytest.create_glcontext("Celerytest", 1366, 768, false)

local function should_quit(event)
    local escape = event.type == "key_up" and event.kname == "Escape"
    return event.type == "quit" or escape
end

while true do
    local event = celerytest.poll()
    if event ~= nil then
        if should_quit(event) then celerytest.remove_glcontext(); return; end
    end
    celerytest.sleep(1)
end