CC = cl
LINK = link
CFLAGS = /c /MD /W1 /nologo

all: build

build: so_stdio.dll

so_stdio.dll: so_stdio.obj utils.obj
	$(LINK) /nologo /dll /out:so_stdio.dll /implib:so_stdio.lib so_stdio.obj utils.obj
	
so_stdio.obj: so_stdio.c so_stdio.h
	$(CC) $(CFLAGS) so_stdio.c /Foso_stdio.obj

utils.obj: utils.c utils.h
	$(CC) $(CFLAGS) utils.c /Foutils.obj
	
clean:
	del *.obj *.lib *.dll *exp