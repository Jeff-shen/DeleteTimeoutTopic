#include "zookeeper.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/select.h>
#include <getopt.h>
#include <time.h>
#include <errno.h>
#include <assert.h>
#include <locale>

void watcher(zhandle_t *zzh, int type, int state, const char *path, void* context)
{
	fprintf(stdout, "watcher recv\n");
}

int main(int argc, char **argv) {
	static struct option long_options[] = 
	{
			{"host",     required_argument, NULL, 'h'}, 
			{"infile",      required_argument, NULL, 'i'},
			{"outfile",      required_argument, NULL, 'o'},
			{"path", required_argument, NULL, 'p'},
			{"timeout(s)", required_argument, NULL, 't'},
			{"debug",    no_argument, NULL, 'd'},
			{NULL,      0,                 NULL, 0},
	};

	tzset();
	setlocale(LC_ALL, "");

	char *zooHost = nullptr;
	int64_t now = (int64_t)time(NULL);
	int timeout = -1;
	FILE *inFp = nullptr;
	FILE *outFp = nullptr;

	char path[256] = {0};
	char node[128] = {0};
	char topic[128] = {0};

	int ret = 0;
	int opt = 0;
	int option_index = 0;

	zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);

	while ((opt = getopt_long(argc, argv, "h:i:o:p:t:d", long_options, &option_index)) != -1) {
		switch (opt) {
		case 'h':
			zooHost = optarg;
			fprintf(stdout, "zooHost=%s\n", optarg);
			break;
		case 'i':
			inFp = fopen(optarg, "r+");
			if (inFp == nullptr)
			{
				fprintf(stderr, "open file %s to read failed\n", optarg);
				return -1;
			}
			fprintf(stdout, "open file %s to read success\n", optarg);
			break;
		case 'o':
			outFp = fopen(optarg, "w+");
			if (outFp == nullptr)
			{
				fprintf(stderr, "open file %s to write failed\n", optarg);
				return -1;
			}
			fprintf(stdout, "open file %s to write success\n", optarg);
			break;
		case 'p':
			snprintf(path, sizeof(path), "%s", optarg);
			break;

		case 't':
			timeout = atoi(optarg);
			fprintf(stdout, "Timeout=%d\n", timeout);
			break;
		case 'd':
			zoo_set_debug_level(ZOO_LOG_LEVEL_DEBUG);
			fprintf(stdout, "zoo_set_debug_level=ZOO_LOG_LEVEL_DEBUG\n");
			break;
		case '?':
			if (optopt == 'h')
			{
				fprintf(stderr, "Option -%c requires host list.\n", optopt);
			}
			else if (isprint(optopt)) 
			{
				fprintf(stderr, "Unknown option `-%c'.\n", optopt);
			}
			else
			{
				fprintf(stderr,"Unknown option character `\\x%x'.\n", optopt);
				return -1;
			}
		}
	}

	if (zooHost == nullptr)
	{
		fprintf(stderr, "Please input zooHost: -h ip:port\n");
		return -1;
	}

	if (timeout == -1)
	{
		fprintf(stderr, "Please input timeout: -t Seconds\n");
		return -1;
	}

	if (inFp == nullptr)
	{
		fprintf(stderr, "Please input zoo node source list file: -i filepath\n");
		return -1;
	}

	if (outFp == nullptr)
	{
		fprintf(stderr, "Please input zoo node result list file: -o filepath\n");
		return -1;
	}

	if (strlen(path) == 0)
	{
		fprintf(stderr, "Please input path: -p /brokers/topics\n");
		return -1;
	}

	zhandle_t* zh = zookeeper_init(zooHost, watcher, 3000, 0, nullptr, 0);
	if (zh == nullptr) 
	{
		fprintf(stderr, "zookeeper_init failed, host=%s\n", zooHost);
		return -1;
	}

	fprintf(stdout, "zookeeper_init success\n");

	struct Stat stat;
	while (!feof(inFp))
	{
		stat.ctime = 0;
		memset(node, 0, sizeof(node));
		memset(topic, 0, sizeof(topic));

		fgets(topic, sizeof(topic) - 1, inFp);
		if (strlen(topic) == 0)
		{
			continue;
		}

		for (int i = 0; i < strlen(path); i++)
		{
			if (path[i] != '\n' && path != '\0')
			{
				node[i] = path[i];
			}
		}

		if (node[strlen(node)] != '/')
		{
			node[strlen(node)] = '/';
		}

		int len = strlen(node);
		for (int i = 0; i < strlen(topic); i++)
		{
			if (topic[i] != '\n')
			{
				node[len+i] = topic[i];
			}
		}

		if (strstr(node, "__consumer_offsets"))
		{
			continue;
		}

		ret = zoo_exists(zh, node, 0, &stat);
		if (ret == 0)
		{
			now = (int64_t)time(NULL);
			int64_t old = stat.ctime / 1000;
			int64_t delta = now - old;
			//fprintf(stdout, "check node timeout: node=%s, now=%lld, ctime=%lld, delta=%lld, timeout=%d\n", node, now, old, delta, timeout);
			if (delta >= timeout)
			{
				fprintf(stdout, "\033[33mnode timeout: node=%s, now=%lld, ctime=%lld, delta=%lld, timeout=%d\033[33m\n", node, now, old, delta, timeout);
				fprintf(stdout, "\033[0m");
				fputs(topic, outFp);
			}
		}
		else
		{
			fprintf(stderr, "\033[31mcheck node exists failed: node=%s\033[31m\n", node);
			fprintf(stdout, "\033[0m");
		}
	}


	fclose(outFp);
	fclose(inFp);
	ret = zookeeper_close(zh);
	if (ret != 0)
	{
		fprintf(stdout, "\033[31mzookeeper_close failed\033[31m\n");
		fprintf(stdout, "\033[0m");
	}

	return 0;
}
