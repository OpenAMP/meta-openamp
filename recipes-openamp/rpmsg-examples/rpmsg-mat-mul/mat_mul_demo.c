/*
 * Sample demo application that showcases inter processor
 * communication from linux userspace to a remote software
 * context. The application generates random matrices and
 * transmits them to the remote context over rpmsg. The
 * remote application performs multiplication of matrices
 * and transmits the results back to this application.
 */

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <linux/rpmsg.h>

#define RPMSG_BUS_SYS "/sys/bus/rpmsg"

#define PR_DBG(fmt, args ...) printf("%s():%u "fmt, __func__, __LINE__, ##args)
#define SHUTDOWN_MSG    0xEF56A55A
#define MATRIX_SIZE 6

struct _matrix {
	unsigned int size;
	unsigned int elements[MATRIX_SIZE][MATRIX_SIZE];
};

static void matrix_print(struct _matrix *m)
{
	int i, j;

	/* Generate two random matrices */
	printf(" \r\n Master : Linux : Printing results \r\n");

	for (i = 0; i < m->size; ++i) {
		for (j = 0; j < m->size; ++j)
			printf(" %d ", (unsigned int)m->elements[i][j]);
		printf("\r\n");
	}
}

static void generate_matrices(int num_matrices,
				unsigned int matrix_size, void *p_data)
{
	int	i, j, k;
	struct _matrix *p_matrix = p_data;
	time_t	t;
	unsigned long value;

	srand((unsigned) time(&t));

	for (i = 0; i < num_matrices; i++) {
		/* Initialize workload */
		p_matrix[i].size = matrix_size;

		printf(" \r\n Master : Linux : Input matrix %d \r\n", i);
		for (j = 0; j < matrix_size; j++) {
			printf("\r\n");
			for (k = 0; k < matrix_size; k++) {

				value = (rand() & 0x7F);
				value = value % 10;
				p_matrix[i].elements[j][k] = value;
				printf(" %d ",
				(unsigned int)p_matrix[i].elements[j][k]);
			}
		}
		printf("\r\n");
	}

}

static int charfd = -1, fd;
static struct _matrix i_matrix[2];
static struct _matrix r_matrix;

void matrix_mult(int ntimes)
{
	int i;

	for (i=0; i < ntimes; i++){
		generate_matrices(2, MATRIX_SIZE, i_matrix);

		printf("%d: write rpmsg: %lu bytes\n", i, sizeof(i_matrix));
		ssize_t rc = write(fd, i_matrix, sizeof(i_matrix));
		if (rc < 0)
			fprintf(stderr, "write,errno = %ld, %d\n", rc, errno);

		puts("read results");
		do {
			rc = read(fd, &r_matrix, sizeof(r_matrix));
		} while (rc < (int)sizeof(r_matrix));
		matrix_print(&r_matrix);
		printf("End of Matrix multiplication demo Round %d\n", i);
	}
}

void send_shutdown(int fd)
{
	int sdm = SHUTDOWN_MSG;

	if (write(fd, &sdm, sizeof(int)) < 0)
		perror("write SHUTDOWN_MSG\n");
}

int rpmsg_create_ept(int rpfd, struct rpmsg_endpoint_info *eptinfo)
{
	int ret;

	ret = ioctl(rpfd, RPMSG_CREATE_EPT_IOCTL, eptinfo);
	if (ret)
		perror("Failed to create endpoint.\n");
	return ret;
}

static char *get_rpmsg_ept_dev_name(const char *rpmsg_char_name,
				    const char *ept_name,
				    char *ept_dev_name)
{
	char sys_rpmsg_ept_name_path[64];
	char svc_name[64];
	char *sys_rpmsg_path = "/sys/class/rpmsg";
	FILE *fp;
	int i;
	int ept_name_len;

	for (i = 0; i < 128; i++) {
		sprintf(sys_rpmsg_ept_name_path, "%s/%s/rpmsg%d/name",
			sys_rpmsg_path, rpmsg_char_name, i);
		printf("checking %s\n", sys_rpmsg_ept_name_path);
		if (access(sys_rpmsg_ept_name_path, F_OK) < 0)
			continue;
		fp = fopen(sys_rpmsg_ept_name_path, "r");
		if (!fp) {
			printf("failed to open %s\n", sys_rpmsg_ept_name_path);
			break;
		}
		fgets(svc_name, sizeof(svc_name), fp);
		fclose(fp);
		printf("svc_name: %s.\n",svc_name);
		ept_name_len = strlen(ept_name);
		if (ept_name_len > sizeof(svc_name))
			ept_name_len = sizeof(svc_name);
		if (!strncmp(svc_name, ept_name, ept_name_len)) {
			sprintf(ept_dev_name, "rpmsg%d", i);
			return ept_dev_name;
		}
	}

	printf("Not able to RPMsg endpoint file for %s:%s.\n",
	       rpmsg_char_name, ept_name);
	return NULL;
}

