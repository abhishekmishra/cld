json = require("json")
docker = require("luaclibdocker")

function cmd_ctr_ls(d, options, args)
    ctr_ls_str = d:container_ls()
    ctr_ls = json.decode(ctr_ls_str)
    print(ctr_ls)
    for k, v in ipairs(ctr_ls) do
        print("Container #" .. k .. " is " .. v.Names[1])
    end
end
