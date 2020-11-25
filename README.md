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
 # How Use delete_timeout_node.sh
 
 - put both delete_timeout_node.sh and zk_client to kafka root directory
 - set timer to execute delete_timeout_node.sh
