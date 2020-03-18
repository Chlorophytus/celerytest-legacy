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
            env2duiobject = 4,
            env3dshaderprogram = 6,
            env3dshaderopaque = 7,
        },
        shaders = {
            compute = 1,
            fragment = 2,
            geometry = 3,
            vertex = 4,
        },
    }
}
-- shaders
local vert = celerytest.sim.create(celerytest.sim.types.env3dshaderopaque, nil, {
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
local frag = celerytest.sim.create(celerytest.sim.types.env3dshaderopaque, nil, {
    type = celerytest.sim.shaders.fragment,
    source = [[#version 450 core
in vec4 frag_paint;
out vec4 color;
void main(){
    color = frag_paint;
}]]})
celerytest.con.info("VERT ObjID: " .. vert)
celerytest.con.info("FRAG ObjID: " .. frag)

local prog = celerytest.sim.create(celerytest.sim.types.env3dshaderprogram, nil, {
    is_main = false, shaders = {
        vert,
        frag,
    }
})
celerytest.con.info("PROG ObjID: " .. prog)

celerytest.sim.create(celerytest.sim.types.env2duiobject, nil, {
    x = 0,
    y = 0,
    zlevel = 0,
    image = "lib/CGEBackdrop.png",
})

celerytest.sim.create(celerytest.sim.types.env2duiobject, nil, {
    x = 10,
    y = 10,
    zlevel = 1,
    image = "lib/CGE.png",
})

celerytest.sim.create(celerytest.sim.types.env2duiobject, nil, {
    x = 10,
    y = 130,
    zlevel = 1,
    image = "lib/CGEPlay.png",
})