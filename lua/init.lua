celerytest = {
    con = {
        info = __con_info,
        warn = __con_warn,
        error = __con_error
    },
    sim = {
        create = __sim_create,
        create_hint = __sim_create_hint,
        delete = __sim_delete
    }
}

celerytest.con.info("creating a lot of objects")
for i=1,math.pow(2,20) do
    celerytest.sim.create_hint(0, i)
end
celerytest.con.info("created a lot of objects")