# Computer-graphic
небольшая 2D-игру в жанре roguelike

Для запуска программы необходимо выполнить следующие шаги(Linux):
создать папку (для пример CG)
скопировать в нее папку template1_cpp  (/CG/template1_cpp)
перейти в директорию CG
в ней создать папку build (CG/build)
перейти в директорию CG/build
запустить
cd .. && rm -rf build && cp -a template1_cpp build && cd build && cmake CMakeLists.txt && make && ./bin/main
