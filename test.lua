local table = require("table")
local math = require("math")
local uv = require("uv_native")

local b64 = require("./init.lua")

-- copied from:  https://github.com/luvit/luvit/wiki/Snippets
local base64_table = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/'
function base64_pure(data)
  return ((data:gsub( '.', function(x)
    local r, b = '', x:byte()
    for i = 8, 1, -1 do
      r = r .. (b % 2 ^ i - b % 2 ^ (i - 1) > 0 and '1' or '0')
    end
    return r
  end) .. '0000'):gsub('%d%d%d?%d?%d?%d?', function(x)
    if #x < 6 then
      return ''
    end
    local c = 0
    for i = 1, 6 do
      c = c + (x:sub( i, i) == '1' and 2 ^ (6 - i) or 0)
    end
    return base64_table:sub( c + 1, c + 1)
  end) .. ({
    '',
    '==',
    '='
  })[#data % 3 + 1])
end


print("1")
local out = b64.encode("a")
if out ~= "YQ==" then
  error("fatal 1-enc :"..out )
end
out = b64.decode(out) 
if out ~= "a" then
  error("fatal 1-dec :"..out)
end

print("2")
out = b64.encode("ab")
if out ~= "YWI=" then
  error("fatal 2-enc :"..out)
end
out = b64.decode(out) 
if out ~= "ab" then
  error("fatal 2-dec :"..out)
end

print("3")
out = b64.encode("abc")
if out ~= "YWJj" then
  error("fatal 3-enc :"..out)
end
out = b64.decode(out) 
if out ~= "abc" then
  error("fatal 3-dec :"..out)
end

print("4")
out = b64.encode("abcd")
if out ~= "YWJjZA==" then
  error("fatal 4-enc :"..out)
end
out = b64.decode(out) 
if out ~= "abcd" then
  error("fatal 4-dec :"..out)
end


print("hello..")

local orig = "hello world!"
local expect = "aGVsbG8gd29ybGQh"


local out = b64.encode(orig)
if out ~= expect then
  print("expect:",expect, "got:",out )
  error("fatal")
end

local rev = b64.decode(out)
if rev ~= orig then
  print("expect:",orig, "got:",rev)
  error("fatal")
end


-- longer bufs

function test_2way(orig)
  local enc = b64.encode(orig)
  local pure = base64_pure(orig)
  if enc ~= pure then
    print("enc-mismatch. pure:", pure, "enc:",enc )
    error("fatal")
  end
  
  local dec = b64.decode(enc)
  if dec ~= orig then
    print("mismatch. orig:",orig, "enc:",enc, "dec:",dec)
    error("fatal")
  end  
end
print("long..")

for i=1,200,3 do
  local ary={}
  for j=1,i do
    table.insert(ary,"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")
  end  
  local long = table.concat(ary)
  test_2way(long)
end

-- benchmark

local s=""
for i=1,1000 do s = s .. "0123456789" end
local n = 10000
local st = uv.hrtime()
local encoded
for i=1,n do
  encoded = b64.encode(s)
end
local et = uv.hrtime()
local dt =(et-st)/1000
print("bench-enc len:",#s,"n:",n,dt,"sec", (#s*n)/dt/1024/1024, "Mbytes/sec" )

--

st = uv.hrtime()
for i=1,n do
  b64.decode(encoded)
end
et = uv.hrtime()
dt =(et-st)/1000
print("bench-dec len:",#s,"n:",n,dt,"sec", (#s*n)/dt/1024/1024, "Mbytes/sec" )


-- benchmark to compare with pure
st = uv.hrtime()
local puren = 100
for i=1,puren do
  base64_pure(s)
end
et = uv.hrtime()
dt = (et-st)/1000
print("bench-pure-enc len:",#s,"n:",puren,dt,"sec", (#s*puren)/dt/1024/1024, "Mbytes/sec" )



print("test ok")

