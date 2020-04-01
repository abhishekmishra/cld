json = require("json")
docker = require("luaclibdocker")
cmd_container = require("cmd_container")

d = docker.connect()

cmd_ctr_ls(d)