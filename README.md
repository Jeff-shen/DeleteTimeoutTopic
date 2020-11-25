# DeleteTimeoutTopic
- Delete Topic by timeout
# How to compile
- cd zookeeper/
- mkdir build
- cd build
- cmake ..
- make
# zk_client Param
	-h：
	zookeeper host:port 
	-i：
	record all topic name
	-o：
	recored jugemented timeout topic
	-t：
	timeout of the topic need to delete
	-p：
	kafka root path of a topic
	-d：
	output zk_client debug log
# zk_client example
./kafka/zk_client 
-h 127.0.0.1:2181 
-i ./kafka/zk_client_log/infile.20201125_15:35:17 
-o ./kafka/zk_client_log/outfile.20201125_15:35:17 
-t 3600 
-p /brokers/topics
# How Use delete_timeout_node.sh
 
 - put both delete_timeout_node.sh and zk_client to kafka root directory
 - set timer to execute delete_timeout_node.sh
