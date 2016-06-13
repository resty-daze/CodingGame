PYTHON_VERSION = 2.7
PYTHON_INCLUDE = /usr/include/python$(PYTHON_VERSION)
BOOST_PYTHON=/usr/local/Cellar/boost-python/1.60.0/lib/

game.so: game.cpp
	g++ -shared game.cpp -o game.so -L$(BOOST_PYTHON) -L/usr/lib/python$(PYTHON_VERSION)/config -lboost_python -lpython$(PYTHON_VERSION) -I$(PYTHON_INCLUDE) -std=c++11
