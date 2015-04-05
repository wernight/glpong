icl src\AiPaddle.cpp src\Ball.cpp src\Board.cpp src\Firework.cpp src\GLPong.cpp src\Paddle.cpp src\SceneManager.cpp -QaxW -G7 -O3 /I "lib/SDL/include" /link /NODEFAULTLIB:libc /SUBSYSTEM:CONSOLE /LTCG /MACHINE:X86 libcpmt.lib msvcrt.lib opengl32.lib glu32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib lib/SDL/lib/SDL.lib lib/SDL/lib/SDLmain.lib
pause
