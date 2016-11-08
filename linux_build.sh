echo "you'll need to install dependencies if you don't have them"
echo "sudo apt-get install libboost-all-dev libsfml-dev"
g++ main.cpp -O3 -std=c++11 -o fractal_display -lsfml-graphics -lsfml-window -lsfml-system
echo "to run :"
echo "chmod +r fractal_display"
echo "./fractal_display"
