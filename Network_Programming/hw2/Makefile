all: alarm_client.c alarm_server.c
	gcc alarm_client.c -o alarm_client
	gcc alarm_server.c -o alarm_server
	gcc select_client.c -o select_client
	gcc select_server.c -o select_server
	gcc setsocket_client.c -o setsocket_client
	gcc setsocket_server.c -o setsocket_server
clean:
	rm -f alarm_client
	rm -f alarm_server
	rm -f select_client
	rm -f select_server
	rm -f setsocket_client
	rm -f setsocket_server
