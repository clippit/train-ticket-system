#!/bin/sh

data_file="data.sqlite"
sql_file="dummy.sql"

# init database
if [[ -e $data_file ]]; then
	rm -f $data_file
fi
sqlite3 $data_file < $sql_file

# init commands
params=("-u user1 -p 123456"
	"-u user1 -p 123456 -s"
	"-u user1 -p 123456 -s --from nanjing"
	"-u user2 -p 123456 -o --number G1 --amount 5"
	"-u user1 -p 123456 -s --to shanghai"
	"-u user2 -p 123456 -s --number 1 --from nanjing"
	"-u user1 -p 123456 -o --number G1 --amount 5 -v"
	"-u user2 -p 123456 -s --to beijing -v"
	"-u user1 -p 123456 -o --number G7131 --amount 3"
	"-u user2 -p 123456 -s --from shanghai"
	"-u user2 -p 123456 -o --number G7131 --amount 3 -v"
	"-u user2 -p 123456 -v -f --orderid 1"
	"-u user1 -p 123456 -s --to shanghai --from beijing"
	"-u user1 -p 123456 -f --orderid 1"
	"-u user1 -p 123456 -o --number G1 --amount 5 -v"
)

# start
killall ticketd

if [[ $1 == online ]]; then
	./ticketd -M
else
	./ticketd
fi

for p in "${params[@]}"; do
	if [[ $1 == online ]]; then
		./ticket -M localhost $p &
	else
		./ticket $p &
	fi
done

wait
killall ticketd
exit