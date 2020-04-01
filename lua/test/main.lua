json = require("json")
docker = require("luaclibdocker")
cmd_container = require("cmd_container")

d = docker.connect()

ls_options = {
    ["--all"] = {
        ["val"] = false,
    },
    ["--filter"] = {
        ["val"] = nil,
    },
    ["--format"] = {
        ["val"] = nil,
    },
    ["--last"] = {
        ["val"] = nil,
    },
    ["--latest"] = {
        ["val"] = false,
    },
    ["--no-trunc"] = {
        ["val"] = false,
    },
    ["--quiet"] = {
        ["val"] = false,
    },
    ["--size"] = {
        ["val"] = 0,
    },
}
output = cmd_ctr_ls(d, ls_options)
for k, v in ipairs(output) do
    print("Container #" .. k)
    for kt, vt in pairs(v) do
        print("\t" .. kt .. " = ")
        print(vt)
    end
    print()
end