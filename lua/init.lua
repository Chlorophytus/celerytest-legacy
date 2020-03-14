celerytest = {
    con = {
        info = __con_info,
        warn = __con_warn,
        error = __con_error
    },
    sim = {
        create = __sim_create,
        delete = __sim_delete,
        types = {
            simobject = 1,
            shaderobject = 2,
            shaderlist = 3,
            env2duiobject = 4
        },
        shaders = {
            compute = 1,
            vertex = 2,
            fragment = 3,
            geometry = 4,
        }
    }
}
-- shaders
local vert = celerytest.sim.create(celerytest.sim.types.shaderobject, nil, {
    type = celerytest.sim.shaders.vertex,
    source = [[#version 450 core
layout(location = 10) in vec2 vert;
out vec4 frag_paint;
void main() {
    // Pass d's z val for Z feedback, thus allowing us to discard.
    frag_paint = vec4(1.0f, 0.0f, 1.0f, 0.0f);
    gl_Position =  vec4(vert.xy, 0.0f, 1.0f);
}
]]})
local frag = celerytest.sim.create(celerytest.sim.types.shaderobject, nil, {
    type = celerytest.sim.shaders.fragment,
    source = [[#version 450 core
in vec4 frag_paint;
out vec4 color;
void main(){
    color = frag_paint;
}]]})
celerytest.con.info("VERT ObjID: " .. vert)
celerytest.con.info("FRAG ObjID: " .. frag)

local prog = celerytest.sim.create(celerytest.sim.types.shaderlist, nil, {
    vert,
    frag,
})
celerytest.con.info("PROG ObjID: " .. prog)

local ui2d = celerytest.sim.create(celerytest.sim.types.env2duiobject, nil, {
    x = 0,
    y = 0,
    w = 480,
    h = 360,
    zlevel = 0,
})

celerytest.con.info("UI2D ObjID: " .. ui2d)