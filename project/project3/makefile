all : ssu_crontab ssu_crond ssu_rsync

ssu_crontab : ssu_crontab.o
				gcc -o ssu_crontab ssu_crontab.o

ssu_crontab.o : ssu_crontab.c
				  gcc -c ssu_crontab.c

ssu_crond : ssu_crond.o
				gcc -o ssu_crond ssu_crond.o

ssu_crond.o : ssu_crond.c
				  gcc -c ssu_crond.c

ssu_rsync : ssu_rsync.o
				gcc -o ssu_rsync ssu_rsync.o

ssu_rsync.o : ssu_rsync.c
				  gcc -c ssu_rsync.c

clean :
	   rm *.o
