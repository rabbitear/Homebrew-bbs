# makefile for the shack bbs program.
#######################################################
OBJS  = main.o	  menu.o    user.o      \
	misc.o	  chat.o    last.o      \
	gmsg.o	  rmsg.o    file-ed.o   \
	game.o    wumpus.o  game-list.o \
	ulist.o   sflight.o sysop.o     \
	editor2.o gfiles.o
#######################################################
##
## editor.o 	msgtools.o	readmsg.o
#######################################################

bbs:  $(OBJS)
	gcc -o bbs $(OBJS) 

main.o:  main.c  shack.h  user.h
	gcc -c main.c

user.o:  user.c  shack.h  user.h
	gcc -c user.c

menu.o:  menu.c  shack.h
	gcc -c menu.c

gfiles.o:  gfiles.c  shack.h
	gcc -c gfiles.c

chat.o:  chat.c  shack.h
	gcc -c chat.c

misc.o:  misc.c  shack.h
	gcc -c misc.c

gmsg.o:  gmsg.c  shack.h  msg.h   rooms.h
	gcc -c gmsg.c

rmsg.o:  rmsg.c  shack.h  user.h  rooms.h
	gcc -c rmsg.c

editor2.o:   editor2.c 	shack.h  msg.h
	gcc -c editor2.c

file-ed.o:      file-ed.c      shack.h  msg.h
	gcc -c file-ed.c

last.o:  last.c  shack.h
	gcc -c last.c

ulist.o: ulist.c shack.h user.h
	gcc -c ulist.c

wumpus.o:      wumpus.c      shack.h
	gcc -c wumpus.c

game.o:	       game.c        shack.h  user.h
	gcc -c game.c

sflight.o:	sflight.c    shack.h  game.h
	gcc -c sflight.c

game-list.o:	game-list.c  shack.h  game.h  user.h
	gcc -c game-list.c

sysop.o:	sysop.c	     shack.h  user.h
	gcc -c sysop.c

##poker.o:       poker.c       shack.h
##	gcc -c poker.c


#########################################################################
##                                                                     ##
##                 to install the bbs type 'make install'              ##
##                                                                     ##
#########################################################################



install: bbs
	sudo mv ./bbs /home/bbs/bbs

