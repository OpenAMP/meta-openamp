/*
 * echo_test.c
 *
 *  Created on: Oct 4, 2014
 *      Author: etsam
 */

/*
 * Test application that data integraty of inter processor
 * communication from linux userspace to a remote software
 * context. The application sends chunks of data to the
 * remote processor. The remote side echoes the data back
 * to application which then validates the data returned.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>

/* Shutdown message ID */

struct _payload {
	unsigned long num;
	unsigned long size;
	char data[];
};

static int fd, err_cnt;

struct _payload *i_payload;
struct _payload *r_payload;

#define RPMSG_GET_KFIFO_SIZE 1
#define RPMSG_GET_AVAIL_DATA_SIZE 2
#define RPMSG_GET_FREE_SPACE 3

#define RPMSG_HEADER_LEN 16
#define MAX_RPMSG_BUFF_SIZE (512 - RPMSG_HEADER_LEN)
#define PAYLOAD_MIN_SIZE	1
#define PAYLOAD_MAX_SIZE	(MAX_RPMSG_BUFF_SIZE - 24)
#define NUM_PAYLOADS		(PAYLOAD_MAX_SIZE/PAYLOAD_MIN_SIZE)

int main(int argc, char *argv[])
{
	int flag = 1;
	int cmd, ret, i, j;
	int size, bytes_rcvd, bytes_sent;
	err_cnt = 0;
	int opt;
	char *rpmsg_dev="/dev/rpmsg0";
	int ntimes = 1;

	while ((opt = getopt(argc, argv, "d:n:")) != -1) {
		switch (opt) {
		case 'd':
			rpmsg_dev = optarg;
			break;
		case 'n':
			ntimes = atoi(optarg);
			break;
		default:
			printf("getopt return unsupported option: -%c\n",opt);
			break;
		}
	}
	printf("\r\n Echo test start \r\n");

	printf("\r\n Open rpmsg dev! \r\n");

	fd = open(rpmsg_dev, O_RDWR | O_NONBLOCK);

	if (fd < 0) {
		perror("Failed to open rpmsg file /dev/rpmsg0.");
		return -1;
	}

	i_payload = (struct _payload *)malloc(2 * sizeof(unsigned long) + PAYLOAD_MAX_SIZE);
	r_payload = (struct _payload *)malloc(2 * sizeof(unsigned long) + PAYLOAD_MAX_SIZE);

	if (i_payload == 0 || r_payload == 0) {
		printf("ERROR: Failed to allocate memory for payload.\n");
		return -1;
	}

	for (j=0; j < ntimes; j++){
		printf("\r\n **********************************");
		printf("****\r\n");
		printf("\r\n  Echo Test Round %d \r\n", j);
		printf("\r\n **********************************");
		printf("****\r\n");
		for (i = 0, size = PAYLOAD_MIN_SIZE; i < NUM_PAYLOADS;
		i++, size++) {
			i_payload->num = i;
			i_payload->size = size;

			/* Mark the data buffer. */
			memset(&(i_payload->data[0]), 0xA5, size);

			printf("\r\n sending payload number");
			printf(" %d of size %d\r\n", i_payload->num,
			(2 * sizeof(unsigned long)) + size);

			bytes_sent = write(fd, i_payload,
			(2 * sizeof(unsigned long)) + size);

			if (bytes_sent <= 0) {
				printf("\r\n Error sending data");
				printf(" .. \r\n");
				break;
			}
			printf("echo test: sent : %d\n", bytes_sent);

			r_payload->num = 0;
			bytes_rcvd = read(fd, r_payload,
					(2 * sizeof(unsigned long)) + PAYLOAD_MAX_SIZE);
			while (bytes_rcvd <= 0) {
				usleep(10000);
				bytes_rcvd = read(fd, r_payload,
					(2 * sizeof(unsigned long)) + PAYLOAD_MAX_SIZE);
			}
			printf(" received payload number ");
			printf("%d of size %d\r\n", r_payload->num, bytes_rcvd);

			/* Validate data buffer integrity. */
			for (i = 0; i < r_payload->size; i++) {

				if (r_payload->data[i] != 0xA5) {
					printf(" \r\n Data corruption");
					printf(" at index %d \r\n", i);
					err_cnt++;
					break;
				}
			}
			bytes_rcvd = read(fd, r_payload,
			(2 * sizeof(unsigned long)) + PAYLOAD_MAX_SIZE);

		}
		printf("\r\n **********************************");
		printf("****\r\n");
		printf("\r\n Echo Test Round %d Test Results: Error count = %d\r\n",
		j, err_cnt);
		printf("\r\n **********************************");
		printf("****\r\n");
	}

	free(i_payload);
	free(r_payload);

	return 0;
}
