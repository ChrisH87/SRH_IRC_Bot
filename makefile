# Variables
Version = libircclient-1.6

all: delete install

# delete
delete:
	rm $(Version).tar.gz
	rm -r $(Version)

# download and installation
install:
	wget http://sourceforge.net/projects/libircclient/files/libircclient/1.6/libircclient-1.6.tar.gz
	tar -zxf $(Version).tar.gz
	cd $(Version); ./configure; make; cd -
	cd SRH_IRC_Bot; make; cd -
