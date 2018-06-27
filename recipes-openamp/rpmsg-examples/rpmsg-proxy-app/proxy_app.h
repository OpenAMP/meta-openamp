#include <stdint.h>

/* System call definitions */
#define OPEN_SYSCALL_ID		1
#define CLOSE_SYSCALL_ID	2
#define WRITE_SYSCALL_ID	3
#define READ_SYSCALL_ID		4
#define ACK_STATUS_ID		5
#define TERM_SYSCALL_ID		6


#define FILE_NAME_LEN		50

struct _rpc_data {
	struct rpmsg_channel *rpmsg_chnl;
	struct rpmsg_endpoint *rp_ept;
	void *rpc_lock;
	void *sync_lock;
	void *data;
};

struct _sys_call_args {
	int32_t int_field1;
	int32_t int_field2;
	uint32_t data_len;
	char data[0];
};

/* System call rpc data structure */
struct _sys_rpc {
	uint32_t id;
	struct _sys_call_args	sys_call_args;
};
