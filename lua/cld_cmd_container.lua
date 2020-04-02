json = require("json")
docker = require("luaclibdocker")
cld_cmd_util = require("cld_cmd_util")

local cld_cmd_container = {}

function cld_cmd_container.dummy(d)
    print("Running dummy fn")
end

function cld_cmd_container.ls(d, options, args)
    all = false
    limit = 0
    size = true

    if options ~= nil then
        if options["all"] ~= nil then
            all = options["all"].val
        end
    end

    ctr_ls_str = d:container_ls(all, limit, size)
    ctr_ls = json.decode(ctr_ls_str)
    -- print(ctr_ls_str)

    output = {}
    for k, v in ipairs(ctr_ls) do
        c = {}
        c["Command"] = v.Command
        c["CreatedAt"] = v.Created
        c["ID"] = v.Id
        c["Image"] = v.Image
        c["Labels"] = v.Labels
        c["LocalVolumes"] = v.Mounts
        c["Mounts"] = v.Mounts
        c["Names"] = v.Names
        c["Networks"] = v.NetworkSettings.Networks
        c["Ports"] = v.Ports
        c["RunningFor"] = nil
        c["Size"] = v.SizeRootFs
        c["Status"] = v.Status

        table.insert(output, c)
        -- print("Container #" .. k .. " is " .. v.Names[1])
    end

    hdrs, fmt_output, colwdths = cld_cmd_container.ls_format(output)

    cld_cmd_util.display_table(hdrs, fmt_output, colwdths)

    return output
end

function cld_cmd_container.ls_format(output)
    hdrs = {
        "CONTAINER ID",
        "IMAGE",
        "COMMAND",
        "CREATED",
        "STATUS"
    }
    fmtout = {}
    colwdths = {}
    fmtout["CONTAINER ID"] = {}
    fmtout["IMAGE"] = {}
    fmtout["COMMAND"] = {}
    fmtout["CREATED"] = {}
    fmtout["STATUS"] = {}
    colwdths["CONTAINER ID"] = 15
    colwdths["IMAGE"] = 15
    colwdths["COMMAND"] = 25
    colwdths["CREATED"] = 25
    colwdths["STATUS"] = 50
    -- fmtout["CONTAINER ID"] = {}
    -- fmtout["CONTAINER ID"] = {}
    for k, c in ipairs(output) do
        table.insert(fmtout["CONTAINER ID"], c["ID"])
        table.insert(fmtout["IMAGE"], c["Image"])
        table.insert(fmtout["COMMAND"], c["Command"])
        table.insert(fmtout["CREATED"], os.date("%d-%m-%Y:%H:%M:%S", c["CreatedAt"]))
        table.insert(fmtout["STATUS"], c["Status"])
        -- fmtout["PORTS"], c["Ports"])
        -- fmtout["NAMES"], c["ID"])
    end
    return hdrs, fmtout, colwdths
end

return cld_cmd_container