static int bind_rpmsg_chrdev(const char *rpmsg_dev_name)
{
	char fpath[256];
	char *rpmsg_chdrv = "rpmsg_chrdev";
	int fd;
	int ret;

	/* rpmsg dev overrides path */
	sprintf(fpath, "%s/devices/%s/driver_override",
		RPMSG_BUS_SYS, rpmsg_dev_name);
	PR_DBG("open %s\n", fpath);
	fd = open(fpath, O_WRONLY);
	if (fd < 0) {
		fprintf(stderr, "Failed to open %s, %s\n",
			fpath, strerror(errno));
		return -EINVAL;
	}
	ret = write(fd, rpmsg_chdrv, strlen(rpmsg_chdrv) + 1);
	if (ret < 0) {
		fprintf(stderr, "Failed to write %s to %s, %s\n",
			rpmsg_chdrv, fpath, strerror(errno));
		return -EINVAL;
	}
	close(fd);

	/* bind the rpmsg device to rpmsg char driver */
	sprintf(fpath, "%s/drivers/%s/bind", RPMSG_BUS_SYS, rpmsg_chdrv);
	fd = open(fpath, O_WRONLY);
	if (fd < 0) {
		fprintf(stderr, "Failed to open %s, %s\n",
			fpath, strerror(errno));
		return -EINVAL;
	}
	PR_DBG("write %s to %s\n", rpmsg_dev_name, fpath);
	ret = write(fd, rpmsg_dev_name, strlen(rpmsg_dev_name) + 1);
	if (ret < 0) {
		fprintf(stderr, "Failed to write %s to %s, %s\n",
			rpmsg_dev_name, fpath, strerror(errno));
		return -EINVAL;
	}
	close(fd);
	return 0;
}

static int get_rpmsg_chrdev_fd(const char *rpmsg_dev_name,
			       char *rpmsg_ctrl_name)
{
	char dpath[2*NAME_MAX];
	DIR *dir;
	struct dirent *ent;
	int fd;

	sprintf(dpath, "%s/devices/%s/rpmsg", RPMSG_BUS_SYS, rpmsg_dev_name);
	PR_DBG("opendir %s\n", dpath);
	dir = opendir(dpath);
	if (dir == NULL) {
		fprintf(stderr, "opendir %s, %s\n", dpath, strerror(errno));
		return -EINVAL;
	}
	while ((ent = readdir(dir)) != NULL) {
		if (!strncmp(ent->d_name, "rpmsg_ctrl", 10)) {
			sprintf(dpath, "/dev/%s", ent->d_name);
			closedir(dir);
			PR_DBG("open %s\n", dpath);
			fd = open(dpath, O_RDWR | O_NONBLOCK);
			if (fd < 0) {
				fprintf(stderr, "open %s, %s\n",
					dpath, strerror(errno));
				return fd;
			}
			sprintf(rpmsg_ctrl_name, "%s", ent->d_name);
			return fd;
		}
	}

	fprintf(stderr, "No rpmsg_ctrl file found in %s\n", dpath);
	closedir(dir);
	return -EINVAL;
}

static void set_src_dst(char *out, struct rpmsg_endpoint_info *pep)
{
	long dst = 0;
	char *lastdot = strrchr(out, '.');

	if (lastdot == NULL)
		return;
	dst = strtol(lastdot + 1, NULL, 10);
	if ((errno == ERANGE && (dst == LONG_MAX || dst == LONG_MIN))
	    || (errno != 0 && dst == 0)) {
		return;
	}
	pep->dst = (unsigned int)dst;
}

