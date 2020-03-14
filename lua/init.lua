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
            shaderlist = 3
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
layout(location = 10)in vec4 vert;
out vec4 frag_paint;
void main() {
    frag_paint = vec4(vec3(1.0f), vert.w / 8.0f);// Pass d's z val for Z feedback, thus allowing us to discard.
    gl_Position = vert;
}
]]})
local frag = celerytest.sim.create(celerytest.sim.types.shaderobject, nil, {
    type = celerytest.sim.shaders.fragment,
    source = [[#version 450 core
in vec4 frag_paint;
layout(location = 20) uniform vec3 back;
out vec4 color;
void main(){
    // Mix
    if (frag_paint.a < 0.001f) { discard; }
    color = vec4(mix(frag_paint.rgb, back, clamp(frag_paint.a, 0.0f, 1.0f)), 1.0f);
}]]})
celerytest.con.info("VERT ObjID: " .. vert)
celerytest.con.info("FRAG ObjID: " .. frag)

local prog = celerytest.sim.create(celerytest.sim.types.shaderlist, nil, {
    vert,
    frag
})
