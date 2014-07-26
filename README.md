# luvit-base64

<a href="http://travis-ci.org/kengonakajima/luvit-base64"><img src="https://secure.travis-ci.org/kengonakajima/luvit-base64.png"></a>

Faster base64 implemantation for [luvit.io](http://luvit.io).

## Install

```bash
npm install luvit-base64
```

If you're not familiar with [npm](http://npmjs.org) check this out:

https://github.com/voronianski/luvit-npm-example#how-to
https://github.com/luvitrocks/luvit-utopia#install

## Example

```lua
local base64 = require('luvit-base64')
local enc = base64.encode('hello')
local dec = base64.decode(enc)
assert(dec == 'hello')
```

## Build

Tested on Linux and OS X:

    git clone git@github.com:kengonakajima/luvit-base64.git
    cd luvit-base64
    make
    make test

## Benchmark

On Macbook Pro (core i5, 2.53GHz), it encode/decodes 200~300 times faster than [pure-lua base64](https://github.com/luvit/luvit/wiki/Snippets) on luvit(LuaJIT2).

    bench-enc len:	10000	n:	10000	0.44514688801765	sec	214.23811826545	Mbytes/sec
    bench-dec len:	10000	n:	10000	0.53394065189362	sec	178.61054651375	Mbytes/sec
    bench-pure-enc len:	10000	n:	100	1.6483834090233	sec	0.57855127101245	Mbytes/sec


With compile option -O3, it gets > 4 times faster :

    bench-enc len:	10000	n:	10000	0.093911471001804	sec	1015.5035441708	Mbytes/sec
    bench-dec len:	10000	n:	10000	0.099035511001945	sec	962.96197874672	Mbytes/sec
    bench-pure-enc len:	10000	n:	100	1.0801547200009	sec	0.88290529009162	Mbytes/sec

## License (MIT)

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
'Software'), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
