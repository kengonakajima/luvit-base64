local b64 = require("./init.lua")


local orig = "hello world!"
local expect = "aGVsbG8gd29ybGQh"


local out = b64.encode(orig)
if out ~= expect then
  print("expect:",expect, "got:",out )
  error("fatal")
end

print("test ok")

