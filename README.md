luvit-base64
============
<a href="http://travis-ci.org/kengonakajima/luvit-base64"><img src="https://secure.travis-ci.org/kengonakajima/luvit-base64.png"></a>


faster base64 implemantation for luvit

Build
====
tested on Linux(ubuntu11) and osx 10.7.


    git clone git@github.com:kengonakajima/luvit-base64.git
    cd luvit-base64
    make
    make test


Use
====
Currently it's not assuming lum or some package managers.

    local base64 = require("./luvit-base64") -- this finds init.lua in the dir
    enc = base64.encode("hello")
    dec = base64.decode(enc)
    assert( dec == "hello" )
    

    
Benchmark
====
On my macbook pro (core i5, 2.53GHz), it encode/decodes 200~300 times faster than
[pure-lua base64](https://github.com/luvit/luvit/wiki/Snippets) on luvit(LuaJIT2).

    bench-enc len:	10000	n:	10000	0.44514688801765	sec	214.23811826545	Mbytes/sec
    bench-dec len:	10000	n:	10000	0.53394065189362	sec	178.61054651375	Mbytes/sec
    bench-pure-enc len:	10000	n:	100	1.6483834090233	sec	0.57855127101245	Mbytes/sec


With compile option -O3, it gets >4 times faster :

    bench-enc len:	10000	n:	10000	0.093911471001804	sec	1015.5035441708	Mbytes/sec
    bench-dec len:	10000	n:	10000	0.099035511001945	sec	962.96197874672	Mbytes/sec
    bench-pure-enc len:	10000	n:	100	1.0801547200009	sec	0.88290529009162	Mbytes/sec