/*
 * return the first dirent matching rpmsg-openamp-demo-channel
 * in /sys/bus/rpmsg/devices/ E.g.:
 *	virtio0.rpmsg-openamp-demo-channel.-1.1024
 */
static void lookup_channel(char *out, struct rpmsg_endpoint_info *pep)
{
	char dpath[] = RPMSG_BUS_SYS "/devices";
	struct dirent *ent;
	DIR *dir = opendir(dpath);

	if (dir == NULL) {
		fprintf(stderr, "opendir %s, %s\n", dpath, strerror(errno));
		return;
	}
	while ((ent = readdir(dir)) != NULL) {
		if (strstr(ent->d_name, pep->name)) {
			strncpy(out, ent->d_name, NAME_MAX);
			set_src_dst(out, pep);
			PR_DBG("using dev file: %s\n", out);
			closedir(dir);
			return;
		}
	}
	closedir(dir);
	fprintf(stderr, "No dev file for %s in %s\n", pep->name, dpath);
}

int main(int argc, char *argv[])
{
	int ntimes = 1;
	int opt, ret;
	char rpmsg_dev[NAME_MAX] = "virtio0.rpmsg-openamp-demo-channel.-1.0";
	char rpmsg_char_name[16];
	char fpath[2*NAME_MAX];
	struct rpmsg_endpoint_info eptinfo = {
		.name = "rpmsg-openamp-demo-channel", .src = 0, .dst = 0
	};
	char ept_dev_name[16];
	char ept_dev_path[32];

	printf("\r\n Matrix multiplication demo start \r\n");

	/* Load rpmsg_char driver */
	printf("\r\nMaster>probe rpmsg_char\r\n");
	ret = system("set -x; lsmod; modprobe rpmsg_char");
	if (ret < 0) {
		perror("Failed to load rpmsg_char driver.\n");
		return -EINVAL;
	}

	lookup_channel(rpmsg_dev, &eptinfo);

	while ((opt = getopt(argc, argv, "d:n:s:e:")) != -1) {
		switch (opt) {
		case 'd':
			strncpy(rpmsg_dev, optarg, sizeof(rpmsg_dev));
			break;
		case 'n':
			ntimes = atoi(optarg);
			break;
		case 's':
			eptinfo.src = atoi(optarg);
			break;
		case 'e':
			eptinfo.dst = atoi(optarg);
			break;
		default:
			printf("getopt return unsupported option: -%c\n",opt);
			break;
		}
	}

	sprintf(fpath, RPMSG_BUS_SYS "/devices/%s", rpmsg_dev);
	if (access(fpath, F_OK)) {
		fprintf(stderr, "access(%s): %s\n", fpath, strerror(errno));
		return -EINVAL;
	}
	ret = bind_rpmsg_chrdev(rpmsg_dev);
	if (ret < 0)
		return ret;
	charfd = get_rpmsg_chrdev_fd(rpmsg_dev, rpmsg_char_name);
	if (charfd < 0)
		return charfd;

	/* Create endpoint from rpmsg char driver */
	PR_DBG("rpmsg_create_ept: %s[src=%#x,dst=%#x]\n",
		eptinfo.name, eptinfo.src, eptinfo.dst);
	ret = rpmsg_create_ept(charfd, &eptinfo);
	if (ret) {
		fprintf(stderr, "rpmsg_create_ept %s\n", strerror(errno));
		return -EINVAL;
	}
	if (!get_rpmsg_ept_dev_name(rpmsg_char_name, eptinfo.name,
				    ept_dev_name))
		return -EINVAL;
	sprintf(ept_dev_path, "/dev/%s", ept_dev_name);

	printf("open %s\n", ept_dev_path);
	fd = open(ept_dev_path, O_RDWR | O_NONBLOCK);
	if (fd < 0) {
		perror(ept_dev_path);
		close(charfd);
		return -1;
	}

	PR_DBG("matrix_mult(%d)\n", ntimes);
	matrix_mult(ntimes);

	send_shutdown(fd);
	close(fd);
	if (charfd >= 0)
		close(charfd);

	printf("\r\n Quitting application .. \r\n");
	printf(" Matrix multiply application end \r\n");

	return 0;
}
