#!/bin/sh

set -x

#env
kafka_root=~/kafka
zk_client=${kafka_root}/zk_client
zk_client_log_dir=${kafka_root}/zk_client_log

#parm
zoo_host="127.0.0.1:2181"
node_path="/brokers/topics"
kafka_host="127.0.0.1:9092"
time_out=3600

if [ ! -e "${zk_client}" ]
then
    echo "${zk_client} not exist!"
    exit 0
fi

if [ ! -e "${zk_client_log_dir}" ]
then
    mkdir -P ${zk_client_log_dir}
fi

cd ${kafka_root}
op_time=$(date "+%Y%m%d_%H:%M:%S")

infile="${zk_client_log_dir}/infile.${op_time}"
if [ ! -e "${infile}" ]
then
    touch ${infile}
fi

outfile="${zk_client_log_dir}/outfile.${op_time}"
if [ ! -e "${outfile}" ]
then
     touch ${outfile}
fi

log="${zk_client_log_dir}/log.${op_time}"
if [ ! -e "${log}" ]
then
    touch ${log}
fi

${kafka_root}/bin/kafka-topics.sh --bootstrap-server ${kafka_host} --list > ${infile}
${zk_client} -h ${zoo_host} -i ${infile} -o ${outfile} -t ${time_out} -p ${node_path}

while read topic
do
    ${kafka_root}/bin/kafka-topics.sh --bootstrap-server ${kafka_host}  --delete --topic  ${topic}
    echo "${kafka_root}/bin/kafka-topics.sh --bootstrap-server ${kafka_host}  --delete --topic  ${topic}" >> ${log}
done<${outfile}

rm -f ${infile}
rm -f ${outfile}
