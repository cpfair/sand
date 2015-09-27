sand
====

[sand](http://sand.cpfx.ca) uses [rock garden](https://github.com/cpfair/rockgarden) to add new features to existing Pebble apps.


Running sand locally
--------------------

### You'll need

* Python 3.4
* Pebble 3.x SDK (make sure the `pebble` tool is in your PATH)

### Setup

1. Check out this repo
1. Create a virtualenv (`virtualenv -p python3 .env && source .env/bin/activate`)
1. Install requirements from `requirements.txt` (`pip install -r requirements.txt`)
1. `(cd modules && python generate_icons.py)` (this creates `icons.h` from `icons.svg`)
1. `python www.py`
