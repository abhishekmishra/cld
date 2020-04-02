json = require("json")
docker = require("luaclibdocker")
cld_cmd = require("cld_cmd")

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
output = cld_cmd.ctr.cmd_ctr_ls(d, ls_options)
for k, v in ipairs(output) do
    print("Container #" .. k)
    for kt, vt in pairs(v) do
        print("\t" .. kt .. " = ")
        print(vt)
    end
    print()
end