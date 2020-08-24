json = require("json")
docker = require("luaclibdocker")
CLD = require("cld")

d = docker.connect()

local cld = CLD:new(d)

ls_options = {
    ["all"] = {["val"] = false},
    ["filter"] = {["val"] = nil},
    ["format"] = {["val"] = nil},
    ["last"] = {["val"] = 0},
    ["latest"] = {["val"] = false},
    ["no-trunc"] = {["val"] = false},
    ["quiet"] = {["val"] = false},
    ["size"] = {["val"] = false}
}
-- output = cld_cmd.ctr.ls(d, ls_options)
output = cld:run("container", "ls", ls_options)
for k, v in ipairs(output) do
    print("Container #" .. k)
    for kt, vt in pairs(v) do
        print("\t" .. kt .. " = ")
        print(vt)
    end
    print()
end
