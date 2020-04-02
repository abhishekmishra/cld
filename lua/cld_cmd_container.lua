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
        "STATUS",
        "PORTS",
        "NAMES"
    }
    fmtout = {}
    colwdths = {}
    fmtout["CONTAINER ID"] = {}
    fmtout["IMAGE"] = {}
    fmtout["COMMAND"] = {}
    fmtout["CREATED"] = {}
    fmtout["STATUS"] = {}
    fmtout["PORTS"] = {}
    fmtout["NAMES"] = {}
    colwdths["CONTAINER ID"] = 15
    colwdths["IMAGE"] = 15
    colwdths["COMMAND"] = 25
    colwdths["CREATED"] = 25
    colwdths["STATUS"] = 20
    colwdths["PORTS"] = 25
    colwdths["NAMES"] = 50
    for k, c in ipairs(output) do
        local ports_str = ""
        local count = 1
        for _, p in ipairs(c["Ports"]) do
            ports_str = ports_str .. p.IP .. ":" .. p.PublicPort .. "->" .. p.PrivatePort .. "/" .. p.Type
            if count < #c["Ports"] then
                ports_str = ports_str .. ", "
            end
        end

        local names_str = ""
        count = 1
        for _, p in ipairs(c["Names"]) do
            names_str = names_str .. p
            if count < #c["Names"] then
                names_str = names_str .. ", "
            end
        end

        table.insert(fmtout["CONTAINER ID"], c["ID"])
        table.insert(fmtout["IMAGE"], c["Image"])
        table.insert(fmtout["COMMAND"], c["Command"])
        table.insert(fmtout["CREATED"], os.date("%d-%m-%Y:%H:%M:%S", c["CreatedAt"]))
        table.insert(fmtout["STATUS"], c["Status"])
        table.insert(fmtout["PORTS"], ports_str)
        table.insert(fmtout["NAMES"], names_str)
    end
    return hdrs, fmtout, colwdths
end

return cld_cmd_container